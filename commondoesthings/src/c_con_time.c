// c_con_time.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
PyObject *
c_contime(PyObject *self, PyObject *args)
{
    Py_buffer buf_a, buf_b;
    PyObject *obj_a, *obj_b;
    if (!PyArg_ParseTuple(args, "OO", &obj_a, &obj_b))
    {
        return NULL;
    }
    if (PyObject_GetBuffer(obj_a, &buf_a, PyBUF_SIMPLE) != 0)
    {
        PyErr_SetString(PyExc_TypeError, "Invalid argument; first object must be byte like.");
        return NULL;
    }
    if (PyObject_GetBuffer(obj_b, &buf_b, PyBUF_SIMPLE) != 0)
    {
        PyBuffer_Release(&buf_a);
        PyErr_SetString(PyExc_TypeError, "Invalid argument; second object must be byte like.");
        return NULL;
    }
    int result = 1;
    if (buf_a.len != buf_b.len)
    {
        result = 0;
    }
    size_t compare_len = (buf_a.len < buf_b.len) ? buf_a.len : buf_b.len;
    const unsigned char *a = (const unsigned char *)buf_a.buf;
    const unsigned char *b = (const unsigned char *)buf_b.buf;
    unsigned char diff = 0;
    for (size_t i = 0; i < compare_len; i++)
    {
        diff |= a[i] ^ b[i];
    }

    if (diff != 0)
    {
        result = 0;
    }
    PyBuffer_Release(&buf_a);
    PyBuffer_Release(&buf_b);
    if (result)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

// end