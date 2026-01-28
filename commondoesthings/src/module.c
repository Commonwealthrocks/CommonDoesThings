// module.c
// last updated: 28/01/2026 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#ifdef _WIN32
#define PLATFORM_WINDOWS
#include <windows.h>
#include <wincrypt.h>
#else
#define PLATFORM_LINUX
#include <sys/random.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern PyObject *c_run(PyObject *self, PyObject *args);
extern PyObject *c_bytes_clear(PyObject *self, PyObject *args);
extern PyObject *c_random(PyObject *self, PyObject *args);
extern PyObject *c_random_range(PyObject *self, PyObject *args);
extern PyObject *c_read(PyObject *self, PyObject *args, PyObject *kwargs);
extern PyObject *c_write(PyObject *self, PyObject *args, PyObject *kwargs);
extern PyObject *c_set_array(PyObject *self, PyObject *args);
extern PyObject *c_HelloWorld(PyObject *self, PyObject *args);
extern PyObject *c_fish(PyObject *self, PyObject *args);
extern PyObject *c_iusenixosbtw(PyObject *self, PyObject *args);
extern PyObject *c_rmfile(PyObject *self, PyObject *args, PyObject *kwargs);
extern PyObject *c_rmdir(PyObject *self, PyObject *args, PyObject *kwargs);
extern PyObject *c_contime(PyObject *self, PyObject *args);
extern PyObject *c_fhash(PyObject *self, PyObject *args);
extern PyObject *c_shash(PyObject *self, PyObject *args);
extern PyObject *c_fhash_file(PyObject *self, PyObject *args);
extern PyObject *c_shash_file(PyObject *self, PyObject *args);
extern PyObject *c_listdir(PyObject *self, PyObject *args, PyObject *kwargs);
extern PyMethodDef c_methods[];
static struct PyModuleDef c_module = {
    PyModuleDef_HEAD_INIT,
    "c",
    "Does this really matter? Oh well nobody will use ts anyways.",
    -1,
    c_methods};
PyMODINIT_FUNC
PyInit_c(void)
{
    return PyModule_Create(&c_module);
}

// end