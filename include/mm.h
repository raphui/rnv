#ifndef MM_H
#define MM_H

#include <stdint.h>

enum {
	ROM,
	RAM,
};

enum {
	RO = (1 << 0),
	RW = (1 << 1),
	XN = (1 << 2),
};

struct memory {
	void *mem;
	uint32_t base_addr;
	int size;
	int type;
	int attr;
};

int mm_create_mapping(struct memory *mem, uint32_t base_addr, int size, int type, int attr);
int mm_read(struct memory *mem, uint32_t addr);
void mm_write(struct memory *mem, uint32_t addr, int value);
int mm_read_u16(struct memory *mem, uint32_t addr);
void mm_write_u16(struct memory *mem, uint32_t addr, int value);
int mm_read_u8(struct memory *mem, uint32_t addr);
void mm_write_u8(struct memory *mem, uint32_t addr, int value);

int mm_read_s8(struct memory *mem, uint32_t addr);
int mm_read_s16(struct memory *mem, uint32_t addr);
int mm_read_s32(struct memory *mem, uint32_t addr);
#endif /* MM_H */
