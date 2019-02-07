#include "musicplayerdrawable.h"

// menus
#include "fontpool.h"
#include "psdlayer.h"

// framework
#include "gldevice.h"
#include "framework/timerhandler.h"

// game settings
#include "gamesettings.h"

// tools
#include "tools/filestream.h"

// game
#include "soundmanager.h"

// Qt
#include <QGLContext>
#include <QImage>

// cmath
#include <math.h>


#define ANIMATION_DELAY_TIME 3000
#define FADE_IN_TIME 2000
#define FADE_IDLE_TIME 5000
#define FADE_OUT_TIME 2000

#define LAYER_TRACK_LINE_1 "track_name_line_1"
#define LAYER_TRACK_LINE_2 "track_name_line_2"
#define LAYER_ARTIST "artist_name"
#define LAYER_ALBUM "album_name"

#define MENU_Y_FACTOR_INGAME 0.865f // 1.0f
#define MENU_Y_FACTOR_MENUS 0.865f

#define ALBUM_CORRECTION_OFFSET_X 0
#define ALBUM_CORRECTION_OFFSET_Y 5
#define ARTIST_CORRECTION_OFFSET_X -2
#define ARTIST_CORRECTION_OFFSET_Y 10
#define TRACK_LINE_1_CORRECTION_OFFSET_X 0
#define TRACK_LINE_1_CORRECTION_OFFSET_Y 2
#define TRACK_LINE_2_CORRECTION_OFFSET_X 0
#define TRACK_LINE_2_CORRECTION_OFFSET_Y 2

/*
#define FONT_OFFSET_ARTIST_X 150
#define FONT_OFFSET_ARTIST_Y 40
#define FONT_OFFSET_ALBUM_X 150
#define FONT_OFFSET_ALBUM_Y 80
#define FONT_OFFSET_TRACK_X 150
#define FONT_OFFSET_TRACK_Y 113
*/


//-----------------------------------------------------------------------------
/*!
*/
MusicPlayerDrawable::MusicPlayerDrawable(RenderDevice* dev)
 : QObject(),
   Drawable(dev),
   mFont(0),
   mAnimationFactor(0.0f),
   mFadeIn(false),
   mIdle(false),
   mFadeOut(false),
   mMaxWidth(-1),
   mFontOffsetArtistX(0),
   mFontOffsetArtistY(0),
   mFontOffsetArtistHeight(0),
   mFontOffsetAlbumX(0),
   mFontOffsetAlbumY(0),
   mFontOffsetAlbumHeight(0),
   mFontOffsetTrackLine1X(0),
   mFontOffsetTrackLine1Y(0),
   mFontOffsetTrackLine1Height(0),
   mFontOffsetTrackLine2X(0),
   mFontOffsetTrackLine2Y(0),
   mFontOffsetTrackLine2Height(0),
   mAnimating(false),
   mInGame(false)
{
   mFilename = "data/musicplayer/player.psd";
}


