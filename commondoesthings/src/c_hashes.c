// c_hashes.c
// last updated: 28/01/2026 <d/m/y>
// common-does-things
#include <Python.h>
#include <string.h>
#include <stdint.h>
#ifdef _MSC_VER
#include <stdlib.h>
#endif
#define XXH_PRIME32_1 0x9E3779B1U
#define XXH_PRIME32_2 0x85EBCA77U
#define XXH_PRIME32_3 0xC2B2AE3DU
#define XXH_PRIME32_4 0x27D4EB2FU
#define XXH_PRIME32_5 0x165667B1U
static inline uint32_t xxh_rotl32(uint32_t x, int r)
{
    return (x << r) | (x >> (32 - r));
}
static uint32_t
xxhash32(const void *input, size_t len)
{
    const uint8_t *p = (const uint8_t *)input;
    const uint8_t *const end = p + len;
    uint32_t h32;
    if (len >= 16)
    {
        const uint8_t *const limit = end - 16;
        uint32_t v1 = XXH_PRIME32_1 + XXH_PRIME32_2;
        uint32_t v2 = XXH_PRIME32_2;
        uint32_t v3 = 0;
        uint32_t v4 = XXH_PRIME32_1;
        do
        {
            v1 += *(uint32_t *)p * XXH_PRIME32_2;
            v1 = xxh_rotl32(v1, 13);
            v1 *= XXH_PRIME32_1;
            p += 4;
            v2 += *(uint32_t *)p * XXH_PRIME32_2;
            v2 = xxh_rotl32(v2, 13);
            v2 *= XXH_PRIME32_1;
            p += 4;
            v3 += *(uint32_t *)p * XXH_PRIME32_2;
            v3 = xxh_rotl32(v3, 13);
            v3 *= XXH_PRIME32_1;
            p += 4;
            v4 += *(uint32_t *)p * XXH_PRIME32_2;
            v4 = xxh_rotl32(v4, 13);
            v4 *= XXH_PRIME32_1;
            p += 4;
        } while (p <= limit);

        h32 = xxh_rotl32(v1, 1) + xxh_rotl32(v2, 7) + xxh_rotl32(v3, 12) + xxh_rotl32(v4, 18);
    }
    else
    {
        h32 = XXH_PRIME32_5;
    }
    h32 += (uint32_t)len;

    while (p + 4 <= end)
    {
        h32 += *(uint32_t *)p * XXH_PRIME32_3;
        h32 = xxh_rotl32(h32, 17) * XXH_PRIME32_4;
        p += 4;
    }
    while (p < end)
    {
        h32 += (*p) * XXH_PRIME32_5;
        h32 = xxh_rotl32(h32, 11) * XXH_PRIME32_1;
        p++;
    }
    h32 ^= h32 >> 15;
    h32 *= XXH_PRIME32_2;
    h32 ^= h32 >> 13;
    h32 *= XXH_PRIME32_3;
    h32 ^= h32 >> 16;
    return h32;
}
#define HA256_BLOCK_SIZE 64
#define HA256_HASH_SIZE 32
typedef struct
{
    uint32_t state[8];
    uint8_t buffer[HA256_BLOCK_SIZE];
    uint64_t count;
} HA256_CTX;
static const uint8_t HA256_SBOX[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

static inline uint32_t ha256_rotr(uint32_t x, int n)
{
#ifdef _MSC_VER
    return _rotr(x, n);
#else
    return (x >> n) | (x << (32 - n));
#endif
}

static void
ha256_compress(uint32_t state[8], const uint8_t block[HA256_BLOCK_SIZE])
{
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    int i;
    // Prepare message schedule
    for (i = 0; i < 16; i++)
    {
        w[i] = ((uint32_t)block[i * 4] << 24) |
               ((uint32_t)block[i * 4 + 1] << 16) |
               ((uint32_t)block[i * 4 + 2] << 8) |
               ((uint32_t)block[i * 4 + 3]);
    }
    for (i = 16; i < 64; i += 4) // Unrolled by 4
    {
#define M_SCH_STEP(k)                                                                                \
    {                                                                                                \
        uint32_t s0 = ha256_rotr(w[(k) - 15], 7) ^ ha256_rotr(w[(k) - 15], 18) ^ (w[(k) - 15] >> 3); \
        uint32_t s1 = ha256_rotr(w[(k) - 2], 17) ^ ha256_rotr(w[(k) - 2], 19) ^ (w[(k) - 2] >> 10);  \
        w[(k)] = w[(k) - 16] + s0 + w[(k) - 7] + s1;                                                 \
        w[(k)] ^= HA256_SBOX[w[(k)] & 0xFF];                                                         \
    }
        M_SCH_STEP(i)
        M_SCH_STEP(i + 1)
        M_SCH_STEP(i + 2)
        M_SCH_STEP(i + 3)
    }
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];
    for (i = 0; i < 64; i++)
    {
        uint32_t S1 = ha256_rotr(e, 6) ^ ha256_rotr(e, 11) ^ ha256_rotr(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = h + S1 + ch + w[i];
        uint32_t S0 = ha256_rotr(a, 2) ^ ha256_rotr(a, 13) ^ ha256_rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}
static void
ha256_init(HA256_CTX *ctx)
{
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->count = 0;
}
static void
ha256_update(HA256_CTX *ctx, const uint8_t *data, size_t len)
{
    size_t index = ctx->count % HA256_BLOCK_SIZE;
    ctx->count += len;
    if (index + len >= HA256_BLOCK_SIZE)
    {
        size_t fill = HA256_BLOCK_SIZE - index;
        memcpy(ctx->buffer + index, data, fill);
        ha256_compress(ctx->state, ctx->buffer);
        index = 0;
        data += fill;
        len -= fill;
        while (len >= HA256_BLOCK_SIZE)
        {
            ha256_compress(ctx->state, data);
            data += HA256_BLOCK_SIZE;
            len -= HA256_BLOCK_SIZE;
        }
    }
    if (len > 0)
    {
        memcpy(ctx->buffer + index, data, len);
    }
}

static void
ha256_final(HA256_CTX *ctx, uint8_t hash[HA256_HASH_SIZE])
{
    size_t index = ctx->count % HA256_BLOCK_SIZE;
    size_t pad_len = (index < 56) ? (56 - index) : (120 - index);
    uint8_t padding[128];
    padding[0] = 0x80;
    memset(padding + 1, 0, pad_len - 1);
    uint64_t bit_count = ctx->count * 8;
    for (int i = 0; i < 8; i++)
    {
        padding[pad_len + i] = (bit_count >> (56 - i * 8)) & 0xFF;
    }
    ha256_update(ctx, padding, pad_len + 8);
    for (int i = 0; i < 8; i++)
    {
        hash[i * 4] = (ctx->state[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}
PyObject *
c_fhash(PyObject *self, PyObject *args)
{
    Py_buffer buffer;
    PyObject *data_obj;
    if (!PyArg_ParseTuple(args, "O", &data_obj))
    {
        return NULL;
    }
    if (PyObject_GetBuffer(data_obj, &buffer, PyBUF_SIMPLE) != 0)
    {
        PyErr_SetString(PyExc_TypeError, "Argument must be bytes-like");
        return NULL;
    }
    uint32_t hash = xxhash32(buffer.buf, buffer.len);
    PyBuffer_Release(&buffer);

    return PyLong_FromUnsignedLong(hash);
}
PyObject *
c_shash(PyObject *self, PyObject *args)
{
    Py_buffer buffer;
    PyObject *data_obj;
    if (!PyArg_ParseTuple(args, "O", &data_obj))
    {
        return NULL;
    }
    if (PyObject_GetBuffer(data_obj, &buffer, PyBUF_SIMPLE) != 0)
    {
        PyErr_SetString(PyExc_TypeError, "Argument must be bytes-like");
        return NULL;
    }
    HA256_CTX ctx;
    uint8_t hash[HA256_HASH_SIZE];
    ha256_init(&ctx);
    ha256_update(&ctx, buffer.buf, buffer.len);
    ha256_final(&ctx, hash);
    PyBuffer_Release(&buffer);
    return PyBytes_FromStringAndSize((const char *)hash, HA256_HASH_SIZE);
}
PyObject *
c_fhash_file(PyObject *self, PyObject *args)
{
    const char *filename;
    FILE *fp;
    uint8_t buffer[8192];
    size_t bytes_read;
    uint32_t hash = XXH_PRIME32_5;
    if (!PyArg_ParseTuple(args, "s", &filename))
    {
        return NULL;
    }
    fp = fopen(filename, "rb");
    if (!fp)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_FileNotFoundError, filename);
        return NULL;
    }
    size_t total_len = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        total_len += bytes_read;
        for (size_t i = 0; i < bytes_read; i++)
        {
            hash += buffer[i] * XXH_PRIME32_5;
            hash = xxh_rotl32(hash, 11) * XXH_PRIME32_1;
        }
    }
    fclose(fp);
    hash ^= hash >> 15;
    hash *= XXH_PRIME32_2;
    hash ^= hash >> 13;
    hash *= XXH_PRIME32_3;
    hash ^= hash >> 16;
    return PyLong_FromUnsignedLong(hash);
}

PyObject *
c_shash_file(PyObject *self, PyObject *args)
{
    const char *filename;
    FILE *fp;
    uint8_t buffer[8192];
    size_t bytes_read;
    HA256_CTX ctx;
    uint8_t hash[HA256_HASH_SIZE];
    if (!PyArg_ParseTuple(args, "s", &filename))
    {
        return NULL;
    }
    fp = fopen(filename, "rb");
    if (!fp)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_FileNotFoundError, filename);
        return NULL;
    }
    ha256_init(&ctx);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        ha256_update(&ctx, buffer, bytes_read);
    }
    fclose(fp);
    ha256_final(&ctx, hash);
    return PyBytes_FromStringAndSize((const char *)hash, HA256_HASH_SIZE);
}

// end