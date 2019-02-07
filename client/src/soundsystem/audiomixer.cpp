#include "audiomixer.h"
#include "audiosample.h"
#include "sounddevice.h"
#include <memory.h>
#include <math.h>
#include <stdio.h>

AudioMixer::AudioMixer()
: mFrequency(SoundDeviceInterface::Instance()->getFrequency())
{
   mRampSteps= mFrequency * MIXER_VOLUMERAMP_STEPS / 44100;
   mInvRampSteps= 1.0f / mRampSteps;
}

MixerChannel* AudioMixer::addChannel()
{
   MixerChannel* chn= new MixerChannel();
   memset(chn, 0, sizeof(MixerChannel));
   mChannels.add(chn);
   return chn;
}

bool AudioMixer::removeChannel(MixerChannel *channel)
{
   bool res= mChannels.remove(channel);
   if (res) // TODO: why?
      delete channel;
   return res;
}

/*
static short ftoi16(float x)
{
   union
   {
      float f;
      int i;
   } u_fi;

   u_fi.f = x + 12582912.0f; // Magic Number: 2^23 + 2^22
   return (short)u_fi.i;
}
*/

int max= 0;
void AudioMixer::convert(short *dst, float *src, int len)
{
   // TODO: add volume parameter
   const float scale= 1.0f; // 32768.0f;

   if (len <=0 || !dst || !src) 
      return;

   for (int count=0; count<len<<1; count++)
   {
      int val= (int) (src[count] * scale);

      dst[count]= (val < -32768 ? -32768 : val > 32767 ? 32767 : val);
   }

}

// local enum:
typedef enum{
   eMixEndCause_Buffer=0,
   eMixEndCause_Sample,
   eMixEndCause_MinimumStep,
} eMixEndCause;


void AudioMixer::processLoop(long long& lmixpos, AudioSample *pSample, MixerChannel* pChannel)
{
   switch ( pSample->mLoopMode )
   {
      case AudioSample::NormalLoop:
      {
         // we really got to do that to manage the right frequency, in short loop cases with high freq :
         int ppos = (int)(lmixpos >> 32 & 0xffffffff);
         int end = pSample->mLoopStart + pSample->mLoopLen;
         if (ppos >= end)
         {
            while(ppos>=end)
               ppos -= pSample->mLoopLen;

            if(ppos < pSample->mLoopStart)
               ppos =pSample->mLoopStart; // happens when loopstart=0 and low bits left.

            lmixpos &= (long long)(0x00000000ffffffffULL);
            lmixpos |= (((long long)ppos)<<32); 
         }
         break;
      }
      
      case AudioSample::BiDirLoop:
      {
         int loopend= pSample->mLoopStart+pSample->mLoopLen;
         int loopstart= pSample->mLoopStart;
         int pos= (int) (lmixpos >> 32 & 0xffffffff);

         while ( true )
         {
            if( pChannel->SpeedDir() == Forwards )
            {
               if (pos>=loopend)
               {
                  unsigned int frac= (unsigned int) (lmixpos & 0xffffffff);
                  lmixpos= ((long long) (loopend-(pos-loopend)-1) << 32) | ((~frac) & 0xffffffff);
                  pos= (int) (lmixpos >> 32 & 0xffffffff);
                  pChannel->SetSpeedDir( Backwards );
               }
               else
                  break;
            }

            if( pChannel->SpeedDir() == Backwards )
            {
               if (pos < loopstart)
               {
                  unsigned int frac= (unsigned int) (lmixpos & 0xffffffff);
                  lmixpos= ( (long long) (loopstart-1+(loopstart-pos)) << 32) | ((~frac) & 0xffffffff);
                  pos= (int) (lmixpos >> 32 & 0xffffffff);
                  pChannel->SetSpeedDir( Forwards );
               }
               else
                  break;
            }
         }
      }

      case AudioSample::NoLoop:
      {
         int ppos = (int)(lmixpos >> 32 & 0xffffffff);
         int end = pSample->mLoopStart + pSample->mLoopLen;
         if (ppos>=end)
         {
            lmixpos= ((long long) (pSample->mLoopStart + pSample->mLoopLen - 1)) << 32;
            pChannel->mSpeed= 0;
         }
         break;
      }
   }
}

