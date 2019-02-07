#ifndef MAINAPP_H
#define MAINAPP_H

#include <qframe.h>
#include "renderdevice.h"
#include "fps.h"

class SceneGraph;

class MainApp : public QWidget
{
   Q_OBJECT

public:
   MainApp();
   ~MainApp();
   virtual void keyPressEvent ( QKeyEvent * e );

private:
   RenderDevice *dev;
   SceneGraph *scene;
   FPS fps;
   float time;
   float rotx, roty;

private slots:
   void test();
};

#endif
