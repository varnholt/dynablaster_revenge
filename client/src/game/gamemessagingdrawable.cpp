#include "gamemessagingdrawable.h"

// Qt
#include <QGLContext>
#include <QKeyEvent>

// game
#include "bombermanclient.h"
#include "gamestatemachine.h"
#include "psdlayer.h"
#include "fontpool.h"
#include "wordwrap.h"

// shared
#include "constants.h"
#include "playerinfo.h"

// framework
#include "globaltime.h"

// engine
#include "gldevice.h"
#include "renderdevice.h"

#include <math.h>

#define LINEEDIT_SAY           "lineedit_say"
#define LINEEDIT_SAY_Y         1057
#define LABEL_PLAYER_NAME      "chat_player_name"
#define LABEL_PLAYER_OFFSET_Y  30
#define MESSAGE_LAYER_POSITION 1020
#define MESSAGE_STACK_OFFSET   35
#define MESSAGE_OFFSET_X       5
#define MESSAGE_OFFSET_Y       1060
#define MESSAGE_OFFSET_DIFF    25.0f
#define MESSAGE_LENGTH_MAX     400
#define FONT_SCALE_NICK        0.08f
#define FONT_SCALE_MESSAGE     0.15f
#define ACTIVATION_TIME        300
#define ACTIVATION_FACTOR      0.003333333f
#define ACTIVATION_OFFSET      100
#define CURSOR_UPDATE_TIME     0.5f
#define MESSAGE_FIELD_WIDTH    80

GameMessagingDrawable::GameMessagingDrawable(RenderDevice* dev)
   : QObject(),
     Drawable(dev),
     mLineEditSayLayer(0),
     mPlayerNameLayer(0),
     mFont(0),
     mActive(false),
     mCursorPosition(0)
{
   mFilename = "data/game/messaging_bar.psd";

   connect(
      BombermanClient::getInstance(),
      SIGNAL(gameStarted()),
      this,
      SLOT(disableIngameMessaging())
   );

   connect(
      GameStateMachine::getInstance(),
      SIGNAL(stateChanged()),
      this,
      SLOT(gameStateChanged())
   );
}


GameMessagingDrawable::~GameMessagingDrawable()
{
   qDeleteAll(mPsdLayers);
   mPsdLayers.clear();
}


void GameMessagingDrawable::messageReceived(
   int /*senderId*/,
   const QString& text,
   bool typingFinished
)
{   
   if (mVisible)
   {
      if (text.trimmed().isEmpty())
         return;

      if (typingFinished)
      {
         QStringList lines =
            WordWrap::wrap(
               text,
               CLIENT_MESSAGE_TEXT_MAXIMUM
            );

         // prepend nick to others lines if available
         QStringList processedLines;
         QString message = lines.at(0);
         processedLines.push_back(message);

         if (lines.length() > 1)
         {
            int startIndex = message.indexOf(":");
            if (startIndex != -1)
            {
               QString nick = message.left(startIndex);

               if (message.trimmed() == QString("%1:").arg(nick))
                  processedLines.clear();

               for (int i = 1; i < lines.size(); i++)
               {
                  processedLines.push_back(
                     QString("%1: %2").arg(nick).arg(lines.at(i))
                  );
               }
            }
         }

         foreach (const QString& line, processedLines)
         {
            mFont->buildVertices(
               FONT_SCALE_MESSAGE,
               qPrintable(line),
               MESSAGE_OFFSET_X,
               MESSAGE_OFFSET_Y - MESSAGE_STACK_OFFSET
            );

            AnimatedGameMessage* message = new AnimatedGameMessage();
            message->setMessage(line);
            message->setVertices(mFont->getVertices());
            message->initialize();

            mMessages.prepend(message);

            connect(
               message,
               SIGNAL(expired()),
               this,
               SLOT(popMessage())
            );
         }
      }
   }
}


void GameMessagingDrawable::clearMessage()
{
   mMessage.clear();
}


