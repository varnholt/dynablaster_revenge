#ifndef MENUMOUSECURSOR_H
#define MENUMOUSECURSOR_H

// base
#include "drawable.h"

// psd
#include "image/psd.h"

#include "framework/frametimer.h"
// Qt
#include <QImage>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

class PSDLayer;

class MenuMouseCursor
  : public QObject,
    public Drawable
{
   Q_OBJECT

   public:

      enum State
      {
         Default,
         Clicked,
         Busy
      };

      MenuMouseCursor(
         RenderDevice* dev,
         bool visible = false
      );

      virtual ~MenuMouseCursor();

      virtual void initializeGL();
      virtual void paintGL();
      virtual void animate(float globalTime);

      //! mouse events
      virtual void mousePressEvent(int x, int y, Qt::MouseButton = Qt::LeftButton);
      virtual void mouseMoveEvent(int x, int y);
      virtual void mouseReleaseEvent(QMouseEvent* event);


   public slots:

      void setBusy(bool);


   protected:

      void initializeLayers();

      void initGlParameters();
      void cleanupGlParameters();

      void paintDefaultCursor();
      void paintClickedCursor();
      void paintCursor(PSDLayer* layer, float opacity=1.0f);
      void paintBusyIcon();

      PSD mPsd;

      QString mFilename;

      PSDLayer* mDefaultLayer;
      PSDLayer* mClickedLayer;
      PSDLayer* mBusyLayer;

      int mBusyX;
      int mBusyY;

      int mX;
      int mY;

      float mSizeFactor;

      bool mBusy;
      bool mMousePressed;

      FrameTimer mClickTime;

      float mTime;
};

#endif // MENUMOUSECURSOR_H
