/*******************************************************
 *
 *      Um.c
 *      by Greg Pickart and Eli Rosmarin
 *      November 17, 2017
 *
 *      COMP 40 Fall 2017 - HW6 'UM'
 *      Um.c contains the implementation of the UM module. The UM 
 *      represents a simple virtual machine, with registers, memory, and 
 *      simple input and output. Uses the Um_registers and Um_segments
 *      modules to represent the registers and memory, respectively. 
 *
 *******************************************************/

#include "Um.h"
#include <sys/stat.h>
#include <um-dis.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/*******************************************************
 *
 *      TYPEDEFS, CONSTANT DEFINITIONS, AND STRUCT DEFINITIONS
 *
 *******************************************************/

#define THREE_BYTES 24
#define ONE_BYTE 8
#define LOW_ORDER_MASK 0xff
#define HIGH_ORDER_MASK 0xff000000
#define MID_HIGH_ORDER_MASK 0xff0000
#define MID_LOW_ORDER_MASK 0xff00


#define CODE_SEG 0
#define OP_WIDTH 4
#define REG_WIDTH 3
#define LV_WIDTH 25

#define A_LSB 6
#define A_LV_LSB 25
#define LV_LSB 0
#define B_LSB 3
#define C_LSB 0
#define OP_LSB 28

#define EOF_FLAG ~0

typedef uint32_t Um_instruction;

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

typedef struct Instructions {
        Um_opcode op;
        Um_register ra;
        Um_register rb;
        Um_register rc;
        Um_register lv_ra;
        Word lv_val;
} Instructions;

struct Segments {
        Seq_T available_IDs;
        Seq_T seg_array; 
};

struct UM {
        Register *registers;
        Segments segments;
        uint32_t counter;     
};

/*******************************************************
 *
 *      PRIVATE HELPER FUNCTIONS
 *
 *******************************************************/

/* swap_endian() function
 * Parameters:  word: uint32_t type
 *
 * Returns:     Word with swapped endian: uint32_t type
 *
 * Purpose:     Swaps the endian-ness of the given uint32_t and returns 
 *              the result. 
 */     
static inline uint32_t swap_endian(uint32_t word)
{
        return ((word >> THREE_BYTES) & LOW_ORDER_MASK) |
                ((word << ONE_BYTE) & MID_HIGH_ORDER_MASK) |
                ((word >> ONE_BYTE) & MID_LOW_ORDER_MASK) |
                ((word << THREE_BYTES) & HIGH_ORDER_MASK);
}

/* init_instructions() function
 * Parameters:  none
 *
 * Returns:     Initialized Instructions struct
 *
 * Purpose:     Creates a new Instructions struct and initializes all of 
 *              its members to 0. Returns the struct.
 */
static inline Instructions init_instructions()
{
        Instructions instr;
        instr.op = 0;
        instr.ra = 0;
        instr.rb = 0;
        instr.rc = 0;
        instr.lv_ra = 0;
        instr.lv_val = 0;
        return instr;
}

/* read_program() function
 * Parameters:  um: UM type; program: char * type
 *
 * Returns:     void
 *
 * Purpose:     Reads the um program in the given file and initializes 
 *              Segment O in the given UM to store the UM instructions
 *              read from the program.
 */
static inline void read_program(UM um, char *program)
{
        struct stat buffer;
        FILE *fp;
        size_t instr_size = sizeof(Um_instruction);
        int num_instr, i;

        stat(program, &buffer);
        fp = fopen(program, "r");
        if (fp == NULL) {
                fprintf(stderr, "Could not open file %s for reading\n", 
                        program);
                exit(EXIT_FAILURE);
        }
        num_instr = buffer.st_size / instr_size;
        Um_instruction stream[num_instr];
        UMSegment_map(um->segments, num_instr, NULL, 0);
        fread(stream, instr_size, (size_t) num_instr, fp);
        for (i = 0; i < num_instr; i++) {
                stream[i] = swap_endian(stream[i]);
                UMSegment_insert(um->segments, CODE_SEG, i, stream[i]);
        }
        fclose(fp);
}

