#include <stdint.h>
#include <stdio.h>
#include <vm.h>
#include <inst.h>
#include <bit_ops.h>

static void (*inst_opcodes[512])(struct vm *vm, uint32_t inst);
static void (*inst_pseudo_opcodes[32])(struct vm *vm, uint32_t inst);

static void inst_install_opcode(void (*func)(struct vm *, uint32_t), int opcode)
{
	inst_opcodes[opcode] = func;
}

static void inst_install_pseudo_opcode(void (*func)(struct vm *, uint32_t), int opcode)
{
	inst_pseudo_opcodes[opcode] = func;
}

static inline int decode_jal_imm(const uint32_t instruction)
{
	uint32_t imm = (bit_cut(instruction, 31, 1) << 20) |
			(bit_cut(instruction, 12, 8) << 12) |
			(bit_cut(instruction, 20, 1) << 11) |
			(bit_cut(instruction, 21, 10) << 1);

	return sign_extend(imm, 21);
}

static inline int decode_branch_imm(uint32_t instruction)
{
	uint32_t imm = (bit_cut(instruction, 31, 1) << 12) |
			(bit_cut(instruction, 7, 1)  << 11) |
			(bit_cut(instruction, 25, 6) << 5)  |
			(bit_cut(instruction, 8, 4)  << 1);

	return sign_extend(imm, 13);
}

static void inst_lui(struct vm *vm, uint32_t inst)
{
	printf("LUI\n");

	int rds = bit_cut(inst, 7, 5);
	int imm = sign_extend(inst & 0xFFFFF000, 32);

	vm_write_register(vm, rds, imm);
}

static void inst_auipc(struct vm *vm, uint32_t inst)
{
	printf("AUPIC\n");

	int rds = bit_cut(inst, 7, 5);
	int imm = sign_extend(inst & 0xFFFFF000, 32);
	uint32_t pc = vm_read_pc(vm);

	vm_write_register(vm, rds, pc + imm);
}

static void inst_jal(struct vm *vm, uint32_t inst)
{
	printf("JAL\n");

	int rds = bit_cut(inst, 7, 5);
	int offset = decode_jal_imm(inst);
	uint32_t pc = vm_read_pc(vm);

	vm_write_register(vm, rds, pc + 4);
	vm_write_pc(vm, pc + offset - 4);
}

static void inst_jalr(struct vm *vm, uint32_t inst)
{
	printf("JALR\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int offset = sign_extend(bit_cut(inst, 20, 12), 12);
	uint32_t pc = vm_read_pc(vm);
	uint32_t jmp_addr = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, pc + 4);
	vm_write_pc(vm, ((jmp_addr + offset) & (~(uint32_t)1)) - 4);
}

