//////////////////////////////////////////////////
//	I W R A M . C							//
//////////////////////////////////////////////////

#include "iwram.h"

// module contains functions which execute in iwram for extra speed

#define ROM_BANKSWITCH (volatile u16*)(0x96B592E)
#define WRITE_LOC_1 (volatile u16*)(0x987654*2+0x8000000)
#define WRITE_LOC_2 (volatile u16*)(0x012345*2+0x8000000)
#define WRITE_LOC_3 (volatile u16*)(0x007654*2+0x8000000)
#define WRITE_LOC_4 (volatile u16*)(0x765400*2+0x8000000)
#define WRITE_LOC_5 (volatile u16*)(0x013450*2+0x8000000)

// JPEG functions


/* Converts left-to-right coefficient indices into zig-zagged indices. */
const unsigned char ToZigZag [JPEG_DCTSIZE2] =
{
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};

/* This converts values in the range [-32 .. 32] to [0 .. 32] by clamping
 * values outside of that range.  To use it, add 32 to your input.
 */
const unsigned char ComponentRange [32 * 3] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31
};



/* Convert a chunk of YCbCr data to the output format.  YBlock, CbBlock,
 * and CrBlock are the pointers to the relevant chunks; each sample is
 * between -64 and 64, although out-of-range values are possible.
 * nHorzFactor and nVertFactor, where n is Y, Cb, and Cr, hold the
 * multipliers for each coordinate.  Shift right by horzMax and vertMax to
 * get the actual point to sample data from.  M211 is true if the
 * component factors satisfy a 2:1:1 relationship; this leads to a much
 * faster conversion.
 * out and outStride are the output pointers and the number of samples
 * in an output row.  Finally, ComponentRange is a pointer to the
 * JPEG_ComponentRange array.
 */
 
