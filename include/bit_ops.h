#ifndef BIT_OPS_H
#define BIT_OPS_H

#include <stdint.h>

static inline int32_t sign_extend(uint32_t val, uint8_t bits)
{
	return ((int32_t)(val << (32 - bits))) >> (32 - bits);
}

static inline uint32_t bit_mask(uint8_t count)
{
	return (1ULL << count) - 1;
}

static inline uint32_t bit_cut(uint32_t val, uint8_t pos, uint8_t bits)
{
	return (val >> pos) & bit_mask(bits);
}

static inline uint32_t bit_replace(uint32_t val, uint8_t pos, uint8_t bits, uint32_t rep)
{
	return (val & (~(bit_mask(bits) << pos))) | ((rep & bit_mask(bits)) << pos);
}

static inline uint32_t bit_check(uint32_t val, uint8_t pos)
{
	return (val >> pos) & 0x1;
}

#endif /* BIT_OPS_H */
