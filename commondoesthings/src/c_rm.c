// c_rm.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define rmdir _rmdir
#define stat _stat
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#else
#include <unistd.h>
#include <dirent.h>
#endif
#define ZERO_CHUNK_SIZE (8 * 1024 * 1024) // too big? o.o
static int
zero_file_contents(const char *filepath)
{
    FILE *fp = fopen(filepath, "r+b");
    if (!fp)
    {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (file_size <= 0)
    {
        fclose(fp);
        return 0; // dot
    }
    unsigned char *zero_buffer = (unsigned char *)calloc(ZERO_CHUNK_SIZE, 1);
    if (!zero_buffer)
    {
        fclose(fp);
        return -1;
    }
    long remaining = file_size;
    while (remaining > 0)
    {
        size_t chunk_size = (remaining > ZERO_CHUNK_SIZE) ? ZERO_CHUNK_SIZE : remaining;
        size_t written = fwrite(zero_buffer, 1, chunk_size, fp);
        if (written != chunk_size)
        {
            free(zero_buffer);
            fclose(fp);
            return -1;
        }
        remaining -= written;
    }
    fflush(fp);
    free(zero_buffer);
    fclose(fp);
    return 0;
}
#ifdef _WIN32
static int
recursive_delete_dir(const char *dirpath, int zero_files)
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
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (recursive_delete_dir(full_path, zero_files) != 0)
            {
                FindClose(hFind);
                return -1;
            }
            if (rmdir(full_path) != 0)
            {
                FindClose(hFind);
                return -1;
            }
        }
        else
        {
            if (zero_files)
            {
                zero_file_contents(full_path);
            }
            if (remove(full_path) != 0)
            {
                FindClose(hFind);
                return -1;
            }
        }
    } while (FindNextFileA(hFind, &find_data) != 0);
    FindClose(hFind);
    return 0;
}
#else
static int
recursive_delete_dir(const char *dirpath, int zero_files)
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
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, entry->d_name);
        struct stat statbuf;
        if (stat(full_path, &statbuf) != 0)
        {
            closedir(dir);
            return -1;
        }
        if (S_ISDIR(statbuf.st_mode))
        {
            if (recursive_delete_dir(full_path, zero_files) != 0)
            {
                closedir(dir);
                return -1;
            }
            if (rmdir(full_path) != 0)
            {
                closedir(dir);
                return -1;
            }
        }
        else
        {
            if (zero_files)
            {
                zero_file_contents(full_path);
            }
            if (remove(full_path) != 0)
            {
                closedir(dir);
                return -1;
            }
        }
    }
    closedir(dir);
    return 0;
}
#endif
PyObject *
c_rmfile(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *filepath;
    int zero = 0;

    static char *kwlist[] = {"path", "zero", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|p", kwlist, &filepath, &zero))
    {
        return NULL;
    }
    struct stat statbuf;
    if (stat(filepath, &statbuf) != 0)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_FileNotFoundError, filepath);
        return NULL;
    }
    if (S_ISDIR(statbuf.st_mode))
    {
        PyErr_Format(PyExc_IsADirectoryError, "Path is a directory; use c.rmdir() instead: %s", filepath);
        return NULL;
    }
    if (zero)
    {
        if (zero_file_contents(filepath) != 0)
        {
            PyErr_Format(PyExc_OSError, "Failed; could not zero file contents: %s", filepath);
            return NULL;
        }
    }
    if (remove(filepath) != 0)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_OSError, filepath);
        return NULL;
    }
    Py_RETURN_NONE;
}
PyObject *
c_rmdir(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *dirpath;
    int zero = 0;
    int recursive = 0;
    static char *kwlist[] = {"path", "zero", "recursive", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|pp", kwlist, &dirpath, &zero, &recursive))
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
        PyErr_Format(PyExc_NotADirectoryError, "Path is a file; use c.rmfile() instead: %s", dirpath);
        return NULL;
    }
    if (recursive)
    {
        if (recursive_delete_dir(dirpath, zero) != 0)
        {
            PyErr_Format(PyExc_OSError, "Failed; could not recursively delete directory: %s", dirpath);
            return NULL;
        }
    }
    if (rmdir(dirpath) != 0)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_OSError, dirpath);
        return NULL;
    }
    Py_RETURN_NONE;
}

// end //