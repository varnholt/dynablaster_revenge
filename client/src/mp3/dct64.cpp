// Calculation of the inverse MDCT in different window sizes
// sincerly ripped from lame

#include "dct64.h"
#include "mpegstream.h"

# define REAL_SCALE_DCT64(x)				(x)

extern float *pnts[5];
extern float ispow[8207];
extern float aa_ca[8],aa_cs[8];
extern float win[4][36];
extern float win1[4][36];
extern float COS6_1,COS6_2;
extern float tfcos36[9];
extern float tfcos12[3];
extern float cos9[3],cos18[3];
extern float tan1_1[16],tan2_1[16],tan1_2[16],tan2_2[16];
extern float pow1_1[2][16],pow2_1[2][16],pow1_2[2][16],pow2_2[2][16];

#define REAL_MUL(x, y)						((x) * (y))

void dct64(float *out0,float *out1,float *samples)
{
  float bufs[64];

 {
  int i,j;
  float *b1,*b2,*bs,*costab;

  b1 = samples;
  bs = bufs;
  costab = pnts[0]+16;
  b2 = b1 + 32;

  for(i=15;i>=0;i--)
    *bs++ = (*b1++ + *--b2); 
  for(i=15;i>=0;i--)
    *bs++ = (*--b2 - *b1++) * (*--costab);

  b1 = bufs;
  costab = pnts[1]+8;
  b2 = b1 + 16;

  {
    for(i=7;i>=0;i--)
      *bs++ = (*b1++ + *--b2); 
    for(i=7;i>=0;i--)
      *bs++ = (*--b2 - *b1++) * (*--costab);
    b2 += 32;
    costab += 8;
    for(i=7;i>=0;i--)
      *bs++ = (*b1++ + *--b2); 
    for(i=7;i>=0;i--)
      *bs++ = (*b1++ - *--b2) * (*--costab);
    b2 += 32;
  }

  bs = bufs;
  costab = pnts[2];
  b2 = b1 + 8;

  for(j=2;j;j--)
  {
    for(i=3;i>=0;i--)
      *bs++ = (*b1++ + *--b2); 
    for(i=3;i>=0;i--)
      *bs++ = (*--b2 - *b1++) * costab[i];
    b2 += 16;
    for(i=3;i>=0;i--)
      *bs++ = (*b1++ + *--b2); 
    for(i=3;i>=0;i--)
      *bs++ = (*b1++ - *--b2) * costab[i];
    b2 += 16;
  }

  b1 = bufs;
  costab = pnts[3];
  b2 = b1 + 4;

  for(j=4;j;j--)
  {
    *bs++ = (*b1++ + *--b2); 
    *bs++ = (*b1++ + *--b2);
    *bs++ = (*--b2 - *b1++) * costab[1];
    *bs++ = (*--b2 - *b1++) * costab[0];
    b2 += 8;
    *bs++ = (*b1++ + *--b2); 
    *bs++ = (*b1++ + *--b2);
    *bs++ = (*b1++ - *--b2) * costab[1];
    *bs++ = (*b1++ - *--b2) * costab[0];
    b2 += 8;
  }
  bs = bufs;
  costab = pnts[4];

  for(j=8;j;j--)
  {
    float v0,v1;
    v0=*b1++; v1 = *b1++;
    *bs++ = (v0 + v1);
    *bs++ = (v0 - v1) * (*costab);
    v0=*b1++; v1 = *b1++;
    *bs++ = (v0 + v1);
    *bs++ = (v1 - v0) * (*costab);
  }

 }


 {
  float *b1;
  int i;

  for(b1=bufs,i=8;i;i--,b1+=4)
    b1[2] += b1[3];

  for(b1=bufs,i=4;i;i--,b1+=8)
  {
    b1[4] += b1[6];
    b1[6] += b1[5];
    b1[5] += b1[7];
  }

  for(b1=bufs,i=2;i;i--,b1+=16)
  {
    b1[8]  += b1[12];
    b1[12] += b1[10];
    b1[10] += b1[14];
    b1[14] += b1[9];
    b1[9]  += b1[13];
    b1[13] += b1[11];
    b1[11] += b1[15];
  }
 }


  out0[0x10*16] = REAL_SCALE_DCT64(bufs[0]);
  out0[0x10*15] = REAL_SCALE_DCT64(bufs[16+0]  + bufs[16+8]);
  out0[0x10*14] = REAL_SCALE_DCT64(bufs[8]);
  out0[0x10*13] = REAL_SCALE_DCT64(bufs[16+8]  + bufs[16+4]);
  out0[0x10*12] = REAL_SCALE_DCT64(bufs[4]);
  out0[0x10*11] = REAL_SCALE_DCT64(bufs[16+4]  + bufs[16+12]);
  out0[0x10*10] = REAL_SCALE_DCT64(bufs[12]);
  out0[0x10* 9] = REAL_SCALE_DCT64(bufs[16+12] + bufs[16+2]);
  out0[0x10* 8] = REAL_SCALE_DCT64(bufs[2]);
  out0[0x10* 7] = REAL_SCALE_DCT64(bufs[16+2]  + bufs[16+10]);
  out0[0x10* 6] = REAL_SCALE_DCT64(bufs[10]);
  out0[0x10* 5] = REAL_SCALE_DCT64(bufs[16+10] + bufs[16+6]);
  out0[0x10* 4] = REAL_SCALE_DCT64(bufs[6]);
  out0[0x10* 3] = REAL_SCALE_DCT64(bufs[16+6]  + bufs[16+14]);
  out0[0x10* 2] = REAL_SCALE_DCT64(bufs[14]);
  out0[0x10* 1] = REAL_SCALE_DCT64(bufs[16+14] + bufs[16+1]);
  out0[0x10* 0] = REAL_SCALE_DCT64(bufs[1]);

  out1[0x10* 0] = REAL_SCALE_DCT64(bufs[1]);
  out1[0x10* 1] = REAL_SCALE_DCT64(bufs[16+1]  + bufs[16+9]);
  out1[0x10* 2] = REAL_SCALE_DCT64(bufs[9]);
  out1[0x10* 3] = REAL_SCALE_DCT64(bufs[16+9]  + bufs[16+5]);
  out1[0x10* 4] = REAL_SCALE_DCT64(bufs[5]);
  out1[0x10* 5] = REAL_SCALE_DCT64(bufs[16+5]  + bufs[16+13]);
  out1[0x10* 6] = REAL_SCALE_DCT64(bufs[13]);
  out1[0x10* 7] = REAL_SCALE_DCT64(bufs[16+13] + bufs[16+3]);
  out1[0x10* 8] = REAL_SCALE_DCT64(bufs[3]);
  out1[0x10* 9] = REAL_SCALE_DCT64(bufs[16+3]  + bufs[16+11]);
  out1[0x10*10] = REAL_SCALE_DCT64(bufs[11]);
  out1[0x10*11] = REAL_SCALE_DCT64(bufs[16+11] + bufs[16+7]);
  out1[0x10*12] = REAL_SCALE_DCT64(bufs[7]);
  out1[0x10*13] = REAL_SCALE_DCT64(bufs[16+7]  + bufs[16+15]);
  out1[0x10*14] = REAL_SCALE_DCT64(bufs[15]);
  out1[0x10*15] = REAL_SCALE_DCT64(bufs[16+15]);

}


