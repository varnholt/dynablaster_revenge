#include "stdafx.h"
#include "mainapp.h"
#include "gldevice.h"
#include "scenegraph.h"
#include "mesh.h"
#include "logoreflection.h"
#include "texturemapping.h"
#include "glowring.h"
#include "camera.h"
#include <qtimer.h>
#include <qevent.h>

MainApp::MainApp()
: QWidget()
{
   dev= new GLDevice();
   dev->init(1024,576);

   scene= new SceneGraph();
   scene->load("data/player.hjb");
   scene->setCamera(0);

   TextureMapping *tex= new TextureMapping("player-lightmap");
   scene->addMaterial(tex);

   Mesh *mesh= (Mesh*)scene->getNode("player");
   if (mesh)
      tex->addMesh(mesh);


   QTimer *timer= new QTimer();
   connect(timer, SIGNAL(timeout()), this, SLOT(test()));
   timer->start(10);
}


MainApp::~MainApp()
{
   delete scene;

   // close window
   dev->close();
   delete dev;
}


void MainApp::test()
{
   // logo tiefe z= -7.408..+12.592
   // rand: z= -0.202..+4.798
   // randabstand: 1
   // light: 215,215,227
   if (dev->abort())
   {
      delete this;
      return;
   }

/*
   Camera *cam= (Camera*)scene->getCamera();
   PosTrack& pos= cam->getPositionTrack();
   PosKey& key= pos.key(0);

   if (activeDevice->getKey(0)==1)
      key.setValue( key.value() + Vector(0,0,0.1) );

   if (activeDevice->getKey(1)==1)
      key.setValue( key.value() + Vector(0,0,-0.1) );

   if (activeDevice->getKey(2)==1)
      key.setValue( key.value() + Vector(0.1,0,0) );

   if (activeDevice->getKey(3)==1)
      key.setValue( key.value() + Vector(-0.1,0,0) );
*/

   // clear screen
   dev->clear(); 

   // render scene
   scene->render(time);

   Mesh *mesh= (Mesh*)scene->getNode("player");
   if (mesh)
   {
      Matrix mat;
      mat.rotateZ(time / 1000.0);
      mesh->setUserTransformable(true);
      mesh->setTransform(mat);
   }

   // display scene
   dev->display();

   // update timer
   time+=10;
   if (time>scene->getLastFrame()) time=0;

   // frame counter
   fps.next();
//	printf("fps: %f \r", fps.get());
}

void MainApp::keyPressEvent(QKeyEvent* event)
{
   switch (event->key())
   {
      case Qt::Key_Up:
         printf("up");
         break;

      case Qt::Key_Down:
         printf("down");
         break;

      case Qt::Key_Left:
         printf("left");
         break;

      case Qt::Key_Right:
         printf("right");
         break;
   }
}
