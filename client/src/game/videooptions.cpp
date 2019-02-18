// header
#include "videooptions.h"

// engine
#include "framebuffer.h"
#include "gldevice.h"

// Qt
#include <QApplication>
#include <QDesktopWidget>

VideoOptions* VideoOptions::sInstance = 0;


VideoOptions::VideoOptions()
 : mAspectFixed(false)
{
   sInstance = this;
}


void VideoOptions::initialize()
{
   mResolutions.clear();
   mSamples.clear();

   initializeModes();
   initializeSamples();
}


const QList<int>& VideoOptions::getResolutions() const
{
   return mResolutions;
}


int VideoOptions::getResolution(int index) const
{
   int resolution = 1;
   if (index >= 0 && index < mResolutions.size())
      resolution = mResolutions[index];

   return resolution;
}


const QList<int>& VideoOptions::getSampleList() const
{
   return mSamples;
}

int VideoOptions::getSamples(int index) const
{
   if (index > 0 && index < mSamples.size())
      return mSamples[index];
   else
      return 1;
}

VideoOptions *VideoOptions::getInstance()
{
   return sInstance;
}


void VideoOptions::initializeModes()
{
   QSize size = QApplication::desktop()->size();

   int width = size.width();
   int height = size.height();

   if (width / (float)height >= 3.0f)
      width /= 2;

   int factor = 1;
   while (width / factor > 320)
   {
      mResolutions.append( factor );
      factor++;
   }
}


void VideoOptions::initializeSamples()
{
   // find out number of samples available
   GLenum internalFormat = GL_RGBA;
   GLint samplesLength = 0;

#ifndef Q_OS_MAC
   // check if extension is available
   if (glGetInternalformativ)
   {
      // get number of available samples
      glGetInternalformativ(
          GL_RENDERBUFFER,
          internalFormat,
          GL_NUM_SAMPLE_COUNTS,
          1,
          &samplesLength
      );

      if (samplesLength > 0)
      {
         // get sample list
         GLint *samples = new GLint[ samplesLength ];

         glGetInternalformativ(
             GL_RENDERBUFFER,
             internalFormat,
             GL_SAMPLES,
             samplesLength,
             samples
         );

         // test samples
         for (int i = 0; i < samplesLength; i++)
         {
            FrameBuffer *fb = new FrameBuffer(640,480, samples[i]);
            if (fb->samples() == samples[i])
               mSamples.append(samples[i]);
            delete fb;
         }

         delete[] samples;
      }
   }
#endif

   if (samplesLength == 0)
   {
      int samples[]={1,2,4,6,8,16,32,64,128,256,-1};
      int index= 0;
      while (samples[index]>0)
      {
         FrameBuffer *fb = new FrameBuffer(640,480, samples[index]);
         if (fb->samples() > 0)
         {
            if (!mSamples.contains(fb->samples()))
               mSamples.append( fb->samples() );
         }
         delete fb;
         index++;
      }
   }

   qSort(mSamples);
}


void VideoOptions::setAspectFixed(bool fixedAspect)
{
   mAspectFixed = fixedAspect;
}


bool VideoOptions::isAspectFixed() const
{
   return mAspectFixed;
}


