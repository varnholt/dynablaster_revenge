// header
#include "gamehelpdrawable.h"
#include "psdlayer.h"

// Qt
#include <QGLContext>

// tools
#include "gldevice.h"
#include "tools/filestream.h"

// math
#include "math.h"

// game
#include "fontpool.h"
#include "helpmanager.h"
#include "soundmanager.h"

#define SCREEN_WIDTH            1920
#define SCREEN_HEIGHT           1080
#define SCREEN_OFFSET_TOP       100
#define TEXT_HEADLINE_SCALE     0.15f
#define TEXT_FONT_SCALE         0.10f

#define LAYER_ICON_INFO         "icon_info"
#define LAYER_SPRITE_INFO       "sprite_info"
#define LAYER_ICON_ERROR        "icon_error"
#define LAYER_SPRITE_ERROR      "sprite_error"
#define LAYER_LABEL_ERROR_TITLE "label_error_title"
#define LAYER_ERROR_BUBBLE      "error_bubble"
#define LAYER_OFFSET_TITLE      "text_offset_title"
#define LAYER_OFFSET_1          "text_offset_1"
#define LAYER_OFFSET_2          "text_offset_2"
#define LAYER_OFFSET_3          "text_offset_3"

#define COLOR_TITLE_INFO         0.055f, 0.365f, 0.514f
#define COLOR_TITLE_ERROR        0.369f, 0.0f,   0.0f


GameHelpDrawable::GameHelpDrawable(RenderDevice* dev)
   : QObject(),
     Drawable(dev),
     mLayerIconInfo(0),
     mLayerSpriteInfo(0),
     mLayerIconError(0),
     mLayerSpriteError(0),
     mLayerTitle(0),
     mLayerBubble(0),
     mLayerLineTitle(0),
     mLayerLine1(0),
     mLayerLine2(0),
     mLayerLine3(0),
     mFont(0),
     mPopupDuration(0),
     mFadeInDuration(0),
     mFadeOutDuration(0),
     mIdleDuration(0)
{
   mFilename = "data/errorhandler/errorhandler.psd";

   // connect to help manager
   connect(
      HelpManager::getInstance(),
      SIGNAL(
         messageAdded(
            QString,
            QString,
            Constants::HelpSeverity,
            Constants::HelpLocation,
            int
         )
      ),
      this,
      SLOT(
         showHelp(
            QString,
            QString,
            Constants::HelpSeverity,
            Constants::HelpLocation,
            int
         )
      )
            );
}


GameHelpDrawable::~GameHelpDrawable()
{
   delete mLayerIconInfo;
   delete mLayerSpriteInfo;
   delete mLayerIconError;
   delete mLayerSpriteError;
   delete mLayerTitle;
   delete mLayerBubble;
   delete mLayerLineTitle;
   delete mLayerLine1;
   delete mLayerLine2;
   delete mLayerLine3;
}


void GameHelpDrawable::showHelp(
   const QString& page,
   const QString& message,
   Constants::HelpSeverity severity,
   Constants::HelpLocation location,
   int delay
)
{
   qDebug("GameHelpDrawable::showHelp: %s", qPrintable(message));

   HelpElement* help = new HelpElement(
      page,
      message,
      severity,
      location,
      delay
   );

   if (
         severity == Constants::HelpSeverityError
      || delay == 0
   )
   {
      mHelpItems.prepend(help);
   }
   else
   {
      mHelpItems << help;
   }
}


void GameHelpDrawable::pageChanged(const QString &page)
{
   mPage = page;
}


void GameHelpDrawable::updateHelpItems()
{
   if (!mHelpItems.isEmpty())
   {
      HelpElement* help = mHelpItems.first();

      if (!help->isStarted())
      {
         if (
                help->getPage().toLower() == mPage.toLower()
             || help->getPage().isEmpty()
         )
         {
            if (help->isDelayElapsed())
            {
               help->start();

               playSound(help);

               // restart time
               mAnimationTime.restart();

               // init times
               mPopupDuration = help->getPopupDuration();
               mFadeInDuration = help->getFadeInDuration();
               mFadeOutDuration = help->getFadeOutDuration();
               mIdleDuration = help->getIdleDuratiotion();
            }
            else if (
                   help->isDelayed()
               && !help->isDelayElapsed()
               && !help->isDelayStarted()
            )
            {
               help->startDelayTime();
            }
         }
         else
         {
            // help is obsolete
            delete help;
            mHelpItems.pop_front();

            // update queue again
            updateHelpItems();
         }
      }
      else
      {
         if (help->isFinished())
         {
            // help has been displayed
            delete help;
            mHelpItems.pop_front();

            // update queue again
            updateHelpItems();
         }
      }
   }
}


