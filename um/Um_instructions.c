#include "Um_instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <um-dis.h>

/*******************************************************
 *
 *      CONSTANT DEFINITIONS AND STRUCT DEFINITIONS
 *
 *******************************************************/

#define SEQ_HINT 0

struct Segments {
        Seq_T available_IDs;
        Seq_T seg_array; 
};

/*******************************************************
 *
 *      PRIVATE HELPER FUNCTIONS
 *
 *******************************************************/

/* new_segment() function
 * Parameters:  size: int type
 *
 * Returns:     UArray_T representing new segment
 *
 * Purpose:     Creates a new segment of length size and returns it. 
 */
static inline UArray_T new_segment(int size)
{
        UArray_T segment = UArray_new(size, sizeof(Word));
        return segment;
}

/*******************************************************
 *
 *      PUBLIC MEMBER FUNCTIONS
 *
 *******************************************************/

/* UMSegment_new() function
 * Parameters:  none
 *
 * Returns:     Pointer to Segments struct representing new UM memory
 *
 * Purpose:     Allocates space for a new Segments struct pointer and 
 *              initializes the two sequences of the struct. Returns the 
 *              initializes Segments.
 */     
Segments UMSegment_new()
{
        Segments segments = malloc(sizeof(struct Segments));
        *segments = (struct Segments) {
                Seq_new(SEQ_HINT), Seq_new(SEQ_HINT)
        };
        return segments;
}


/* UMSegment_length() function
 * Parameters:  segments: Segments type; ID: Segment_ID type
 *
 * Returns:     int representing length of segment
 *
 * Purpose:     Returns the length of the segment in the given segment
 *              array with ID ID.
 */     
int UMSegment_length(Segments segments, Segment_ID ID)
{
        UArray_T curr_segment = Seq_get(segments->seg_array, ID);
        return UArray_length(curr_segment);
}

/* UMSegment_replace() function
 * Parameters:  segments: Segments type; src: Segment_ID type; dest: 
 *              Segment_ID type
 *
 * Returns:     void
 *
 * Purpose:     Replaces the segment at ID dest with the segment at ID src 
 *              in the given segment array and frees the replaced segment.
 *              Effectively copies src segment into dest segment.
 */
void UMSegment_copy(Segments segments, Segment_ID src, Segment_ID dest)
{
        UArray_T src_segment, dest_segment;
        if (src != dest) {
                src_segment = Seq_get(segments->seg_array, src);
                dest_segment = Seq_get(segments->seg_array, dest);
                UArray_free(&dest_segment);
                Seq_put(segments->seg_array, dest, NULL);
                dest_segment = UArray_copy(src_segment, 
                                           UArray_length(src_segment));
                Seq_put(segments->seg_array, dest, dest_segment);
        }
}

/* UMSegment_map() function
 * Parameters:  segments: Segments type; size: int type; registers: 
 *              Register * type; b: Register type
 *
 * Returns:     void
 *
 * Purpose:     Maps a new segment of length size in the given segment
 *              array. If there are available IDs in the segment array, maps 
 *              the new segment at the first available ID. Otherwise maps the 
 *              new segment at ID n where n = length of segment array pre-
 *              mapping. If not mapping segment 0, places the newly mapped
 *              segment ID into register b in the given register array.
 */
void UMSegment_map(Segments segments, int size, Register *registers, 
                   Register b) {
        int available_ID = Seq_length(segments->available_IDs);
        Segment_ID ID;
        if (available_ID) {
                ID = (Segment_ID)(uintptr_t) Seq_get(segments->available_IDs, 
                                         available_ID - 1);
                Seq_put(segments->seg_array, ID, new_segment(size));
                Seq_remhi(segments->available_IDs);
                if (registers != NULL)
                        UMRegister_put(registers, b, ID);
                return;
        } else {
                ID = Seq_length(segments->seg_array);
                Seq_addhi(segments->seg_array, new_segment(size));
                if (registers != NULL)
                        UMRegister_put(registers, b, ID);
        }
}

/* UMSegment_unmap() function
 * Parameters:  segments: Segments type; ID: Segment_ID type
 *
 * Returns:     void
 *
 * Purpose:     Unmaps the segment with ID ID in the given segment array. 
 *              Frees memory associated with the segment and adds the ID 
 *              to the available_IDs sequence of the segment array 
 *              for future reuse. 
 */     
void UMSegment_unmap(Segments segments, Segment_ID ID)
{
        if (ID != 0) {
                UArray_T curr_segment = Seq_get(segments->seg_array, ID);
                UArray_free(&curr_segment);
                Seq_put(segments->seg_array, ID, NULL);
                Seq_addhi(segments->available_IDs, (void *)(uintptr_t) ID);
        }
}

/* UMSegment_free() function
 * Parameters:  segments: Segments type
 *
 * Returns:     void
 *
 * Purpose:     Frees the memory associated with the given segment array.
 *              Iterates over the array to only free segments that have 
 *              not already been unmapped.
 */     
void UMSegment_free(Segments segments)
{
        int i;
        int num_segs = Seq_length(segments->seg_array);
        UArray_T curr_segment;

        for (i = 0; i < num_segs; i++) {
                curr_segment = Seq_get(segments->seg_array, i);
                if (curr_segment != NULL) {
                        UArray_free(&curr_segment);
                }
        }
        Seq_free(&segments->seg_array);
        Seq_free(&segments->available_IDs);
        free(segments);
}

/* UMSegment_at() function
 * Parameters:  segments: Segments type; ID: Segment_ID type; address: int 
 *              type
 *
 * Returns:     Word (uint32_t)
 *
 * Purpose:     Returns the word value in the segment with ID ID in the 
 *              given segment array at address address. 
 */
Word UMSegment_at(Segments segments, Segment_ID ID, int address)
{
        UArray_T curr_segment = Seq_get(segments->seg_array, ID);
        Word *word = (uint32_t *)(curr_segment->elems + (address * curr_segment->size));
                        //UArray_at(curr_segment, address);
        return *word;
}

/* UMSegment_insert() function
 * Parameters:  segments: Segments type; ID: Segment_ID type; address:
 *              int type; value: Word type
 *
 * Returns:     void
 *
 * Purpose:     Inserts the given value into the segment in the given 
 *              segment array at ID ID at address address. 
 */
void UMSegment_insert(Segments segments, Segment_ID ID, int address, 
                      Word value)
{
        UArray_T curr_segment = Seq_get(segments->seg_array, ID);
        Word *word = (uint32_t *)(curr_segment->elems + (address * curr_segment->size));
                        //UArray_at(curr_segment, address);
        *word = value;
}

/* UMSegment_type() function
 * Parameters:  segments: Segments type; ID: Segment_ID type; address: 
 *              int type
 *
 * Returns:     Word (uint32_t)
 *
 * Purpose:     Clears the value in the given segment array at the 
 *              segment with ID ID at address address by setting it 
 *              equal to 0. Returns the previous value. 
 */     
Word UMSegment_remove(Segments segments, Segment_ID ID, int address)
{
        Word prev = UMSegment_at(segments, ID, address);
        UMSegment_insert(segments, ID, address, 0);
        return prev;
}

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