void GameMessagingDrawable::keyPressEvent(QKeyEvent* event)
{
   if (GameStateMachine::getInstance()->getState() == Constants::GameActive)
   {
      if (
            event->key() == Qt::Key_Return
         || event->key() == Qt::Key_Enter
      )
      {
         bool wasActive = isActive();
         toggleActive();

         if (wasActive)
         {
            if (!mMessage.isEmpty())
            {
               BombermanClient::getInstance()->sendMessage(
                  mMessage,
                  true
               );

               clearMessage();
            }
         }
         else
         {
            setCursorPosition(mMessage.length());
         }
      }
      else
      {
         if (isActive())
         {
            if (event->key() == Qt::Key_Escape)
            {
               clearMessage();
               toggleActive();
            }
            else if (event->key() == Qt::Key_Backspace)
            {
               if (isCursorAtEnd())
               {
                  // chop from end
                  mMessage.chop(1);
                  moveCursorLeft();
               }
               else
               {
                  if (getCursorPosition() > 0)
                  {
                     // replace chars
                     mMessage = mMessage.replace(getCursorPosition() - 1, 1, "");
                     moveCursorLeft();
                  }
               }
            }
            else if (event->key() == Qt::Key_Delete)
            {
               if (!isCursorAtEnd())
               {
                  // replace chars
                  mMessage = mMessage.replace(getCursorPosition(), 1, "");
               }
            }
            else if (event->key() == Qt::Key_Left)
            {
               moveCursorLeft();
            }
            else if (event->key() == Qt::Key_Right)
            {
               moveCursorRight();
            }
            else if (event->key() == Qt::Key_Home)
            {
               moveCursorToStart();
            }
            else if (event->key() == Qt::Key_End)
            {
               moveCursorToEnd();
            }
            else if (!event->text().isEmpty())
            {
               if (isCursorAtEnd())
               {
                  // append chars
                  if (mMessage.length() < MESSAGE_LENGTH_MAX)
                     mMessage.append(event->text());
               }
               else
               {
                  // replace chars
                  mMessage = mMessage.replace(getCursorPosition(), 1, event->text());
               }

               moveCursorRight();
            }
         }
      }

      updateMessageVertices();
   }
}


void GameMessagingDrawable::setVisible(bool visible)
{
   Drawable::setVisible(visible);

   if (visible)
   {
      // build nick vertices only once
      buildNickVertices();      
   }
//   else
//   {
//      clearMessage();
//   }
}


void GameMessagingDrawable::updateMessageVertices()
{
   int i0 = mCursorPosition - MESSAGE_FIELD_WIDTH;
   i0 = qMax(i0, 0);
   QString visibleText = mMessage.mid(i0, MESSAGE_FIELD_WIDTH);

   // build message vertices
   mFont->buildVertices(
      FONT_SCALE_MESSAGE,
      qPrintable(visibleText),
      mLineEditSayLayer->getLeft(),
      LINEEDIT_SAY_Y
   );

   mMessageVertices.copy(mFont->getVertices());
}


void GameMessagingDrawable::buildNickVertices()
{
   PlayerInfo *playerInfo= BombermanClient::getInstance()->getCurrentPlayerInfo();
   if (playerInfo)
   {
      mFont->buildVertices(
         FONT_SCALE_NICK,
         qPrintable(
            playerInfo->getNick()
         ),
         0,
         0
      );
      mNickVertices.copy(mFont->getVertices());
   }
   else
   {
      mNickVertices.clear();
   }
}


void GameMessagingDrawable::popMessage()
{
   sender()->deleteLater();

   // nice.
   mMessages.pop_back();
}


void GameMessagingDrawable::disableIngameMessaging()
{
   setActive(false);
}


void GameMessagingDrawable::gameStateChanged()
{
   if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
   {
      disableIngameMessaging();
   }
   else if (GameStateMachine::getInstance()->getState() == Constants::GameActive)
   {
      // copy buffer from client to textedit
      mMessage = BombermanClient::getInstance()->getMessage();
      updateMessageVertices();
   }
}


void GameMessagingDrawable::initializeGL()
{
   mFont = FontPool::Instance()->get("default");

   initializeLayers();
}


void GameMessagingDrawable::initGlParameters()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(
      0.0f,
      1920,
      1080,
      0.0f,
      -1.0f,
      1.0f
   );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


void GameMessagingDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


void GameMessagingDrawable::setActive(bool active)
{
   mActive = active;
}


bool GameMessagingDrawable::isActive() const
{
   return mActive;
}


void GameMessagingDrawable::toggleActive()
{
   setActive(!isActive());   
   restartActivationTime();   
}


void GameMessagingDrawable::drawText(bool drawUserInput)
{
   AnimatedGameMessage* message = 0;

   int x = 0;
   int y = 0;
   float col = 0.0f;

   for (int f = 0; f < 5; f++)
   {
      switch (f)
      {
         case 0:
         {
            col = 0.0f;
            x = -1;
            y = 0;
            break;
         }

         case 1:
         {
            col = 0.0f;
            x = 1;
            y = 0;
            break;
         }

         case 2:
         {
            col = 0.0f;
            x = 0;
            y = -1;
            break;
         }

         case 3:
         {
            col = 0.0f;
            x = 0;
            y = 1;
            break;
         }

         case 4:
         {
            col = 0.75f;
            x = 0;
            y = 0;
            break;
         }
      }

      glPushMatrix();

      glTranslatef(x, y, 0);

      // draw user input
      if (isActive() && drawUserInput)
      {
         glColor4f(col, col, col, 1.0f);
         mFont->draw( mMessageVertices );
      }

      // draw messages
      for (int i = 0; i < mMessages.size(); i++)
      {
         message = mMessages.at(i);

         // reset opacity
         glColor4f(
            col,
            col,
            col,
            1.0f - (i * 0.08)
         );

         glTranslatef(0.0f, -MESSAGE_OFFSET_DIFF, 0.0f);

         mFont->draw( message->getVertices() );
      }

      glPopMatrix();
   }
}


