#ifndef CPU_H
#define CPU_H

#include <stdint.h>

struct registers {
	uint32_t zero;	/* Hard-wireed zero */
	uint32_t ra;	/* Return address */	
	uint32_t sp;	/* Stack pointer */
	uint32_t gp;	/* Global pointer */
	uint32_t tp;	/* Thread pointer  */
	uint32_t t0;	/* Temporary/alternate link register */
	uint32_t t1;	/* Temporary register */
	uint32_t t2;	/* Temporary register */
	uint32_t s0;	/* Saved register/frame pointer */
	uint32_t s1;	/* Saved register */
	uint32_t a0;	/* Function arguments/return values */
	uint32_t a1;	/* Function arguments/return values */
	uint32_t a2;	/* Function arguments */
	uint32_t a3;	/* Function arguments */
	uint32_t a4;	/* Function arguments */
	uint32_t a5;	/* Function arguments */
	uint32_t a6;	/* Function arguments */
	uint32_t a7;	/* Function arguments */
	uint32_t s2;	/* Saved register */
	uint32_t s3;	/* Saved register */
	uint32_t s4;	/* Saved register */
	uint32_t s5;	/* Saved register */
	uint32_t s6;	/* Saved register */
	uint32_t s7;	/* Saved register */
	uint32_t s8;	/* Saved register */
	uint32_t s9;	/* Saved register */
	uint32_t s10;	/* Saved register */
	uint32_t s11;	/* Saved register */
	uint32_t t3;	/* Temporary */
	uint32_t t4;	/* Temporary */
	uint32_t t5;	/* Temporary */
	uint32_t t6;	/* Temporary */
};

struct cpu {
	struct registers regs;
	uint32_t pc;	/* Program counter */
};

#endif /* CPU_H */
