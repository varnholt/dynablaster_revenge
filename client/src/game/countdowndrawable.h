#ifndef COUNTDOWNDRAWABLE_H
#define COUNTDOWNDRAWABLE_H

// base
#include "drawable.h"

// engine
#include "image/psd.h"

// Qt
#include <QImage>
#include <QObject>
#include <QList>

class PSDLayer;

class CountdownDrawable : public QObject, public Drawable
{
   Q_OBJECT

   public:

      //! constructor
      CountdownDrawable(RenderDevice* dev);

      //! destructor
      virtual ~CountdownDrawable();

      //! initialize
      void initializeGL();

      //! draw
      void paintGL();

      //! animate
      void animate(float time);


   public slots:

      //! time left
      void countdown(int);


   protected:

      //! draw main overlay
      void drawCountdown();

      //! initialize layers
      void initializeLayers();

      //! init gl parameters
      void initGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();


      // overlay members

      //! psd instance
      PSD mPsd;

      //! list of psd layers
      QList<PSDLayer*> mPsdLayers;

      //! list of layer ids
      QList<int> mLayerIds;

      //! list of layer alphas
      QList<float> mLayerAlphas;

      //! filename to load from
      QString mFilename;

      //! animation layer index
      int mTimeLeft;

      //! animation start time
      float mAnimationStartTime;

      //! animation started flag
      bool mAnimationActive;

      //! delta time
      float mDeltaTime;

      //! time
      float mTime;

      //! detlta time initialized
      bool mDeltaTimeInitialized;
};

#endif // COUNTDOWNDRAWABLE_H
