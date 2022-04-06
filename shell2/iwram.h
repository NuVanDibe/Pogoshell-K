
#include <pogo.h>
#include "gba.h"
#include "gba-jpeg-decode.h"

#define CODE_IN_IWRAM __attribute__ ((section (".iwram"), long_call))
#define VAR_IN_IWRAM __attribute__ ((section (".iwram")))

extern const unsigned char ToZigZag [];

extern const unsigned char ComponentRange [];

extern void ConvertBlock (
    signed char *YBlock, signed char *CbBlock, signed char *CrBlock,
    int YHorzFactor, int YVertFactor, int CbHorzFactor, int CbVertFactor, 
	int CrHorzFactor, int CrVertFactor, int horzMax, int vertMax,
    char M211, volatile JPEG_OUTPUT_TYPE *out, int bx, int by, int outStride, int outHeight,
	const unsigned char *ComponentRange) CODE_IN_IWRAM;


extern void IDCT_Columns (JPEG_FIXED_TYPE *zz) CODE_IN_IWRAM;

extern void IDCT_Rows (const JPEG_FIXED_TYPE *zz, signed char *chunk, int chunkStride) CODE_IN_IWRAM;

extern void DecodeCoefficients (
    JPEG_FIXED_TYPE *dcLast, JPEG_FIXED_TYPE *zz, JPEG_FIXED_TYPE *quant,
    JPEG_HuffmanTable *dcTable, JPEG_HuffmanTable *acTable,
    const unsigned char **dataBase, unsigned int *bitsLeftBase,
    unsigned long int *bitsDataBase, const unsigned char *toZigZag) CODE_IN_IWRAM;

extern void merge_sort(void *array, int count, int size, int cf(void *a, void *b)) CODE_IN_IWRAM;

extern void render_jpg(int x, int y, int w0, int h0, int wi, int hi, int mode, int scale, int rotate, int toshift) CODE_IN_IWRAM;
