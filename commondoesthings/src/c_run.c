// c_run.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif
PyObject *
c_run(PyObject *self, PyObject *args)
{
    const char *command;
    FILE *pipe;
    char buffer[128];
    if (!PyArg_ParseTuple(args, "s", &command))
    {
        return NULL;
    }
    pipe = popen(command, "r");
    if (!pipe)
    {
        PyErr_SetString(PyExc_IOError, "No exec; could not execute command.");
        return NULL;
    }
    PyObject *byte_list = PyList_New(0);
    if (!byte_list)
    {
        pclose(pipe);
        return NULL;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        size_t len = strlen(buffer);
        PyObject *chunk = PyBytes_FromStringAndSize(buffer, len);
        if (!chunk)
        {
            Py_DECREF(byte_list);
            pclose(pipe);
            return NULL;
        }
        PyList_Append(byte_list, chunk);
        Py_DECREF(chunk);
    }
    pclose(pipe);
    PyObject *empty_bytes = PyBytes_FromStringAndSize("", 0);
    PyObject *method_name = PyUnicode_FromString("join");
    PyObject *result_bytes = PyObject_CallMethodObjArgs(empty_bytes, method_name, byte_list, NULL);
    Py_XDECREF(empty_bytes);
    Py_XDECREF(method_name);
    Py_XDECREF(byte_list);
    return result_bytes;
}

// end