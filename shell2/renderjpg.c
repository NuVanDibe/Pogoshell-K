
#include "iwram.h"

extern JPEG_OUTPUT_TYPE *jpg_ptr;
extern int jpg_w;
extern int jpg_h;

void render_jpg(int x, int y, int w0, int h0, int wi, int hi, int mode, int scale, int rotate, int toshift)
{
	unsigned int high, low;

	unsigned short *src, *src2, *src3, *src4;
	unsigned short *s, *s2, *s3, *s4;
	unsigned short *dst, *d;

	int w, h;
	int dh, dw;
	int dh2, dw2, dw2i;
	int dx, dy;
	int mw, mh;
	int shift, l, m;

	s2 = s3 = s4 = NULL;

	dh2 = dw2i = 0;

	shift = (toshift && (mode || (scale < 0))) ? 1 : 0;

	dst = (unsigned short *)0x06000000; //vram

	switch(rotate)
	{
		case 3:
			dx = 240;
			dy = -1;
			dst += 240-1;
			break;
		case 2:
			dx = -1;
			dy = -240;
			dst += 240*160-1;
			break;
		case 1:
			dx = -240;
			dy = 1;
			dst += (160-1)*240;
			break;
		default:
			dx = 1;
			dy = 240;
			break;
	}

	mw = (((rotate&1) ? 160 : 240)-w0)>>1;
	mh = (((rotate&1) ? 240 : 160)-h0)>>1;

	if (mode)
		x = y = 0;

	src = &jpg_ptr[x + y * jpg_w];

	s = src;
	if (shift) {
		dh2 = dw2 = 0;
		s2 = s3 = s4 = s;
		dw2 += jpg_w;
		while (dw2 >= (wi << 1))
		{
			dw2 -= (wi << 1);
			s2++;
			s4++;
		}
		dh2 += jpg_h;
		while (dh2 >= (hi << 1))
		{
			dh2 -= (hi << 1);
			s3 += jpg_w;
			s4 += jpg_w;
		}
		dh2 >>= 1;
		dw2 >>= 1;
		dw2i = dw2;
	}
	l = mh;
	while (l--) {
		d = dst;
		m = (rotate&1) ? 160 : 240;
		while(m--) {
			*d = 0x0;
			d += dx;
		}
		dst += dy;
	}
	dh = 0;
	h = h0;//<<shift;
	d = dst;
	while (h--)
	{
		dw = 0;
		w = w0;//<<shift;
		l = mw;
		while(l--) {
			*dst = 0x0;
			dst += dx;
		}
		src = s;
		if (shift) {
			dw2 = dw2i;
			src2 = s2;
			src3 = s3;
			src4 = s4;
			while (w--)
			{
				high =  (*src & 0x739c);
				low  =  (*src & 0x0c63);
				high += (*src2 & 0x739c);
				low  += (*src2 & 0x0c63);
				high += (*src3 & 0x739c);
				low  += (*src3 & 0x0c63);
				high += (*src4 & 0x739c);
				low  += (*src4 & 0x0c63);
				*dst = (high+(low&0x318c))>>2;
				dst += dx;

				dw += jpg_w;
				while (dw >= wi)
				{
					dw -= wi;
					src++;
					src3++;
				}
				dw2 += jpg_w;
				while (dw2 >= wi)
				{
					dw2 -= wi;
					src2++;
					src4++;
				}
			}
			dh += jpg_h;
			while (dh >= hi)
			{
				dh -= hi;
				s += jpg_w;
				s2 += jpg_w;
			}
			dh2 += jpg_h;
			while (dh2 >= hi)
			{
				dh2 -= hi;
				s3 += jpg_w;
				s4 += jpg_w;
			}
		} else {
			while (w--)
			{
				*dst = *src;
				dst += dx;

				dw += jpg_w;
				while (dw >= wi)
				{
					dw -= wi;
					src++;
				}
			}
			dh += jpg_h;
			while (dh >= hi)
			{
				dh -= hi;
				s += jpg_w;
			}
		}
		l = ((rotate&1) ? 160 : 240)-w0-mw;
		while(l--) {
			*dst = 0x0;
			dst += dx;
		}
		d += dy;
		dst = d;
	}
	l = ((rotate&1) ? 240 : 160)-h0-mh;
	while (l--) {
		d = dst;
		m = (rotate&1) ? 160 : 240;
		while(m--) {
			*d = 0x0;
			d += dx;
		}
		dst += dy;
	}
}