/* unpack_instruction() function
 * Parameters:  um: UM type
 *
 * Returns:     Instructions struct
 *
 * Purpose:     Reads the instruction in the code segment of the given 
 *              UM pointed too by the UM program counter. Unpacks the 
 *              instruction fields into a new Instructions struct using the 
 *              Bitpack interface and returns the Instructions struct.
 */
static inline Instructions unpack_instruction(UM um)
{
        Instructions new_instr = init_instructions();
	Segments segments = um->segments;
	UArray_T curr_segment = Seq_get((segments->seg_array), CODE_SEG);
        Word *word = (uint32_t *)(curr_segment->elems + (um->counter * curr_segment->size));
        Um_instruction raw_instr = *word;
	  //UMSegment_at(um->segments, CODE_SEG, um->counter);
        unsigned hi = OP_LSB + OP_WIDTH;
        new_instr.op = ((raw_instr << (32 - hi)) >> (32 - OP_WIDTH));

        if (new_instr.op == HALT)
                return new_instr;

        if (new_instr.op == LV) {
                hi = A_LV_LSB + REG_WIDTH;
                new_instr.lv_ra = ((raw_instr << (32 - hi)) >> (32 - REG_WIDTH));
                hi = LV_LSB + LV_WIDTH;
                new_instr.lv_val = ((raw_instr << (32 - hi)) >> (32 - LV_WIDTH));

        } else if (new_instr.op == LOADP || new_instr.op == MAP) {
                hi = B_LSB + REG_WIDTH;
                new_instr.rb = ((raw_instr << (32 - hi)) >> (32 - REG_WIDTH));

                hi = C_LSB + REG_WIDTH;
                new_instr.rc = ((raw_instr << (32 - hi)) >> (32 - REG_WIDTH));

        } else if (new_instr.op == UNMAP || new_instr.op == OUT || new_instr.op == IN) {
                hi = C_LSB + REG_WIDTH;
                new_instr.rc = ((raw_instr << (32 - hi)) >> (32 - REG_WIDTH));

        } else {
                hi = A_LSB + REG_WIDTH;
                new_instr.ra = ((raw_instr << (32 - hi)) >> (32 - REG_WIDTH));
                
                hi = B_LSB + REG_WIDTH;
                new_instr.rb = ((raw_instr << (32 - hi)) >> (32 - REG_WIDTH));
                
                hi = C_LSB + REG_WIDTH;
                new_instr.rc = ((raw_instr << (32 - hi)) >> (32 - REG_WIDTH));
        } 

        return new_instr;
}

/* UM_execute() function
 * Parameters:  um: UM type; instr: Instructions type
 *
 * Returns:     void
 *
 * Purpose:     Executes the UM instruction detailed by the fields of the 
 *              given Instructions struct on the given UM. 
 */
