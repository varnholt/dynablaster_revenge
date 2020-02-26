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

   Gui(const QGLFormat& format);
   ~Gui() = default;

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


   RenderDevice* mDevice = nullptr; //!< render device
   SceneGraph* mScene = nullptr;

   // fps
   int mFrames = 0;           //!< number of rendered frames
   float mFrameTime = 0.0f;   //!< frame time
   QTime mTimer;
   float mTime = 0.0f;
};
