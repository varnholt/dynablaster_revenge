#include "mpegstream.h"
#include "debug.h"
#include "dct64.h"

#define REAL_MUL_SYNTH(x, y)				((x) * (y))

/* Part 1a: All straight 1to1 decoding functions */
#define BLOCK 0x40 /* One decoding block is 64 samples. */

#define WRITE_SAMPLE(samples,sum) \
   if( (sum) > 32767) { *(samples) = 0x7fff; } \
  else if( (sum) < -32768) { *(samples) = -0x8000; } \
  else { *(samples) = (short)sum; }

/* Main synth function, uses the plain dct64 or dct64_i386. */
int synth_1to1(float *bandPtr, int channel, MpegStream *fr, int final)
{
#define BACKPEDAL 0x10 /* We use autoincrement and thus need this re-adjustment for window/b0. */
   static const int step = 2;
	short *samples = fr->buffer.data + fr->buffer.fill;

	float *b0, **buf; /* (*buf)[0x110]; */
	int clip = 0; 
	int bo1;

	if(!channel)
	{
		fr->bo--;
		fr->bo &= 0xf;
		buf = fr->real_buffs[0];
	}
	else
	{
		samples++;
      buf = fr->real_buffs[1];
	}

	if(fr->bo & 0x1)
	{
		b0 = buf[0];
		bo1 = fr->bo;
      dct64(buf[1]+((fr->bo+1)&0xf),buf[0]+fr->bo,bandPtr);
	}
	else
	{
		b0 = buf[1];
		bo1 = fr->bo+1;
      dct64(buf[0]+fr->bo,buf[1]+fr->bo+1,bandPtr);
	}

	{
      int j;
		float *window = fr->decwin + 16 - bo1;

		for(j=(BLOCK/4); j; j--, b0+=0x400/BLOCK-BACKPEDAL, window+=0x800/BLOCK-BACKPEDAL, samples+=step)
		{
			float sum;

         sum  = REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);
			sum += REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);
			sum += REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);
			sum += REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);
			sum += REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);
			sum += REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);
			sum += REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);
			sum += REAL_MUL_SYNTH(*window++, *b0++);
			sum -= REAL_MUL_SYNTH(*window++, *b0++);

			WRITE_SAMPLE(samples,sum);
		}

		{
			float sum;
			sum  = REAL_MUL_SYNTH(window[0x0], b0[0x0]);
			sum += REAL_MUL_SYNTH(window[0x2], b0[0x2]);
			sum += REAL_MUL_SYNTH(window[0x4], b0[0x4]);
			sum += REAL_MUL_SYNTH(window[0x6], b0[0x6]);
			sum += REAL_MUL_SYNTH(window[0x8], b0[0x8]);
			sum += REAL_MUL_SYNTH(window[0xA], b0[0xA]);
			sum += REAL_MUL_SYNTH(window[0xC], b0[0xC]);
			sum += REAL_MUL_SYNTH(window[0xE], b0[0xE]);

			WRITE_SAMPLE(samples,sum);
			samples += step;
			b0-=0x400/BLOCK;
			window-=0x800/BLOCK;
		}
		window += bo1<<1;

		for(j=(BLOCK/4)-1; j; j--, b0-=0x400/BLOCK+BACKPEDAL, window-=0x800/BLOCK-BACKPEDAL, samples+=step)
		{
			float sum;

         sum = -REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);
			sum -= REAL_MUL_SYNTH(*(--window), *b0++);

         WRITE_SAMPLE(samples,sum);
		}
	}

	if(final) fr->buffer.fill += BLOCK;

	return clip;
#undef BACKPEDAL
}

/* Now we have possibly some special synth_1to1 ...
   ... they produce signed short; the mono functions defined above work on the special synths, too. */

/* Mono synth, wrapping over SYNTH_NAME */
int synth_1to1_mono(float *bandPtr, MpegStream *fr)
{
   short samples_tmp[BLOCK];
   short *tmp1 = samples_tmp;
   int i,ret;

   /* save buffer stuff, trick samples_tmp into there, decode, restore */
   short *samples = fr->buffer.data;
   int pnt = fr->buffer.fill;
   fr->buffer.data = samples_tmp;
   fr->buffer.fill = 0;
   ret = synth_1to1(bandPtr, 0, fr, 0); /* decode into samples_tmp */
   fr->buffer.data = samples; // restore original value

   /* now append samples from samples_tmp */
   samples += pnt; /* just the next mem in frame buffer */
   for(i=0;i<(BLOCK/2);i++)
   {
      samples[i] = *tmp1;
      tmp1 += 2;
   }
   fr->buffer.fill = pnt + BLOCK/2;

   return ret;
}

/* The call of left and right plain synth, wrapped.
   This may be replaced by a direct stereo optimized synth. */
int synth_stereo_wrap(float *bandPtr_l, float *bandPtr_r, MpegStream *fr)
{
	int clip;
	clip  = synth_1to1(bandPtr_l, 0, fr, 0);
	clip += synth_1to1(bandPtr_r, 1, fr, 1);
	return clip;
}
