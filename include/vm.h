#ifndef VM_H
#define VM_H

#include <cpu.h>
#include <mm.h>

struct vm {
	struct cpu cpu;
	struct memory rom;
	struct memory ram;
};

struct vm *vm_init(uint32_t entry_point, int rom_size, int ram_size);
int vm_load_bin(struct vm *vm, void *bin, int size);
void vm_run(struct vm *vm);
void vm_dump_registers(struct vm *vm);
void vm_dump_rom(struct vm *vm, int size);

int vm_read_pc(struct vm *vm);
void vm_write_pc(struct vm *vm, uint32_t pc);
int vm_read_register(struct vm *vm, int reg);
void vm_write_register(struct vm *vm, int reg, int value);

void vm_load(struct vm *vm, int addr, int reg);
void vm_store(struct vm *vm, int addr, int reg);
void vm_load_u16(struct vm *vm, int addr, int reg);
void vm_store_u16(struct vm *vm, int addr, int reg);
void vm_load_u8(struct vm *vm, int addr, int reg);
void vm_store_u8(struct vm *vm, int addr, int reg);

void vm_load_s32(struct vm *vm, int addr, int reg);
void vm_store_s32(struct vm *vm, int addr, int reg);
void vm_load_s16(struct vm *vm, int addr, int reg);
void vm_store_s16(struct vm *vm, int addr, int reg);
void vm_load_s8(struct vm *vm, int addr, int reg);
void vm_store_s8(struct vm *vm, int addr, int reg);


#endif /* VM_H */
