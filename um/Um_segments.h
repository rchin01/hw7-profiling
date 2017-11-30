/*******************************************************
 *
 *      Um_segments.h
 *      by Greg Pickart and Eli Rosmarin
 *      November 14, 2017
 *
 *      COMP 40 Fall 2017 - HW6 'UM'
 *      Um_segments.c contains the interface of the UM segment module. 
 *      The UM segment module represents the segmented memory of the UM. 
 *      The memory is word addressable and provides the ability to map up
 *      to 2^32 segments. The implementation of the Um segments is 
 *      protected from the interface and is defined in Um_segments.c.
 *
 *******************************************************/

#ifndef UM_SEGMENTS
#define UM_SEGMENTS

#include <stdint.h>
#include "seq.h"
#include "uarray.h"
#include "Um_registers.h"

typedef uint32_t Segment_ID;
typedef uint32_t Word;

typedef struct Segments *Segments;

Segments UMSegment_new();
int UMSegment_length(Segments segments, Segment_ID ID);
void UMSegment_map(Segments segments, int size, Register *registers, 
		   Register b);
void UMSegment_copy(Segments segments, Segment_ID src, Segment_ID dest);
void UMSegment_unmap(Segments segments, Segment_ID ID);
Word UMSegment_at(Segments segments, Segment_ID ID, int address);
void UMSegment_free(Segments segments);
void UMSegment_insert(Segments segments, Segment_ID ID, int address, 
                      Word value);
Word UMSegment_remove(Segments segments, Segment_ID ID, int address);

#endif
