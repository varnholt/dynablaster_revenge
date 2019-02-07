#pragma once

class AudioSourceInterface
{
public:
   AudioSourceInterface();
   virtual ~AudioSourceInterface();

   virtual int process(int numSamples) = 0;
   virtual int capacity() const = 0;

   virtual void setVolume(float);
   virtual float getVolume() const;

   virtual void toggleMute();
   virtual bool isMuted() const;

protected:

   float mVolume;
   bool mMuted;

private:
};
