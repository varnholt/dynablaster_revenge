#ifndef GAMEHELPDRAWABLE_H
#define GAMEHELPDRAWABLE_H

// base
#include "drawable.h"

// Qt
#include <QImage>
#include <QObject>
#include <QQueue>

// engine
#include "image/psd.h"
#include "framework/frametimer.h"

// game
#include "helpelement.h"

// forward declarations
class BitmapFont;
class PSDLayer;

class GameHelpDrawable : public QObject, public Drawable
{
   Q_OBJECT

   public:

      //! constructor
      GameHelpDrawable(RenderDevice* dev);

      //! destructor
      virtual ~GameHelpDrawable();

      //! initialize
      void initializeGL();

      //! draw
      void paintGL();


   public slots:

      virtual void showHelp(
         const QString& page,
         const QString& message,
         Constants::HelpSeverity severity,
         Constants::HelpLocation location,
         int delay = 0
      );

      virtual void pageChanged(const QString& page);


   protected slots:

      //! process next item
      void updateHelpItems();

      //! a unit test
      void unitTest1();


   protected:

      //! initialize gl parameters
      void initGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();

      //! initialize available layers
      void initializeLayers();

      //! draw a quad
      void drawQuad(
         PSDLayer* layer,
         float left,
         float right,
         float top,
         float bottom,
         float opacity,
         float scaleX = 1.0f,
         float scaleY = 1.0f
      );


      //! popup animation time from 0..1
      float getNormalizedPopupTime() const;

      //! fade-in animation time from 0..1
      float getNormalizedFadeInTime() const;

      //! fade-out animation time from 0..1
      float getNormalizedFadeOutTime() const;


   protected:

      //! play a sound according to the element's severity
      void playSound(HelpElement* element);

      PSD mPsd;

      PSDLayer* mLayerIconInfo;
      PSDLayer* mLayerSpriteInfo;
      PSDLayer* mLayerIconError;
      PSDLayer* mLayerSpriteError;
      PSDLayer* mLayerTitle;
      PSDLayer* mLayerBubble;
      PSDLayer* mLayerLineTitle;
      PSDLayer* mLayerLine1;
      PSDLayer* mLayerLine2;
      PSDLayer* mLayerLine3;

      QString mFilename;

      BitmapFont* mFont;

      QQueue<HelpElement*> mHelpItems;

      QString mPage;

      FrameTimer mAnimationTime;

      int mPopupDuration;
      int mFadeInDuration;
      int mFadeOutDuration;
      int mIdleDuration;
};

#endif // GAMEHELPDRAWABLE_H
