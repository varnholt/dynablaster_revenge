#pragma once

#include "decode.h"

class SideInfo;
class GranulesInfo;

class Layer3
{
public:
   Layer3();

   void init();
   void initStuff();
   int process(MpegStream *fr);
   int dequantizeSample(MpegStream *fr, float xr[SBLIMIT][SSLIMIT], int *scf, GranulesInfo *gr_info, int sfreq, int part2bits);
   int getScaleFactors1(MpegStream *fr, int *scf, GranulesInfo *gr_info);
   int getScaleFactors2(MpegStream *fr, int *scf, GranulesInfo *gr_info, int i_stereo);
   int getSideInfo(MpegStream *fr, SideInfo *si, int stereo, int ms_stereo, long sfreq, int single);

   int longLimit[9][23];
   int shortLimit[9][14];
   float gainpow2[256+118+4];

   float (*hybrid_in)[SBLIMIT][SSLIMIT];  /* ALIGNED(16) real hybridIn[2][SBLIMIT][SSLIMIT]; */
   float (*hybrid_out)[SSLIMIT][SBLIMIT]; /* ALIGNED(16) real hybridOut[2][SSLIMIT][SBLIMIT]; */

   int part2remain;
};

