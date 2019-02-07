#ifndef MUSICPLAYERDRAWABLE_H
#define MUSICPLAYERDRAWABLE_H

#include <QObject>

// base
#include "drawable.h"

// framework
#include "tools/array.h"
#include "image/psd.h"
#include "framework/frametimer.h"
#include "vertex.h"

class QImage;
class BitmapFont;
class PSDLayer;

class MusicPlayerDrawable : public QObject, public Drawable
{
    Q_OBJECT

   public:

       MusicPlayerDrawable(RenderDevice*);
       virtual ~MusicPlayerDrawable();

       void initializeGL();
       void paintGL();
       void animate(float time);


       virtual void setVisible(bool visible);

       bool isInGame() const;
       void setInGame(bool game);

public slots:

      void showCurrentlyPlaying(
         const QString& artist,
         const QString& album,
         const QString& track
      );


      void showCurrentlyPlayingUnitTest();


   protected slots:

      void startAnimation();


   protected:

      void initializeLayers();

      void initGlParameters();

      void cleanupGlParameters();

      PSD mPsd;

      QList<PSDLayer*> mPsdLayers;

      QString mFilename;

      BitmapFont* mFont;

      QString mArtist;

      QString mAlbum;

      QString mTrackLine1;

      QString mTrackLine2;

      FrameTimer mAnimationStopTime;

      float mAnimationFactor;

      bool mFadeIn;

      bool mIdle;

      bool mFadeOut;

      int mMaxWidth;

      int mFontOffsetArtistX;
      int mFontOffsetArtistY;
      int mFontOffsetArtistHeight;

      int mFontOffsetAlbumX;
      int mFontOffsetAlbumY;
      int mFontOffsetAlbumHeight;

      int mFontOffsetTrackLine1X;
      int mFontOffsetTrackLine1Y;
      int mFontOffsetTrackLine1Height;

      int mFontOffsetTrackLine2X;
      int mFontOffsetTrackLine2Y;
      int mFontOffsetTrackLine2Height;

      bool mAnimating;

      bool mInGame;
};

#endif // MUSICPLAYERDRAWABLE_H
