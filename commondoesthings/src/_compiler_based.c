// _compiler_based.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
PyObject *
c_HelloWorld(PyObject *self, PyObject *args)
{
    const char *input_str;
    if (!PyArg_ParseTuple(args, "s", &input_str))
    {
        return NULL;
    }
    PySys_WriteStdout("Hello World...?\n");
    Py_RETURN_NONE;
}
#ifdef _WIN32
PyObject *
c_fish(PyObject *self, PyObject *args)
{
    system("start https://www.youtube.com/watch?v=YAgJ9XugGBo/");
    Py_RETURN_NONE;
}
PyObject *
c_iusenixosbtw(PyObject *self, PyObject *args)
{
    system("start https://nixos.org/");
    Py_RETURN_NONE;
}
#else
PyObject *
c_fish(PyObject *self, PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "c.fish(); only available on Windows.");
    return NULL;
}
PyObject *
c_iusenixosbtw(PyObject *self, PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "c.iusenixosbtw(); only available on Windows.");
    return NULL;
}
#endif

// end //