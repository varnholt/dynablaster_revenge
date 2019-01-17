#ifndef PLAYERDISEASE_H
#define PLAYERDISEASE_H

// Qt
#include <QObject>
#include <QSet>
#include <QTime>

// constants
#include "constants.h"

class PlayerDisease : public QObject
{
    Q_OBJECT

public:

   //! constructor
   PlayerDisease(QObject *parent = 0);

   //! setter for disease type
   void setType(Constants::SkullType);

   //! getter for disease type
   Constants::SkullType getType() const;

   //! setter for disease duration
   void setDuration(int duration);

   //! getter for disease duration
   int getDuration() const;

   //! check if disease is still active
   bool isActive() const;

   //! activate disease
   void activate();

   //! set random skull type
   void randomizeType();


   //! getter for player id
   int getPlayerId() const;

   //! setter for player id
   void setPlayerId(int playerId);


   //! setter for supported skulls
   static void setSupportedSkulls(const QSet<Constants::SkullType>& skulls);

   //! getter for supported skulls;
   static QSet<Constants::SkullType> getSupportedSkulls();

   //! setter for skull faces
   static void setSkullFaces(QList<Constants::SkullType>& faces);

   //! getter for supported skulls;
   static QList<Constants::SkullType> getSkullFaces();

   //! generate random skull faces
   static QList<Constants::SkullType> generateSkullFaces();

   //! getter for type by face
   static Constants::SkullType getTypeByFace(int face);


   // skull type implementations

   //! apply autofire
   void applyAutofire(qint8 &keysPressed);

   //! apply keyboard invert
   void applyKeyboardInvert(qint8& keysPressed);


public slots:

   //! abort infection
   void abort();


signals:

   //! disease stopped
   void stopped();


protected:

   //! disease type
   Constants::SkullType mType;

   //! disease duration
   int mDuration;

   //! active time
   QTime mActiveTime;

   //! player id
   int mPlayerId;

   //! all skulls that are supported/enabled
   static QSet<Constants::SkullType> sSupportedSkulls;

   //! skull cube setup
   static QList<Constants::SkullType> sCubeFaces;
};

#endif // PLAYERDISEASE_H
