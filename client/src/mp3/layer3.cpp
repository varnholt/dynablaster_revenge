#include "mpegstream.h"
#include "huffman.h"
#include "debug.h"
#include "dct64.h"
#include "decode.h"
#include <memory.h>
#include <math.h>

#define REAL_SCALE_LAYER3(x, y)			(x)
#define REAL_MUL_SCALE_LAYER3(x, y, z)		((x) * (y))
#define REAL_MUL_15(x, y)					((x) * (y))

// define CUT_SFB21 if you want to cut-off the frequency above 16kHz
// #define CUT_SFB21

// static tables...
float ispow[8207];
float aa_ca[8],aa_cs[8];
float win[4][36];
float win1[4][36];
float COS6_1,COS6_2;
float tfcos36[9];
float tfcos12[3];
float cos9[3],cos18[3];
float tan1_1[16],tan2_1[16],tan1_2[16],tan2_2[16];
float pow1_1[2][16],pow2_1[2][16],pow1_2[2][16],pow2_2[2][16];

class GranulesInfo
{
public:
	int scfsi;
	unsigned part2_3_length;
	unsigned big_values;
	unsigned scalefac_compress;
	unsigned block_type;
	unsigned mixed_block_flag;
	unsigned table_select[3];
	int maxband[3];
	int maxbandl;
	unsigned maxb;
	unsigned region1start;
	unsigned region2start;
	unsigned preflag;
	unsigned scalefac_scale;
	unsigned count1table_select;
	float *full_gain[3];
	float *pow2gain;
};

class SideInfo
{
public:
	unsigned main_data_begin;
	unsigned private_bits;
	// Hm, funny... struct inside struct...
   struct { GranulesInfo gr[2]; } ch[2];
};

class BandInfoStruct
{
public:
	unsigned short longIdx[23];
	unsigned char longDiff[22];
	unsigned short shortIdx[14];
	unsigned char shortDiff[13];
};

