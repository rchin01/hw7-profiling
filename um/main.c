/*******************************************************
 *
 *      main.c
 *      by Greg Pickart and Eli Rosmarin
 *      November 17, 2017
 *
 *      COMP 40 Fall 2017 - HW6 'UM'
 *      main.c contains the driver for the UM virtual machine. 
 *
 *      The UM is invoked from the command line using the command:
 *      ./um [program.um]
 *
 *      Uses the UM module to represent the virtual machine. Improper 
 *      usage from the command line results in graceful termination. 
 *
 *******************************************************/

#include <stdlib.h>
#include "Um.h"
#include <stdio.h>
#include <um-dis.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
        (void) argc;
        /* check command line arguments */
        UM um = UM_new((char *) argv[1]);
        UM_run(um);
        UM_free(um);
        return 0;
} 