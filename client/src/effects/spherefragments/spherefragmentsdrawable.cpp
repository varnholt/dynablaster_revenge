#include "spherefragmentsdrawable.h"

// Qt
#include <QApplication>
#include <QSettings>

// cmath
#include "math.h"

// framework
#include "engine/nodes/mesh.h"
#include "engine/nodes/scenegraph.h"
#include "engine/render/geometry.h"
#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include "maindrawable.h"
#include "tools/filestream.h"

// spherefragments
#include "bombsocketgeometryvbo.h"
#include "bombfuzegeometryvbo.h"
#include "spheregeometryvbo.h"
#include "spherefragmentcontainer.h"
#include "duplicatealpha.h"
#include "blendquad.h"

// postproduction
#include "postproduction/blurfilter.h"

// settings
#include "gamesettings.h"


SphereFragmentsDrawable::SphereFragmentsDrawable(RenderDevice* dev, bool visible)
  : QObject(),
    Drawable(dev, visible),
    mSceneGraphEarth(0),
    mSceneGraphBomb(0),
    mBomb(0),
    mFuze(0),
    mSocket(0),
    mFragmentContainer(0),
    mBlur(0),
    mAlphaDuplicate(0),
    mBlendQuad(0),
    mAlpha(1.0f),
    mScale(0.65f)
{   
}


SphereFragmentsDrawable::~SphereFragmentsDrawable()
{
   delete mAlphaDuplicate;
   delete mBlendQuad;
   delete mSceneGraphEarth;
   delete mBlur;
   delete mFragmentContainer;
}


void SphereFragmentsDrawable::initializeGL()
{
   FileStream::addPath("data/shaders");
   FileStream::addPath("data/effects/spherefragments/shaders");
   FileStream::addPath("data/effects/spherefragments/meshes");
   FileStream::addPath("data/effects/spherefragments/images");

   mAlphaDuplicate= new DuplicateAlpha();
   mBlendQuad= new BlendQuad();

   // earth
   mSceneGraphEarth = new SceneGraph();
   mSceneGraphEarth->load("voronoisphere.hjb");
   Mesh* sphere = (Mesh*)mSceneGraphEarth->getNode("inner_sphere");
   mBomb = new SphereGeometryVbo(sphere->getPart(0));
   mBomb->initialize();

   // bomb socket and fuze
   mSceneGraphBomb = new SceneGraph();
   mSceneGraphBomb->load("fuze_socket.hjb");

   Mesh* socket = (Mesh*)mSceneGraphBomb->getNode("bomb_socket");
   Geometry* socketGeometry = socket->getPart(0);
   // unfuckGeometry(socketGeometry);
   mSocket = new BombSocketGeometryVbo(socketGeometry);
   mSocket->initialize();

   Mesh* fuze = (Mesh*)mSceneGraphBomb->getNode("bomb_fuze");
   Geometry* fuzeGeometry = fuze->getPart(0);
   // unfuckGeometry(fuzeGeometry);
   mFuze = new BombFuzeGeometryVbo(fuzeGeometry);
   mFuze->initialize();

   // Mesh* sphere = (Mesh*)mSceneGraphBomb->getNode("bomb_sphere");

   removeFragments();

   mCamera = Vector(0.0f, 0.0f, 2.6f);
   mPositionOffset = Vector(0.0f, 0.0f, 0.0f);

   GameSettings::StyleSettings* styleSettings =
      GameSettings::getInstance()->getStyleSettings();

   mPositionOffset.x =
      styleSettings->value("style/spherefragments_offset_x", 0.0f).toFloat();

   mPositionOffset.y =
      styleSettings->value("style/spherefragments_offset_y", 0.35f).toFloat();

   mPositionOffset.z =
      styleSettings->value("style/spherefragments_offset_z", 0.0f).toFloat();

   mScale =
      styleSettings->value("style/spherefragments_scale", 0.65f).toFloat();

   // init blur filter
   mBlur = new BlurFilter();
   mBlur->init();

   mFragmentContainer = new SphereFragmentContainer( mSceneGraphEarth );

   FileStream::removePath("data/effects/spherefragments/images");
}