void GameMessagingDrawable::paintGL()
{
   /*

     +----------------------------------------------------+
     |                                                    | GameMessage[0]
     +----------------------------------------------------+
     |                                                    | GameMessage[1]
     +----------------------------------------------------+
     |                                                    | GameMessage[2]
     +----------------------------------------------------+

   */

   initGlParameters();

   // draw message overlay
   bool drawUserInput = drawMessageOverlay();

   drawText(drawUserInput);

   if (drawUserInput)
      drawCursor();

   // cleanup
   cleanupGlParameters();
}


bool GameMessagingDrawable::drawMessageOverlay()
{
   // init
   bool fullyVisible = false;

   // do positioning
   //                                 (300 .. 0)                             *    0.0033333
   // = 1..0
   int time = getActivationTime().isValid() ? getActivationTime().elapsed() : ACTIVATION_TIME;
   float factor = (qMax(ACTIVATION_TIME - time, 0)) * ACTIVATION_FACTOR;

   if (!isActive())
      factor = 1.0f - factor;

   int offset = MESSAGE_LAYER_POSITION + ACTIVATION_OFFSET * factor;

   fullyVisible = (offset == MESSAGE_LAYER_POSITION);

   // draw overlay layers
   for (int layerIndex = 0; layerIndex < mPsdLayers.size(); layerIndex++)
   {
      PSDLayer *layer= mPsdLayers[layerIndex];

      layer->render(0, offset, 1.0f - factor);
   }

   // draw text
   if (!getMessage().isEmpty())
   {
      glPushMatrix();
      glTranslatef(
         mPlayerNameLayer->getLeft(),
         offset + LABEL_PLAYER_OFFSET_Y,
         0
      );
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      mFont->draw( mMessageVertices );
      glPopMatrix();
   }

   return fullyVisible;
}


void GameMessagingDrawable::restartActivationTime()
{
   mActivationTime.restart();
}


const FrameTimer& GameMessagingDrawable::getActivationTime() const
{
   return mActivationTime;
}


const QString &GameMessagingDrawable::getMessage() const
{
   return mMessage;
}


void GameMessagingDrawable::setMessage(const QString &value)
{
   mMessage = value;
}


void GameMessagingDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* layer= mPsd.getLayer(l);
      PSDLayer* renderLayer= new PSDLayer( layer );

      mPsdLayers << renderLayer;

      if (QString(layer->getName()).compare(LINEEDIT_SAY) == 0)
      {
         mLineEditSayLayer = renderLayer;
      }

      else if (QString(layer->getName()).compare(LABEL_PLAYER_NAME) == 0)
      {
         mPlayerNameLayer = renderLayer;
      }
   }
}



void GameMessagingDrawable::setCursorPosition(int index)
{
   mCursorPosition = index;

   updateMessageVertices();
}


int GameMessagingDrawable::getCursorPosition() const
{
   return mCursorPosition;
}


void GameMessagingDrawable::moveCursorRight()
{
   setCursorPosition(
      qMin(
         getCursorPosition() + 1,
         getMessage().length()
      )
   );
}


void GameMessagingDrawable::moveCursorLeft()
{
   setCursorPosition(
      qMax(
         getCursorPosition() - 1,
         0
      )
   );
}


void GameMessagingDrawable::moveCursorToStart()
{
   setCursorPosition(0);
}


void GameMessagingDrawable::moveCursorToEnd()
{
   setCursorPosition(getMessage().length());
}


bool GameMessagingDrawable::isCursorAtEnd() const
{
   return (getCursorPosition() == getMessage().length());
}


void GameMessagingDrawable::drawCursor()
{
   float alpha = 0.25f + fmod(GlobalTime::Instance()->getTime(), 0.5f);

   // qDebug("%f", alphaFactor);

   float left = 0.0f;
   float right = 0.0f;
   float top = 0.0f;
   float bottom = 0.0f;

   mFont->getCursor(
      FONT_SCALE_MESSAGE,
      getCursorPosition(),
      left, right, top, bottom
   );

   glBindTexture(GL_TEXTURE_2D, 0);

   glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

   if (mColor.isValid())
   {
      glColor4ub(mColor.red(), mColor.green(), mColor.blue(), 128 * alpha);
   }
   else
   {
      glColor4ub(255, 255, 255, 128 * alpha);
   }

   glBegin(GL_QUADS);

   glVertex3f(
      left,
      top,
      0
   );

   glVertex3f(
      right,
      top,
      0
   );

   glVertex3f(
      right,
      bottom,
      0
   );

   glVertex3f(
      left,
      bottom,
      0
   );

   glEnd();

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