//-----------------------------------------------------------------------------
/*!
*/
MusicPlayerDrawable::~MusicPlayerDrawable()
{
   qDeleteAll(mPsdLayers);
   mPsdLayers.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::initializeGL()
{
   FileStream::addPath("data/musicplayer");

   // init layers
   initializeLayers();

   // init font
   mFont = FontPool::Instance()->get("default");

   FileStream::removePath("data/musicplayer");
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* layer = mPsd.getLayer(l);

      QString layerName = layer->getName();

      if (layerName == LAYER_TRACK_LINE_1)
      {
         mFontOffsetTrackLine1X = layer->getLeft() + TRACK_LINE_1_CORRECTION_OFFSET_X;
         mFontOffsetTrackLine1Y = layer->getTop() + TRACK_LINE_1_CORRECTION_OFFSET_Y;
         mFontOffsetTrackLine1Height = layer->getHeight();
      }
      else if (layerName == LAYER_TRACK_LINE_2)
      {
         mFontOffsetTrackLine2X = layer->getLeft() + TRACK_LINE_2_CORRECTION_OFFSET_X;
         mFontOffsetTrackLine2Y = layer->getTop() + TRACK_LINE_2_CORRECTION_OFFSET_X;
         mFontOffsetTrackLine2Height = layer->getHeight();
      }
      else if (layerName == LAYER_ALBUM)
      {
         mFontOffsetAlbumX = layer->getLeft() + ALBUM_CORRECTION_OFFSET_X;
         mFontOffsetAlbumY = layer->getTop() + ALBUM_CORRECTION_OFFSET_Y;
         mFontOffsetAlbumHeight = layer->getHeight();
      }
      else if (layerName == LAYER_ARTIST)
      {
         mFontOffsetArtistX = layer->getLeft() + ARTIST_CORRECTION_OFFSET_X;
         mFontOffsetArtistY = layer->getTop() + ARTIST_CORRECTION_OFFSET_Y;
         mFontOffsetArtistHeight = layer->getHeight();
      }

      // determine max layer width
      if (layer->getWidth() + layer->getLeft() > mMaxWidth)
         mMaxWidth = layer->getWidth() + layer->getLeft();

      mPsdLayers << new PSDLayer(layer);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::paintGL()
{
   initGlParameters();

   // one quad for each layer
   float xOffset = (1.0f - mAnimationFactor) * mMaxWidth;

   for (int layerIndex = 0; layerIndex < mPsd.getLayerCount(); layerIndex++)
   {
      PSD::Layer* psdLayer = mPsd.getLayer(layerIndex);

      if (psdLayer->isVisible())
      {
         // move top up for menu mode
         float top =
            isInGame()
               ? -psdLayer->getTop() * (1.0 - MENU_Y_FACTOR_INGAME)
               : -psdLayer->getTop() * (1.0 - MENU_Y_FACTOR_MENUS);

         mPsdLayers[layerIndex]->render(-xOffset, top);
      }
   }

   // glPushMatrix();

   // glTranslatef(-xOffset, 0.0f, 0.0f);

   // draw artist
   glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
   mFont->buildVertices(
      0.12f,
      qPrintable(mArtist),
      mFontOffsetArtistX - xOffset,
      (isInGame()
         ? mFontOffsetArtistY * MENU_Y_FACTOR_INGAME
         : mFontOffsetArtistY * MENU_Y_FACTOR_MENUS)
       + mFontOffsetArtistHeight
   );

   mFont->draw();

   // draw album
   glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
   mFont->buildVertices(
      0.1f,
      qPrintable(mAlbum),
      mFontOffsetAlbumX - xOffset,
      (isInGame()
         ? mFontOffsetAlbumY * MENU_Y_FACTOR_INGAME
         : mFontOffsetAlbumY * MENU_Y_FACTOR_MENUS)
      + mFontOffsetAlbumHeight
   );

   mFont->draw();

   // draw track
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   mFont->buildVertices(
      0.1f,
      qPrintable(mTrackLine1),
      mFontOffsetTrackLine1X - xOffset,
      (isInGame()
         ? mFontOffsetTrackLine1Y * MENU_Y_FACTOR_INGAME
         : mFontOffsetTrackLine1Y * MENU_Y_FACTOR_MENUS)
      + mFontOffsetTrackLine1Height
   );

   mFont->draw();

   mFont->buildVertices(
      0.1f,
      qPrintable(mTrackLine2),
      mFontOffsetTrackLine2X - xOffset,
      (isInGame()
         ? mFontOffsetTrackLine2Y * MENU_Y_FACTOR_INGAME
         : mFontOffsetTrackLine2Y * MENU_Y_FACTOR_MENUS)
      + mFontOffsetTrackLine2Height
   );

   mFont->draw();

   // glPopMatrix();

   cleanupGlParameters();
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::animate(float /*time*/)
{
   // "mAnimationFactor == 1" => player is fully visible

   float msecs = FrameTimer::currentTime().msecsTo(mAnimationStopTime);

   if (mFadeIn)
   {
      if (msecs < 0.0f)
      {
         mFadeIn = false;
         mIdle = true;

         mAnimationStopTime = FrameTimer::currentTime();
         mAnimationStopTime = mAnimationStopTime.addMSecs(FADE_IDLE_TIME);
         mAnimationFactor = 1.0f;
      }
      else
      {
         mAnimationFactor = 1.0f - (msecs / FADE_IN_TIME);
         mAnimationFactor = sin(mAnimationFactor * M_PI * 0.5f);
         mAnimationFactor = mAnimationFactor * mAnimationFactor * mAnimationFactor;
      }
   }
   else if (mIdle)
   {
      if (msecs < 0.0f)
      {
         mIdle = false;
         mFadeOut = true;

         mAnimationStopTime = FrameTimer::currentTime();
         mAnimationStopTime = mAnimationStopTime.addMSecs(FADE_OUT_TIME);
         mAnimationFactor = 1.0f;
      }
      else
      {
         mAnimationFactor = 1.0f;
      }
   }
   else if (mFadeOut)
   {
      if (msecs < 0.0f)
      {
         mFadeOut = false;
         mVisible = false;

         // animation ended
         mAnimating = false;

         mAnimationFactor = 0.0f;
      }
      else
      {
         mAnimationFactor = msecs / FADE_OUT_TIME;
         mAnimationFactor = sin(mAnimationFactor * M_PI * 0.5f);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::startAnimation()
{
   mAnimationStopTime = FrameTimer::currentTime();
   mAnimationStopTime = mAnimationStopTime.addMSecs(FADE_IN_TIME);

   mFadeIn = true;
   mVisible = true;
}


//-----------------------------------------------------------------------------
/*!
   \param artist artist who's played
   \param album album album name
   \param track track name
*/
void MusicPlayerDrawable::showCurrentlyPlaying(
   const QString& artist,
   const QString& album,
   const QString& track
)
{   
   if (
         SoundManager::getInstance()->isMusicMuted()
      || qFuzzyCompare(SoundManager::getInstance()->getVolumeMusic(), 0.0f)
      || !GameSettings::getInstance()->getAudioSettings()->isMusicPlayerVisibile()
   )
   {
      return;
   }

   // default situation
   mArtist = artist;
   mAlbum = album;
   mTrackLine2.clear();
   mTrackLine1 = track;

   // fit-in track 2
   int count = 0;
   while (mTrackLine1.length() > 20)
   {
      mTrackLine1 = track;
      mTrackLine2.clear();

      count++;

      QStringList words = mTrackLine1.split(" ");

      for (int i = 0; i < count; i++)
      {
         mTrackLine2.prepend(" ");
         mTrackLine2.prepend(words.takeLast());
      }

      mTrackLine2 = mTrackLine2.trimmed();
      mTrackLine1 = words.join(" ");
   }

//   QStringList trackWords = track.split(" ");
//   if (trackWords.size() > 2)
//   {
//      mTrackLine1 = QString("%1 %2").arg(trackWords[0]).arg(trackWords[1]);
//
//      trackWords.removeFirst();
//      trackWords.removeFirst();
//
//      mTrackLine2 = trackWords.join(" ");
//   }

   if (!mAnimating)
   {
      mAnimating = true;

      TimerHandler::singleShot(
         ANIMATION_DELAY_TIME,
         this,
         SLOT(startAnimation())
      );
   }
}



//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::showCurrentlyPlayingUnitTest()
{
   mArtist = "deathstar";
   mAlbum = "wenz0r";
   mTrackLine1 = "bomberman";

   startAnimation();
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::initGlParameters()
{
   Matrix ortho= Matrix::ortho(
      0.0f,
      mPsd.getWidth(),
      mPsd.getHeight(),
      0.0f,
      1.0f,
      -1.0f
   );
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(ortho);

   glMatrixMode(GL_MODELVIEW);    
   glLoadIdentity();     

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);    
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::setVisible(bool /*visible*/)
{
   mVisible = true;
}


//-----------------------------------------------------------------------------
/*!
*/
bool MusicPlayerDrawable::isInGame() const
{
   return mInGame;
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::setInGame(bool game)
{
   mInGame = game;
}


//-----------------------------------------------------------------------------
/*!
*/
void MusicPlayerDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);    
   glDepthMask(GL_TRUE);
}



