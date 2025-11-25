// c_docs.c
// last updated: 25/11/2025 <d/m/y>
// common-does-things
#define PY_SSIZE_T_CLEAN
#include <Python.h>
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
PyMethodDef c_methods[] = {
    {"run",
     (PyCFunction)c_run,
     METH_VARARGS,
     "run(command) -> bytes\n\n"
     "Execute a shell command and return its output as bytes.\n\n"
     "Args:\n"
     "    command (str): the shell command to execute.\n\n"
     "Returns:\n"
     "    bytes: The command output.\n\n"
     "Example:\n"
     "    >>> import commondoesthings as c\n"
     "    >>> output = c.run('echo Hello')\n"},
    {"bytes",
     (PyCFunction)c_bytes_clear,
     METH_VARARGS,
     "bytes(bytearray) -> None\n\n"
     "Securely clear/wipe a bytearray object by overwriting with zeros.\n\n"
     "Args:\n"
     "    bytearray: the bytearray object to clear.\n\n"
     "Example:\n"
     "    >>> ba = bytearray(b'This is very secret!!!')\n"
     "    >>> c.bytes(ba)\n"
     "    >>> ba\n"
     "    bytearray(b'\\x00\\x00\\x00\\x00\\x00\\x00')\n"},
    {"random",
     (PyCFunction)c_random,
     METH_VARARGS,
     "random(n_bytes) -> bytes\n\n"
     "Generate cryptographically secure random bytes.\n\n"
     "Args:\n"
     "    n_bytes (int): number of random bytes to generate.\n\n"
     "Returns:\n"
     "    bytes: cryptographically secure random bytes.\n\n"
     "Example:\n"
     "    >>> key = c.random(32)  ## 256-bit key\n"},
    {"random_range",
     (PyCFunction)c_random_range,
     METH_VARARGS,
     "random_range(min_val, max_val) -> int\n\n"
     "Generate a cryptographically secure random integer in the range [min_val, max_val).\n\n"
     "Args:\n"
     "    min_val (int): minimum value (inclusive).\n"
     "    max_val (int): maximum value (exclusive).\n\n"
     "Returns:\n"
     "    int: random integer in specified range.\n\n"
     "Example:\n"
     "    >>> roll = c.random_range(1, 7)  ## cryptographic range\n"},
    {"read",
     (PyCFunction)c_read,
     METH_VARARGS | METH_KEYWORDS,
     "read(filename, binary=False) -> str or bytes\n\n"
     "Read file contents.\n\n"
     "Args:\n"
     "    filename (str): path to the file to read.\n"
     "    binary (bool): if True, return bytes; if False, return string (default: False).\n\n"
     "Returns:\n"
     "    str or bytes: File contents.\n\n"
     "Example:\n"
     "    >>> text = c.read('file.txt')\n"
     "    >>> data = c.read('file.bin', binary=True)\n"},
    {"write",
     (PyCFunction)c_write,
     METH_VARARGS | METH_KEYWORDS,
     "write(filename, data, binary=False) -> None\n\n"
     "Write data to a file.\n\n"
     "Args:\n"
     "    filename (str): path to the file to write.\n"
     "    data (str or bytes): data to write.\n"
     "    binary (bool): if True, write as binary; if False, write as text (default: False).\n\n"
     "Example:\n"
     "    >>> c.write('output.txt', 'Hello World')\n"
     "    >>> c.write('output.bin', b'\\x00\\x01\\x02', binary=True)\n"},
    {"set_array",
     (PyCFunction)c_set_array,
     METH_VARARGS,
     "set_array(size) -> bytearray\n\n"
     "Create a new bytearray of the specified size.\n\n"
     "Args:\n"
     "    size (int): size of the bytearray to create.\n\n"
     "Returns:\n"
     "    bytearray: a new bytearray object.\n\n"
     "Example:\n"
     "    >>> buffer = c.set_array(256)\n"},
    {"HelloWorld",
     (PyCFunction)c_HelloWorld,
     METH_VARARGS,
     "HelloWorld(text) -> None\n\n"
     "A gimmick function that prints 'Hello World...?' to stdout.\n\n"
     "Args:\n"
     "    text (str): Input text (not used, just required).\n\n"
     "Example:\n"
     "    >>> c.HelloWorld('anything')\n"
     "    Hello World...?\n"},
    {"fish",
     (PyCFunction)c_fish,
     METH_NOARGS,
     "fish() -> None\n\n"
     "Opens the fish video in browser (Windows only).\n\n"
     "Example:\n"
     "    >>> c.fish()\n"},
    {"iusenixosbtw",
     (PyCFunction)c_iusenixosbtw,
     METH_NOARGS,
     "iusenixosbtw() -> None\n\n"
     "Opens the NixOS website in browser (Windows only).\n\n"
     "Example:\n"
     "    >>> c.iusenixosbtw()\n"},
    {"rmfile",
     (PyCFunction)c_rmfile,
     METH_VARARGS | METH_KEYWORDS,
     "rmfile(path, zero=False) -> None\n\n"
     "Delete a file, optionally zeroing its contents first.\n\n"
     "Args:\n"
     "    path (str): path to the file to delete.\n"
     "    zero (bool): if True, overwrite file with zeros before deletion (default: False).\n\n"
     "Example:\n"
     "    >>> c.rmfile('secret.txt', zero=True)  ## secure delete\n"
     "    >>> c.rmfile('temp.txt')  ## normal delete\n"},
    {"rmdir",
     (PyCFunction)c_rmdir,
     METH_VARARGS | METH_KEYWORDS,
     "rmdir(path, zero=False, recursive=False) -> None\n\n"
     "Delete a directory, optionally with recursive deletion and file zeroing.\n\n"
     "Args:\n"
     "    path (str): path to the directory to delete.\n"
     "    zero (bool): if True, overwrite all files with zeros before deletion (default: False).\n"
     "    recursive (bool): if True, delete all contents recursively (default: False).\n\n"
     "Example:\n"
     "    >>> c.rmdir('temp_folder', recursive=True, zero=True)  ## secure recursive delete\n"
     "    >>> c.rmdir('empty_folder')  ## delete empty directory\n"},
    {"contime",
     (PyCFunction)c_contime,
     METH_VARARGS,
     "contime(a, b) -> bool\n\n"
     "Constant time comparison to prevent timing attacks.\n\n"
     "This function compares two bytes like objects in constant time,\n"
     "meaning the comparison takes the same amount of time regardless\n"
     "of where the first mismatch occurs. This prevents attackers from\n"
     "using timing information to guess secrets byte-by-byte.\n\n"
     "Args:\n"
     "    a (bytes-like): first value to compare.\n"
     "    b (bytes-like): second value to compare.\n\n"
     "Returns:\n"
     "    bool: True if equal, False otherwise.\n\n"
     "Example:\n"
     "    >>> expected_mac = c.random(32)\n"
     "    >>> computed_mac = calculate_mac(data)\n"
     "    >>> if c.contime(expected_mac, computed_mac):\n"
     "    ...     print('authenticated!')\n"},
    {NULL, NULL, 0, NULL}};

// end