/*******************************************************
 *
 *      Um_registers.h
 *      by Greg Pickart and Eli Rosmarin
 *      November 16, 2017
 *
 *      COMP 40 Fall 2017 - HW6 'UM'
 *      Um_registers.c contains the interface of the UM register module. 
 *      The UM register module represents the emulated multi-purpose 
 *      registers of the UM. The implementation of the Um registers is 
 *      protected from the interface and is defined in Um_registers.c
 *
 *******************************************************/

#ifndef UM_REGISTERS
#define UM_REGISTERS

#include "stdint.h"

typedef uint32_t Register;

Register *UMRegister_new();
void UMRegister_free(Register *registers);
void UMRegister_put(Register *registers, Register a, uint32_t value);
uint32_t UMRegister_get(Register *registers, Register a);
void UMRegister_move(Register *registers, Register a, Register b);
void UMRegister_add(Register *registers, Register a, Register b, Register c);
void UMRegister_mult(Register *registers, Register a, Register b, Register c);
void UMRegister_div(Register *registers, Register a, Register b, Register c);
void UMRegister_nand(Register *registers, Register a, Register b, Register c);


#endif