void GameHelpDrawable::unitTest1()
{
   qDebug("GameHelpDrawable::unitTest1()");

   HelpManager::getInstance()->addMessage(
      "",
      tr("Ho noze;They killed kenny;What now"),
      Constants::HelpSeverityError
   );
}


void GameHelpDrawable::initializeGL()
{
   FileStream::addPath("data/errorhandler");

   // init layers
   initializeLayers();

   // init font
   mFont = FontPool::Instance()->get("default");

   FileStream::removePath("data/errorhandler");
}


void GameHelpDrawable::drawQuad(
   PSDLayer* layer,
   float left,
   float right,
   float top,
   float bottom,
   float opacity,
   float scaleX,
   float scaleY
)
{
   // bind texture
   glBindTexture(
      GL_TEXTURE_2D,
      layer->getTexture()
   );

   // set opacity
   glColor4f(
      1.0,
      1.0,
      1.0,
      opacity
   );

   glBegin(GL_QUADS);

   glTexCoord2f(0.0f, 0.0f);
   glVertex3f(left - scaleX, top - scaleY, 0.0f);

   glTexCoord2f(0.0f, layer->getV());
   glVertex3f(left - scaleX, bottom + scaleY, 0.0f);

   glTexCoord2f(layer->getU(), layer->getV());
   glVertex3f(right + scaleX, bottom + scaleY, 0.0f);

   glTexCoord2f(layer->getU(), 0.0f);
   glVertex3f(right + scaleX, top - scaleY, 0.0f);

   glEnd();
}


