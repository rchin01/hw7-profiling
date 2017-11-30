/*******************************************************
 *
 *      Um.h
 *      by Greg Pickart and Eli Rosmarin
 *      November 17, 2017
 *
 *      COMP 40 Fall 2017 - HW6 'UM'
 *      Um.c contains the interface of the UM module. The UM 
 *      represents a simple virtual machine, with registers, memory, and 
 *      simple input and output. Implementation of the Um module is 
 *      protected from the interface and is defined in Um.c.
 *
 *******************************************************/

#ifndef UM_H
#define UM_H

//#include "Um_segments.h"
//#include "Um_registers.h"
#include "Um_instructions.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct UM *UM;

UM UM_new(char *program);
void UM_free(UM um);
void UM_run(UM um);

#endif