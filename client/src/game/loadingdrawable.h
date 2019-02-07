#ifndef LOADINGDRAWABLE_H
#define LOADINGDRAWABLE_H

// drawable
#include "drawable.h"

// framework
#include "psdlayer.h"

// Qt
#include <QObject>
#include <QList>

// forward declarations
class RenderDevice;

class LoadingDrawable : public QObject, public Drawable
{
   Q_OBJECT

   public:

      //! constructor
      LoadingDrawable(RenderDevice* dev, bool visible = false);

      //! destructor
      virtual ~LoadingDrawable();

      //! overwrite setvisible
      virtual void setVisible(bool);

      //! we're always visible
      bool isVisible() const;

      //! paint
      virtual void paintGL();

      //! initialize gl context
      virtual void initializeGL();


   public slots:

      //! start / stop loading
      void setLoading(bool enabled);

      //! started loading a file
      void loadingStarted();

      //! stopped loading a file
      void loadingStopped();


   protected:

      //! initialize layers
      void initializeLayers();

      //! initialize gl params
      void initGlParameters();

      //! cleanup gl params
      void cleanupGlParameters();

      //! loading psd file
      PSD mPsd;

      QList<PSDLayer*> mPsdLayers;

      //! loading flag
      bool mLoading;
};

#endif // LOADINGDRAWABLE_H
