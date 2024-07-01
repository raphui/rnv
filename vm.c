#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vm.h>
#include <inst.h>

static uint32_t vm_fetch_inst(struct vm *vm)
{
	uint32_t inst;

	inst = mm_read(&vm->rom, vm->cpu.pc);

	vm->cpu.pc += 4;

	return inst;
}

int vm_read_pc(struct vm *vm)
{
	return vm->cpu.pc;
}

void vm_write_pc(struct vm *vm, uint32_t pc)
{
	vm->cpu.pc = pc;
}

int vm_read_register(struct vm *vm, int reg)
{
	uint32_t *r = (void *)(&vm->cpu.regs) + (reg * sizeof(uint32_t));

	return (int)*r;
}

void vm_write_register(struct vm *vm, int reg, int value)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	*r = value;
}

void vm_load(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	 *r = mm_read(&vm->rom, addr);
}

void vm_store(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	mm_write(&vm->rom, addr, *r);
}

void vm_load_u16(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	 *r = mm_read_u16(&vm->rom, addr);
}

void vm_store_u16(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	mm_write_u16(&vm->rom, addr, *r);
}

void vm_load_u8(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	 *r = mm_read_u8(&vm->rom, addr);
}

void vm_store_u8(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	mm_write_u8(&vm->rom, addr, *r);
}

void vm_load_s8(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	 *r = mm_read_s8(&vm->rom, addr);
}

void vm_load_s16(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	 *r = mm_read_s16(&vm->rom, addr);
}

void vm_load_s32(struct vm *vm, int addr, int reg)
{
	uint32_t *r = (void *)&vm->cpu.regs + (reg * sizeof(uint32_t));

	 *r = mm_read_s32(&vm->rom, addr);
}

struct vm *vm_init(uint32_t entry_point, int rom_size, int ram_size)
{
	int ret;
	struct vm *vm = NULL;

	vm = malloc(sizeof(*vm));

	memset(&vm->cpu.regs, 0x0, sizeof(vm->cpu.regs));

	ret = mm_create_mapping(&vm->rom, entry_point, rom_size, ROM, RO);
	if (ret < 0) {
		goto err_free;
	}

	ret = mm_create_mapping(&vm->ram, 0x20000, ram_size, RAM, RW | XN);
	if (ret < 0) {
		goto err_free;
	}

	vm->cpu.regs.sp = vm->ram.base_addr + ram_size;
	vm->cpu.pc = 0x0;

	inst_init(vm);

	return vm;

err_free:
	free(vm);
	return NULL;
}

int vm_load_bin(struct vm *vm, void *bin, int size)
{
	int ret = 0;

	memcpy((void *)vm->rom.mem, bin, size);

	return ret;	
}

void vm_run(struct vm *vm)
{
	uint32_t inst;

	vm->cpu.pc = vm->rom.base_addr;
		
	do {
		vm_dump_registers(vm);

		inst = vm_fetch_inst(vm);

		printf("PC (0x%08x) = 0x%08x\n", (uint32_t)vm->cpu.pc - 4, inst);

		inst_execute(vm, inst);
	} while(inst && ((vm->cpu.pc > vm->rom.base_addr) && (vm->cpu.pc < (vm->rom.base_addr + vm->rom.size))));
}

void vm_dump_registers(struct vm *vm)
{
	uint32_t *regs = (uint32_t *)&vm->cpu.regs;
	int size = sizeof(vm->cpu.regs) / sizeof(vm->cpu.regs.a0);

	for(int i = 0; i < size; i++, regs++) {
		printf("x%d = 0x%x\n", i, *regs);
	}
}

void vm_dump_rom(struct vm *vm, int size)
{
	uint32_t *p = (uint32_t *)vm->rom.mem;

	for (int i = 0; i < size; i++, p++) {
		printf("0x%lx (phys: %p) = 0x%08x\n", p - (uint32_t *)vm->rom.mem + vm->rom.base_addr, p, *p);
	}
}