/* Techy details about our friendly MPEG data. Fairly constant over the years;-) */
static const BandInfoStruct bandInfo[9] =
{
	{ /* MPEG 1.0 */
		{0,4,8,12,16,20,24,30,36,44,52,62,74, 90,110,134,162,196,238,288,342,418,576},
		{4,4,4,4,4,4,6,6,8, 8,10,12,16,20,24,28,34,42,50,54, 76,158},
		{0,4*3,8*3,12*3,16*3,22*3,30*3,40*3,52*3,66*3, 84*3,106*3,136*3,192*3},
		{4,4,4,4,6,8,10,12,14,18,22,30,56}
	},
	{
		{0,4,8,12,16,20,24,30,36,42,50,60,72, 88,106,128,156,190,230,276,330,384,576},
		{4,4,4,4,4,4,6,6,6, 8,10,12,16,18,22,28,34,40,46,54, 54,192},
		{0,4*3,8*3,12*3,16*3,22*3,28*3,38*3,50*3,64*3, 80*3,100*3,126*3,192*3},
		{4,4,4,4,6,6,10,12,14,16,20,26,66}
	},
	{
		{0,4,8,12,16,20,24,30,36,44,54,66,82,102,126,156,194,240,296,364,448,550,576},
		{4,4,4,4,4,4,6,6,8,10,12,16,20,24,30,38,46,56,68,84,102, 26},
		{0,4*3,8*3,12*3,16*3,22*3,30*3,42*3,58*3,78*3,104*3,138*3,180*3,192*3},
		{4,4,4,4,6,8,12,16,20,26,34,42,12}
	},
	{ /* MPEG 2.0 */
		{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
		{6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54 } ,
		{0,4*3,8*3,12*3,18*3,24*3,32*3,42*3,56*3,74*3,100*3,132*3,174*3,192*3} ,
		{4,4,4,6,6,8,10,14,18,26,32,42,18 }
	},
	{ /* Twiddling 3 values here (not just 330->332!) fixed bug 1895025. */
		{0,6,12,18,24,30,36,44,54,66,80,96,114,136,162,194,232,278,332,394,464,540,576},
		{6,6,6,6,6,6,8,10,12,14,16,18,22,26,32,38,46,54,62,70,76,36 },
		{0,4*3,8*3,12*3,18*3,26*3,36*3,48*3,62*3,80*3,104*3,136*3,180*3,192*3},
		{4,4,4,6,8,10,12,14,18,24,32,44,12 }
	},
	{
		{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
		{6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54 },
		{0,4*3,8*3,12*3,18*3,26*3,36*3,48*3,62*3,80*3,104*3,134*3,174*3,192*3},
		{4,4,4,6,8,10,12,14,18,24,30,40,18 }
	},
	{ /* MPEG 2.5 */
		{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
		{6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54},
		{0,12,24,36,54,78,108,144,186,240,312,402,522,576},
		{4,4,4,6,8,10,12,14,18,24,30,40,18}
	},
	{
		{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
		{6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54},
		{0,12,24,36,54,78,108,144,186,240,312,402,522,576},
		{4,4,4,6,8,10,12,14,18,24,30,40,18}
	},
	{
		{0,12,24,36,48,60,72,88,108,132,160,192,232,280,336,400,476,566,568,570,572,574,576},
		{12,12,12,12,12,12,16,20,24,28,32,40,48,56,64,76,90,2,2,2,2,2},
		{0, 24, 48, 72,108,156,216,288,372,480,486,492,498,576},
		{8,8,8,12,16,20,24,28,36,2,2,2,26}
	}
};

static int mapbuf0[9][152];
static int mapbuf1[9][156];
static int mapbuf2[9][44];
static int *map[9][3];
static int *mapend[9][3];

static unsigned int n_slen2[512]; /* MPEG 2.0 slen for 'normal' mode */
static unsigned int i_slen2[256]; /* MPEG 2.0 slen for intensity stereo */

Layer3::Layer3()
 : part2remain(0)
{
   memset(longLimit, 0, sizeof(int)*9*23);
   memset(shortLimit, 0, sizeof(int)*9*14);
   memset(gainpow2, 0, sizeof(float)*(256+118+4));
}

void Layer3::initStuff()
{
   int i,j;

   for(i=-256;i<118+4;i++)	gainpow2[i+256] = (float) pow((double)2.0,-0.25 * (double) (i+210));

   for(j=0;j<9;j++)
   {
      for(i=0;i<23;i++)
      {
         longLimit[j][i] = (bandInfo[j].longIdx[i] - 1 + 8) / 18 + 1;
         if(longLimit[j][i] > SBLIMIT )
            longLimit[j][i] = SBLIMIT;
      }
      for(i=0;i<14;i++)
      {
         shortLimit[j][i] = (bandInfo[j].shortIdx[i] - 1) / 18 + 1;
         if(shortLimit[j][i] > SBLIMIT )
            shortLimit[j][i] = SBLIMIT;
      }
   }
}

// init tables for layer-3
void Layer3::init()
{
	int i,j,k,l;

	for(i=0;i<8207;i++)
	   ispow[i] = (float) (pow((double)i,(double)4.0/3.0));

	for(i=0;i<8;i++)
	{
		const double Ci[8] = {-0.6,-0.535,-0.33,-0.185,-0.095,-0.041,-0.0142,-0.0037};
		double sq = sqrt(1.0+Ci[i]*Ci[i]);
		aa_cs[i] = (float)(1.0/sq);
		aa_ca[i] = (float)(Ci[i]/sq);
	}

	for(i=0;i<18;i++)
	{
		win[0][i]    = win[1][i]    =
			(float)( 0.5*sin(M_PI/72.0 * (double)(2*(i+0) +1)) / cos(M_PI * (double)(2*(i+0) +19) / 72.0) );
		win[0][i+18] = win[3][i+18] =
			(float)( 0.5*sin(M_PI/72.0 * (double)(2*(i+18)+1)) / cos(M_PI * (double)(2*(i+18)+19) / 72.0) );
	}
	for(i=0;i<6;i++)
	{
		win[1][i+18] = (float)(0.5 / cos ( M_PI * (double) (2*(i+18)+19) / 72.0 ));
		win[3][i+12] = (float)(0.5 / cos ( M_PI * (double) (2*(i+12)+19) / 72.0 ));
		win[1][i+24] = (float)(0.5 * sin( M_PI / 24.0 * (double) (2*i+13) ) / cos ( M_PI * (double) (2*(i+24)+19) / 72.0 ));
		win[1][i+30] = win[3][i] = (float)(0.0);
		win[3][i+6 ] = (float)(0.5 * sin( M_PI / 24.0 * (double) (2*i+1 ) ) / cos ( M_PI * (double) (2*(i+6 )+19) / 72.0 ));
	}

	for(i=0;i<9;i++)
	tfcos36[i] = (float)(0.5 / cos ( M_PI * (double) (i*2+1) / 36.0 ));

	for(i=0;i<3;i++)
	tfcos12[i] = (float)(0.5 / cos ( M_PI * (double) (i*2+1) / 12.0 ));

	COS6_1 = (float)(cos( M_PI / 6.0 * (double) 1));
	COS6_2 = (float)(cos( M_PI / 6.0 * (double) 2));

	cos9[0]  = (float)(cos(1.0*M_PI/9.0));
	cos9[1]  = (float)(cos(5.0*M_PI/9.0));
	cos9[2]  = (float)(cos(7.0*M_PI/9.0));
	cos18[0] = (float)(cos(1.0*M_PI/18.0));
	cos18[1] = (float)(cos(11.0*M_PI/18.0));
	cos18[2] = (float)(cos(13.0*M_PI/18.0));

	for(i=0;i<12;i++)
	{
		win[2][i] = (float)(0.5 * sin( M_PI / 24.0 * (double) (2*i+1) ) / cos ( M_PI * (double) (2*i+7) / 24.0 ));
	}

	for(i=0;i<16;i++)
	{
		double t = tan( (double) i * M_PI / 12.0 );
		tan1_1[i] = (float)(t / (1.0+t));
		tan2_1[i] = (float)(1.0 / (1.0 + t));
		tan1_2[i] = (float)(M_SQRT2 * t / (1.0+t));
		tan2_2[i] = (float)(M_SQRT2 / (1.0 + t));

		for(j=0;j<2;j++)
		{
			double base = pow(2.0,-0.25*(j+1.0));
			double p1=1.0,p2=1.0;
			if(i > 0)
			{
				if( i & 1 ) p1 = pow(base,(i+1.0)*0.5);
				else p2 = pow(base,i*0.5);
			}
			pow1_1[j][i] = (float)(p1);
			pow2_1[j][i] = (float)(p2);
			pow1_2[j][i] = (float)(M_SQRT2 * p1);
			pow2_2[j][i] = (float)(M_SQRT2 * p2);
		}
	}

	for(j=0;j<4;j++)
	{
		const int len[4] = { 36,36,12,36 };
		for(i=0;i<len[j];i+=2) win1[j][i] = + win[j][i];

		for(i=1;i<len[j];i+=2) win1[j][i] = - win[j][i];
	}

	for(j=0;j<9;j++)
	{
      const BandInfoStruct *bi = &bandInfo[j];
		int *mp;
		int cb,lwin;
		const unsigned char *bdf;

		mp = map[j][0] = mapbuf0[j];
		bdf = bi->longDiff;
		for(i=0,cb = 0; cb < 8 ; cb++,i+=*bdf++)
		{
			*mp++ = (*bdf) >> 1;
			*mp++ = i;
			*mp++ = 3;
			*mp++ = cb;
		}
		bdf = bi->shortDiff+3;
		for(cb=3;cb<13;cb++)
		{
			int l = (*bdf++) >> 1;
			for(lwin=0;lwin<3;lwin++)
			{
				*mp++ = l;
				*mp++ = i + lwin;
				*mp++ = lwin;
				*mp++ = cb;
			}
			i += 6*l;
		}
		mapend[j][0] = mp;

		mp = map[j][1] = mapbuf1[j];
		bdf = bi->shortDiff+0;
		for(i=0,cb=0;cb<13;cb++)
		{
			int l = (*bdf++) >> 1;
			for(lwin=0;lwin<3;lwin++)
			{
				*mp++ = l;
				*mp++ = i + lwin;
				*mp++ = lwin;
				*mp++ = cb;
			}
			i += 6*l;
		}
		mapend[j][1] = mp;

		mp = map[j][2] = mapbuf2[j];
		bdf = bi->longDiff;
		for(cb = 0; cb < 22 ; cb++)
		{
			*mp++ = (*bdf++) >> 1;
			*mp++ = cb;
		}
		mapend[j][2] = mp;
	}

	for(i=0;i<5;i++)
	for(j=0;j<6;j++)
	for(k=0;k<6;k++)
	{
		int n = k + j * 6 + i * 36;
		i_slen2[n] = i|(j<<3)|(k<<6)|(3<<12);
	}
	for(i=0;i<4;i++)
	for(j=0;j<4;j++)
	for(k=0;k<4;k++)
	{
		int n = k + j * 4 + i * 16;
		i_slen2[n+180] = i|(j<<3)|(k<<6)|(4<<12);
	}
	for(i=0;i<4;i++)
	for(j=0;j<3;j++)
	{
		int n = j + i * 3;
		i_slen2[n+244] = i|(j<<3) | (5<<12);
		n_slen2[n+500] = i|(j<<3) | (2<<12) | (1<<15);
	}
	for(i=0;i<5;i++)
	for(j=0;j<5;j++)
	for(k=0;k<4;k++)
	for(l=0;l<4;l++)
	{
		int n = l + k * 4 + j * 16 + i * 80;
		n_slen2[n] = i|(j<<3)|(k<<6)|(l<<9)|(0<<12);
	}
	for(i=0;i<5;i++)
	for(j=0;j<5;j++)
	for(k=0;k<4;k++)
	{
		int n = k + j * 4 + i * 20;
		n_slen2[n+400] = i|(j<<3)|(k<<6)|(1<<12);
	}
}


int Layer3::getSideInfo(MpegStream *fr, SideInfo *si,int stereo, int ms_stereo,long sfreq,int single)
{
	int ch, gr;
	int powdiff = (single == SINGLE_MIX) ? 4 : 0;

	const int tabs[2][5] = { { 2,9,5,3,4 } , { 1,8,1,2,9 } };
	const int *tab = tabs[fr->lsf];

	si->main_data_begin = fr->getbits(tab[1]);

	if(si->main_data_begin > fr->bitreservoir)
	{
		//  overwrite main_data_begin for the really available bit reservoir
      fr->backbits(tab[1]);
		if(fr->lsf == 0)
		{
			fr->wordpointer[0] = (unsigned char) (fr->bitreservoir >> 1);
			fr->wordpointer[1] = (unsigned char) ((fr->bitreservoir & 1) << 7);
		}
		else fr->wordpointer[0] = (unsigned char) fr->bitreservoir;

		// zero "side-info" data for a silence-frame
		memset(fr->wordpointer+2, 0, fr->ssize-2);

		// reread the new bit reservoir offset
		si->main_data_begin = fr->getbits(tab[1]);
	}

	// Keep track of the available data bytes for the bit reservoir.
   fr->bitreservoir = fr->bitreservoir + fr->mFrameSize - fr->ssize - (fr->mErrorProtection ? 2 : 0);
	// Limit the reservoir to the max for MPEG 1.0 or 2.x .
	if(fr->bitreservoir > (unsigned int) (fr->lsf == 0 ? 511 : 255))
	fr->bitreservoir = (fr->lsf == 0 ? 511 : 255);

	if (stereo == 1)
      si->private_bits = fr->getbits_fast(tab[2]);
	else 
      si->private_bits = fr->getbits_fast(tab[3]);

	if(!fr->lsf) for(ch=0; ch<stereo; ch++)
	{
		si->ch[ch].gr[0].scfsi = -1;
		si->ch[ch].gr[1].scfsi = fr->getbits_fast(4);
	}

	for (gr=0; gr<tab[0]; gr++)
	for (ch=0; ch<stereo; ch++)
	{
      GranulesInfo *gr_info = &(si->ch[ch].gr[gr]);

		gr_info->part2_3_length = fr->getbits(12);
		gr_info->big_values = fr->getbits(9);
		if(gr_info->big_values > 288)
		{
			printf("big_values too large!\n");
			gr_info->big_values = 288;
		}
      gr_info->pow2gain = fr->layer3.gainpow2+256 - fr->getbits_fast(8) + powdiff;
      if(ms_stereo) gr_info->pow2gain += 2;

		gr_info->scalefac_compress = fr->getbits(tab[4]);

		if(fr->get1bit()) // window switch flag 
		{ 
			int i;
			gr_info->block_type       = fr->getbits_fast(2);
			gr_info->mixed_block_flag = fr->get1bit();
			gr_info->table_select[0]  = fr->getbits_fast(5);
			gr_info->table_select[1]  = fr->getbits_fast(5);

         gr_info->table_select[2] = 0;
			for(i=0;i<3;i++)
         gr_info->full_gain[i] = gr_info->pow2gain + (fr->getbits_fast(3)<<3);

			if(gr_info->block_type == 0)
			{
				printf("Blocktype == 0 and window-switching == 1 not allowed.\n");
				return 1;
			}

			/* region_count/start parameters are implicit in this case. */       
         if( (!fr->lsf || (gr_info->block_type == 2)) && !fr->mMpeg25)
			{
				gr_info->region1start = 36>>1;
				gr_info->region2start = 576>>1;
			}
			else
			{
            if(fr->mMpeg25)
				{ 
					int r0c,r1c;
					if((gr_info->block_type == 2) && (!gr_info->mixed_block_flag) ) r0c = 5;
					else r0c = 7;

					// r0c+1+r1c+1 == 22, always.
					r1c = 20 - r0c;
					gr_info->region1start = bandInfo[sfreq].longIdx[r0c+1] >> 1 ;
					gr_info->region2start = bandInfo[sfreq].longIdx[r0c+1+r1c+1] >> 1; 
				}
				else
				{
					gr_info->region1start = 54>>1;
					gr_info->region2start = 576>>1; 
				} 
			}
		}
		else
		{
			int i,r0c,r1c;
			for (i=0; i<3; i++)
			gr_info->table_select[i] = fr->getbits_fast(5);

			r0c = fr->getbits_fast(4); // 0 .. 15 
			r1c = fr->getbits_fast(3); // 0 .. 7
			gr_info->region1start = bandInfo[sfreq].longIdx[r0c+1] >> 1 ;

			// max(r0c+r1c+2) = 15+7+2 = 24
			if(r0c+1+r1c+1 > 22) gr_info->region2start = 576>>1;
			else gr_info->region2start = bandInfo[sfreq].longIdx[r0c+1+r1c+1] >> 1;

			gr_info->block_type = 0;
			gr_info->mixed_block_flag = 0;
		}
		if(!fr->lsf) gr_info->preflag = fr->get1bit();

		gr_info->scalefac_scale = fr->get1bit();
		gr_info->count1table_select = fr->get1bit();
	}
	return 0;
}


// read scalefactors
int Layer3::getScaleFactors1(MpegStream *fr, int *scf, GranulesInfo *gr_info)
{
	const unsigned char slen[2][16] =
	{
		{0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4},
		{0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3}
	};
	int numbits;
	int num0 = slen[0][gr_info->scalefac_compress];
	int num1 = slen[1][gr_info->scalefac_compress];

	if(gr_info->block_type == 2)
	{
		int i=18;
		numbits = (num0 + num1) * 18;

		if(gr_info->mixed_block_flag)
		{
			for (i=8;i;i--)
			*scf++ = fr->getbits_fast(num0);

			i = 9;
			numbits -= num0; /* num0 * 17 + num1 * 18 */
		}

		for(;i;i--) *scf++ = fr->getbits_fast(num0);

		for(i = 18; i; i--) *scf++ = fr->getbits_fast(num1);

		*scf++ = 0; *scf++ = 0; *scf++ = 0; /* short[13][0..2] = 0 */
	}
	else
	{
		int i;
		int scfsi = gr_info->scfsi;

		if(scfsi < 0)
		{ // scfsi < 0 => granule == 0 
			for(i=11;i;i--) *scf++ = fr->getbits_fast(num0);

			for(i=10;i;i--) *scf++ = fr->getbits_fast(num1);

			numbits = (num0 + num1) * 10 + num0;
			*scf++ = 0;
		}
		else
		{
			numbits = 0;
			if(!(scfsi & 0x8))
			{
				for (i=0;i<6;i++) *scf++ = fr->getbits_fast(num0);

				numbits += num0 * 6;
			}
			else scf += 6; 

			if(!(scfsi & 0x4))
			{
				for (i=0;i<5;i++) *scf++ = fr->getbits_fast(num0);

				numbits += num0 * 5;
			}
			else scf += 5;

			if(!(scfsi & 0x2))
			{
				for(i=0;i<5;i++) *scf++ = fr->getbits_fast(num1);

				numbits += num1 * 5;
			}
			else scf += 5;

			if(!(scfsi & 0x1))
			{
				for (i=0;i<5;i++) *scf++ = fr->getbits_fast(num1);

				numbits += num1 * 5;
			}
			else scf += 5;

			*scf++ = 0;
		}
	}
	return numbits;
}


int Layer3::getScaleFactors2(MpegStream *fr, int *scf, GranulesInfo *gr_info, int i_stereo)
{
	const unsigned char *pnt;
	int i,j,n=0,numbits=0;
	unsigned int slen;

	const unsigned char stab[3][6][4] =
	{
		{  { 6, 5, 5,5 } , { 6, 5, 7,3 } , { 11,10,0,0},
			{ 7, 7, 7,0 } , { 6, 6, 6,3 } , {  8, 8,5,0}  },
		{  { 9, 9, 9,9 } , { 9, 9,12,6 } , { 18,18,0,0},
         {12,12,12,0 } , {12, 9, 9,6 } , { 15,12,9,0}  },
		{  { 6, 9, 9,9 } , { 6, 9,12,6 } , { 15,18,0,0},
			{ 6,15,12,0 } , { 6,12, 9,6 } , {  6,18,9,0}  }
	}; 

	if(i_stereo) // i_stereo AND second channel -> do_layer3() checks this
	   slen = i_slen2[gr_info->scalefac_compress>>1];
	else
	   slen = n_slen2[gr_info->scalefac_compress];

	gr_info->preflag = (slen>>15) & 0x1;

	n = 0;  
	if( gr_info->block_type == 2 )
	{
		n++;
		if(gr_info->mixed_block_flag) n++;
	}

	pnt = stab[n][(slen>>12)&0x7];

	for(i=0;i<4;i++)
	{
		int num = slen & 0x7;
		slen >>= 3;
		if(num)
		{
			for(j=0;j<(int)(pnt[i]);j++) *scf++ = fr->getbits_fast(num);

			numbits += pnt[i] * num;
		}
		else
		for(j=0;j<(int)(pnt[i]);j++) *scf++ = 0;
	}
  
	n = (n << 1) + 1;
	for(i=0;i<n;i++) *scf++ = 0;

	return numbits;
}

static unsigned char pretab_choice[2][22] =
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,3,2,0}
};

//	Dequantize samples / Huffman decoding
#define BITSHIFT ((sizeof(long)-1)*8)
#define REFRESH_MASK \
	while(num < BITSHIFT) { \
		mask |= ((unsigned long)fr->getbyte())<<(BITSHIFT-num); \
		num += 8; \
		part2remain -= 8; }

int Layer3::dequantizeSample(MpegStream *fr, float xr[SBLIMIT][SSLIMIT],int *scf, GranulesInfo *gr_info,int sfreq,int part2bits)
{
	int shift = 1 + gr_info->scalefac_scale;
	float *xrpnt = (float *) xr;
	int l[3],l3;
   part2remain = gr_info->part2_3_length - part2bits;
	int *me;

	/* mhipp tree has this split up a bit... */
   unsigned int num=fr->getbitoffset();
	long mask;
	mask  = ((unsigned long) fr->getbits(num))<<BITSHIFT;
	mask <<= 8-num;
	part2remain -= num;

	{
		int bv       = gr_info->big_values;
		int region1  = gr_info->region1start;
		int region2  = gr_info->region2start;
		l3 = ((576>>1)-bv)>>1;   

		/* we may lose the 'odd' bit here !! check this later again */
		if(bv <= region1)
		{
			l[0] = bv;
			l[1] = 0;
			l[2] = 0;
		}
		else
		{
			l[0] = region1;
			if(bv <= region2)
			{
				l[1] = bv - l[0];
				l[2] = 0;
			}
			else
			{
				l[1] = region2 - l[0];
				l[2] = bv - region2;
			}
		}
	}
 
	if(gr_info->block_type == 2)
	{
		/* decoding with short or mixed mode BandIndex table */
		int i,max[4];
		int step=0,lwin=3,cb=0;
      float v = 0.0;
      int *m,mc;

		if(gr_info->mixed_block_flag)
		{
			max[3] = -1;
			max[0] = max[1] = max[2] = 2;
			m = map[sfreq][0];
			me = mapend[sfreq][0];
		}
		else
		{
			max[0] = max[1] = max[2] = max[3] = -1;
			/* max[3] not really needed in this case */
			m = map[sfreq][1];
			me = mapend[sfreq][1];
		}

		mc = 0;
		for(i=0;i<2;i++)
		{
			int lp = l[i];
			const struct newhuff *h = ht+gr_info->table_select[i];
			for(;lp;lp--,mc--)
			{
            int x,y;
				if( (!mc) )
				{
					mc    = *m++;
					xrpnt = ((float *) xr) + (*m++);
					lwin  = *m++;
					cb    = *m++;
					if(lwin == 3)
					{
                  v = gr_info->pow2gain[(*scf++) << shift];
						step = 1;
					}
					else
					{
						v = gr_info->full_gain[lwin][(*scf++) << shift];
						step = 3;
					}
				}
				{
					const short *val = h->table;
					REFRESH_MASK;
					while((y=*val++)<0)
					{
						if (mask < 0) val -= y;

						num--;
						mask <<= 1;
					}
					x = y >> 4;
					y &= 0xf;
				}
				if(x == 15 && h->linbits)
				{
					max[lwin] = cb;
					REFRESH_MASK;
					x += ((unsigned long) mask) >> (BITSHIFT+8-h->linbits);
					num -= h->linbits+1;
					mask <<= h->linbits;
					if(mask < 0) *xrpnt = REAL_MUL_SCALE_LAYER3(-ispow[x], v, gainpow2_scale_idx);
					else         *xrpnt = REAL_MUL_SCALE_LAYER3( ispow[x], v, gainpow2_scale_idx);

					mask <<= 1;
				}
				else if(x)
				{
					max[lwin] = cb;
					if(mask < 0) *xrpnt = REAL_MUL_SCALE_LAYER3(-ispow[x], v, gainpow2_scale_idx);
					else         *xrpnt = REAL_MUL_SCALE_LAYER3( ispow[x], v, gainpow2_scale_idx);

					num--;
					mask <<= 1;
				}
				else *xrpnt = (float)(0.0);

				xrpnt += step;
				if(y == 15 && h->linbits)
				{
					max[lwin] = cb;
					REFRESH_MASK;
					y += ((unsigned long) mask) >> (BITSHIFT+8-h->linbits);
					num -= h->linbits+1;
					mask <<= h->linbits;
					if(mask < 0) *xrpnt = REAL_MUL_SCALE_LAYER3(-ispow[y], v, gainpow2_scale_idx);
					else         *xrpnt = REAL_MUL_SCALE_LAYER3( ispow[y], v, gainpow2_scale_idx);

					mask <<= 1;
				}
				else if(y)
				{
					max[lwin] = cb;
					if(mask < 0) *xrpnt = REAL_MUL_SCALE_LAYER3(-ispow[y], v, gainpow2_scale_idx);
					else         *xrpnt = REAL_MUL_SCALE_LAYER3( ispow[y], v, gainpow2_scale_idx);

					num--;
					mask <<= 1;
				}
				else *xrpnt = (float)(0.0);

				xrpnt += step;
			}
		}

		for(;l3 && (part2remain+num > 0);l3--)
		{
			const struct newhuff* h;
			const short* val;
         short a;
			/*
				This is only a humble hack to prevent a special segfault.
				More insight into the real workings is still needed.
				Especially why there are (valid?) files that make xrpnt exceed the array with 4 bytes without segfaulting, more seems to be really bad, though.
			*/
			if(!(xrpnt < &xr[SBLIMIT][0]+5))
			{
				printf("attempted xrpnt overflow (%p !< %p) \n", (void*) xrpnt, (void*) &xr[SBLIMIT][0]);
				return 2;
			}
			h = htc+gr_info->count1table_select;
			val = h->table;

			REFRESH_MASK;
			while((a=*val++)<0)
			{
				if(mask < 0) val -= a;

				num--;
				mask <<= 1;
			}
			if(part2remain+num <= 0)
			{
				num -= part2remain+num;
				break;
			}

			for(i=0;i<4;i++)
			{
				if(!(i & 1))
				{
					if(!mc)
					{
						mc = *m++;
						xrpnt = ((float *) xr) + (*m++);
						lwin = *m++;
						cb = *m++;
						if(lwin == 3)
						{
                     v = gr_info->pow2gain[(*scf++) << shift];
							step = 1;
						}
						else
						{
							v = gr_info->full_gain[lwin][(*scf++) << shift];
							step = 3;
						}
					}
					mc--;
				}
				if( (a & (0x8>>i)) )
				{
					max[lwin] = cb;
               if(part2remain+num <= 0)
					break;

					if(mask < 0) *xrpnt = -REAL_SCALE_LAYER3(v, gainpow2_scale_idx);
					else         *xrpnt =  REAL_SCALE_LAYER3(v, gainpow2_scale_idx);

					num--;
					mask <<= 1;
				}
				else *xrpnt = (float)(0.0);

				xrpnt += step;
			}
		}

		if(lwin < 3)
		{ /* short band? */
			while(1)
			{
				for(;mc > 0;mc--)
				{
					*xrpnt = (float)(0.0); xrpnt += 3; /* short band -> step=3 */
					*xrpnt = (float)(0.0); xrpnt += 3;
				}
				if(m >= me)
				break;

				mc    = *m++;
				xrpnt = ((float *) xr) + *m++;
				if(*m++ == 0)
				break; /* optimize: field will be set to zero at the end of the function */

				m++; /* cb */
			}
		}

		gr_info->maxband[0] = max[0]+1;
		gr_info->maxband[1] = max[1]+1;
		gr_info->maxband[2] = max[2]+1;
		gr_info->maxbandl   = max[3]+1;

		{
			int rmax = max[0] > max[1] ? max[0] : max[1];
			rmax = (rmax > max[2] ? rmax : max[2]) + 1;
         gr_info->maxb = rmax ? fr->layer3.shortLimit[sfreq][rmax] : fr->layer3.longLimit[sfreq][max[3]+1];
		}

	}
	else
	{
		/* decoding with 'long' BandIndex table (block_type != 2) */
		const unsigned char *pretab = pretab_choice[gr_info->preflag];
		int i,max = -1;
		int cb = 0;
		int *m = map[sfreq][2];
      float v = 0.0;
		int mc = 0;

		/* long hash table values */
		for(i=0;i<3;i++)
		{
			int lp = l[i];
			const struct newhuff *h = ht+gr_info->table_select[i];

			for(;lp;lp--,mc--)
			{
				int x,y;
				if(!mc)
				{
					mc = *m++;
					cb = *m++;
#ifdef CUT_SFB21
					if(cb == 21)
						v = 0.0;
					else
#endif
					{
                  v = gr_info->pow2gain[(*(scf++) + (*pretab++)) << shift];
					}
				}
				{
					const short *val = h->table;
					REFRESH_MASK;
					while((y=*val++)<0)
					{
						if (mask < 0) val -= y;

						num--;
						mask <<= 1;
					}
					x = y >> 4;
					y &= 0xf;
				}

				if(x == 15 && h->linbits)
				{
					max = cb;
					REFRESH_MASK;
					x += ((unsigned long) mask) >> (BITSHIFT+8-h->linbits);
					num -= h->linbits+1;
					mask <<= h->linbits;
					if(mask < 0) *xrpnt++ = REAL_MUL_SCALE_LAYER3(-ispow[x], v, gainpow2_scale_idx);
					else         *xrpnt++ = REAL_MUL_SCALE_LAYER3( ispow[x], v, gainpow2_scale_idx);

					mask <<= 1;
				}
				else if(x)
				{
					max = cb;
					if(mask < 0) *xrpnt++ = REAL_MUL_SCALE_LAYER3(-ispow[x], v, gainpow2_scale_idx);
					else         *xrpnt++ = REAL_MUL_SCALE_LAYER3( ispow[x], v, gainpow2_scale_idx);
					num--;

					mask <<= 1;
				}
				else *xrpnt++ = (float)(0.0);

				if(y == 15 && h->linbits)
				{
					max = cb;
					REFRESH_MASK;
					y += ((unsigned long) mask) >> (BITSHIFT+8-h->linbits);
					num -= h->linbits+1;
					mask <<= h->linbits;
					if(mask < 0) *xrpnt++ = REAL_MUL_SCALE_LAYER3(-ispow[y], v, gainpow2_scale_idx);
					else         *xrpnt++ = REAL_MUL_SCALE_LAYER3( ispow[y], v, gainpow2_scale_idx);

					mask <<= 1;
				}
				else if(y)
				{
					max = cb;
					if(mask < 0) *xrpnt++ = REAL_MUL_SCALE_LAYER3(-ispow[y], v, gainpow2_scale_idx);
					else         *xrpnt++ = REAL_MUL_SCALE_LAYER3( ispow[y], v, gainpow2_scale_idx);

					num--;
					mask <<= 1;
				}
				else *xrpnt++ = (float)(0.0);
			}
		}

		/* short (count1table) values */
		for(;l3 && (part2remain+num > 0);l3--)
		{
			const struct newhuff *h = htc+gr_info->count1table_select;
			const short *val = h->table;
         short a;

			REFRESH_MASK;
			while((a=*val++)<0)
			{
				if (mask < 0) val -= a;

				num--;
				mask <<= 1;
			}
			if(part2remain+num <= 0)
			{
				num -= part2remain+num;
				break;
			}

			for(i=0;i<4;i++)
			{
				if(!(i & 1))
				{
					if(!mc)
					{
						mc = *m++;
						cb = *m++;
#ifdef CUT_SFB21
						if(cb == 21)
							v = 0.0;
						else
#endif
						{
                     v = gr_info->pow2gain[((*scf++) + (*pretab++)) << shift];
						}
					}
					mc--;
				}
				if( (a & (0x8>>i)) )
				{
					max = cb;
					if(part2remain+num <= 0)
					break;

					if(mask < 0) *xrpnt++ = -REAL_SCALE_LAYER3(v, gainpow2_scale_idx);
					else         *xrpnt++ =  REAL_SCALE_LAYER3(v, gainpow2_scale_idx);

					num--;
					mask <<= 1;
				}
				else *xrpnt++ = (float)(0.0);
			}
		}

		gr_info->maxbandl = max+1;
      gr_info->maxb = fr->layer3.longLimit[sfreq][gr_info->maxbandl];
	}

	part2remain += num;
	fr->backbits(num);
	num = 0;

	while(xrpnt < &xr[SBLIMIT][0]) 
	*xrpnt++ = (float)(0.0);

	while( part2remain > 16 )
	{
		fr->skipbits(16); /* Dismiss stuffing Bits */
		part2remain -= 16;
	}
	if(part2remain > 0) fr->skipbits(part2remain);
	else if(part2remain < 0)
	{
		return 1; /* -> error */
	}
	return 0;
}


/* calculate real channel values for Joint-I-Stereo-mode */
static void III_i_stereo(float xr_buf[2][SBLIMIT][SSLIMIT],int *scalefac, GranulesInfo *gr_info,int sfreq,int ms_stereo,int lsf)
{
	float (*xr)[SBLIMIT*SSLIMIT] = (float (*)[SBLIMIT*SSLIMIT] ) xr_buf;
   const BandInfoStruct *bi = &bandInfo[sfreq];

	const float *tab1,*tab2;

	int tab;
/* TODO: optimize as static */
	const float *tabs[3][2][2] =
	{ 
		{ { tan1_1,tan2_1 }       , { tan1_2,tan2_2 } },
		{ { pow1_1[0],pow2_1[0] } , { pow1_2[0],pow2_2[0] } },
		{ { pow1_1[1],pow2_1[1] } , { pow1_2[1],pow2_2[1] } }
	};

	tab = lsf + (gr_info->scalefac_compress & lsf);
	tab1 = tabs[tab][ms_stereo][0];
	tab2 = tabs[tab][ms_stereo][1];

	if(gr_info->block_type == 2)
	{
		int lwin,do_l = 0;
		if( gr_info->mixed_block_flag ) do_l = 1;

		for(lwin=0;lwin<3;lwin++)
		{ /* process each window */
			/* get first band with zero values */
			int is_p,sb,idx,sfb = gr_info->maxband[lwin];  /* sfb is minimal 3 for mixed mode */
			if(sfb > 3) do_l = 0;

			for(;sfb<12;sfb++)
			{
				is_p = scalefac[sfb*3+lwin-gr_info->mixed_block_flag]; /* scale: 0-15 */ 
				if(is_p != 7)
				{
					float t1,t2;
					sb  = bi->shortDiff[sfb];
					idx = bi->shortIdx[sfb] + lwin;
					t1  = tab1[is_p]; t2 = tab2[is_p];
					for (; sb > 0; sb--,idx+=3)
					{
						float v = xr[0][idx];
						xr[0][idx] = REAL_MUL_15(v, t1);
                  xr[1][idx] = REAL_MUL_15(v, t2);
					}
				}
			}

#if 1
// in the original: copy 10 to 11 , here: copy 11 to 12 , maybe still wrong??? (copy 12 to 13?)
			is_p = scalefac[11*3+lwin-gr_info->mixed_block_flag]; /* scale: 0-15 */
			sb   = bi->shortDiff[12];
			idx  = bi->shortIdx[12] + lwin;
#else
			is_p = scalefac[10*3+lwin-gr_info->mixed_block_flag]; /* scale: 0-15 */
			sb   = bi->shortDiff[11];
			idx  = bi->shortIdx[11] + lwin;
#endif
			if(is_p != 7)
			{
				float t1,t2;
				t1 = tab1[is_p]; t2 = tab2[is_p];
				for( ; sb > 0; sb--,idx+=3 )
				{  
					float v = xr[0][idx];
					xr[0][idx] = REAL_MUL_15(v, t1);
					xr[1][idx] = REAL_MUL_15(v, t2);
				}
			}
		} /* end for(lwin; .. ; . ) */

		/* also check l-part, if ALL bands in the three windows are 'empty' and mode = mixed_mode */
		if(do_l)
		{
			int sfb = gr_info->maxbandl;
			int idx;
			if(sfb > 21) return; /* similarity fix related to CVE-2006-1655 */

			idx = bi->longIdx[sfb];
			for( ; sfb<8; sfb++ )
			{
				int sb = bi->longDiff[sfb];
				int is_p = scalefac[sfb]; /* scale: 0-15 */
				if(is_p != 7)
				{
					float t1,t2;
					t1 = tab1[is_p]; t2 = tab2[is_p];
					for( ; sb > 0; sb--,idx++)
					{
						float v = xr[0][idx];
						xr[0][idx] = REAL_MUL_15(v, t1);
						xr[1][idx] = REAL_MUL_15(v, t2);
					}
				}
				else idx += sb;
			}
		}     
	} 
	else
	{ /* ((gr_info->block_type != 2)) */
		int sfb = gr_info->maxbandl;
		int is_p,idx;
		if(sfb > 21) return; /* tightened fix for CVE-2006-1655 */

		idx = bi->longIdx[sfb];
		for ( ; sfb<21; sfb++)
		{
			int sb = bi->longDiff[sfb];
			is_p = scalefac[sfb]; /* scale: 0-15 */
			if(is_p != 7)
			{
				float t1,t2;
				t1 = tab1[is_p]; t2 = tab2[is_p];
				for( ; sb > 0; sb--,idx++)
				{
					 float v = xr[0][idx];
					 xr[0][idx] = REAL_MUL_15(v, t1);
					 xr[1][idx] = REAL_MUL_15(v, t2);
				}
			}
			else idx += sb;
		}

		is_p = scalefac[20];
		if(is_p != 7)
		{  /* copy l-band 20 to l-band 21 */
			int sb;
			float t1 = tab1[is_p],t2 = tab2[is_p]; 

			for( sb = bi->longDiff[21]; sb > 0; sb--,idx++ )
			{
				float v = xr[0][idx];
				xr[0][idx] = REAL_MUL_15(v, t1);
				xr[1][idx] = REAL_MUL_15(v, t2);
			}
		}
	}
}


static void III_antialias(float xr[SBLIMIT][SSLIMIT], GranulesInfo *gr_info)
{
	int sblim;

	if(gr_info->block_type == 2)
	{
			if(!gr_info->mixed_block_flag) return;

			sblim = 1; 
	}
	else sblim = gr_info->maxb-1;

	/* 31 alias-reduction operations between each pair of sub-bands */
	/* with 8 butterflies between each pair                         */

	{
		int sb;
		float *xr1=(float *) xr[1];

		for(sb=sblim; sb; sb--,xr1+=10)
		{
			int ss;
			float *cs=aa_cs,*ca=aa_ca;
			float *xr2 = xr1;

			for(ss=7;ss>=0;ss--)
			{ /* upper and lower butterfly inputs */
            float bu = *--xr2,bd = *xr1;
            *xr2   = (bu * (*cs)) - (bd * (*ca));
            *xr1++ = (bd * (*cs++)) + (bu * (*ca++));
			}
		}
	}
}

/* 
	This is an optimized DCT from Jeff Tsay's maplay 1.2+ package.
	Saved one multiplication by doing the 'twiddle factor' stuff
	together with the window mul. (MH)

	This uses Byeong Gi Lee's Fast Cosine Transform algorithm, but the
	9 point IDCT needs to be reduced further. Unfortunately, I don't
	know how to do that, because 9 is not an even number. - Jeff.

	Original Message:

	9 Point Inverse Discrete Cosine Transform

	This piece of code is Copyright 1997 Mikko Tommila and is freely usable
	by anybody. The algorithm itself is of course in the public domain.

	Again derived heuristically from the 9-point WFTA.

	The algorithm is optimized (?) for speed, not for small rounding errors or
	good readability.

	36 additions, 11 multiplications

	Again this is very likely sub-optimal.

	The code is optimized to use a minimum number of temporary variables,
	so it should compile quite well even on 8-register Intel x86 processors.
	This makes the code quite obfuscated and very difficult to understand.

	References:
	[1] S. Winograd: "On Computing the Discrete Fourier Transform",
	    Mathematics of Computation, Volume 32, Number 141, January 1978,
	    Pages 175-199
*/


static void III_hybrid(float fsIn[SBLIMIT][SSLIMIT], float tsOut[SSLIMIT][SBLIMIT], int ch, GranulesInfo *gr_info, MpegStream *fr)
{
	float (*block)[2][SBLIMIT*SSLIMIT] = fr->hybrid_block;
	int *blc = fr->hybrid_blc;

	float *tspnt = (float *) tsOut;
	float *rawout1,*rawout2;
	int bt = 0;
	size_t sb = 0;

	{
		int b = blc[ch];
		rawout1=block[b][ch];
		b=-b+1;
		rawout2=block[b][ch];
		blc[ch] = b;
	}
  
	if(gr_info->mixed_block_flag)
	{
		sb = 2;
		dct36(fsIn[0],rawout1,rawout2,win[0],tspnt);
		dct36(fsIn[1],rawout1+18,rawout2+18,win1[0],tspnt+1);
		rawout1 += 36; rawout2 += 36; tspnt += 2;
	}
 
	bt = gr_info->block_type;
	if(bt == 2)
	{
		for(; sb<gr_info->maxb; sb+=2,tspnt+=2,rawout1+=36,rawout2+=36)
		{
			dct12(fsIn[sb]  ,rawout1   ,rawout2   ,win[2] ,tspnt);
			dct12(fsIn[sb+1],rawout1+18,rawout2+18,win1[2],tspnt+1);
		}
	}
	else
	{
		for(; sb<gr_info->maxb; sb+=2,tspnt+=2,rawout1+=36,rawout2+=36)
		{
			dct36(fsIn[sb],rawout1,rawout2,win[bt],tspnt);
			dct36(fsIn[sb+1],rawout1+18,rawout2+18,win1[bt],tspnt+1);
		}
	}

	for(;sb<SBLIMIT;sb++,tspnt++)
	{
		int i;
		for(i=0;i<SSLIMIT;i++)
		{
			tspnt[i*SBLIMIT] = *rawout1++;
			*rawout2++ = (float)(0.0);
		}
	}
}


/* And at the end... the main layer3 handler */
int Layer3::process(MpegStream *fr)
{
	int gr, ch, ss,clip=0;
	int scalefacs[2][39]; /* max 39 for short[13][3] mode, mixed: 38, long: 22 */
   SideInfo sideinfo;
	int single;
   int stereo;
	int ms_stereo,i_stereo;
   int sfreq = fr->mSampleRate;
	int stereo1,granules;

	if(fr->mChannels == 1)
	{ 
      stereo= 1;
		stereo1 = 1;
		single = 0;
	}
	else
   {
      stereo= 2;
      single= SINGLE_STEREO;
	   stereo1 = 2;
   }

   if(fr->mMode == MPG_MD_JOINT_STEREO)
	{
      ms_stereo = (fr->mModeExt & 0x2)>>1;
      i_stereo  = fr->mModeExt & 0x1;
	}
	else 
      ms_stereo = i_stereo = 0;

	granules = fr->lsf ? 1 : 2;

	/* quick hack to keep the music playing */
	/* after having seen this nasty test file... */
   if(getSideInfo(fr, &sideinfo, stereo, ms_stereo, sfreq, single))
	{
		return clip;
	}

	fr->set_pointer(sideinfo.main_data_begin);

	for(gr=0;gr<granules;gr++)
	{
      /*  hybridIn[2][SBLIMIT][SSLIMIT] */
      float (*hybridIn)[SBLIMIT][SSLIMIT] = fr->layer3.hybrid_in;
      /*  hybridOut[2][SSLIMIT][SBLIMIT] */
      float (*hybridOut)[SSLIMIT][SBLIMIT] = fr->layer3.hybrid_out;

      {
         GranulesInfo *gr_info = &(sideinfo.ch[0].gr[gr]);
			long part2bits;
			if(fr->lsf)
            part2bits = getScaleFactors2(fr, scalefacs[0],gr_info,0);
			else
            part2bits = getScaleFactors1(fr, scalefacs[0],gr_info);

         if(dequantizeSample(fr, hybridIn[0], scalefacs[0],gr_info,sfreq,part2bits))
			{
				return clip;
			}
		}

		if(stereo == 2)
		{
         GranulesInfo *gr_info = &(sideinfo.ch[1].gr[gr]);
			long part2bits;
			if(fr->lsf) 
         part2bits = getScaleFactors2(fr, scalefacs[1],gr_info,i_stereo);
			else
         part2bits = getScaleFactors1(fr, scalefacs[1],gr_info);

         if(dequantizeSample(fr, hybridIn[1],scalefacs[1],gr_info,sfreq,part2bits))
			{
				return clip;
			}

			if(ms_stereo)
			{
				int i;
				unsigned int maxb = sideinfo.ch[0].gr[gr].maxb;
				if(sideinfo.ch[1].gr[gr].maxb > maxb) maxb = sideinfo.ch[1].gr[gr].maxb;

				for(i=0;i<SSLIMIT*(int)maxb;i++)
				{
               float tmp0 = ((float *)hybridIn[0])[i];
               float tmp1 = ((float *)hybridIn[1])[i];
               ((float *)hybridIn[0])[i] = tmp0 + tmp1;
               ((float *)hybridIn[1])[i] = tmp0 - tmp1;
				}
			}

         if(i_stereo) III_i_stereo(hybridIn,scalefacs[1],gr_info,sfreq,ms_stereo,fr->lsf);

			if(ms_stereo || i_stereo  )
			{
				if(gr_info->maxb > sideinfo.ch[0].gr[gr].maxb) 
				   sideinfo.ch[0].gr[gr].maxb = gr_info->maxb;
				else
				   gr_info->maxb = sideinfo.ch[0].gr[gr].maxb;
			}

		}

		for(ch=0;ch<stereo1;ch++)
		{
         GranulesInfo *gr_info = &(sideinfo.ch[ch].gr[gr]);
         III_antialias(hybridIn[ch],gr_info);
         III_hybrid(hybridIn[ch], hybridOut[ch], ch,gr_info, fr);
		}

		for(ss=0;ss<SSLIMIT;ss++)
		{
			if(single != SINGLE_STEREO)
            clip += synth_1to1_mono(hybridOut[0][ss], fr);
			else
            clip += synth_stereo_wrap(hybridOut[0][ss], hybridOut[1][ss], fr);

		}
	}
  
	return clip;
}