void SphereFragmentsDrawable::projectionSetup()
{
    float aspect = 9.0f/16.0f;

    glMatrixMode(GL_PROJECTION);

    Matrix proj= Matrix::scale(mScale, mScale, mScale);
    proj= proj * Matrix::position(-mCamera);
    proj= proj * Matrix::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f);
    glLoadMatrixf( proj );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void SphereFragmentsDrawable::paintGL()
{
   mFragmentContainer->animate(GlobalTime::Instance()->getTime());

   FrameBuffer::push();

   FrameBuffer* earthFb= MainDrawable::getInstance()->getRenderBuffer();
   FrameBuffer* auraFb= MainDrawable::getInstance()->getRenderBuffer(1);
   FrameBuffer* bombFb= MainDrawable::getInstance()->getRenderBuffer(2);

   projectionSetup();   

   // lighting pass 1

   // draw earth fragments once into a framebuffer, reuse later
   earthFb->bind();
   mDevice->clear();

   // put bomb into zbuffer to black backside fragments
   mBomb->draw( Vector4(1,1,1,0) );
   mSocket->draw(Vector4(1, 1, 1, 1));

   mFragmentContainer->drawFragments( mCamera );
   earthFb->unbind();


   // atmosphere pass

   // create white mask from alpha channel
   auraFb->bind();
   mDevice->clear();
   mAlphaDuplicate->process(
      earthFb->texture(),
      Vector4(1.0, 1.0, 1.0, 1.0)
   );

   // blur white mask
   mBlur->setRadius(30.0f * auraFb->width() / 1920.0f);
   mBlur->process(auraFb->texture());
   auraFb->unbind();


   // lava glow pass
   // draw the bomb...
   bombFb->bind();
   mDevice->clear();
   mBomb->draw(Vector4(1, 1, 1, 1));

   // draw black fragments
   glEnable(GL_BLEND);
   mAlphaDuplicate->process(earthFb->texture(), Vector4(0.0, 0.0, 0.0, 1.0));
   glDisable(GL_BLEND);

   // blur
   mBlur->setRadius(40.0f * bombFb->width() / 1920.0f);
   mBlur->process(bombFb->texture());
   bombFb->unbind();


   // lighting pass 2

   // draw bomb into fragment fb
   earthFb->bind();
   mBomb->draw(Vector4(1, 1, 1, 1));   

   // draw bomb parts
   drawBombParts();

   earthFb->unbind();

   FrameBuffer::pop();


   // layer composition pass

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);

   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //

   Vector4 colorAtmosphere = Vector4(0.7f, 0.9f, 1.5f, 0.5f) * mAlpha;
   Vector4 colorFragments  = Vector4(1.0f, 1.0f, 1.0f, 1.0f) * mAlpha;
   Vector4 colorGlow       = Vector4(4.0f, 4.0f, 4.0f, 1.0f) * mAlpha;

   // atmosphere
   mBlendQuad->process(
      auraFb->texture(),
      colorAtmosphere,
      1.0f,
      mPositionOffset
   );

   // fragments and inner sphere
   mBlendQuad->process(
      earthFb->texture(),
      colorFragments,
      1.0f,
      mPositionOffset
   );

   // add glow
   glBlendFunc(GL_ONE, GL_ONE);
   mBlendQuad->process(
      bombFb->texture(),
      colorGlow,
      1.0f,
      mPositionOffset
   );

   // cleanup
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void SphereFragmentsDrawable::drawBombParts()
{
   // glEnable(GL_BLEND);
   // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //
   mSocket->draw(Vector4(1, 1, 1, 1));
   mFuze->draw(Vector4(1, 1, 1, 1));
}


void SphereFragmentsDrawable::removeFragments()
{
   for (int i=0; i<mSceneGraphBomb->getChildCount(); i++)
   {
      Node* node= mSceneGraphBomb->getChild(i);
      String name= node->name();
      int index= name.indexOf("_");
      if (index>=0)
      {
         name= name.mid(0, index);

         Node* frag= mSceneGraphEarth->getChild(name);
         if (frag && frag->id()==Node::idMesh)
         {
            Mesh* mesh= (Mesh*)frag;
            mesh->setVisible(false);
         }
      }
   }
}