CODE_IN_IWRAM void ConvertBlock (
    signed char *YBlock, signed char *CbBlock, signed char *CrBlock,
    int YHorzFactor, int YVertFactor, int CbHorzFactor, int CbVertFactor, int CrHorzFactor, int CrVertFactor, int horzMax, int vertMax,
    char M211, volatile JPEG_OUTPUT_TYPE *out, int bx, int by, int outStride, int outHeight, const unsigned char *ComponentRange)
{
    int px, py;
    
    /* Since we need to offset all indices into this anyway, we might as well do it once only. */
    ComponentRange += 32;

// Screw this "optimization"
#if 0
/* Do the faster 2:1:1 code if the image scan satisfies that relationship. */
    if (M211)
    {
        /* Nothing complex here.  Because of its nature, we can do Cb and Cr
         * conversion only once for every four pixels.  This optimization is
         * done implicitly, using GCC's optimizer for gleaning the actual
         * advantage.
         */
         
        for (py = 0; by + py < outHeight && py < 2 * JPEG_DCTSIZE; py += 2)
        {
            volatile JPEG_OUTPUT_TYPE *row = &out [outStride * py];
            volatile JPEG_OUTPUT_TYPE *rowEnd = row + JPEG_DCTSIZE * 2;
            
            for (px = 0 ; px + bx < outStride && row < rowEnd; px++, row += 2, YBlock += 2, CbBlock ++, CrBlock ++)
            {
                int Cb = *CbBlock, Cr = *CrBlock;
                JPEG_Convert (row [0], YBlock [0], Cb, Cr);
				if (bx + px + 1 < outStride)
                   JPEG_Convert (row [1], YBlock [1], Cb, Cr);
				if (by + py + 1 < outHeight) {
                   JPEG_Convert (row [outStride], YBlock [2 * JPEG_DCTSIZE + 0], Cb, Cr);
				if (bx + px + 1 < outStride)
                      JPEG_Convert (row [outStride+1], YBlock [2 * JPEG_DCTSIZE + 1], Cb, Cr);
		}
            }
            
            YBlock += JPEG_DCTSIZE * 2;
        }
    }

    else {
#endif
    for (py = 0; by + py < outHeight && py < vertMax; py ++)
    {
        signed char *YScan = YBlock + (py * YVertFactor >> 8) * (horzMax * YHorzFactor >> 8);
        signed char *CbScan = CbBlock + (py * CbVertFactor >> 8) * (horzMax * CbHorzFactor >> 8);
        signed char *CrScan = CrBlock + (py * CrVertFactor >> 8) * (horzMax * CrHorzFactor >> 8);
        
        volatile JPEG_OUTPUT_TYPE *row = &out [outStride * py];
        
        for (px = 0; bx + px < outStride && px < horzMax; px ++, row ++)
        {
            int Y = YScan [px * YHorzFactor >> 8];
            int Cb = CbScan [px * CbHorzFactor >> 8];
            int Cr = CrScan [px * CrHorzFactor >> 8];
            
            JPEG_Convert (*row, Y, Cb, Cr);
        }
    }
    
    /* Make sure all variables are referenced. */
    (void) YHorzFactor; (void) YVertFactor; (void) CbHorzFactor;
    (void) CbVertFactor; (void) CrHorzFactor; (void) CrVertFactor;
    (void) horzMax; (void) vertMax; (void) px; (void) py;
    (void) YBlock; (void) CbBlock; (void) CrBlock;
    (void) M211; (void) out; (void) outStride;
}


/* Compute the columns half of the IDCT. */
CODE_IN_IWRAM void IDCT_Columns (JPEG_FIXED_TYPE *zz)
{
    JPEG_FIXED_TYPE tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11;
    JPEG_FIXED_TYPE *ez = zz + JPEG_DCTSIZE;

    /* The first column will always have a non-zero coefficient, the DC. */
    goto skipFirstCheckb;
    
    for ( ; zz < ez; zz ++)
    {
        /* A column containing only zeroes will output only zeroes.  Since we
         * output in-place, we don't need to do anything in that case.
         */
        if (!zz [0 * JPEG_DCTSIZE] && !zz [1 * JPEG_DCTSIZE]
        && !zz [2 * JPEG_DCTSIZE] && !zz [3 * JPEG_DCTSIZE]
        && !zz [4 * JPEG_DCTSIZE] && !zz [5 * JPEG_DCTSIZE]
        && !zz [6 * JPEG_DCTSIZE] && !zz [7 * JPEG_DCTSIZE])
            continue;
        
    skipFirstCheckb:
        tmp0 = zz [0 * JPEG_DCTSIZE];
        tmp1 = zz [2 * JPEG_DCTSIZE];
        tmp2 = zz [4 * JPEG_DCTSIZE];
        tmp3 = zz [6 * JPEG_DCTSIZE];
        
        tmp6 = tmp1 + tmp3;
        tmp7 = JPEG_FIXMUL (tmp1 - tmp3, JPEG_FTOFIX (1.414213562)) - tmp6;
        tmp1 = tmp0 - tmp2 + tmp7;
        tmp0 = tmp0 + tmp2 + tmp6;
        
        tmp3 = tmp0 - (tmp6 << 1);
        tmp2 = tmp1 - (tmp7 << 1);
        
        tmp4 = zz [1 * JPEG_DCTSIZE];
        tmp5 = zz [3 * JPEG_DCTSIZE];
        tmp6 = zz [5 * JPEG_DCTSIZE];
        tmp7 = zz [7 * JPEG_DCTSIZE];
        
        tmp10 = tmp4 - tmp7;
        
        tmp8 = tmp6 + tmp5;
        tmp9 = tmp4 + tmp7;
        tmp7 = tmp9 + tmp8;
        tmp11 = JPEG_FIXMUL (tmp9 - tmp8, JPEG_FTOFIX (1.414213562));
        
        tmp8 = tmp6 - tmp5;
        tmp9 = JPEG_FIXMUL (tmp8 + tmp10, JPEG_FTOFIX (1.847759065));
        
        tmp6 = JPEG_FIXMUL (JPEG_FTOFIX (-2.613125930), tmp8) + tmp9 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = JPEG_FIXMUL (JPEG_FTOFIX (1.082392200), tmp10) - tmp9 + tmp5; 
        
        zz [0 * JPEG_DCTSIZE] = tmp0 + tmp7;
        zz [1 * JPEG_DCTSIZE] = tmp1 + tmp6;
        zz [2 * JPEG_DCTSIZE] = tmp2 + tmp5;
        zz [3 * JPEG_DCTSIZE] = tmp3 - tmp4;
        zz [4 * JPEG_DCTSIZE] = tmp3 + tmp4;
        zz [5 * JPEG_DCTSIZE] = tmp2 - tmp5;
        zz [6 * JPEG_DCTSIZE] = tmp1 - tmp6;
        zz [7 * JPEG_DCTSIZE] = tmp0 - tmp7;
    }
}

/* Compute the rows half of the IDCT, loading the component information into
 * chunk as values in the range -64 to 64, although it can go somewhat outside
 * of that range.  chunkStride is the number of bytes in a row in chunk.
 */
 
CODE_IN_IWRAM void IDCT_Rows (const JPEG_FIXED_TYPE *zz, signed char *chunk, int chunkStride)
{
    JPEG_FIXED_TYPE tmp0, tmp1, tmp2, tmp3, tmp10, tmp11, tmp12, tmp13;
    JPEG_FIXED_TYPE tmp4, tmp5, tmp6, tmp7, z5, z10, z11, z12, z13;
    int row;
    
    for (row = 0; row < JPEG_DCTSIZE; row ++, zz += JPEG_DCTSIZE, chunk += chunkStride)
    {
        tmp10 = zz [0] + zz [4];
        tmp11 = zz [0] - zz [4];

        tmp13 = zz [2] + zz [6];
        tmp12 = JPEG_FIXMUL (zz [2] - zz [6], JPEG_FTOFIX (1.414213562)) - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;
        
        z13 = zz [5] + zz [3];
        z10 = zz [5] - zz [3];
        z11 = zz [1] + zz [7];
        z12 = zz [1] - zz [7];

        tmp7 = z11 + z13;
        tmp11 = JPEG_FIXMUL (z11 - z13, JPEG_FTOFIX (1.414213562));

        z5 = JPEG_FIXMUL (z10 + z12, JPEG_FTOFIX (1.847759065));
        tmp10 = JPEG_FIXMUL (JPEG_FTOFIX (1.082392200), z12) - z5;
        tmp12 = JPEG_FIXMUL (JPEG_FTOFIX (-2.613125930), z10) + z5;
        
        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        /* This shifts by an extra bit to remove the need for clamping at
         * this point.  Thus the normative samples are in the range -64 to 63.
         * This requires a later bit-shift, but that comes for free with the ARM
         * instruction set, and has an acceptable, likely imperceptible, loss
         * of quality.
         */
         
        chunk [0] = JPEG_FIXTOI (tmp0 + tmp7) >> 4;
        chunk [1] = JPEG_FIXTOI (tmp1 + tmp6) >> 4;
        chunk [2] = JPEG_FIXTOI (tmp2 + tmp5) >> 4;
        chunk [3] = JPEG_FIXTOI (tmp3 - tmp4) >> 4;
        chunk [4] = JPEG_FIXTOI (tmp3 + tmp4) >> 4;
        chunk [5] = JPEG_FIXTOI (tmp2 - tmp5) >> 4;
        chunk [6] = JPEG_FIXTOI (tmp1 - tmp6) >> 4;
        chunk [7] = JPEG_FIXTOI (tmp0 - tmp7) >> 4;
    }
}

#define JPEG_Value(COUNT, OUT) \
    do { \
        unsigned int value = JPEG_BITS_GET (COUNT); \
        \
        if (value < (unsigned int) (1 << ((unsigned int) (COUNT - 1)))) \
            value += (-1 << COUNT) + 1; \
        (OUT) = value; \
    } while (0)

/* Decode the coefficients from the input stream and do dequantization at the
 * same time.  dcLast is the previous block's DC value and is updated.  zz is
 * the output coefficients and will be all ready for an IDCT.  quant is the
 * quantization table to use, dcTable and acTable are the Huffman tables for
 * the DC and AC coefficients respectively, dataBase, bitsLeftBase, and
 * bitsDataBase are for input stream state, and toZigZag is a pointer to
 * JPEG_ToZigZag or to its IWRAM copy.
 */
 
CODE_IN_IWRAM void DecodeCoefficients (
    JPEG_FIXED_TYPE *dcLast, JPEG_FIXED_TYPE *zz, JPEG_FIXED_TYPE *quant,
    JPEG_HuffmanTable *dcTable, JPEG_HuffmanTable *acTable,
    const unsigned char **dataBase, unsigned int *bitsLeftBase,
    unsigned long int *bitsDataBase, const unsigned char *toZigZag)
{
    unsigned bits_left = *bitsLeftBase, bits_data = *bitsDataBase; /* Input stream state. */
    const unsigned char *data = *dataBase; /* Input stream state. */
    int r, s, diff; /* Various temporary data variables. */
    int index = 1; /* The current zig-zagged index. */
    
    /* Clear all coefficients to zero. */
    {
        JPEG_FIXED_TYPE *ez = zz + JPEG_DCTSIZE2;
        do *-- ez = 0;
        while (ez > zz);
    }
    
    /* Read the DC coefficient. */
    JPEG_BITS_CHECK ();
    JPEG_HuffmanTable_Decode (dcTable, s);
    JPEG_Value (s, diff);
    
    /* Store the DC coefficient. */
    *dcLast += diff;
    zz [toZigZag [0]] = *dcLast * quant [0];

    while (1)
    {
        /* Read a bits/run-length value. */
        JPEG_BITS_CHECK ();
        JPEG_HuffmanTable_Decode (acTable, s);
        r = s >> 4;
        s &= 15;
    
        /* If there is a value at this cell +r, then read it. */
        if (s)
        {
            index += r;
            JPEG_Value (s, r);
            zz [toZigZag [index]] = r * quant [index];
            if (index == JPEG_DCTSIZE2 - 1)
                break;
            index ++;
        }
        /* Otherwise we skip 16 cells or finish up. */
        else
        {
            if (r != 15)
                break;
            index += 16;
        }
    }
    
    /* Restore state for the caller. */
    *bitsDataBase = bits_data;
    *bitsLeftBase = bits_left;
    *dataBase = data;
}

CODE_IN_IWRAM void merge_sort(void *array, int count, int size, int cf(void *a, void *b))
{
	int i, join, actualsize;
	int left, middle;
	char *a = (char *)array;
	char tmp_item[size];

	for (i = 0; i < count; i += 2)
	{
		if (i < count - 1)
		{
			if (cf(&a[i * size], &a[(i + 1) * size]) > 0) {
				memcpy(tmp_item, &a[i * size], size);
				memcpy(&a[i * size], &a[(i+1) * size], size);
				memcpy(&a[(i+1) * size], tmp_item, size);
			}
		}
	}
	for (i = 4; i < count*2; i *= 2)
	{
		for (join = 0; join < count; join += i)
		{
			actualsize = count - join;
			if (actualsize > i)
				actualsize = i;
			left = join;
			middle = join + i/2;
			while (left < middle && middle < join+actualsize) {
				if (cf(&a[left * size], &a[middle * size]) > 0) {
					memcpy(tmp_item, &a[middle * size], size);
					memmove(&a[(left+1) * size], &a[left * size], size*(middle-left));
					memcpy(&a[left * size], tmp_item, size);
					middle++;
				}
				left++;
			}
		}
	}
}

extern JPEG_OUTPUT_TYPE *jpg_ptr;
extern int jpg_w;
extern int jpg_h;

#define MIN(a,b) (a<b ? a : b)
#define MAX(a,b) (a>b ? a : b)

#define R(x) (x&31)
#define G(x) ((x>>5)&31)
#define B(x) ((x>>10)&31)

CODE_IN_IWRAM void render_jpg(int x, int y, int scale, int hdbw, int wdbh)
{
	static unsigned short red[240];
	static unsigned short green[240];
	static unsigned short blue[240];

	int wi, hi;
	unsigned short *src, *dst;

	int w, h;
	int dh, dw;

	dst = (unsigned short *)0x06000000; //vram

	switch (scale)
	{
		// Aspect
		case 1:
			w = 240;
			//h = 240 * jpg_h / jpg_w;
			h = hdbw;
			if (h > 160) {
				h = 160;
				//w = 160 * jpg_w / jpg_h;
				w = wdbh;
			}
			/**(int *)(0x02000000) = w;
			*(int *)(0x02000004) = h;
			while(1);*/
			dh = 0;
			dst += (160-h)/2*240;
			src = jpg_ptr;
			hi = h*2;
			while (hi--)
			{
				dw = 0;
				wi = w*2;
				if (!(hi&1))
					dst += (240 - w)/2;
				x = 0;
				while (wi--)
				{
					if (!(wi&1) || !(hi&1)) {
						red[x] += R(*src);
						green[x] += G(*src);
						blue[x] += B(*src);
					} else {
						red[x] = R(*src);
						green[x] = G(*src);
						blue[x] = B(*src);
					}
					
					if (!(wi&1) && !(hi&1)) {
						*dst++ = ((red[x]>>2)&31) |
						          (((green[x]>>2)&31)<<5) |
						          (((blue[x]>>2)&31)<<10);
					}

					if (!(wi&1))
						x++;

					dw += jpg_w;
					while (dw >= w*2)
					{
						dw -= w*2;
						src++;
					}
				}
				if (!(hi&1))
					dst += 240-w-(240 - w)/2;
				src -= jpg_w;
				dh += jpg_h;
				while (dh >= h*2)
				{
					dh -= h*2;
					src += jpg_w;
				}
			}
			break;
		// Stretch
		case 2:
			h = 320;
			dh = 0;
			src = jpg_ptr;
			while (h--)
			{
				dw = 0;
				w = 480;
				x = 0;
				while (w--)
				{
					if (!(w&1) || !(h&1)) {
						red[x] += R(*src);
						green[x] += G(*src);
						blue[x] += B(*src);
					} else {
						red[x] = R(*src);
						green[x] = G(*src);
						blue[x] = B(*src);
					}
					
					if (!(w&1) && !(h&1)) {
						*dst++ = ((red[x]>>2)&31) |
						          (((green[x]>>2)&31)<<5) |
						          (((blue[x]>>2)&31)<<10);
					}

					if (!(w&1))
						x++;

					dw += jpg_w;
					while (dw >= 480)
					{
						dw -= 480;
						src++;
					}
				}
				src -= jpg_w;
				dh += jpg_h;
				while (dh >= 320)
				{
					dh -= 320;
					src += jpg_w;
				}
			}
			break;
		default:
			w = MIN(jpg_w, 240);
			h = MIN(jpg_h, 160);

			if((x+240) > jpg_w)
				x = (jpg_w-240);
			if((y+160) > jpg_h)
				y = (jpg_h-160);
			if(x < 0) x = 0;
			if(y < 0) y = 0;

			src = &jpg_ptr[x + y * jpg_w];
			if (h < 160)
				dst += (160 - h)/2*240;
			while(h--)
			{
				wi = w;
				dst += (240 - w)/2;
				while(wi--)
					*dst++ = *src++;
				dst += 240-w-(240 - w)/2;
				src += (jpg_w - w);
			}
			break;
	}
}

