#ifndef INST_H
#define INST_H

#include <stdint.h>
#include <vm.h>

/*
 * RV32I Base ISA
 */

// U/J type instructions
#define RV32I_LUI          0xD
#define RV32I_AUIPC        0x5
#define RV32I_JAL          0x1B
// R-type instructions
#define RV32I_SLLI         0x24
#define RV32I_SRLI_SRAI    0xA4
#define RV32I_ADD_SUB      0xC
#define RV32I_SLL          0x2C
#define RV32I_SLT          0x4C
#define RV32I_SLTU         0x6C
#define RV32I_XOR          0x8C
#define RV32I_SRL_SRA      0xAC
#define RV32I_OR           0xCC
#define RV32I_AND          0xEC
// I/S/B type instructions
#define RV32I_JALR         0x19
#define RV32I_BEQ          0x18
#define RV32I_BNE          0x38
#define RV32I_BLT          0x98
#define RV32I_BGE          0xB8
#define RV32I_BLTU         0xD8
#define RV32I_BGEU         0xF8
#define RV32I_LB           0x0
#define RV32I_LH           0x20
#define RV32I_LW           0x40
#define RV32I_LBU          0x80
#define RV32I_LHU          0xA0
#define RV32I_SB           0x8
#define RV32I_SH           0x28
#define RV32I_SW           0x48
#define RV32I_ADDI         0x4
#define RV32I_SLTI         0x44
#define RV32I_SLTIU        0x64
#define RV32I_XORI         0x84
#define RV32I_ORI          0xC4
#define RV32I_ANDI         0xE4

#define RV32I_ECALL_EBREAK 0x1C
#define RV32I_FENCE	   0x3
/*
 * RV64I-only instructions
 */

// R-type instructions
#define RV64I_ADDIW       0x6
#define RV64I_SLLIW       0x26
#define RV64I_SRLIW_SRAIW 0xA6
#define RV64I_ADDW_SUBW   0xE
#define RV64I_SLLW        0x2E
#define RV64I_SRLW_SRAW   0xAE
// I/S/B type instructions
#define RV64I_LWU         0xC0
#define RV64I_LD          0x60
#define RV64I_SD          0x68


/*
 * RVC Compressed instructions
 */

/*  opcode 0 */
#define RVC_ADDI4SPN     0x0
#define RVC_FLD          0x4
#define RVC_LW           0x8
#define RVC_FLW          0xC
#define RVC_RESERVED1    0x10
#define RVC_FSD          0x14
#define RVC_SW           0x18
#define RVC_FSW          0x1C
/*  opcode 1 */
#define RVC_ADDI         0x1  // this is also NOP when rs/rd == 0
#define RVC_JAL          0x5  // only exists on RV32!
#define RVC_LI           0x9
#define RVC_ADDI16SP_LUI 0xD  // this is ADDI16SP when rd == 2 or LUI (rd!=0)
#define RVC_ALOPS1       0x11 // a lot of operations packed tightly, idk about performance
#define RVC_J            0x15
#define RVC_BEQZ         0x19
#define RVC_BNEZ         0x1D
/*  opcode 2 */
#define RVC_SLLI         0x2
#define RVC_FLDSP        0x6
#define RVC_LWSP         0xA
#define RVC_FLWSP        0xE
#define RVC_ALOPS2       0x12 // same as RVC_ALOPS1
#define RVC_FSDSP        0x16
#define RVC_SWSP         0x1A
#define RVC_FSWSP        0x1E



#define RV32_LOAD_I_TYPE	0x3
#define RV32_FENCE		0xF
#define RV32_ECALL_EBREAK	0x73
#define RV32_LOGIC_I_TYPE	0x13
#define RV32_AUPIC		0x17
#define RV64_LOGIC_I_TYPE	0x1B
#define RV32_STORE_S_TYPE	0x23
#define RV32_LOGIC_R_TYPE	0x33
#define RV32_LUI		0x37
#define RV64_LOGIC_R_TYPE	0x3B
#define RV32_BRANCH_B_TYPE	0x63
#define RV32_JALR		0x67
#define RV32_JAL		0x6F

#define RV_OPCODE_MASK		0x7F


void inst_init(struct vm *vm);
void inst_execute(struct vm *vm, int inst);

#endif /* INST_H */
