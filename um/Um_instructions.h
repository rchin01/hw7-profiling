#ifndef UM_INSTRUCTIONS
#define UM_INSTRUCTIONS

#include <stdint.h>
#include "seq.h"
#include "uarray.h"

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