#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mm.h>

static int mm_get_offset(struct memory *mem, uint32_t addr)
{
	int offset = 0;

	if (addr)
		offset = addr - mem->base_addr;

	return offset;
}

int mm_create_mapping(struct memory *mem, uint32_t base_addr, int size, int type, int attr)
{
	int ret = 0;

	mem->mem = malloc(size);
	if (!mem->mem) {
		ret = -ENOMEM;
		goto err;
	}

	mem->base_addr = base_addr;
	mem->size = size;
	mem->type = type;
	mem->attr = attr;

err:
	return ret;
}

int mm_read(struct memory *mem, uint32_t addr)
{
	int offset = mm_get_offset(mem, addr);

	return *(uint32_t *)(mem->mem + offset);
}

void mm_write(struct memory *mem, uint32_t addr, int value)
{
	int offset = mm_get_offset(mem, addr);

	*(uint32_t *)(mem->mem + offset) = value;
}

int mm_read_u16(struct memory *mem, uint32_t addr)
{
	int offset = mm_get_offset(mem, addr);

	return *(uint16_t *)(mem->mem + offset);
}

void mm_write_u16(struct memory *mem, uint32_t addr, int value)
{
	int offset = mm_get_offset(mem, addr);

	*(uint16_t *)(mem->mem + offset) = value;
}

int mm_read_u8(struct memory *mem, uint32_t addr)
{
	int offset = mm_get_offset(mem, addr);

	return *(uint8_t *)(mem->mem + offset);
}

void mm_write_u8(struct memory *mem, uint32_t addr, int value)
{
	int offset = mm_get_offset(mem, addr);

	*(uint8_t *)(mem->mem + offset) = value;
}

int mm_read_s8(struct memory *mem, uint32_t addr)
{
	int offset = mm_get_offset(mem, addr);

	return *(int8_t *)(mem->mem + offset);
}

int mm_read_s16(struct memory *mem, uint32_t addr)
{
	int offset = mm_get_offset(mem, addr);

	return *(int16_t *)(mem->mem + offset);
}

int mm_read_s32(struct memory *mem, uint32_t addr)
{
	int offset = mm_get_offset(mem, addr);

	return *(int32_t *)(mem->mem + offset);
}
