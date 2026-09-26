#include "spherefragmentsdrawable.h"

// engine
#include "framework/framebuffer.h"
#include "gldevice.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "math/vector4.h"
#include "nodes/mesh.h"
#include "nodes/node.h"
#include "nodes/scenegraph.h"
#include "render/geometry.h"
#include "tools/filestream.h"
#include "tools/string.h"

// spherefragments
#include "blendquad.h"
#include "bombfuzegeometryvbo.h"
#include "bombsocketgeometryvbo.h"
#include "duplicatealpha.h"
#include "spherefragmentcontainer.h"
#include "spheregeometryvbo.h"

// postproduction
#include "postproduction/blurfilter.h"

SphereFragmentsDrawable::SphereFragmentsDrawable(RenderDevice* dev, bool visible)
    : Drawable(dev, visible),
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
      mScale(0.65f),
      mEarthFb(0),
      mAuraFb(0),
      mBombFb(0)
{
}

SphereFragmentsDrawable::~SphereFragmentsDrawable()
{
   delete mAlphaDuplicate;
   delete mBlendQuad;
   delete mSceneGraphEarth;
   delete mSceneGraphBomb;
   delete mBomb;
   delete mFuze;
   delete mSocket;
   delete mBlur;
   delete mFragmentContainer;
   delete mEarthFb;
   delete mAuraFb;
   delete mBombFb;
}

void SphereFragmentsDrawable::initializeGL()
{
   FileStream::addPath("data/shaders");
   FileStream::addPath("data/effects/spherefragments/shaders");
   FileStream::addPath("data/effects/spherefragments/meshes");
   FileStream::addPath("data/effects/spherefragments/images");

   mAlphaDuplicate = new DuplicateAlpha();
   mBlendQuad = new BlendQuad();

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
   mSocket = new BombSocketGeometryVbo(socketGeometry);
   mSocket->initialize();

   Mesh* fuze = (Mesh*)mSceneGraphBomb->getNode("bomb_fuze");
   Geometry* fuzeGeometry = fuze->getPart(0);
   mFuze = new BombFuzeGeometryVbo(fuzeGeometry);
   mFuze->initialize();

   removeFragments();

   mCamera = Vector(0.0f, 0.0f, 2.6f);
   mPositionOffset = Vector(0.0f, 0.0f, 0.0f);

   // the legacy version read spherefragments_offset_{x,y,z}/spherefragments_scale from
   // GameSettings::StyleSettings - this effect isn't wired to that subsystem, so it uses the
   // same fallback values GameSettings itself falls back to when no override exists, i.e. the
   // real default appearance of a freshly-installed game.
   mPositionOffset.x = 0.0f;
   mPositionOffset.y = 0.35f;
   mPositionOffset.z = 0.0f;
   mScale = 0.65f;

   // init blur filter
   mBlur = new BlurFilter();
   mBlur->init();

   mFragmentContainer = new SphereFragmentContainer(mSceneGraphEarth);

   FileStream::removePath("data/effects/spherefragments/images");
}

void SphereFragmentsDrawable::projectionSetup()
{
   float aspect = 9.0f / 16.0f;

   Matrix proj = Matrix::scale(mScale, mScale, mScale);
   proj = proj * Matrix::position(-mCamera);
   proj = proj * Matrix::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f);

   static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(proj);
}

