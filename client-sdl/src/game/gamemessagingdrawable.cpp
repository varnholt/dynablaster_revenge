// GLES3 port of client/src/game/gamemessagingdrawable.cpp.

#include "gamemessagingdrawable.h"

#include <QKeyEvent>

#include "bombermanclient.h"
#include "gamestatemachine.h"
#include "menus/fontpool.h"
#include "menus/psdlayer.h"
#include "wordwrap.h"

#include "constants.h"
#include "playerinfo.h"

#include "framework/globaltime.h"

#include "gldevice.h"

#include <math.h>

#define LINEEDIT_SAY "lineedit_say"
#define LINEEDIT_SAY_Y 1057
#define LABEL_PLAYER_NAME "chat_player_name"
#define LABEL_PLAYER_OFFSET_Y 30
#define MESSAGE_LAYER_POSITION 1020
#define MESSAGE_STACK_OFFSET 35
#define MESSAGE_OFFSET_X 5
#define MESSAGE_OFFSET_Y 1060
#define MESSAGE_OFFSET_DIFF 25.0f
#define MESSAGE_LENGTH_MAX 400
#define FONT_SCALE_NICK 0.08f
#define FONT_SCALE_MESSAGE 0.15f
#define ACTIVATION_TIME 300
#define ACTIVATION_FACTOR 0.003333333f
#define ACTIVATION_OFFSET 100
#define CURSOR_UPDATE_TIME 0.5f
#define MESSAGE_FIELD_WIDTH 80

GameMessagingDrawable::GameMessagingDrawable(RenderDevice* dev)
    : QObject(),
      Drawable(dev),
      mLineEditSayLayer(nullptr),
      mPlayerNameLayer(nullptr),
      mFont(nullptr),
      mActive(false),
      mCursorPosition(0),
      mCursorTexture(0),
      mCursorVertexBuffer(0)
{
   mFilename = "data/game/messaging_bar.psd";

   BombermanClient::getInstance()->gameStartedSignal.connect([this]() { disableIngameMessaging(); });
   connect(GameStateMachine::getInstance(), SIGNAL(stateChanged()), this, SLOT(gameStateChanged()));
}

GameMessagingDrawable::~GameMessagingDrawable()
{
   for (auto* layer : mPsdLayers)
   {
      delete layer;
   }
   mPsdLayers.clear();
}

