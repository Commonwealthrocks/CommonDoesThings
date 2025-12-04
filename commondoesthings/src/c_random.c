// c_random.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "advapi32.lib")
#else
#include <sys/random.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif
static int
get_secure_random_bytes(unsigned char *buffer, size_t size)
{
#ifdef _WIN32
    HCRYPTPROV hProvider = 0;
    if (!CryptAcquireContextW(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        return -1;
    }
    if (!CryptGenRandom(hProvider, (DWORD)size, buffer))
    {
        CryptReleaseContext(hProvider, 0);
        return -1;
    }
    CryptReleaseContext(hProvider, 0);
    return 0;
#else
    ssize_t result = getrandom(buffer, size, 0);
    if (result == (ssize_t)size)
    {
        return 0;
    }
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }
    ssize_t bytes_read = read(fd, buffer, size);
    close(fd);
    if (bytes_read != (ssize_t)size)
    {
        return -1;
    }
    return 0;
#endif
}
PyObject *
c_random(PyObject *self, PyObject *args)
{
    long n_bytes;
    PyObject *result = NULL;
    unsigned char *buffer = NULL;
    if (!PyArg_ParseTuple(args, "l", &n_bytes))
    {
        return NULL;
    }
    if (n_bytes < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Negative bytes; bytes cannot be negative");
        return NULL;
    }
    if (n_bytes == 0)
    {
        return PyBytes_FromStringAndSize(NULL, 0);
    }
    buffer = (unsigned char *)PyMem_Malloc(n_bytes);
    if (!buffer)
    {
        return PyErr_NoMemory();
    }
    if (get_secure_random_bytes(buffer, n_bytes) != 0)
    {
        PyErr_SetString(PyExc_OSError, "Unable to generate; cannot generate random bytes.");
        PyMem_Free(buffer);
        return NULL;
    }
    result = PyBytes_FromStringAndSize((const char *)buffer, n_bytes);
    PyMem_Free(buffer);
    return result;
}
PyObject *
c_random_range(PyObject *self, PyObject *args)
{
    long min_val, max_val;
    if (!PyArg_ParseTuple(args, "ll", &min_val, &max_val))
    {
        return NULL;
    }
    if (min_val >= max_val)
    {
        PyErr_SetString(PyExc_ValueError, "max_val too small; min_val must be strictly less than max_val.");
        return NULL;
    }
    long range = max_val - min_val;
    if (range < 0)
    {
        PyErr_SetString(PyExc_OverflowError, "Range too large; must be standard long integer.");
        return NULL;
    }
    unsigned long mask = range;
    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;
    unsigned long secure_rand;
    do
    {
        if (get_secure_random_bytes((unsigned char *)&secure_rand, sizeof(secure_rand)) != 0)
        {
            PyErr_SetString(PyExc_OSError, "Unable to generate; cannot generate random bytes.");
            return NULL;
        }
        secure_rand &= mask;
    } while (secure_rand >= (unsigned long)range);
    return PyLong_FromLong(secure_rand + min_val);
}

// end //