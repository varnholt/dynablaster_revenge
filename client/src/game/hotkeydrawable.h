#pragma once

#include <QObject>

// base
#include "drawable.h"

// framework
#include "tools/array.h"
#include "image/psd.h"
#include "framework/frametimer.h"
#include "vertex.h"

class QImage;
class BitmapFont;
class PSDLayer;

class HotkeyDrawable : public QObject, public Drawable
{
    Q_OBJECT

   public:

      //! constructor
      HotkeyDrawable(RenderDevice*);

      //! destructor
      virtual ~HotkeyDrawable();

      //! initialize in gl context
      void initializeGL();

      //! paint
      void paintGL();

      //! animate drawable
      void animate(float time);

      //! overwrite setVisible
      virtual void setVisible(bool visible);

      //! getter for shown flag
      bool isShown() const;


   public slots:

      //! show hotkeys
      void showHotkeys();


   protected:

      //! initialize layers
      void initializeLayers();

      //! initialize gl parameters
      void initGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();

      //! calc alpha
      float computeAlpha();

      //! psd
      PSD mPsd;

      //! list of layers
      QList<PSDLayer*> mPsdLayers;

      //! bitmap font
      BitmapFont* mFont;

      //! animation timer
      FrameTimer mAnimationTimer;

      //! fade flag
      bool mShow;
};