static inline void UM_execute(UM um, Instructions instr)
{
        char in;
        Word load_word;
        Um_register ra = instr.ra, rb = instr.rb, rc = instr.rc;
        Um_register lv_ra = instr.lv_ra;
        Word lv_val = instr.lv_val;
        Word a_val = um->registers[ra];
                //UMRegister_get(um->registers, ra);
        Word b_val = um->registers[rb];
                //UMRegister_get(um->registers, rb);
        Word c_val = um->registers[rc];
                //UMRegister_get(um->registers, rc);

        Word *a_valp = &(um->registers[ra]);
        Word *b_valp = &(um->registers[rb]);
        Word *c_valp = &(um->registers[rc]);

        Segments segments = um->segments;
        UArray_T curr_segment;
        Word *word;
        UArray_T src_segment, dest_segment;

        switch (instr.op) {
                case CMOV:
                        if (c_val == 0)
                                return;
                        *a_valp = *b_valp;
                        //UMRegister_move(um->registers, ra, rb);
                        break;
                case SLOAD:
                        curr_segment = Seq_get((segments->seg_array), b_val);
                        load_word = *(uint32_t *)(curr_segment->elems + (c_val * curr_segment->size));
                        //load_word = UMSegment_at(um->segments, b_val, c_val);
                        *a_valp = load_word;
                        //UMRegister_put(um->registers, ra, load_word);
                        break;
                case SSTORE:
                        curr_segment = Seq_get(segments->seg_array, a_val);
                        word = (uint32_t *)(curr_segment->elems + (b_val * curr_segment->size));
                        *word = c_val;
                        //UMSegment_insert(um->segments, a_val, b_val, c_val);
                        break;
                case ADD: 
                        *a_valp = *b_valp + *c_valp;
                        //UMRegister_add(um->registers, ra, rb, rc);
                        break;
                case MUL: 
                        *a_valp = *b_valp * *c_valp;
                        //UMRegister_mult(um->registers, ra, rb, rc);
                        break;
                case DIV:
                        *a_valp = *b_valp / *c_valp;
                        //UMRegister_div(um->registers, ra, rb, rc);
                        break;
                case NAND:
                        *a_valp = ~(*b_valp & *c_valp);
                        //UMRegister_nand(um->registers, ra, rb, rc);
                        break;
                case HALT:
                        UM_free(um);
                        exit(EXIT_SUCCESS);
                        break;
                case MAP: 
                        UMSegment_map(um->segments, c_val, um->registers, rb);
                        break;
                case UNMAP:
                        UMSegment_unmap(um->segments, c_val);
                        break;
                case OUT:
                        putchar((unsigned char) c_val);
                        break;
                case IN: 
                        in = getchar();
                        fflush(NULL);
                        if (in == EOF)
                                in = EOF_FLAG;
                        *c_valp = in;
                        //UMRegister_put(um->registers, rc, in);
                        break;
                case LOADP:
                        if (b_val != CODE_SEG) {
                                src_segment = Seq_get(segments->seg_array, b_val);
                                dest_segment = Seq_get(segments->seg_array, CODE_SEG);
                                UArray_free(&dest_segment);
                                Seq_put(segments->seg_array, CODE_SEG, NULL);
                                dest_segment = UArray_copy(src_segment, 
                                                           UArray_length(src_segment));
                                Seq_put(segments->seg_array, CODE_SEG, dest_segment);
                              
                                //UMSegment_copy(um->segments, b_val, CODE_SEG);
                        }
                        um->counter = c_val;
                        break;
                case LV:
                        um->registers[lv_ra] = lv_val;
                        //UMRegister_put(um->registers, lv_ra, lv_val);
                        break;
                }
}


/*******************************************************
 *
 *      PUBLIC MEMBER FUNCTIONS
 *
 *******************************************************/

/* UM_new() function
 * Parameters:  program: char * type
 *
 * Returns:     Initialized UM
 *
 * Purpose:     Initializes a new UM and laods the UM program in the file
 *              with filename 'program' into the code segment of the UM.
 *              Returns the initialized UM. 
 */
UM UM_new(char *program)
{
        UM um = malloc(sizeof(struct UM));
        um->registers = UMRegister_new();
        um->segments = UMSegment_new();
        um->counter = 0;
        read_program(um, program);
        return um;
}

/* UM_free() function
 * Parameters:  um: UM type
 *
 * Returns:     void
 *
 * Purpose:     Frees any allocated memory associated with the given UM.
 */
void UM_free(UM um)
{
        UMRegister_free(um->registers);
        UMSegment_free(um->segments);
        free(um);
}

/* UM_run() function
 * Parameters:  um: UM type
 *
 * Returns:     void
 *
 * Purpose:     'Runs' the UM with a main instruction loop. In each 
 *              iteration of the loop, picks up the next UM instruction in 
 *              the loaded program, executes it, and moves to the next 
 *              instruction. 
 */
void UM_run(UM um)
{
        Instructions curr_instr = unpack_instruction(um);
        int code_length = UMSegment_length(um->segments, CODE_SEG);

        while (curr_instr.op != HALT || (int) um->counter < code_length) {
                curr_instr = unpack_instruction(um);
                um->counter++;
                UM_execute(um, curr_instr);
        }
        UM_free(um);
        exit(EXIT_SUCCESS);
}
