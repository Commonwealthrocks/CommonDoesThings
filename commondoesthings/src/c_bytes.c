// c_bytes.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
PyObject *
c_bytes_clear(PyObject *self, PyObject *args)
{
    PyObject *data_obj;
    char *buffer;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "O", &data_obj))
    {
        return NULL;
    }
    if (!PyByteArray_Check(data_obj))
    {
        PyErr_SetString(PyExc_TypeError, "c.bytes(); only works on mutable bytearray objects.");
        return NULL;
    }
    buffer = PyByteArray_AS_STRING(data_obj);
    size = PyByteArray_GET_SIZE(data_obj);
    if (size > 0)
    {
        volatile char *v_buffer = (volatile char *)buffer;
        memset((void *)v_buffer, 0, size);
    }
    Py_RETURN_NONE;
}
PyObject *
c_set_array(PyObject *self, PyObject *args)
{
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "n", &size))
    {
        return NULL;
    }
    if (size < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Negative size; size cannot be negative");
        return NULL;
    }
    return PyByteArray_FromStringAndSize(NULL, size);
}

// end