void dct36(float *inbuf,float *o1,float *o2,float *wintab,float *tsbuf)
{
   float tmp[18];

   {
      float *in = inbuf;

      in[17]+=in[16]; in[16]+=in[15]; in[15]+=in[14];
      in[14]+=in[13]; in[13]+=in[12]; in[12]+=in[11];
      in[11]+=in[10]; in[10]+=in[9];  in[9] +=in[8];
      in[8] +=in[7];  in[7] +=in[6];  in[6] +=in[5];
      in[5] +=in[4];  in[4] +=in[3];  in[3] +=in[2];
      in[2] +=in[1];  in[1] +=in[0];

      in[17]+=in[15]; in[15]+=in[13]; in[13]+=in[11]; in[11]+=in[9];
      in[9] +=in[7];  in[7] +=in[5];  in[5] +=in[3];  in[3] +=in[1];


#if 1
      {
         float t3;
         {
            float t0, t1, t2;

            t0 = REAL_MUL(COS6_2, (in[8] + in[16] - in[4]));
            t1 = REAL_MUL(COS6_2, in[12]);

            t3 = in[0];
            t2 = t3 - t1 - t1;
            tmp[1] = tmp[7] = t2 - t0;
            tmp[4]          = t2 + t0 + t0;
            t3 += t1;

            t2 = REAL_MUL(COS6_1, (in[10] + in[14] - in[2]));
            tmp[1] -= t2;
            tmp[7] += t2;
         }
         {
            float t0, t1, t2;

            t0 = REAL_MUL(cos9[0], (in[4] + in[8] ));
            t1 = REAL_MUL(cos9[1], (in[8] - in[16]));
            t2 = REAL_MUL(cos9[2], (in[4] + in[16]));

            tmp[2] = tmp[6] = t3 - t0      - t2;
            tmp[0] = tmp[8] = t3 + t0 + t1;
            tmp[3] = tmp[5] = t3      - t1 + t2;
         }
      }
      {
         float t1, t2, t3;

         t1 = REAL_MUL(cos18[0], (in[2]  + in[10]));
         t2 = REAL_MUL(cos18[1], (in[10] - in[14]));
         t3 = REAL_MUL(COS6_1,    in[6]);

         {
            float t0 = t1 + t2 + t3;
            tmp[0] += t0;
            tmp[8] -= t0;
         }

         t2 -= t3;
         t1 -= t3;

         t3 = REAL_MUL(cos18[2], (in[2] + in[14]));

         t1 += t3;
         tmp[3] += t1;
         tmp[5] -= t1;

         t2 -= t3;
         tmp[2] += t2;
         tmp[6] -= t2;
      }

#else
      {
         float t0, t1, t2, t3, t4, t5, t6, t7;

         t1 = REAL_MUL(COS6_2, in[12]);
         t2 = REAL_MUL(COS6_2, (in[8] + in[16] - in[4]));

         t3 = in[0] + t1;
         t4 = in[0] - t1 - t1;
         t5     = t4 - t2;
         tmp[4] = t4 + t2 + t2;

         t0 = REAL_MUL(cos9[0], (in[4] + in[8]));
         t1 = REAL_MUL(cos9[1], (in[8] - in[16]));

         t2 = REAL_MUL(cos9[2], (in[4] + in[16]));

         t6 = t3 - t0 - t2;
         t0 += t3 + t1;
         t3 += t2 - t1;

         t2 = REAL_MUL(cos18[0], (in[2]  + in[10]));
         t4 = REAL_MUL(cos18[1], (in[10] - in[14]));
         t7 = REAL_MUL(COS6_1, in[6]);

         t1 = t2 + t4 + t7;
         tmp[0] = t0 + t1;
         tmp[8] = t0 - t1;
         t1 = REAL_MUL(cos18[2], (in[2] + in[14]));
         t2 += t1 - t7;

         tmp[3] = t3 + t2;
         t0 = REAL_MUL(COS6_1, (in[10] + in[14] - in[2]));
         tmp[5] = t3 - t2;

         t4 -= t1 + t7;

         tmp[1] = t5 - t0;
         tmp[7] = t5 + t0;
         tmp[2] = t6 + t4;
         tmp[6] = t6 - t4;
      }
#endif

      {
         float t0, t1, t2, t3, t4, t5, t6, t7;

         t1 = REAL_MUL(COS6_2, in[13]);
         t2 = REAL_MUL(COS6_2, (in[9] + in[17] - in[5]));

         t3 = in[1] + t1;
         t4 = in[1] - t1 - t1;
         t5 = t4 - t2;

         t0 = REAL_MUL(cos9[0], (in[5] + in[9]));
         t1 = REAL_MUL(cos9[1], (in[9] - in[17]));

         tmp[13] = REAL_MUL((t4 + t2 + t2), tfcos36[17-13]);
         t2 = REAL_MUL(cos9[2], (in[5] + in[17]));

         t6 = t3 - t0 - t2;
         t0 += t3 + t1;
         t3 += t2 - t1;

         t2 = REAL_MUL(cos18[0], (in[3]  + in[11]));
         t4 = REAL_MUL(cos18[1], (in[11] - in[15]));
         t7 = REAL_MUL(COS6_1, in[7]);

         t1 = t2 + t4 + t7;
         tmp[17] = REAL_MUL((t0 + t1), tfcos36[17-17]);
         tmp[9]  = REAL_MUL((t0 - t1), tfcos36[17-9]);
         t1 = REAL_MUL(cos18[2], (in[3] + in[15]));
         t2 += t1 - t7;

         tmp[14] = REAL_MUL((t3 + t2), tfcos36[17-14]);
         t0 = REAL_MUL(COS6_1, (in[11] + in[15] - in[3]));
         tmp[12] = REAL_MUL((t3 - t2), tfcos36[17-12]);

         t4 -= t1 + t7;

         tmp[16] = REAL_MUL((t5 - t0), tfcos36[17-16]);
         tmp[10] = REAL_MUL((t5 + t0), tfcos36[17-10]);
         tmp[15] = REAL_MUL((t6 + t4), tfcos36[17-15]);
         tmp[11] = REAL_MUL((t6 - t4), tfcos36[17-11]);
      }

#define MACRO(v) { \
      float tmpval; \
      tmpval = tmp[(v)] + tmp[17-(v)]; \
      out2[9+(v)] = REAL_MUL(tmpval, w[27+(v)]); \
      out2[8-(v)] = REAL_MUL(tmpval, w[26-(v)]); \
      tmpval = tmp[(v)] - tmp[17-(v)]; \
      ts[SBLIMIT*(8-(v))] = out1[8-(v)] + REAL_MUL(tmpval, w[8-(v)]); \
      ts[SBLIMIT*(9+(v))] = out1[9+(v)] + REAL_MUL(tmpval, w[9+(v)]); }

      {
         float *out2 = o2;
         float *w = wintab;
         float *out1 = o1;
         float *ts = tsbuf;

         MACRO(0);
         MACRO(1);
         MACRO(2);
         MACRO(3);
         MACRO(4);
         MACRO(5);
         MACRO(6);
         MACRO(7);
         MACRO(8);
      }

   }
}


