#include "menumousecursor.h"

// base
#include "defaultshader.h"
#include "framework/gldevice.h"
#include "math/matrix.h"
#include "math/quat.h"
#include "psdlayer.h"

// texture names
#define DEFAULT "default"
#define BUSY "busy"
#define CLICKED "clicked"

// cmath
#include <algorithm>
#include <cmath>

namespace
{
constexpr float kPi = 3.14159265358979323846f;
}

MenuMouseCursor::MenuMouseCursor(RenderDevice* dev, bool visible)
    : Drawable(dev, visible),
      mDefaultLayer(0),
      mClickedLayer(0),
      mBusyLayer(0),
      mBusyX(0),
      mBusyY(0),
      mX(0),
      mY(0),
      mSizeFactor(1.0f),
      mBusy(false),
      mMousePressed(false),
      mTime(0.0f)
{
   mFilename = "data/cursors/cursor_small.psd";
}

MenuMouseCursor::~MenuMouseCursor()
{
   delete mDefaultLayer;
   mDefaultLayer = 0;

   delete mClickedLayer;
   mClickedLayer = 0;

   delete mBusyLayer;
   mBusyLayer = 0;
}

void MenuMouseCursor::animate(float time)
{
   mTime = time;
}

void MenuMouseCursor::mousePressEvent(int /*x*/, int /*y*/)
{
   mMousePressed = true;
   mClickTime.restart();
}

void MenuMouseCursor::mouseReleaseEvent()
{
   mMousePressed = false;
}

void MenuMouseCursor::mouseMoveEvent(int x, int y)
{
   mX = x;
   mY = y;
}

void MenuMouseCursor::setBusy(bool busy)
{
   mBusy = busy;
}

void MenuMouseCursor::initializeGL()
{
   initializeLayers();
}

void MenuMouseCursor::initializeLayers()
{
   mPsd.load(mFilename.c_str());

   // assign layers to menu page items

   PSD::Layer* psdlayer;

   psdlayer = mPsd.getLayer(DEFAULT);
   if (psdlayer)
      mDefaultLayer = new PSDLayer(psdlayer);

   psdlayer = mPsd.getLayer(CLICKED);
   if (psdlayer)
      mClickedLayer = new PSDLayer(psdlayer);

   psdlayer = mPsd.getLayer(BUSY);
   if (psdlayer)
   {
      mBusyX = psdlayer->getLeft();
      mBusyY = psdlayer->getTop();
      psdlayer->setX(0);
      psdlayer->setY(0);
      mBusyLayer = new PSDLayer(psdlayer);
   }
}

void MenuMouseCursor::paintGL()
{
   // init
   initGlParameters();

   paintDefaultCursor();
   paintClickedCursor();
   paintBusyIcon();

   // cleanup
   cleanupGlParameters();
}

void MenuMouseCursor::paintCursor(PSDLayer* layer, float opacity)
{
   layer->render(mX, mY, opacity);
}

void MenuMouseCursor::paintDefaultCursor()
{
   if (!mDefaultLayer)
      return;

   paintCursor(mDefaultLayer);
}

void MenuMouseCursor::paintClickedCursor()
{
   if (!mClickedLayer)
      return;

   float opacity = std::max(300 - (int)mClickTime.elapsed(), 0) * 0.00003f * mClickedLayer->getOpacity();

   if (opacity > 0.0f)
   {
      paintCursor(mClickedLayer, opacity);
   }
}

void MenuMouseCursor::paintBusyIcon()
{
   if (mBusy)
   {
      // legacy fixed-function transform stack (glPushMatrix/glTranslatef x4/glScalef/glRotatef)
      // collapsed algebraically: mSizeFactor is always 1.0 (no setter exists), so the scale is a
      // no-op and drops out; every translate after the rotate commutes with every other one, so
      // they collapse into a single post-rotation offset. Net effect: rotate the icon around its
      // own center, then move it to (mX+mBusyX, mY+mBusyY). Not visually verified yet (mBusy is
      // never true in the ported code so far) - double check against the original once something
      // actually sets it.
      const float w = static_cast<float>(mBusyLayer->getWidth());
      const float h = static_cast<float>(mBusyLayer->getHeight());
      const float angle = mTime * kPi / 180.0f;

      Matrix pre;
      pre.translate(Vector(-0.5f * w, -0.5f * h, 0.0f));

      Matrix rot(Quat(0.0f, 0.0f, std::sin(angle * 0.5f), std::cos(angle * 0.5f)));

      Matrix post;
      post.translate(Vector(mX + mBusyX + 0.5f * w, mY + mBusyY + 0.5f * h, 0.0f));

      Matrix world = pre * rot * post;
      activeDevice->push(world);

      mBusyLayer->render();

      activeDevice->pop();
   }
}

void MenuMouseCursor::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}

void MenuMouseCursor::initGlParameters()
{
   GLDevice* device = static_cast<GLDevice*>(activeDevice);

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   // shader and projection must be set *before* push() - GLDevice::push() uploads the combined
   // MVP uniform immediately, into whatever shader/projection happen to be current at that exact
   // moment (see project memory - MenuDrawable's cross-fade blit hit the exact same ordering bug).
   // Also: shader 0 means "no program bound" in GLES3, not "fixed function" - PSDLayer::render()
   // (which paintCursor() uses) needs the shared menu shader actually bound, not released to 0.
   activeDevice->setShader(getDefaultMenuShader());
   device->setProjectionMatrix(Matrix::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f));
   device->push(Matrix());
}
