#include "gameplaybackdisplay.h"

// Qt
#include "gldevice.h"

// shared
#include "constants.h"

// game
#include "fontpool.h"
#include "gameplayback.h"
#include "gamestatemachine.h"


GamePlaybackDisplay::GamePlaybackDisplay(QObject *parent)
 : QObject(parent),
   mFont(0)
{
}



void GamePlaybackDisplay::initialize()
{
   // init font
   mFont = FontPool::Instance()->get("large");
}


void GamePlaybackDisplay::draw(float time) const
{
   if (GamePlayback::getInstance()->isReplaying())
   {
      Constants::GameState state = GameStateMachine::getInstance()->getState();

      if (
            state == Constants::GamePreparing
         || state == Constants::GameActive
      )
      {
         if ( ((int) (time /** 0.01*/) ) % 2 )
         {
            initGlParameters();
            drawTextOverlay();
            cleanupGlParameters();
         }
      }
   }
}


void GamePlaybackDisplay::initGlParameters() const
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glOrtho(0, 1920, 1080, 0, -1.0f, 1.0f);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


void GamePlaybackDisplay::cleanupGlParameters() const
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


void GamePlaybackDisplay::drawTextOverlay() const
{
   float alpha = 0.3f;

   glColor4f(1.0f, 1.0f, 1.0f, alpha);

   mFont->buildVertices(
      0.5f,
      qPrintable(tr("press any key to start")),
      0.0f,
      700.0f,
      1920.0f
   );

   mFont->draw(mFont->getVertices());

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
