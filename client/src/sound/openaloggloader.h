#ifndef __ogg_h__
#define __ogg_h__

// openal
#include <AL/al.h>
//#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

// Qt
#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QMutex>


class OpenAlOggLoader : public QObject
{
   Q_OBJECT

    public:

        //! constructor
        OpenAlOggLoader();

        //! destructor
        ~OpenAlOggLoader();

        //! open a file
        void open(const QString& path);

        //! release a file
        void release();

        //! display file information
        void display();

        //! play a file
        bool play();

        //! file is still playing
        bool isPlaying();

        //! continue updating the stream
        bool update();

   signals:

        void playing(
           const QString& artist,
           const QString& album,
           const QString& track
        );

   private:

        //!
        bool stream(ALuint buffer);

        //!
        void empty();

        //!
        void check();

        //! parse comment data
        void parseCommentData();

        //!
        QString errorString(int code);

        //! serialize member access
        QMutex mMutex;

        //! file to open
        FILE* mFile;

        //! oggvorbis filestream
        OggVorbis_File mOggStream;

        //! vorbis file info object
        vorbis_info* mVorbisInfo;

        //! vorbis comment object
        vorbis_comment* mVorbisComment;

        //! buffers used for the track
        ALuint mBuffers[2];

        //! one source id for the music
        ALuint mSource;

        //! vorbis format
        ALenum mFormat;


        // track information (extracted from vorbis comment)

        //! current artist
        QString mArtist;

        //! current track
        QString mTrack;

        //! current album
        QString mAlbum;
};


#endif // __ogg_h__
