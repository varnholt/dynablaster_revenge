#pragma once

class FrameParameter
{
public:
	int rva; // 0: nothing, 1: radio/mix/track 2: album/audiophile 
	long timeout;
	double outscale;
	long resync_limit;
	long index_size;
	long preframes;

   void default_pars();

};

