//-----------------------------------------------------------------------------
// From https://github.com/PeterScott/murmur3
// Modified slightly for returning the first 64 bits of the 128-bit hash value.

// MurmurHash3 was written by Austin Appleby, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

#include <stdint.h>

//-----------------------------------------------------------------------------

void MurmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);

uint64_t MurmurHash3_x64_128(const void *key, int len, uint32_t seed);

//-----------------------------------------------------------------------------

#endif // _MURMURHASH3_H_
