#ifndef VIDEOOPTIONS_H
#define VIDEOOPTIONS_H

#include <QList>
#include <QMap>

class VideoOptions
{
   public:

      VideoOptions();

      virtual void initialize();

      const QList<int>& getResolutions() const;

      int getResolution(int index) const;

      int getSamples(int index) const;

      const QList<int>& getSampleList() const;

      static VideoOptions* getInstance();

      //! only list default mode's aspect
      void setAspectFixed(bool fixedAspect);

      //! list only default mode's aspect
      bool isAspectFixed() const;


   protected:

      virtual void initializeModes();

      virtual void initializeSamples();

      QList<int> mResolutions;

      QList<int> mSamples;

      static VideoOptions* sInstance;

      bool mAspectFixed;
};

#endif // VIDEOOPTIONS_H
