/*******************************************************
 *
 *      Um_registers.c
 *      by Greg Pickart and Eli Rosmarin
 *      November 16, 2017
 *
 *      COMP 40 Fall 2017 - HW6 'UM'
 *      Um_registers.c contains the implementation of the UM register module. 
 *      The UM register module represents the emulated multi-purpose 
 *      registers of the UM. Uses a simple C array of unsigned 32 bit
 *      integers to represent the Um registers. 
 *
 *******************************************************/

#include "Um_registers.h"
#include <stdlib.h>
#include <stdio.h>

#define NUM_REGS 8

/*******************************************************
 *
 *      PUBLIC MEMBER FUNCTIONS
 *
 *******************************************************/

/* UMRegister_new() function
 * Parameters:  none
 *
 * Returns:     Register * array
 *
 * Purpose:     Allocates a new array of registers and sets their
 *              values to 0. Returns the array.
 */
Register *UMRegister_new()
{
        Register *regs = calloc(sizeof(Register), NUM_REGS);
        return regs;
}

/* UMRegister_free() function
 * Parameters:  registers: Register * type
 *
 * Returns:     void
 *
 * Purpose:     Frees the memory used by the given register array.
 */
void UMRegister_free(Register *registers)
{
        free(registers);
}

/* UMRegister_put() function
 * Parameters:  registers: Register * type; a: Register type; value: 
 *              uint32_t type
 *
 * Returns:     void
 *
 * Purpose:     Puts the given value into the specified register
 *              in the given register array.
 */     
void UMRegister_put(Register *registers, Register a, uint32_t value)
{
        registers[a] = value;
}

/* UMRegister_get() function
 * Parameters:  registers: Register * type; a: Register type
 *
 * Returns:     Value in register array: uint32_t type
 *
 * Purpose:     Returns the value in the given register array at the 
 *              specified register.
 */
uint32_t UMRegister_get(Register *registers, Register a)
{
        return registers[a];
}

/* UMRegister_move() function
 * Parameters:  registers: Register * type; a: Register type; b: Register 
 *              type
 *
 * Returns:     void
 *
 * Purpose:     Moves the contents of register b to register a in the 
 *              given register array.
 */     
void UMRegister_move(Register *registers, Register a, Register b)
{
        registers[a] = registers[b];
}

/* UMRegister_add() function
 * Parameters:  registers: Register * type; a: Register type; b: Register 
 *              type; c: Register type
 *
 * Returns:     void
 *
 * Purpose:     Adds the values in registers b and c and places the result
 *              into register a in the given register array.
 */     
void UMRegister_add(Register *registers, Register a, Register b, Register c)
{
        registers[a] = registers[b] + registers[c];
}


/* UMRegister_mult() function
 * Parameters:  registers: Register * type; a: Register type; b: Register 
 *              type; c: Register type
 *
 * Returns:     void
 *
 * Purpose:     Multiplies the values in registers b and c and places the 
 *              result into register a in the given register array.
 */ 
void UMRegister_mult(Register *registers, Register a, Register b, Register c)
{
        registers[a] = registers[b] * registers[c];
}

/* UMRegister_div() function
 * Parameters:  registers: Register * type; a: Register type; b: Register 
 *              type; c: Register type
 *
 * Returns:     void
 *
 * Purpose:     Divides the value in registers b by the value in register c 
 *              and places the result into register a in the given 
 *              register array.
 */ 
void UMRegister_div(Register *registers, Register a, Register b, Register c)
{
        registers[a] = registers[b] / registers[c];
}

/* UMRegister_nand() function
 * Parameters:  registers: Register * type; a: Register type; b: Register 
 *              type; c: Register type
 *
 * Returns:     void
 *
 * Purpose:     NANDs the values in registers b and c and places the 
 *              result into register a in the given register array. NAND 
 *              first ANDs the two values and then NOTs the result of the AND.
 */ 
void UMRegister_nand(Register *registers, Register a, Register b, Register c)
{
        registers[a] = ~(registers[b] & registers[c]);
}