static void inst_beq(struct vm *vm, uint32_t inst)
{
	printf("BEQ\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = decode_branch_imm(inst);

	if (vm_read_register(vm, rs1) == vm_read_register(vm, rs2))
	{
		uint32_t pc = vm_read_pc(vm);

		vm_write_pc(vm, pc + offset - 4);
	}
}

static void inst_bne(struct vm *vm, uint32_t inst)
{
	printf("BNE\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = decode_branch_imm(inst);

	if (vm_read_register(vm, rs1) != vm_read_register(vm, rs2))
	{
		uint32_t pc = vm_read_pc(vm);

		vm_write_pc(vm, pc + offset - 4);
	}
}

static void inst_blt(struct vm *vm, uint32_t inst)
{
	printf("BLT\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = decode_branch_imm(inst);

	if (vm_read_register(vm, rs1) < vm_read_register(vm, rs2))
	{
		uint32_t pc = vm_read_pc(vm);

		vm_write_pc(vm, pc + offset - 4);
	}
}

static void inst_bge(struct vm *vm, uint32_t inst)
{
	printf("BGE\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = decode_branch_imm(inst);

	if (vm_read_register(vm, rs1) >= vm_read_register(vm, rs2))
	{
		uint32_t pc = vm_read_pc(vm);

		vm_write_pc(vm, pc + offset - 4);
	}
}

static void inst_bltu(struct vm *vm, uint32_t inst)
{
	printf("BLTU\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = decode_branch_imm(inst);

	if ((uint32_t)vm_read_register(vm, rs1) < (uint32_t)vm_read_register(vm, rs2))
	{
		uint32_t pc = vm_read_pc(vm);

		vm_write_pc(vm, pc + offset - 4);
	}
}

static void inst_bgeu(struct vm *vm, uint32_t inst)
{
	printf("BGEU\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = decode_branch_imm(inst);

	if ((uint32_t)vm_read_register(vm, rs1) >= (uint32_t)vm_read_register(vm, rs2))
	{
		uint32_t pc = vm_read_pc(vm);

		vm_write_pc(vm, pc + offset - 4);
	}
}

static void inst_lb(struct vm *vm, uint32_t inst)
{
	printf("LB\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int offset = sign_extend(bit_cut(inst, 20, 12), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_load_s8(vm, addr, rds);
}

static void inst_lh(struct vm *vm, uint32_t inst)
{
	printf("LH\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int offset = sign_extend(bit_cut(inst, 20, 12), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_load_s16(vm, addr, rds);
}

static void inst_lw(struct vm *vm, uint32_t inst)
{
	printf("LW\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int offset = sign_extend(bit_cut(inst, 20, 12), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_load_s32(vm, addr, rds);
}

static void inst_lbu(struct vm *vm, uint32_t inst)
{
	printf("LBU\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int offset = sign_extend(bit_cut(inst, 20, 12), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_load_u8(vm, addr, rds);
}

static void inst_lhu(struct vm *vm, uint32_t inst)
{
	printf("LHU\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int offset = sign_extend(bit_cut(inst, 20, 12), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_load_u16(vm, addr, rds);
}

static void inst_sb(struct vm *vm, uint32_t inst)
{
	printf("SB\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = sign_extend(bit_cut(inst, 7, 5) | (bit_cut(inst, 25, 7) << 5), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_store_u8(vm, addr, rs2);
}

static void inst_sh(struct vm *vm, uint32_t inst)
{
	printf("SH\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = sign_extend(bit_cut(inst, 7, 5) | (bit_cut(inst, 25, 7) << 5), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_store_u16(vm, addr, rs2);
}

static void inst_sw(struct vm *vm, uint32_t inst)
{
	printf("SW\n");

	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);
	int offset = sign_extend(bit_cut(inst, 7, 5) | (bit_cut(inst, 25, 7) << 5), 12);
	int addr = vm_read_register(vm, rs1) + offset;

	vm_store(vm, addr, rs2);
}

static void inst_addi(struct vm *vm, uint32_t inst)
{
	printf("ADDI\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int imm = sign_extend(bit_cut(inst, 20, 12), 12);

	int tmp = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, tmp + imm);
}

static void inst_slti(struct vm *vm, uint32_t inst)
{
	printf("SLTI\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int imm = sign_extend(bit_cut(inst, 20, 12), 12);

	int tmp = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, (tmp < imm) ? 1 : 0);
}

static void inst_sltiu(struct vm *vm, uint32_t inst)
{
	printf("SLTIU\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	uint32_t imm = sign_extend(bit_cut(inst, 20, 12), 12);

	uint32_t tmp = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, (tmp < imm) ? 1 : 0);

}

static void inst_xori(struct vm *vm, uint32_t inst)
{
	printf("XORI\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int imm = sign_extend(bit_cut(inst, 20, 12), 12);

	int tmp = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, tmp ^ imm);

}

static void inst_ori(struct vm *vm, uint32_t inst)
{
	printf("ORI\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int imm = sign_extend(bit_cut(inst, 20, 12), 12);

	int tmp = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, tmp | imm);

}

static void inst_andi(struct vm *vm, uint32_t inst)
{
	printf("ANDI\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int imm = sign_extend(bit_cut(inst, 20, 12), 12);

	int tmp = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, tmp & imm);

}

static void inst_slli(struct vm *vm, uint32_t inst)
{
	printf("SLLI\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int shamt = bit_cut(inst, 20, 5);

	int tmp = vm_read_register(vm, rs1);

	vm_write_register(vm, rds, tmp << shamt);
}

static void inst_srli_srai(struct vm *vm, uint32_t inst)
{
	int funct7;

	funct7 = (inst >> 25) & 0xFF;

	if (funct7 == 0x20) {
		printf("SRAI\n");
	} else {
		printf("SRLI\n");
	}

}

static void inst_add_sub(struct vm *vm, uint32_t inst)
{
	int funct7;

	funct7 = (inst >> 25) & 0xFF;

	if (funct7 == 0x20) {
		printf("SUB\n");
	} else {
		printf("ADD\n");
	}
}

static void inst_sll(struct vm *vm, uint32_t inst)
{
	printf("SLL\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);

	int tmp = vm_read_register(vm, rs1);
	int tmp2 = vm_read_register(vm, rs2);

	vm_write_register(vm, rds, tmp << (tmp2 & bit_mask(5)));

}

static void inst_slt(struct vm *vm, uint32_t inst)
{
	printf("SLT\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);

	int tmp = vm_read_register(vm, rs1);
	int tmp2 = vm_read_register(vm, rs2);

	vm_write_register(vm, rds, (tmp < tmp2) ? 1 : 0);
}

static void inst_sltu(struct vm *vm, uint32_t inst)
{
	printf("SLTU\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);

	uint32_t tmp = vm_read_register(vm, rs1);
	uint32_t tmp2 = vm_read_register(vm, rs2);

	vm_write_register(vm, rds, (tmp < tmp2) ? 1 : 0);
}

static void inst_xor(struct vm *vm, uint32_t inst)
{
	printf("XOR\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);

	uint32_t tmp = vm_read_register(vm, rs1);
	uint32_t tmp2 = vm_read_register(vm, rs2);

	vm_write_register(vm, rds, tmp ^ tmp2);
}

static void inst_srl_sra(struct vm *vm, uint32_t inst)
{
	int funct7;

	funct7 = (inst >> 25) & 0xFF;

	if (funct7 == 0x20) {
		printf("SRA\n");
	} else {
		printf("SRL\n");
	}
}

static void inst_or(struct vm *vm, uint32_t inst)
{
	printf("OR\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);

	uint32_t tmp = vm_read_register(vm, rs1);
	uint32_t tmp2 = vm_read_register(vm, rs2);

	vm_write_register(vm, rds, tmp | tmp2);
}

static void inst_and(struct vm *vm, uint32_t inst)
{
	printf("AND\n");

	int rds = bit_cut(inst, 7, 5);
	int rs1 = bit_cut(inst, 15, 5);
	int rs2 = bit_cut(inst, 20, 5);

	uint32_t tmp = vm_read_register(vm, rs1);
	uint32_t tmp2 = vm_read_register(vm, rs2);

	vm_write_register(vm, rds, tmp & tmp2);
}

static void inst_fence(struct vm *vm, uint32_t inst)
{
	printf("FENCE\n");

}

static void inst_ecall_ebreak(struct vm *vm, uint32_t inst)
{
	printf("ECALL EBREAK\n");

}

#if 0
static void inst_lwu(struct vm *vm, uint32_t inst)
{
	printf("LWU\n");

}

static void inst_ld(struct vm *vm, uint32_t inst)
{
	printf("LD\n");

}

static void inst_sd(struct vm *vm, uint32_t inst)
{
	printf("SD\n");

}

static void inst_addiw(struct vm *vm, uint32_t inst)
{
	printf("ADDIW\n");

}

static void inst_slliw(struct vm *vm, uint32_t inst)
{
	printf("SLLIW\n");

}

static void inst_srliw_sraiw(struct vm *vm, uint32_t inst)
{
	int funct7;

	funct7 = (inst >> 25) & 0xFF;

	if (funct7 == 0x20) {
		printf("SRAIW\n");
	} else {
		printf("SRLIW\n");
	}
}

static void inst_addw_subw(struct vm *vm, uint32_t inst)
{
	int funct7;

	funct7 = (inst >> 25) & 0xFF;

	if (funct7 == 0x20) {
		printf("SUBW\n");
	} else {
		printf("ADDW\n");
	}
}

static void inst_sllw(struct vm *vm, uint32_t inst)
{
	printf("SLLW\n");

}

static void inst_srlw_sraw(struct vm *vm, uint32_t inst)
{
	int funct7;

	funct7 = (inst >> 25) & 0xFF;

	if (funct7 == 0x20) {
		printf("SRAW\n");
	} else {
		printf("SRLW\n");
	}

}

static void inst_pseudo_li(struct vm *vm, uint32_t inst)
{
	printf("PSEUDO LI\n");
}

static void inst_pseudo_addi(struct vm *vm, uint32_t inst)
{
	printf("PSEUDO ADDI\n");
}
#endif

void inst_execute(struct vm *vm, int inst)
{
	int opcode;
	int funct3;

#if 0
	if ((inst & RV_OPCODE_MASK) != RV_OPCODE_MASK) {
		inst_pseudo_opcodes[opcode](vm, inst);
	}
#endif

	switch (inst & RV_OPCODE_MASK) {
	case RV32_LOAD_I_TYPE:
			funct3 = (inst >> 12) & 0x7;
			opcode = (funct3 << 5) | ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_FENCE:
			opcode = ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_ECALL_EBREAK:
			opcode = ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_LOGIC_I_TYPE:
			funct3 = (inst >> 12) & 0x7;
			opcode = (funct3 << 5) | ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_AUPIC:
			opcode = ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
#if 0
	case RV64_LOGIC_I_TYPE:
			funct3 = (inst >> 12) & 0x7;
			opcode = (funct3 << 5) | ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
#endif
	case RV32_STORE_S_TYPE:
			funct3 = (inst >> 12) & 0x7;
			opcode = (funct3 << 5) | ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_LOGIC_R_TYPE:
			funct3 = (inst >> 12) & 0x7;
			opcode = (funct3 << 5) | ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_LUI:
			opcode = ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
#if 0
	case RV64_LOGIC_R_TYPE:
			funct3 = (inst >> 12) & 0x7;
			opcode = (funct3 << 5) | ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
#endif
	case RV32_BRANCH_B_TYPE:
			opcode = ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_JALR:
			opcode = ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	case RV32_JAL:
			opcode = ((inst & RV_OPCODE_MASK) >> 2);

			inst_opcodes[opcode](vm, inst);
			break;
	}
}

void inst_init(struct vm *vm)
{
	inst_install_opcode(inst_lui, RV32I_LUI);
	inst_install_opcode(inst_auipc, RV32I_AUIPC);
	inst_install_opcode(inst_jal, RV32I_JAL);
	inst_install_opcode(inst_slli, RV32I_SLLI);
	inst_install_opcode(inst_srli_srai, RV32I_SRLI_SRAI);
	inst_install_opcode(inst_add_sub, RV32I_ADD_SUB);
	inst_install_opcode(inst_sll, RV32I_SLL);
	inst_install_opcode(inst_slt, RV32I_SLT);
	inst_install_opcode(inst_sltu, RV32I_SLTU);
	inst_install_opcode(inst_xor, RV32I_XOR);
	inst_install_opcode(inst_srl_sra, RV32I_SRL_SRA);
	inst_install_opcode(inst_or, RV32I_OR);
	inst_install_opcode(inst_and, RV32I_AND);
	inst_install_opcode(inst_jalr, RV32I_JALR);
	inst_install_opcode(inst_beq, RV32I_BEQ);
	inst_install_opcode(inst_bne, RV32I_BNE);
	inst_install_opcode(inst_blt, RV32I_BLT);
	inst_install_opcode(inst_bge, RV32I_BGE);
	inst_install_opcode(inst_bltu, RV32I_BLTU);
	inst_install_opcode(inst_bgeu, RV32I_BGEU);
	inst_install_opcode(inst_lb, RV32I_LB);
	inst_install_opcode(inst_lh, RV32I_LH);
	inst_install_opcode(inst_lw, RV32I_LW);
	inst_install_opcode(inst_lbu, RV32I_LBU);
	inst_install_opcode(inst_lhu, RV32I_LHU);
	inst_install_opcode(inst_sb, RV32I_SB);
	inst_install_opcode(inst_sh, RV32I_SH);
	inst_install_opcode(inst_sw, RV32I_SW);
	inst_install_opcode(inst_addi, RV32I_ADDI);
	inst_install_opcode(inst_slti, RV32I_SLTI);
	inst_install_opcode(inst_sltiu, RV32I_SLTIU);
	inst_install_opcode(inst_xori, RV32I_XORI);
	inst_install_opcode(inst_ori, RV32I_ORI);
	inst_install_opcode(inst_andi, RV32I_ANDI);
#if 0
	inst_install_opcode(inst_addiw, RV64I_ADDIW);
	inst_install_opcode(inst_slliw, RV64I_SLLIW);
	inst_install_opcode(inst_srliw_sraiw, RV64I_SRLIW_SRAIW);
	inst_install_opcode(inst_addw_subw, RV64I_ADDW_SUBW);
	inst_install_opcode(inst_sllw, RV64I_SLLW);
	inst_install_opcode(inst_srlw_sraw, RV64I_SRLW_SRAW);
	inst_install_opcode(inst_lwu, RV64I_LWU);
	inst_install_opcode(inst_ld, RV64I_LD);
	inst_install_opcode(inst_sd, RV64I_SD);
#endif
	inst_install_opcode(inst_ecall_ebreak, RV32I_ECALL_EBREAK);
	inst_install_opcode(inst_fence, RV32I_FENCE);

#if 0
	inst_install_pseudo_opcode(inst_pseudo_addi, RVC_ADDI);
	inst_install_pseudo_opcode(inst_pseudo_li, RVC_LI);
#endif
}
