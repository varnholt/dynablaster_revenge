#include "mpegstream.h"
#include "decode.h"

static void I_step_one(unsigned int balloc[], unsigned int scale_index[2][SBLIMIT],MpegStream *fr)
{
	unsigned int *ba=balloc;
	unsigned int *sca = (unsigned int *) scale_index;

	if(fr->mChannels == 2)
	{
		int i;
		int jsbound = fr->mJointStereo;
		for(i=0;i<jsbound;i++)
		{ 
			*ba++ = fr->getbits(4);
			*ba++ = fr->getbits(4);
		}
		for(i=jsbound;i<SBLIMIT;i++) 
         *ba++ = fr->getbits(4);

		ba = balloc;

		for(i=0;i<jsbound;i++)
		{
			if ((*ba++))
				*sca++ = fr->getbits(6);
			if ((*ba++))
				*sca++ = fr->getbits(6);
		}
		for (i=jsbound;i<SBLIMIT;i++)
		if((*ba++))
		{
			*sca++ =  fr->getbits(6);
			*sca++ =  fr->getbits(6);
		}
	}
	else
	{
		int i;
		for(i=0;i<SBLIMIT;i++) *ba++ = fr->getbits(4);

		ba = balloc;
		for (i=0;i<SBLIMIT;i++)
		if ((*ba++))
		*sca++ = fr->getbits(6);
	}
}

static void I_step_two(float fraction[2][SBLIMIT],unsigned int balloc[2*SBLIMIT], unsigned int scale_index[2][SBLIMIT],MpegStream *fr)
{
	int i,n;
	int smpb[2*SBLIMIT]; // values: 0-65535
	int *sample;
   unsigned int *ba;
   unsigned int *sca = (unsigned int *) scale_index;

	if(fr->mChannels == 2)
	{
		int jsbound = fr->mJointStereo;
      float *f0 = fraction[0];
      float *f1 = fraction[1];
		ba = balloc;
		for(sample=smpb,i=0;i<jsbound;i++)
		{
			if((n = *ba++)) *sample++ = fr->getbits(n+1);

			if((n = *ba++)) *sample++ = fr->getbits(n+1);
		}
		for(i=jsbound;i<SBLIMIT;i++) 
		if((n = *ba++))
		*sample++ = fr->getbits(n+1);

		ba = balloc;
		for(sample=smpb,i=0;i<jsbound;i++)
		{
			if((n=*ba++))
			*f0++ = (float)( ((-1)<<n) + (*sample++) + 1) * fr->muls[n+1][*sca++];
			else *f0++ = (float)(0.0);

			if((n=*ba++))
			*f1++ = (float)( ((-1)<<n) + (*sample++) + 1) * fr->muls[n+1][*sca++];
			else *f1++ = (float)(0.0);
		}
		for(i=jsbound;i<SBLIMIT;i++)
		{
			if((n=*ba++))
			{
				float samp = (float)( ((-1)<<n) + (*sample++) + 1);
				*f0++ = samp * fr->muls[n+1][*sca++];
				*f1++ = samp * fr->muls[n+1][*sca++];
			}
			else *f0++ = *f1++ = (float)(0.0);
		}

      for(i=SBLIMIT;i<32;i++)
      {
         fraction[0][i] = fraction[1][i] = 0.0;
      }
	}
	else
	{
      float *f0 = fraction[0];
		ba = balloc;
		for(sample=smpb,i=0;i<SBLIMIT;i++)
		if ((n = *ba++))
		*sample++ = fr->getbits(n+1);

		ba = balloc;
		for(sample=smpb,i=0;i<SBLIMIT;i++)
		{
			if((n=*ba++))
			*f0++ = (float)( ((-1)<<n) + (*sample++) + 1) * fr->muls[n+1][*sca++];
			else *f0++ = (float)(0.0);
		}
      for(i=SBLIMIT;i<32;i++)
         fraction[0][i] = (float)(0.0);
	}
}

int Layer1::process(MpegStream *fr)
{
	int clip=0;
	int i;
	unsigned int balloc[2*SBLIMIT];
	unsigned int scale_index[2][SBLIMIT];
	float (*fraction)[SBLIMIT] = fr->layer1.fraction; // fraction[2][SBLIMIT]

   fr->mJointStereo = (fr->mMode == MPG_MD_JOINT_STEREO) ? (fr->mModeExt<<2)+4 : 32;

	I_step_one(balloc,scale_index,fr);

	for(i=0;i<SCALE_BLOCK;i++)
	{
		I_step_two(fraction,balloc,scale_index,fr);

		if(fr->mChannels == 1)
		   clip += synth_1to1_mono(fraction[0], fr);
      else
		   clip += synth_stereo_wrap(fraction[0], fraction[1], fr);
	}

	return clip;
}


