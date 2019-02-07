#ifndef RoundsDrawable_H
#define RoundsDrawable_H

// base
#include "drawable.h"

// engine
#include "image/psd.h"

// Qt
#include <QImage>
#include <QObject>
#include <QList>

class MotionBlurFilter;
class PSDLayer;

class RoundsDrawable : public QObject, public Drawable
{
   Q_OBJECT

   public:

      //! constructor
      RoundsDrawable(RenderDevice* dev);

      //! destructor
      virtual ~RoundsDrawable();

      //! initialize
      void initializeGL();

      //! draw
      void paintGL();

      //! animate
      void animate(float time);


   public slots:

      void showGame();


   protected:

      //! initialize layers
      void initializeLayers();

      //! init gl parameters
      void initGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();

      //! getter for relative time (current time - start time)
      float getRelativeTime() const;


      // overlay members

      //! psd instance
      PSD mPsd;

      //! list of psd layers
      QList<PSDLayer*> mPsdLayers;

      PSDLayer* mLayerRound;
      PSDLayer* mLayerRoundFinal;
      PSDLayer* mLayerRound1;
      PSDLayer* mLayerRound2;
      PSDLayer* mLayerRound3;
      PSDLayer* mLayerRound4;
      PSDLayer* mLayerRound5;

      //! filename to load from
      QString mFilename;

      //! animation time
      float mTime;

      //! animation start tiem
      float mStartTime;

      //! started flag which is reset on every first countdown
      bool mInitializeTime;

      //! motionblur filter
      MotionBlurFilter* mMotionBlurFilter;

      //! maximum layer width
      float mMaxLayerWidth;
};

#endif // RoundsDrawable_H