void AudioMixer::process(float *mixptr, int numsamples) 
{
   static const float mix_div255 = 1.0f/255.0f;
   static const float mix_VolRamp = (256.0f*255.0f) ;
   static const float mix_divVolRamp = 1.0f/(256.0f*255.0f) ;

   if (numsamples == 0) return;

   // loop through channels. there are 2 channels by track.
   for (int chan = 0; chan < mChannels.size() ; chan++)
   {
      int size= numsamples;
      long long lmixpos;
      unsigned int iLeftToMix,iLeftToMix_old;		

      MixerChannel *pChannel = mChannels[chan];
      if (pChannel->isMuted())
         continue;

      AudioSample *pSample = pChannel->Sample();
      if(!pSample) continue;

      // channel is off
/*
      if (  (pChannel->mLeftVolume == 0 && pChannel->mRampLeftTarget == 0 && pChannel->mRampLeftSpeed < 0) 
         && (pChannel->mRightVolume == 0 && pChannel->mRampRightTarget == 0 && pChannel->mRampRightSpeed < 0))
         continue;
*/

      float *pFloatBuffer= (float *)mixptr;

      unsigned int iRampCount = 0;
      //Calculate Loop Count:
      lmixpos= pChannel->mMixPos;

      while(size)
      {
         long long sampleLeftToMix;	// hhhhllll
         long long sampleSpeed;

         // if the volume has changed, end condition equals a volume ramp
         float	mix_rampspeedleft=0.0f;
         float	mix_rampspeedright=0.0f;
         eMixEndCause mix_endflag = eMixEndCause_Buffer;
         char silent=0;
         iLeftToMix = size;

         sampleSpeed= pChannel->mSpeed;

         processLoop(lmixpos, pSample, pChannel);

         if( pChannel->SpeedDir() == Forwards )
         {
            // work out how many samples left from mixpos to loop end
            long long looplen= (long long)(pSample->mLoopStart + pSample->mLoopLen) << 32;
            sampleLeftToMix= looplen - lmixpos;
         } 
         else // backwards
         {	
            // work out how many samples left from mixpos to loop start
            long long loopStart= (long long)pSample->mLoopStart << 32;
            sampleLeftToMix= lmixpos - loopStart;
         }

         if (sampleLeftToMix < 0)
         {
            sampleLeftToMix= 0;
            continue;
         }
         
         {
            unsigned int sampleleft= 0;
            if (sampleSpeed != 0)
            {
               sampleLeftToMix = (long long) (sampleLeftToMix * 65536.0 * 65536.0 / sampleSpeed);
               sampleleft = (unsigned int) (sampleLeftToMix >> 32 & 0xffffffff);
               if ((sampleLeftToMix & 0xffffffff) !=0) 
                  sampleleft++;
            }
            else
               sampleleft = mRampSteps;

            if (sampleleft < iLeftToMix)
            {
               mix_endflag= eMixEndCause_Sample;
               iLeftToMix= sampleleft;
            }
         }

         iLeftToMix_old = iLeftToMix ;
         // if it tries to continue an old ramp, but the target has changed,
         // set up a new ramp

         if(	(pChannel->mRampCount !=0) &&
            ( pChannel->getLeftVolume() == pChannel->mRampLeftTarget) &&
            ( pChannel->getRightVolume() == pChannel->mRampRightTarget)
            )
         {
            // restore old ramp
            iRampCount = pChannel->mRampCount;
            mix_rampspeedleft = pChannel->mRampLeftSpeed;
            mix_rampspeedright = pChannel->mRampRightSpeed;
         }
         else
         {
            int rightv; //volume slide start
            int leftv = pChannel->getLeftVolume() - (pChannel->mRampLeftVolume>>8);
            pChannel->mRampLeftTarget = pChannel->getLeftVolume();
            if( leftv != 0)
            {
               mix_rampspeedleft = ((float)leftv) * mix_div255 * mInvRampSteps;
               pChannel->mRampLeftSpeed = mix_rampspeedleft;
               iRampCount = mRampSteps;
            }
            // right:
            rightv = pChannel->getRightVolume() - (pChannel->mRampRightVolume>>8);
            pChannel->mRampRightTarget = pChannel->getRightVolume();
            if( rightv != 0)
            {
               mix_rampspeedright = ((float)rightv) * mix_div255 * mInvRampSteps;
               pChannel->mRampRightSpeed = mix_rampspeedright;
               iRampCount = mRampSteps;
            }
         }

         // ramp end:
         if(iRampCount>0)
         {	// we have sound volume changing:
            pChannel->mRampCount = iRampCount;
            if(iRampCount<iLeftToMix) iLeftToMix = iRampCount;	
         } 
         else
         {
            //optim: if no volume change and both volume are zero, no mix !!
            // note: use logic or to check both 0 in one test.
            silent= !(pChannel->mRampRightVolume | pChannel->mRampLeftVolume);
         }

         if(pChannel->SpeedDir() == Backwards)
         {
            sampleSpeed= -sampleSpeed; // ping pong -> change speed:
         }

         if(silent)
         {
            long long multiplier;
            // both volume to zero and no volume change means no mix !
            // this simulate a silent mix.
            size=0;
            // sample run got to be done as if we mix !
            multiplier= ((long long)iLeftToMix<<32);

            multiplier >>=16;
            multiplier *= (sampleSpeed>>16);
            lmixpos+= multiplier;
         }
         else
         {
            float	fMixRampRightVol = ((float) pChannel->mRampRightVolume) * mix_divVolRamp;
            float	fMixRampLeftVol = ((float) pChannel->mRampLeftVolume) * mix_divVolRamp;

            short *pFloatSampleToMix = (short*)pSample->mBuffer; // get pointer to sample buffer
            int jj=iLeftToMix;

            size-=iLeftToMix;

            // inner loop with volume ramp and antialiasing !
            while(jj>0)
            {
               int sampleIndex = (int) (lmixpos >> 32 & 0xffffffff);

               if (sampleIndex >= pSample->mLoopStart+pSample->mLoopLen)
               {
                  printf("index out of range: %d / %d \n", sampleIndex, pSample->mLoopStart + pSample->mLoopLen);
               }

               //old AA unsigned int fractionIndex = ((unsigned int) lmixpos.Get32Low())>>1; // 31 because int->float is unsigned.
               //old AA ffrac = ((float)fractionIndex) * mix_1over2gig; // [0,1]				
               short smp1 = pFloatSampleToMix[ sampleIndex ] ;	
               short smp2 = pFloatSampleToMix[ sampleIndex+1 ] ;	
               float sub= (lmixpos>>16&0xffff) / 65536.0f;

               float samp = smp1 + (smp2-smp1)*sub;

               *(pFloatBuffer++) +=  (samp*fMixRampLeftVol);
               *(pFloatBuffer++) +=  (samp*fMixRampRightVol);

               // ramp volume:
               fMixRampLeftVol += mix_rampspeedleft;
               fMixRampRightVol += mix_rampspeedright;
               jj--;
               lmixpos+=sampleSpeed; // add speed 
            }		
            // volume ramp state restore:
            if( iRampCount > 0 )
            {
               pChannel->mRampRightVolume =(int)(fMixRampRightVol * mix_VolRamp );
               pChannel->mRampLeftVolume =(int)(fMixRampLeftVol * mix_VolRamp );
               iRampCount -= iLeftToMix; //there can be ramp left.
               mix_rampspeedleft = 0.0f;
               mix_rampspeedright = 0.0f;
               pChannel->mRampCount = iRampCount;
               // if rampcount now = 0, a vol. ramp has FINISHED, so finish the rest of the mix
               if(iRampCount ==0)
               {
                  // clear out the ramp speeds
                  pChannel->mRampLeftSpeed = 0;
                  pChannel->mRampRightSpeed = 0;
                  // clamp the 2 volumes together in case the speed wasnt accurate enough!
                  pChannel->mRampLeftVolume = pChannel->getLeftVolume()<<8;
                  pChannel->mRampRightVolume = pChannel->getRightVolume()<<8;
                  // is it 0 because ramp ended only? or both ended together??
                  // if sample ended together with ramp.. problems .. loop isnt handled
                  if( iLeftToMix_old !=  iLeftToMix)
                  {
                     // start again and continue rest of mix
                     continue;
                  }
               }
            } // end volume ramp state restore.
         } // end test if not silent.

         if( mix_endflag != eMixEndCause_Buffer )
         {
            continue;
            if (pChannel->mRampLeftVolume != 0 ||  pChannel->mRampRightVolume != 0)
            {
               pChannel->setLeftVolume(0);
               pChannel->setRightVolume(0);
               continue;
            }
            else
            {
               // if here, simple end of the sample & ramp finished -> next channel.
               break;
            }
         }  // end of test if end because of sample loop. 
         else
            break;
      } // end while( pFloatBuffer < pFloatEndBuffer )
      // FinishUpChannel
      pChannel->mMixPos = lmixpos;
   } // end loop by channel.
} 