void GameMessagingDrawable::messageReceived(int /*senderId*/, const QString& text, bool typingFinished)
{
   if (mVisible)
   {
      if (text.trimmed().isEmpty())
         return;

      if (typingFinished)
      {
         std::vector<QString> lines;
         for (const auto& lineStd : WordWrap::wrap(text.toStdString(), CLIENT_MESSAGE_TEXT_MAXIMUM))
         {
            lines.push_back(QString::fromStdString(lineStd));
         }

         // prepend nick to other lines if available
         QStringList processedLines;
         QString message = lines.at(0);
         processedLines.push_back(message);

         if (lines.size() > 1)
         {
            int startIndex = message.indexOf(":");
            if (startIndex != -1)
            {
               QString nick = message.left(startIndex);

               if (message.trimmed() == QString("%1:").arg(nick))
                  processedLines.clear();

               for (int i = 1; i < lines.size(); i++)
               {
                  processedLines.push_back(QString("%1: %2").arg(nick).arg(lines.at(i)));
               }
            }
         }

         for (const QString& line : processedLines)
         {
            mFont->setColor(1.0f, 1.0f, 1.0f, 1.0f);
            mFont->buildVertices(FONT_SCALE_MESSAGE, qPrintable(line), MESSAGE_OFFSET_X, MESSAGE_OFFSET_Y - MESSAGE_STACK_OFFSET);

            AnimatedGameMessage* message = new AnimatedGameMessage();
            message->setMessage(line.toStdString());
            message->setVertices(mFont->getVertices());
            message->initialize();

            mMessages.insert(mMessages.begin(), message);

            connect(message, SIGNAL(expired()), this, SLOT(popMessage()));
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
      if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
      {
         bool wasActive = isActive();
         toggleActive();

         if (wasActive)
         {
            if (!mMessage.isEmpty())
            {
               BombermanClient::getInstance()->sendMessage(mMessage.toStdString(), true);
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
                  mMessage.chop(1);
                  moveCursorLeft();
               }
               else
               {
                  if (getCursorPosition() > 0)
                  {
                     mMessage = mMessage.replace(getCursorPosition() - 1, 1, "");
                     moveCursorLeft();
                  }
               }
            }
            else if (event->key() == Qt::Key_Delete)
            {
               if (!isCursorAtEnd())
               {
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
                  if (mMessage.length() < MESSAGE_LENGTH_MAX)
                     mMessage.append(event->text());
               }
               else
               {
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
}

void GameMessagingDrawable::updateMessageVertices()
{
   int i0 = mCursorPosition - MESSAGE_FIELD_WIDTH;
   i0 = qMax(i0, 0);
   QString visibleText = mMessage.mid(i0, MESSAGE_FIELD_WIDTH);

   mFont->setColor(1.0f, 1.0f, 1.0f, 1.0f);
   mFont->buildVertices(FONT_SCALE_MESSAGE, qPrintable(visibleText), mLineEditSayLayer->getLeft(), LINEEDIT_SAY_Y);

   mMessageVertices.copy(mFont->getVertices());
}

void GameMessagingDrawable::buildNickVertices()
{
   PlayerInfo* playerInfo = BombermanClient::getInstance()->getCurrentPlayerInfo();
   if (playerInfo)
   {
      mFont->setColor(1.0f, 1.0f, 1.0f, 1.0f);
      mFont->buildVertices(FONT_SCALE_NICK, playerInfo->getNick().c_str(), 0, 0);
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
      mMessage = QString::fromStdString(BombermanClient::getInstance()->getMessage());
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
   Matrix ortho = Matrix::ortho(0.0f, 1920, 1080, 0.0f, -1.0f, 1.0f);
   static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(ortho);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   activeDevice->setShader(0);
}

void GameMessagingDrawable::cleanupGlParameters()
{
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
   // the legacy 5-pass glTranslatef offset trick (4 black outline passes at +-1px, then a real
   // pass) becomes 5 real push()/pop() brackets with a translated world matrix - GLES3 has no
   // matrix stack to abuse for this, but the effect is identical.
   const float offsets[5][3] = {{0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.75f, 0.0f, 0.0f}};

   for (int f = 0; f < 5; f++)
   {
      const float col = offsets[f][0];
      const float x = offsets[f][1];
      const float y = offsets[f][2];

      float ty = 0.0f;

      if (isActive() && drawUserInput)
      {
         mFont->setColor(col, col, col, 1.0f);
         activeDevice->push(Matrix::position(x, y + ty, 0.0f));
         mFont->draw(mMessageVertices);
         activeDevice->pop();
      }

      for (int i = 0; i < mMessages.size(); i++)
      {
         AnimatedGameMessage* message = mMessages.at(i);

         ty -= MESSAGE_OFFSET_DIFF;

         mFont->setColor(col, col, col, 1.0f - (i * 0.08f));
         activeDevice->push(Matrix::position(x, y + ty, 0.0f));
         mFont->draw(message->getVertices());
         activeDevice->pop();
      }
   }
}

void GameMessagingDrawable::paintGL()
{
   initGlParameters();

   bool drawUserInput = drawMessageOverlay();

   drawText(drawUserInput);

   if (drawUserInput)
      drawCursor();

   cleanupGlParameters();
}

bool GameMessagingDrawable::drawMessageOverlay()
{
   bool fullyVisible = false;

   int time = getActivationTime().isValid() ? static_cast<int>(getActivationTime().elapsed()) : ACTIVATION_TIME;
   float factor = (qMax(ACTIVATION_TIME - time, 0)) * ACTIVATION_FACTOR;

   if (!isActive())
      factor = 1.0f - factor;

   int offset = MESSAGE_LAYER_POSITION + static_cast<int>(ACTIVATION_OFFSET * factor);

   fullyVisible = (offset == MESSAGE_LAYER_POSITION);

   for (int layerIndex = 0; layerIndex < mPsdLayers.size(); layerIndex++)
   {
      PSDLayer* layer = mPsdLayers[layerIndex];
      layer->render(0, static_cast<float>(offset), 1.0f - factor);
   }

   if (!getMessage().isEmpty())
   {
      mFont->setColor(1.0f, 1.0f, 1.0f, 1.0f);
      activeDevice->push(Matrix::position(static_cast<float>(mPlayerNameLayer->getLeft()), offset + LABEL_PLAYER_OFFSET_Y, 0.0f));
      mFont->draw(mMessageVertices);
      activeDevice->pop();
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

const QString& GameMessagingDrawable::getMessage() const
{
   return mMessage;
}

void GameMessagingDrawable::setMessage(const QString& value)
{
   mMessage = value;
}

void GameMessagingDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* layer = mPsd.getLayer(l);
      PSDLayer* renderLayer = new PSDLayer(layer);

      mPsdLayers.push_back(renderLayer);

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
   setCursorPosition(qMin(getCursorPosition() + 1, getMessage().length()));
}

void GameMessagingDrawable::moveCursorLeft()
{
   setCursorPosition(qMax(getCursorPosition() - 1, 0));
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
   // see MenuPageTextEditItem::drawCursor()'s own doc comment - same lazily-created 1x1 white
   // texture + dynamic quad replacement for the legacy untextured glColor4ub'd quad.
   float alpha = 0.25f + fmod(GlobalTime::Instance()->getTime(), 0.5f);

   float left = 0.0f;
   float right = 0.0f;
   float top = 0.0f;
   float bottom = 0.0f;

   mFont->getCursor(FONT_SCALE_MESSAGE, getCursorPosition(), left, right, top, bottom);

   if (mCursorTexture == 0)
   {
      unsigned int white = 0xFFFFFFFF;
      mCursorTexture = activeDevice->createTexture(&white, 1, 1, 0);
   }

   glBindTexture(GL_TEXTURE_2D, mCursorTexture);

   glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

   const float quad[] = {
      left, top, -1.0f, 0.0f, 0.0f, right, top,    -1.0f, 1.0f, 0.0f, right, bottom, -1.0f, 1.0f, 1.0f,
      left, top, -1.0f, 0.0f, 0.0f, right, bottom, -1.0f, 1.0f, 1.0f, left,  bottom, -1.0f, 0.0f, 1.0f,
   };

   if (mCursorVertexBuffer == 0)
      mCursorVertexBuffer = activeDevice->createVertexBuffer(sizeof(quad), true);
   else
      activeDevice->allocateVertexBuffer(mCursorVertexBuffer, sizeof(quad), true);

   void* dst = activeDevice->lockVertexBuffer(mCursorVertexBuffer, sizeof(quad));
   memcpy(dst, quad, sizeof(quad));
   activeDevice->unlockVertexBuffer(mCursorVertexBuffer);

   activeDevice->push(Matrix());
   activeDevice->setParameter(activeDevice->getParameterIndex("alpha"), (128.0f / 255.0f) * alpha);

   glBindBuffer(GL_ARRAY_BUFFER, mCursorVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   activeDevice->pop();

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
