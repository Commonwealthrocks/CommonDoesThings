// c_file_io.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
            data_len = PyBytes_GET_SIZE(data_obj); // needs this?
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

// end