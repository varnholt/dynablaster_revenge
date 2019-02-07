#include "audioformat.h"

void AudioFormat::invalidate()
{
   encoding = 0;
   rate     = 0;
   channels = 0;
}

