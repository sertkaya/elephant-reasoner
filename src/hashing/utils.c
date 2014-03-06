/*
 * utils.c
 *
 *  Created on: 07.02.2014
 */

#include <stdint.h>

/* Round up to the next highest power of 2 */
/* from http://www-graphics.stanford.edu/~seander/bithacks.html */
inline uint32_t roundup_pow2(uint32_t value) {
  --value;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  ++value;

  return value;
}
