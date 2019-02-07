#pragma once

class MpegStream;

#define         SBLIMIT                 32
#define         SCALE_BLOCK             12
#define         SSLIMIT                 18

#define         MPG_MD_JOINT_STEREO     1
#define         MPG_MD_MONO             3

int synth_1to1(float*, int, MpegStream*, int);
int synth_1to1_mono(float*, MpegStream*);
int synth_stereo_wrap(float*, float*, MpegStream*);

void  init_layer12(void);
float* init_layer12_table(float *table, int m);
void  init_layer12_stuff(MpegStream *fr);

void prepare_decode_tables(void);
void make_decode_tables(MpegStream *fr);
