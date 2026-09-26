#include "menupagebackgrounditem.h"
#include "framework/gldevice.h"
#include "math/matrix.h"

// math
#include <algorithm>
#include <cstring>

// defines
#define COLOR_CHANGE_DURATION 2000

MenuPageBackgroundItem::MenuPageBackgroundItem()
    : mX(0.0f),
      mY(0.0f),
      mBackgroundColor(BackgroundColorBlue),
      mBackgroundColorPrevious(BackgroundColorBlue),
      mVertexBuffer(0)
{
   mElapsed.start();

   memset(mBackgroundLayers, 0, (BackgroundColorBlue + 1) * sizeof(PSDLayer*));
}

void MenuPageBackgroundItem::initialize()
{
   MenuPageItem::initialize();
}

void MenuPageBackgroundItem::addGradientLayer(PSDLayer* gradient, MenuPageBackgroundItem::BackgroundColor color)
{
   mBackgroundLayers[color] = gradient;
}

void MenuPageBackgroundItem::setBackgroundColor(MenuPageBackgroundItem::BackgroundColor color)
{
   if (color != mBackgroundColor)
   {
      mBackgroundColorPrevious = mBackgroundColor;
      mBackgroundColor = color;
      mFlipBackgroundElapsed.restart();
   }
}

void MenuPageBackgroundItem::draw()
{
   if (mBackgroundLayers[mBackgroundColor])
   {
      float alpha = std::min(mFlipBackgroundElapsed.elapsed() / (float)COLOR_CHANGE_DURATION, 1.0f);

      float alphaInverted = 1.0f - alpha;

      if (alpha > 0.0f)
         mBackgroundLayers[mBackgroundColor]->render(0.0f, 0.0f, alpha);

      if (alphaInverted > 0.0f)
         mBackgroundLayers[mBackgroundColorPrevious]->render(0.0f, 0.0f, alphaInverted);
   }

   mX = sin(mElapsed.elapsed() * 0.0001f);
   mY = cos(mElapsed.elapsed() * 0.0001f);

   // one quad for the whole background, scrolling slowly by animating its texcoords - the
   // legacy immediate-mode draw (glBegin(GL_QUADS), per-vertex glTexCoord2f/glVertex3f) becomes
   // a plain attribute-array draw through the shared texalphaignore shader (already bound by
   // MenuDrawable for the whole page-render pass), rebuilt into a small dynamic buffer every
   // frame since the texcoords change every frame anyway.
   PSD::Layer* psdLayer = getCurrentLayer();

   const float height = static_cast<float>(psdLayer->getHeight());
   const float width = static_cast<float>(psdLayer->getWidth());
   const float xTranslation = static_cast<float>(psdLayer->getLeft());
   const float yTranslation = static_cast<float>(psdLayer->getTop());

   glBindTexture(GL_TEXTURE_2D, getActiveLayer()->getTexture());
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   const float quad[] = {
      xTranslation,         yTranslation,          -1.0f, 0.0f + mX, 0.0f + mY,
      xTranslation,         yTranslation + height, -1.0f, 0.0f + mX, 1.0f + mY,
      xTranslation + width, yTranslation + height, -1.0f, 1.0f + mX, 1.0f + mY,
      xTranslation,         yTranslation,          -1.0f, 0.0f + mX, 0.0f + mY,
      xTranslation + width, yTranslation + height, -1.0f, 1.0f + mX, 1.0f + mY,
      xTranslation + width, yTranslation,          -1.0f, 1.0f + mX, 0.0f + mY,
   };

   if (mVertexBuffer == 0)
      mVertexBuffer = activeDevice->createVertexBuffer(sizeof(quad), true);
   else
      activeDevice->allocateVertexBuffer(mVertexBuffer, sizeof(quad), true);

   void* dst = activeDevice->lockVertexBuffer(mVertexBuffer, sizeof(quad));
   memcpy(dst, quad, sizeof(quad));
   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // positions are already baked in page-pixel space, so the world transform must be identity -
   // reset it in case a previous item's draw call left a translation pushed.
   activeDevice->push(Matrix());
   activeDevice->setParameter(activeDevice->getParameterIndex("alpha"), 1.0f);

   glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   activeDevice->pop();
}
