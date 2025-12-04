// c_file_io.c
// last updated: 04/12/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
#define PATH_SEP '\\'
#define stat _stat
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#else
#include <dirent.h>
#include <fnmatch.h>
#define PATH_SEP '/'
#endif
PyObject *
c_read(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *filename;
    int binary_mode = 0;
    FILE *fp;
    long length;
    char *buffer;
    PyObject *result;
    static char *kwlist[] = {"filename", "binary", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|p", kwlist, &filename, &binary_mode))
    {
        return NULL;
    }
    fp = fopen(filename, "rb");
    if (!fp)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_FileNotFoundError, filename);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (char *)malloc(length + 1);
    if (!buffer)
    {
        fclose(fp);
        return PyErr_NoMemory();
    }
    size_t bytes_read = fread(buffer, 1, length, fp);
    buffer[bytes_read] = '\0';
    fclose(fp);
    if (binary_mode)
    {
        result = PyBytes_FromStringAndSize(buffer, bytes_read);
    }
    else
    {
        result = PyUnicode_DecodeUTF8(buffer, bytes_read, "replace");
    }
    free(buffer);
    return result;
}
PyObject *
c_write(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *filename;
    PyObject *data_obj;
    int binary_mode = 0;
    FILE *fp;
    const char *data;
    Py_ssize_t data_len;
    static char *kwlist[] = {"filename", "data", "binary", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|p", kwlist, &filename, &data_obj, &binary_mode))
    {
        return NULL;
    }
    if (binary_mode)
    {
        if (PyBytes_Check(data_obj))
        {
            data = PyBytes_AS_STRING(data_obj);
            data_len = PyBytes_GET_SIZE(data_obj);
        }
        else if (PyByteArray_Check(data_obj))
        {
            data = PyByteArray_AS_STRING(data_obj);
            data_len = PyByteArray_GET_SIZE(data_obj);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "In binary mode; data must be bytes or bytearray.");
            return NULL;
        }
        fp = fopen(filename, "wb");
    }
    else
    {
        if (!PyUnicode_Check(data_obj))
        {
            PyErr_SetString(PyExc_TypeError, "In text mode; data must be a liner-string.");
            return NULL;
        }
        data = PyUnicode_AsUTF8AndSize(data_obj, &data_len);
        if (!data)
        {
            return NULL;
        }
        fp = fopen(filename, "w");
    }
    if (!fp)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_OSError, filename);
        return NULL;
    }
    size_t written = fwrite(data, 1, data_len, fp);
    fclose(fp);
    if (written != (size_t)data_len)
    {
        PyErr_SetString(PyExc_IOError, "Failed to write; could not write data to file(s).");
        return NULL;
    }
    Py_RETURN_NONE;
}
static int
simple_match(const char *pattern, const char *string)
{
    if (*pattern == '\0' && *string == '\0')
        return 1;
    if (*pattern == '*')
    {
        if (*(pattern + 1) == '\0')
            return 1;
        while (*string != '\0')
        {
            if (simple_match(pattern + 1, string))
                return 1;
            string++;
        }
        return simple_match(pattern + 1, string);
    }
    if (*pattern == '?' || *pattern == *string)
    {
        return simple_match(pattern + 1, string + 1);
    }
    return 0;
}
#ifdef _WIN32
static int
listdir_recursive_win(const char *dirpath, PyObject *result_list, const char *pattern, int fullpath, int files_only, int dirs_only)
{
    WIN32_FIND_DATAA find_data;
    HANDLE hFind;
    char search_path[MAX_PATH];
    char full_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", dirpath);
    hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    do
    {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
        {
            continue;
        }
        snprintf(full_path, MAX_PATH, "%s\\%s", dirpath, find_data.cFileName);
        int is_dir = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if ((files_only && is_dir) || (dirs_only && !is_dir))
        {
            if (is_dir)
            {
                listdir_recursive_win(full_path, result_list, pattern, fullpath, files_only, dirs_only);
            }
            continue;
        }
        if (pattern && !simple_match(pattern, find_data.cFileName))
        {
            if (is_dir)
            {
                listdir_recursive_win(full_path, result_list, pattern, fullpath, files_only, dirs_only);
            }
            continue;
        }
        PyObject *path_str;
        if (fullpath)
        {
            path_str = PyUnicode_FromString(full_path);
        }
        else
        {
            path_str = PyUnicode_FromString(find_data.cFileName);
        }
        if (path_str)
        {
            PyList_Append(result_list, path_str);
            Py_DECREF(path_str);
        }
        if (is_dir)
        {
            listdir_recursive_win(full_path, result_list, pattern, fullpath, files_only, dirs_only);
        }
    } while (FindNextFileA(hFind, &find_data) != 0);
    FindClose(hFind);
    return 0;
}
#else
static int
listdir_recursive_unix(const char *dirpath, PyObject *result_list, const char *pattern, int fullpath, int files_only, int dirs_only)
{
    DIR *dir = opendir(dirpath);
    if (!dir)
    {
        return -1;
    }
    struct dirent *entry;
    char full_path[4096];
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;

            snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, entry->d_name);
            struct stat statbuf;
            if (stat(full_path, &statbuf) != 0)
            {
                continue;
            }
            int is_dir = S_ISDIR(statbuf.st_mode);
            if ((files_only && is_dir) || (dirs_only && !is_dir))
            {
                if (is_dir)
                {
                    listdir_recursive_unix(full_path, result_list, pattern, fullpath, files_only, dirs_only);
                }
                continue;
            }
            if (pattern && !simple_match(pattern, entry->d_name))
            {
                if (is_dir)
                {
                    listdir_recursive_unix(full_path, result_list, pattern, fullpath, files_only, dirs_only);
                }
                continue;
            }
            PyObject *path_str;
            if (fullpath)
            {
                path_str = PyUnicode_FromString(full_path);
            }
            else
            {
                path_str = PyUnicode_FromString(entry->d_name);
            }
            if (path_str)
            {
                PyList_Append(result_list, path_str);
                Py_DECREF(path_str);
            }
            if (is_dir)
            {
                listdir_recursive_unix(full_path, result_list, pattern, fullpath, files_only, dirs_only);
            }
        }
        closedir(dir);
        return 0;
    }