void GameHelpDrawable::paintGL()
{
   updateHelpItems();

   if (!mHelpItems.isEmpty())
   {
      HelpElement* help = mHelpItems.front();

      QString message = help->getMessage();
      QStringList messageLines = message.split(";");
      QString messageLine1 = messageLines.size() > 0 ? messageLines.at(0) : "";
      QString messageLine2 = messageLines.size() > 1 ? messageLines.at(1) : "";
      QString messageLine3 = messageLines.size() > 2 ? messageLines.at(2) : "";

      initGlParameters();

      /*
        |                                                                       |
        |                                                                       |
        |                                                                 ICON__|
        |                                                                 |  |  |
        |                                                                       |
        |                                                          TEXT_________|
        |                                                          |  |         |
        |                                                                       |
        |                                                                       |
        +-----------------------------------------------------------------------+
        |                                                                       |
        0                                                                     1600

         HelpLocationTopLeft,
         HelpLocationBottomLeft
            [SPRITE][BUBBLE]
            sprite move animation from -sprite_width => 0

         HelpLocationTopRight
         HelpLocationButtomRight
            [BUBBLE][SPRITE]
            sprite move animation from SCREEN_WIDTH + sprite_width => SCREEN_WIDTH
      */

      float fadeIn = getNormalizedFadeInTime();
      float fadeOut = getNormalizedFadeOutTime();

      float fade = 0.0f;

      if (mAnimationTime.elapsed() <= mFadeInDuration)
         fade = sin(fadeIn * M_PI * 0.5f);
      else if (mAnimationTime.elapsed() > (mFadeInDuration + mIdleDuration) )
         fade = cos(fadeOut * M_PI * 0.5f);
      else
         fade = 1.0f;

      float iconOffsetX = 0.0f;
      float iconOffsetY = 0.0f;

      float iconWidth = 0.0f;
      float iconHeight = 0.0f;

      float iconTop = 0.0f;
      float iconLeft = 0.0f;

      float bubbleOffsetX = 0.0f;
      float bubbleOffsetY = 0.0f;

      float spriteOffsetX = 0.0f;
      float spriteOffsetY = 0.0f;

      float spriteWidth = 0.0f;
      float spriteHeight = 0.0f;

      bool alignRight = (
               help->getLocation() == Constants::HelpLocationButtomRight
            || help->getLocation() == Constants::HelpLocationTopRight
         );

      bool alignTop = (
               help->getLocation() == Constants::HelpLocationTopLeft
            || help->getLocation() == Constants::HelpLocationTopRight
         );

      PSDLayer *sprite=
            (help->getSeverity() == Constants::HelpSeverityNotification)
               ? mLayerSpriteInfo
               : mLayerSpriteError;

      spriteWidth = sprite->getWidth();
      spriteHeight = sprite->getHeight();

      spriteOffsetX =
         alignRight
            ? SCREEN_WIDTH - (spriteWidth * fade)
            : 0.0f;

      spriteOffsetY =
         alignTop
            ? SCREEN_OFFSET_TOP
            : SCREEN_HEIGHT - SCREEN_OFFSET_TOP;

      drawQuad(
         sprite,
         spriteOffsetX,
         spriteOffsetX + spriteWidth,
         spriteOffsetY,
         spriteOffsetY + spriteHeight,
         1.0f
      );

      float textAlpha = 0.0f;

      float alpha =
         1.0f - getNormalizedFadeOutTime();

      // bubble with text is drawn after fade in
      if (mAnimationTime.elapsed() > help->getFadeInDuration())
      {
         float t = getNormalizedPopupTime();
         float amp = (1.0f - t);
         float frequence = 50.0f;
         float offset = (t - 1.0f) * 0.3f;
         float x = (1.0f + sin(frequence * t) * 0.75) * 0.5f;
         float scale = x * amp + offset;
         scale *= 0.3f;

         // calc text alpha
         if (mAnimationTime.elapsed() <= mFadeInDuration)
         {
            textAlpha = sin(t);
            textAlpha *= textAlpha;
         }
         else if (mAnimationTime.elapsed() > (mFadeInDuration + mIdleDuration))
         {
            textAlpha = cos(fadeOut * M_PI * 0.5f);
         }
         else
         {
            textAlpha = 1.0f;
         }

         // bubble

         bubbleOffsetX =
            alignRight
               ? SCREEN_WIDTH - mPsd.getWidth() + mLayerBubble->getLeft()
               : mLayerBubble->getLeft();

         bubbleOffsetY =
            alignTop
               ? SCREEN_OFFSET_TOP + mLayerBubble->getTop()
               : SCREEN_HEIGHT - SCREEN_OFFSET_TOP + mLayerBubble->getTop();

         drawQuad(
            mLayerBubble,
            bubbleOffsetX,
            bubbleOffsetX + mLayerBubble->getWidth(),
            bubbleOffsetY,
            bubbleOffsetY + mLayerBubble->getHeight(),
            alpha,
            scale * mLayerBubble->getWidth(),
            scale * mLayerBubble->getHeight()
         );

         // draw icon

         PSDLayer* iconTexture =
           (help->getSeverity() == Constants::HelpSeverityNotification)
               ? mLayerIconInfo
               : mLayerIconError;

         iconWidth =
            (help->getSeverity() == Constants::HelpSeverityNotification)
                ? mLayerIconInfo->getWidth()
                : mLayerIconError->getWidth();

         iconHeight =
            (help->getSeverity() == Constants::HelpSeverityNotification)
                ? mLayerIconInfo->getHeight()
                : mLayerIconError->getHeight();

         iconTop =
            (help->getSeverity() == Constants::HelpSeverityNotification)
                ? mLayerIconInfo->getTop()
                : mLayerIconError->getTop();

         iconLeft =
            (help->getSeverity() == Constants::HelpSeverityNotification)
                ? mLayerIconInfo->getLeft()
                : mLayerIconError->getLeft();

         iconOffsetX =
            alignRight
               ? SCREEN_WIDTH - mPsd.getWidth() + iconLeft
               : iconLeft;

         iconOffsetY =
            alignTop
               ? SCREEN_OFFSET_TOP + iconTop
               : SCREEN_HEIGHT - SCREEN_OFFSET_TOP + iconTop;

         drawQuad(
            iconTexture,
            iconOffsetX,
            iconOffsetX + iconWidth,
            iconOffsetY,
            iconOffsetY + iconHeight,
            alpha,
            scale * iconWidth,
            scale * iconHeight
         );

         // draw font
         float textTitleOffsetX = 0.0f;
         float textTitleOffsetY = 0.0f;
         float text1OffsetX = 0.0f;
         // float text1OffsetY = 0.0f;
         float text2OffsetX = 0.0f;
         float text2OffsetY = 0.0f;
         float text3OffsetX = 0.0f;
         float text3OffsetY = 0.0f;

         textTitleOffsetX =
            alignRight
               ? SCREEN_WIDTH - mPsd.getWidth() + mLayerLineTitle->getLeft()
               : mLayerLineTitle->getLeft();

         textTitleOffsetY =
            alignTop
               ? SCREEN_OFFSET_TOP + mLayerLineTitle->getTop()
               : SCREEN_HEIGHT - SCREEN_OFFSET_TOP + mLayerLineTitle->getTop();

         text1OffsetX =
            alignRight
               ? SCREEN_WIDTH - mPsd.getWidth() + mLayerLine1->getLeft()
               : mLayerLine1->getLeft();

//         text1OffsetY =
//            alignTop
//               ? SCREEN_OFFSET_TOP + mLayerLine1->getTop()
//               : SCREEN_HEIGHT - SCREEN_OFFSET_TOP + mLayerLine1->getTop();

         text2OffsetX =
            alignRight
               ? SCREEN_WIDTH - mPsd.getWidth() + mLayerLine2->getLeft()
               : mLayerLine2->getLeft();

         text2OffsetY =
            alignTop
               ? SCREEN_OFFSET_TOP + mLayerLine2->getTop()
               : SCREEN_HEIGHT - SCREEN_OFFSET_TOP + mLayerLine2->getTop();

         text3OffsetX =
            alignRight
               ? SCREEN_WIDTH - mPsd.getWidth() + mLayerLine3->getLeft()
               : mLayerLine3->getLeft();

         text3OffsetY =
            alignTop
               ? SCREEN_OFFSET_TOP + mLayerLine3->getTop()
               : SCREEN_HEIGHT - SCREEN_OFFSET_TOP + mLayerLine3->getTop();

         if (help->getSeverity() == Constants::HelpSeverityError)
            glColor4f(COLOR_TITLE_ERROR, textAlpha);
         else
            glColor4f(COLOR_TITLE_INFO, textAlpha);

         mFont->buildVertices(
            TEXT_HEADLINE_SCALE,
            (help->getSeverity() == Constants::HelpSeverityError)
               ? "Error"
               : "Information",
            textTitleOffsetX,
            textTitleOffsetY
         );
         mFont->draw();

         glColor4f(0.2f, 0.2f, 0.2f, textAlpha);

         mFont->buildVertices(TEXT_FONT_SCALE, qPrintable(messageLine1), text1OffsetX, text2OffsetY);
         mFont->draw( mFont->getVertices() );

         mFont->buildVertices(TEXT_FONT_SCALE, qPrintable(messageLine2), text2OffsetX, text3OffsetY);
         mFont->draw( mFont->getVertices() );

         mFont->buildVertices(TEXT_FONT_SCALE, qPrintable(messageLine3), text3OffsetX, text3OffsetY + (text3OffsetY - text2OffsetY));
         mFont->draw( mFont->getVertices() );

         glColor4f(1,1,1,1);
      }

      cleanupGlParameters();
   }
}


