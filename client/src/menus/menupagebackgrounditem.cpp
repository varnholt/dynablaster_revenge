#include "menupagebackgrounditem.h"

// Qt
#include <QGLContext>
#include <QTimer>

// math
#include <math.h>

// defines
#define COLOR_CHANGE_DURATION 2000
#define LAYER_RED   "background_gradient_red"
#define LAYER_GREEN "background_gradient_green"
#define LAYER_BLUE  "background_gradient_blue"


MenuPageBackgroundItem::MenuPageBackgroundItem(QObject* parent)
   : MenuPageItem(parent),
     mX(0.0f),
     mY(0.0f),
     mBackgroundColor(BackgroundColorBlue),
     mBackgroundColorPrevious(BackgroundColorBlue)
{
   mElapsed.start();

   memset(mBackgroundLayers, 0, (BackgroundColorBlue + 1)*sizeof(PSDLayer*));
   // unitTest1();
}


void MenuPageBackgroundItem::initialize()
{
   MenuPageItem::initialize();
}


void MenuPageBackgroundItem::addGradientLayer(
   PSDLayer *gradient,
   MenuPageBackgroundItem::BackgroundColor color
)
{
   mBackgroundLayers[color]=gradient;
}


void MenuPageBackgroundItem::setBackgroundColor(
   MenuPageBackgroundItem::BackgroundColor color
)
{
   if (color != mBackgroundColor)
   {
      mBackgroundColorPrevious = mBackgroundColor;
      mBackgroundColor = color;
      mFlipBackgroundElapsed.restart();
   }
}


void MenuPageBackgroundItem::unitTest1()
{
   QTimer* timer = new QTimer(this);

   connect(
      timer,
      SIGNAL(timeout()),
      this,
      SLOT(unitTest1Slot())
   );

   timer->setInterval(5000);
   timer->start();
}


void MenuPageBackgroundItem::unitTest1Slot()
{
   int bg = mBackgroundColor;

   if (bg < BackgroundColorBlue)
      bg++;
   else
      bg = 0;

   setBackgroundColor((BackgroundColor)bg);
}


void MenuPageBackgroundItem::draw()
{
   if (mBackgroundLayers[mBackgroundColor])
   {
      float alpha =
         qMin(
            mFlipBackgroundElapsed.elapsed() / (float)COLOR_CHANGE_DURATION,
            1.0f
         );

      float alphaInverted = 1.0f - alpha;

      if (alpha > 0.0f)
         mBackgroundLayers[mBackgroundColor]->render(0.0f, 0.0f, alpha);

      if (alphaInverted > 0.0f)
         mBackgroundLayers[mBackgroundColorPrevious]->render(0.0f, 0.0f, alphaInverted);
   }

   mX = sin(mElapsed.elapsed() * 0.0001f);
   mY = cos(mElapsed.elapsed() * 0.0001f);

   // one quad for each layer
   PSD::Layer* psdLayer = 0;

   float height = 0.0f;
   float width = 0.0f;
   float xTranslation = 0.0f;
   float yTranslation = 0.0f;

   psdLayer = getCurrentLayer();

   height = psdLayer->getHeight();
   width = psdLayer->getWidth();

   xTranslation = psdLayer->getLeft();
   yTranslation = psdLayer->getTop();

   // bind texture
   glBindTexture(
      GL_TEXTURE_2D,
      getActiveLayer()->getTexture()
   );

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   glColor4ub(255,255,255,255);

   glBegin(GL_QUADS);

   glTexCoord2f(0.0f + mX, 0.0f + mY); glVertex3f(xTranslation, yTranslation, 0.0f);
   glTexCoord2f(0.0f + mX, 1.0f + mY); glVertex3f(xTranslation, yTranslation+height, 0.0f);
   glTexCoord2f(1.0f + mX, 1.0f + mY); glVertex3f(xTranslation+width, yTranslation+height, 0.0f);
   glTexCoord2f(1.0f + mX, 0.0f + mY); glVertex3f(xTranslation+width, yTranslation, 0.0f);

   glEnd();
}
