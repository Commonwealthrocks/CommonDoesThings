# **CommonDoesThings** - *Dev kit...?*
*"Why don't I make my OWN **Python** library?"*

*Free, open source, MIT, made with ❤️*

## *"What the hell IS **CommonDoesThings**?"*
Well, it's nothing too much... but **CommonDoesThings** (or **CDT**) is a Python library coded in **C** that contains a load of random tools to use because I like to be special.

### **Think of it as...**

- **Actually useful helper functions** - well, to *ME* they are useful!
- **Very retarded** - you can zero out sensitive information and visit the home page of `NixOS.org` with my tool...?
- **Faster than pure Python or other libs (probably)** - y'know since all of the codec is in **C**; it's still fast...!

Originally I made built in functions like this for **CPython** itself via the source code (`bltinmodule.c`). However I decided to be real with myself and knew for a fact shit like this would *NOT* get passed to the actual **CPython**. 😒

## *"What features does... **CDT** have?"*
Oh boy I'm ~~(not)~~ glad you asked! I'll list off the features for **CommonDoesThings**...

### **Cryptography n' security**
`c.random(bytes)` - generates a set amount of *cryptographic* (random) and secure bytes for use.

`c.random_range(min, max)` - generates / picks a pure random number from the given min and max set range.

`c.contime(x, y)` - constant time comparing to prevent timing attacks.

`c.bytes(bytearray)` overwrites string from **Python** and from RAM by zero'ing it.

### **File operations**
`c.read(path, binary=False)` - file reading; one liner.

`c.write(path, data, binary=False)` file writing; one liner.

`c.rmfile(path, zero=False)` - delete files (optionally zero out the file too).

`c.rmdir(path, zero=False, recursive=False)` - delete directories (**NO SAFEGUARDS!!! USE WITH CAUTION!!**)

### **System utils**
`c.run(command)` - execute shell commands and output is *ONLY* in bytes.

`c.set_array(size)` - create mutable bytearrays / strings.

### **Wtf functions 🙄**
`c.HelloWorld("print")` - returns *"Hello World...?"* to STD / prints it (totally useless).

`c.fish()` - opens a video of a fish spinning on **YouTube**; also useless (**Windows** only).

`c.iusenixosbtw()` - opens up `nixos.org`; also useless (**Windows** only).

---

## **Installing this shit**

### **Via `pip` (source code)**
```bash
git clone https://github.com/Commonwealthrocks/CommonDoesThings.git

cd commondoesthings

pip install .
```
Needs `pip` and **Python** in your systems `PATH` to work; the reason why I could not get it on **PyPI** is cause their site *SUCKS*, so this is the solution.

## **Requirements**
All requirements needed *IF* installing **CommonDoesThings** straight from the source.

### **Python**
**At least Python `3.8+`** - for my case I used **Python** `3.12` so it will differ depending on your version.

### **C compiler**
To compile the **C** code yourself / install it with `pip` you will need a **C** compiler; you can use `MSVC` or `GCC` for **Windows**, or just `GCC` for **Linux**.

## **Docs**

### **Quick showcase on how all of the functions work**
```python
from commondoesthings import *

## generate cryptographic keys / bytes
key = c.random(32)  ## 256-bit key
iv = c.random(16)   ## 128-bit iv

## secure random numbers
rand_range = c.random_range(1, 7)

## constant time comparison (prevent timing attacks)
if c.contime(expected_hash, computed_hash):
    print("Authenticated.")

## file i/o
c.write("hi.txt", "Hello World!")
data = c.read("hi.txt")

## secure file deletion (zero'd then deletes)
c.rmfile("tax_fraud.txt", zero=True)

## secure memory wiping
sensitive = bytearray(b"mysupersecretpassword!!!")
c.bytes(sensitive)  ## now it's all zero'd

## run shell commands
output = c.run("echo Hello")
print(output)
```
About everything you'd need to know about **CommonDoesThings**...

---

Bare in mind some of the functions like `c.rmdir()` are *DANGEROUS* if not used correctly; there are no damn safe guards for this cooked ass library.

If you want a more *"serious"* tool (that's also generous) check out my encryption software; **PyKryptor**!

### **[PyKryptor (repo) <---](https://github.com/Commonwealthrocks/PyKryptor/)**

---