void GameHelpDrawable::initGlParameters()
{
   Matrix ortho= Matrix::ortho(
      0.0f,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      0.0f,
      -1.0f,
      1.0f
   );

   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf( ortho );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


void GameHelpDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


void GameHelpDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* layer = mPsd.getLayer(l);

      QString layerName = layer->getName();

      PSDLayer* renderLayer= new PSDLayer( layer );

      if (layerName == LAYER_ICON_INFO)
      {
         mLayerIconInfo = renderLayer;
      }
      else if (layerName == LAYER_SPRITE_INFO)
      {
         mLayerSpriteInfo = renderLayer;
      }
      else if (layerName == LAYER_ICON_ERROR)
      {
         mLayerIconError = renderLayer;
      }
      else if (layerName == LAYER_SPRITE_ERROR)
      {
         mLayerSpriteError = renderLayer;
      }
      else if (layerName == LAYER_LABEL_ERROR_TITLE)
      {
         mLayerTitle = renderLayer;
      }
      else if (layerName == LAYER_ERROR_BUBBLE)
      {
         mLayerBubble = renderLayer;
      }
      else if (layerName == LAYER_OFFSET_TITLE)
      {
         mLayerLineTitle = renderLayer;
      }
      else if (layerName == LAYER_OFFSET_1)
      {
         mLayerLine1 = renderLayer;
      }
      else if (layerName == LAYER_OFFSET_2)
      {
         mLayerLine2 = renderLayer;
      }
      else if (layerName == LAYER_OFFSET_3)
      {
         mLayerLine3 = renderLayer;
      }
   }
}



float GameHelpDrawable::getNormalizedPopupTime() const
{
   float time =
      qMin(
         1.0f,
         (float)(mAnimationTime.elapsed() - mFadeInDuration) / (float)mPopupDuration
      );

   return time;
}


float GameHelpDrawable::getNormalizedFadeInTime() const
{
   float time =
      qMin(
         1.0f,
         (float)mAnimationTime.elapsed() / (float)mFadeInDuration
      );

   return time;
}


float GameHelpDrawable::getNormalizedFadeOutTime() const
{
   float elapsed =
      (float)mAnimationTime.elapsed()
      - mIdleDuration
      - mFadeInDuration;

   float val = elapsed / mFadeOutDuration;
   float time = qMin(1.0f, val);

   return time;
}


void GameHelpDrawable::playSound(HelpElement *element)
{
   switch (element->getSeverity())
   {
      case Constants::HelpSeverityError:
      {
         SoundManager::getInstance()->playSoundError();
         break;
      }

      case Constants::HelpSeverityNotification:
      {
         SoundManager::getInstance()->playSoundInfo();
         break;
      }

      default:
         break;
   }
}

