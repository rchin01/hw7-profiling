##################################################### 
#
#	Makefile for COMP 40 HW6 'UM'
# 	Greg Pickart and Eli Rosmarin
# 
# 	Includes build rules for segment, register, and UM unit tests
# 	as well as for the overall UM program.
#
#	Also includes build rules for writetests, a simple program for 
#	generating UM unit test binaries. 
#
#####################################################


############## Variables ###############

CC = gcc # The compiler used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path
# 
CFLAGS = -g -O1 -std=c99 -Wall -Wextra -Werror -Wfatal-errors \
-pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64

# Libraries needed for linking
# All programs cii40 (Hanson binaries) and *may* need -lm (math)
# arith40 is for this assignment, netpbm is needed for pnm
LDLIBS = -lcii40-O1 -larith40 -lnetpbm -lbitpack -lm -l40locality -lum-dis -lcii 

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: um


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

um: Um_registers.o Um_segments.o Um.o main.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f um *.o