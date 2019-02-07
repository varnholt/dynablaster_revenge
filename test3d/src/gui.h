#pragma once

#include "framework/globaltime.h"
#include <QGLWidget>
#include <QTime>

class RenderDevice;
class SceneGraph;

class Gui : public QGLWidget, public GlobalTime
{
   Q_OBJECT

public:

   //! constructor
   Gui(const QGLFormat& format);

   //! destructor
   ~Gui();

   float getTime() const;

signals:

   //! fps changed
   void updateFps(float fps);


protected:
   //! inialize gl context
   void initializeGL();

   //! paint gl overwritten
   void paintGL();

   //! resize gl overwritten
   void resizeGL(int width, int height);


   RenderDevice*           mDevice;                      //!< render device
   SceneGraph*             mScene;

   // fps
   int                     mFrames;                      //!< number of rendered frames
   float                   mFrameTime;                   //!< frame time
   QTime                   mTimer;
   float                   mTime;
};