void dct12(float *in, float *rawout1, float *rawout2, float *wi, float *ts)
{
#define DCT12_PART1 \
   in5 = in[5*3];  \
   in5 += (in4 = in[4*3]); \
   in4 += (in3 = in[3*3]); \
   in3 += (in2 = in[2*3]); \
   in2 += (in1 = in[1*3]); \
   in1 += (in0 = in[0*3]); \
   \
   in5 += in3; in3 += in1; \
   \
   in2 = REAL_MUL(in2, COS6_1); \
   in3 = REAL_MUL(in3, COS6_1);

#define DCT12_PART2 \
   in0 += REAL_MUL(in4, COS6_2); \
   \
   in4 = in0 + in2; \
   in0 -= in2;      \
   \
   in1 += REAL_MUL(in5, COS6_2); \
   \
   in5 = REAL_MUL((in1 + in3), tfcos12[0]); \
   in1 = REAL_MUL((in1 - in3), tfcos12[2]); \
   \
   in3 = in4 + in5; \
   in4 -= in5;      \
   \
   in2 = in0 + in1; \
   in0 -= in1;

   {
      float in0,in1,in2,in3,in4,in5;
      float *out1 = rawout1;
      ts[SBLIMIT*0] = out1[0]; ts[SBLIMIT*1] = out1[1]; ts[SBLIMIT*2] = out1[2];
      ts[SBLIMIT*3] = out1[3]; ts[SBLIMIT*4] = out1[4]; ts[SBLIMIT*5] = out1[5];

      DCT12_PART1

      {
         float tmp0,tmp1 = (in0 - in4);
         {
            float tmp2 = REAL_MUL((in1 - in5), tfcos12[1]);
            tmp0 = tmp1 + tmp2;
            tmp1 -= tmp2;
         }
         ts[(17-1)*SBLIMIT] = out1[17-1] + REAL_MUL(tmp0, wi[11-1]);
         ts[(12+1)*SBLIMIT] = out1[12+1] + REAL_MUL(tmp0, wi[6+1]);
         ts[(6 +1)*SBLIMIT] = out1[6 +1] + REAL_MUL(tmp1, wi[1]);
         ts[(11-1)*SBLIMIT] = out1[11-1] + REAL_MUL(tmp1, wi[5-1]);
      }

      DCT12_PART2

      ts[(17-0)*SBLIMIT] = out1[17-0] + REAL_MUL(in2, wi[11-0]);
      ts[(12+0)*SBLIMIT] = out1[12+0] + REAL_MUL(in2, wi[6+0]);
      ts[(12+2)*SBLIMIT] = out1[12+2] + REAL_MUL(in3, wi[6+2]);
      ts[(17-2)*SBLIMIT] = out1[17-2] + REAL_MUL(in3, wi[11-2]);

      ts[(6 +0)*SBLIMIT]  = out1[6+0] + REAL_MUL(in0, wi[0]);
      ts[(11-0)*SBLIMIT] = out1[11-0] + REAL_MUL(in0, wi[5-0]);
      ts[(6 +2)*SBLIMIT]  = out1[6+2] + REAL_MUL(in4, wi[2]);
      ts[(11-2)*SBLIMIT] = out1[11-2] + REAL_MUL(in4, wi[5-2]);
   }

   in++;

   {
      float in0,in1,in2,in3,in4,in5;
      float *out2 = rawout2;

      DCT12_PART1

      {
         float tmp0,tmp1 = (in0 - in4);
         {
            float tmp2 = REAL_MUL((in1 - in5), tfcos12[1]);
            tmp0 = tmp1 + tmp2;
            tmp1 -= tmp2;
         }
         out2[5-1] = REAL_MUL(tmp0, wi[11-1]);
         out2[0+1] = REAL_MUL(tmp0, wi[6+1]);
         ts[(12+1)*SBLIMIT] += REAL_MUL(tmp1, wi[1]);
         ts[(17-1)*SBLIMIT] += REAL_MUL(tmp1, wi[5-1]);
      }

      DCT12_PART2

      out2[5-0] = REAL_MUL(in2, wi[11-0]);
      out2[0+0] = REAL_MUL(in2, wi[6+0]);
      out2[0+2] = REAL_MUL(in3, wi[6+2]);
      out2[5-2] = REAL_MUL(in3, wi[11-2]);

      ts[(12+0)*SBLIMIT] += REAL_MUL(in0, wi[0]);
      ts[(17-0)*SBLIMIT] += REAL_MUL(in0, wi[5-0]);
      ts[(12+2)*SBLIMIT] += REAL_MUL(in4, wi[2]);
      ts[(17-2)*SBLIMIT] += REAL_MUL(in4, wi[5-2]);
   }

   in++;

   {
      float in0,in1,in2,in3,in4,in5;
      float *out2 = rawout2;
      out2[12]=out2[13]=out2[14]=out2[15]=out2[16]=out2[17]=0.0;

      DCT12_PART1

      {
         float tmp0,tmp1 = (in0 - in4);
         {
            float tmp2 = REAL_MUL((in1 - in5), tfcos12[1]);
            tmp0 = tmp1 + tmp2;
            tmp1 -= tmp2;
         }
         out2[11-1] = REAL_MUL(tmp0, wi[11-1]);
         out2[6 +1] = REAL_MUL(tmp0, wi[6+1]);
         out2[0+1] += REAL_MUL(tmp1, wi[1]);
         out2[5-1] += REAL_MUL(tmp1, wi[5-1]);
      }

      DCT12_PART2

      out2[11-0] = REAL_MUL(in2, wi[11-0]);
      out2[6 +0] = REAL_MUL(in2, wi[6+0]);
      out2[6 +2] = REAL_MUL(in3, wi[6+2]);
      out2[11-2] = REAL_MUL(in3, wi[11-2]);

      out2[0+0] += REAL_MUL(in0, wi[0]);
      out2[5-0] += REAL_MUL(in0, wi[5-0]);
      out2[0+2] += REAL_MUL(in4, wi[2]);
      out2[5-2] += REAL_MUL(in4, wi[5-2]);
   }
}