void SphereFragmentsDrawable::paintGL()
{
   mFragmentContainer->animate(GlobalTime::Instance()->getTime());

   const int width = activeDevice->getWidth();
   const int height = activeDevice->getHeight();

   if (!mEarthFb)
      mEarthFb = new FrameBuffer(width, height);
   else
      mEarthFb->setResolution(width, height);

   if (!mAuraFb)
      mAuraFb = new FrameBuffer(width, height);
   else
      mAuraFb->setResolution(width, height);

   if (!mBombFb)
      mBombFb = new FrameBuffer(width, height);
   else
      mBombFb->setResolution(width, height);

   FrameBuffer::push();

   projectionSetup();

   // lighting pass 1

   // draw earth fragments once into a framebuffer, reuse later. Clears to alpha=0 (not the
   // device's default opaque clear color) - this buffer's own alpha channel is later used by
   // mBlendQuad's alpha-blend composite onto the menu (see below), so it must start fully
   // transparent, or the blend replaces the whole full-screen quad with this buffer's flat clear
   // color instead of just the drawn earth/fragments/bomb pixels.
   mEarthFb->bind();
   static_cast<GLDevice*>(mDevice)->clear(0.0f, 0.0f, 0.0f, 0.0f);

   // put bomb into zbuffer to black backside fragments
   mBomb->draw(Vector4(1, 1, 1, 0));
   mSocket->draw(Vector4(1, 1, 1, 1));

   mFragmentContainer->drawFragments(mCamera);
   mEarthFb->unbind();

   // atmosphere pass

   // create white mask from alpha channel. alpha=0 clear - same reasoning as mEarthFb above.
   mAuraFb->bind();
   static_cast<GLDevice*>(mDevice)->clear(0.0f, 0.0f, 0.0f, 0.0f);
   mAlphaDuplicate->process(mEarthFb->texture(), Vector4(1.0f, 1.0f, 1.0f, 1.0f));

   // blur white mask
   mBlur->setRadius(30.0f * mAuraFb->width() / 1920.0f);
   mBlur->process(mAuraFb->texture());
   mAuraFb->unbind();

   // lava glow pass
   // draw the bomb... alpha=0 clear - same reasoning as mEarthFb above.
   mBombFb->bind();
   static_cast<GLDevice*>(mDevice)->clear(0.0f, 0.0f, 0.0f, 0.0f);
   mBomb->draw(Vector4(1, 1, 1, 1));

   // draw black fragments
   glEnable(GL_BLEND);
   mAlphaDuplicate->process(mEarthFb->texture(), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
   glDisable(GL_BLEND);

   // blur
   mBlur->setRadius(40.0f * mBombFb->width() / 1920.0f);
   mBlur->process(mBombFb->texture());
   mBombFb->unbind();

   // lighting pass 2

   // draw bomb into fragment fb
   mEarthFb->bind();
   mBomb->draw(Vector4(1, 1, 1, 1));

   // draw bomb parts
   drawBombParts();

   mEarthFb->unbind();

   FrameBuffer::pop();

   // layer composition pass

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);

   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

   Vector4 colorAtmosphere = Vector4(0.7f, 0.9f, 1.5f, 0.5f) * mAlpha;
   Vector4 colorFragments = Vector4(1.0f, 1.0f, 1.0f, 1.0f) * mAlpha;
   Vector4 colorGlow = Vector4(4.0f, 4.0f, 4.0f, 1.0f) * mAlpha;

   // atmosphere
   mBlendQuad->process(mAuraFb->texture(), colorAtmosphere, 1.0f, mPositionOffset);

   // fragments and inner sphere
   mBlendQuad->process(mEarthFb->texture(), colorFragments, 1.0f, mPositionOffset);

   // add glow
   glBlendFunc(GL_ONE, GL_ONE);
   mBlendQuad->process(mBombFb->texture(), colorGlow, 1.0f, mPositionOffset);

   // cleanup
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SphereFragmentsDrawable::drawBombParts()
{
   mSocket->draw(Vector4(1, 1, 1, 1));
   mFuze->draw(Vector4(1, 1, 1, 1));
}

void SphereFragmentsDrawable::removeFragments()
{
   for (int i = 0; i < mSceneGraphBomb->getChildCount(); i++)
   {
      Node* node = mSceneGraphBomb->getChild(i);
      String name = node->name();
      int index = name.indexOf("_");
      if (index >= 0)
      {
         name = name.mid(0, index);

         Node* frag = mSceneGraphEarth->getChild(name);
         if (frag && frag->id() == Node::idMesh)
         {
            Mesh* mesh = (Mesh*)frag;
            mesh->setVisible(false);
         }
      }
   }
}