#endif
PyObject *
c_listdir(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *dirpath = ".";
    const char *pattern = NULL;
    int recursive = 0;
    int fullpath = 0;
    int files_only = 0;
    int dirs_only = 0;
    static char *kwlist[] = {"path", "pattern", "recursive", "fullpath", "files_only", "dirs_only", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|szpppp", kwlist, &dirpath, &pattern, &recursive, &fullpath, &files_only, &dirs_only))
    {
        return NULL;
    }
    struct stat statbuf;
    if (stat(dirpath, &statbuf) != 0)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_FileNotFoundError, dirpath);
        return NULL;
    }
    if (!S_ISDIR(statbuf.st_mode))
    {
        PyErr_Format(PyExc_NotADirectoryError, "Not a directory: %s", dirpath);
        return NULL;
    }
    PyObject *result_list = PyList_New(0);
    if (!result_list)
    {
        return NULL;
    }
    if (recursive)
    {
#ifdef _WIN32
        if (listdir_recursive_win(dirpath, result_list, pattern, fullpath, files_only, dirs_only) != 0)
#else
            if (listdir_recursive_unix(dirpath, result_list, pattern, fullpath, files_only, dirs_only) != 0)
#endif
        {
            Py_DECREF(result_list);
            PyErr_SetFromErrnoWithFilename(PyExc_OSError, dirpath);
            return NULL;
        }
    }
    else
    {
#ifdef _WIN32
        WIN32_FIND_DATAA find_data;
        HANDLE hFind;
        char search_path[MAX_PATH];
        char full_path[MAX_PATH];
        snprintf(search_path, MAX_PATH, "%s\\*", dirpath);
        hFind = FindFirstFileA(search_path, &find_data);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            Py_DECREF(result_list);
            PyErr_SetFromErrnoWithFilename(PyExc_OSError, dirpath);
            return NULL;
        }
        do
        {
            if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
            {
                continue;
            }
            int is_dir = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            if ((files_only && is_dir) || (dirs_only && !is_dir))
            {
                continue;
            }
            if (pattern && !simple_match(pattern, find_data.cFileName))
            {
                continue;
            }
            PyObject *path_str;
            if (fullpath)
            {
                snprintf(full_path, MAX_PATH, "%s\\%s", dirpath, find_data.cFileName);
                path_str = PyUnicode_FromString(full_path);
            }
            else
            {
                path_str = PyUnicode_FromString(find_data.cFileName);
            }
            if (path_str)
            {
                PyList_Append(result_list, path_str);
                Py_DECREF(path_str);
            }
        } while (FindNextFileA(hFind, &find_data) != 0);
        FindClose(hFind);
#else
            DIR *dir = opendir(dirpath);
            if (!dir)
            {
                Py_DECREF(result_list);
                PyErr_SetFromErrnoWithFilename(PyExc_OSError, dirpath);
                return NULL;
            }
            struct dirent *entry;
            char full_path[4096];
            while ((entry = readdir(dir)) != NULL)
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                {
                    continue;
                }
                snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, entry->d_name);
                struct stat statbuf2;
                if (stat(full_path, &statbuf2) != 0)
                {
                    continue;
                }
                int is_dir = S_ISDIR(statbuf2.st_mode);
                if ((files_only && is_dir) || (dirs_only && !is_dir))
                {
                    continue;
                }
                if (pattern && !simple_match(pattern, entry->d_name))
                {
                    continue;
                }
                PyObject *path_str;
                if (fullpath)
                {
                    path_str = PyUnicode_FromString(full_path);
                }
                else
                {
                    path_str = PyUnicode_FromString(entry->d_name);
                }

                if (path_str)
                {
                    PyList_Append(result_list, path_str);
                    Py_DECREF(path_str);
                }
            }
            closedir(dir);
#endif
    }
    return result_list;
}

// end