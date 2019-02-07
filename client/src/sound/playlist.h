#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QList>
#include <QObject>
#include <QSet>
#include <QString>

class StreamPlayer;

class Playlist : public QObject
{
   Q_OBJECT

   public:

       Playlist(QObject *parent = 0);

       virtual ~Playlist();

       void setMixer(StreamPlayer* mixer);

       void setPlaylist(const QList<QString>&);

       void loadFile(QString filename);

       void setShuffleEnabled(bool shuffle);

       bool isShuffleEnabled() const;


   signals:

      //! playing a track
      void playing(
         const QString& artist,
         const QString& album,
         const QString& track
      );


   public slots:

      void play();
      void pause();
      void stop();


      void playNext();
      void playPrevious();


   private slots:

      void next();
      void previous();

      void playBuffer(char* buffer, int size);


   private:

      int shuffle(int nextIndex);

      StreamPlayer* mMixer;

      QList<QString> mPlaylist;

      QSet<QString> mPlayedTracks;

      bool mShuffle;

      int mIndex;

      char *mBuffer;
};

#endif // PLAYLIST_H

