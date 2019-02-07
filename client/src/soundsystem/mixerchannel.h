#ifndef MIXERCHANNEL_H
#define MIXERCHANNEL_H

class AudioSample;

// Channel type - contains information on a mixing channel
class MixerChannel
{
public:
   MixerChannel();

   unsigned int SpeedDir() const;
   void SetSpeedDir(unsigned int dir);

   void SetSample(AudioSample* sample, int sampleRate= -1);
   AudioSample* Sample() const;		   // currently playing sample

   bool isMuted() const;
   void setMuted(bool mute);
   void toggleMute();

   unsigned int getLeftVolume() const;
   unsigned int getRightVolume() const;

   void setLeftVolume(unsigned int volume);
   void setRightVolume(unsigned int volume);


   int				mVolume;   		// current volume (00-FFh).
   int				mFrequency;		// speed or rate of playback in hz.
   int				mPanning;   	// panning value (00-FFh).
   int            mSampleOffset; // sample offset (sample starts playing from here).

   // software mixer stuff
   unsigned int	mLeftVolume;   // mixing information. adjusted volume for left channel (panning involved)
   unsigned int	mRightVolume;  // mixing information. adjusted volume for right channel (panning involved)
   long long   	mMixPos;			// mixing information. 32:32 fractional position in sample
   long long      mSpeed;		   // mixing information. playback rate 32:32
   unsigned int	mSpeedDir;		// mixing information. playback direction - forwards or backwards

   // software mixer volume ramping stuff
   unsigned int	mRampLeftTarget;     
   unsigned int	mRampRightTarget;    
   unsigned int	mRampLeftVolume;     
   unsigned int	mRampRightVolume;    
   float	         mRampLeftSpeed;
   float	         mRampRightSpeed;
   unsigned int	mRampCount;

private:
   AudioSample*   mSample;		   // currently playing sample
   bool           mMute;
};

#endif
