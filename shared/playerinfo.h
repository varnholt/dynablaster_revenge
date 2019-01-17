#ifndef PLAYERINFO_H
#define PLAYERINFO_H

// game
#include "constants.h"
#include "playerstats.h"

// Qt
#include <QString>
#include <QPointer>

// forward declarations
class PlayerDisease;

class PlayerInfo : public QObject
{

Q_OBJECT

   public:

      PlayerInfo();

      void setId(int);

      void setColor(Constants::Color);

      void setNick(const QString&);

      int getId() const;

      Constants::Color getColor() const;

      QString getNick() const;

      void setPosition(float x, float y, float angle);

      void setPositionDelta(float dx, float dy, float dAngle);

      float getX() const;

      float getY() const;

      float getAngle() const;

      //! setter for delta x
      void setDeltaX(float val);

      //! setter for delta y
      void setDeltaY(float val);

      //! setter for delta angle
      void setDeltaAngle(float val);

      //! getter for player's x delta
      float getDeltaX() const;

      //! getter for player's y delta
      float getDeltaY() const;

      //! getter for player's y delta
      float getAngleDelta() const;

      //! setter for player stats
      void setOverallStats(const PlayerStats&);

      //! getter for player overall stats
      PlayerStats& getOverallStats();

      //! setter for player round stats
      void setRoundStats(const PlayerStats&);

      //! getter for player stats
      PlayerStats& getRoundStats();

      //! setter for killed flag
      void setKilled(bool killed);

      //! getter for killed flag
      bool isKilled() const;

      //! infect player
      void infect(QPointer<PlayerDisease> disease);

      //! check if player is infected
      bool isInfected() const;

      //! getter for disease
      PlayerDisease* getDisease() const;

      //! getter for directions
      qint8 getDirections() const;

      //! setter for directions
      void setDirections(qint8 directions);


   protected:

      int mId;

      Constants::Color mColor;

      //! overall stats
      PlayerStats mOverallStats;

      //! round stats
      PlayerStats mRoundStats;

      QString mNick;

      float mX;

      float mY;

      float mAngle;

      //! player direction x
      float mDeltaX;

      //! player direction y
      float mDeltaY;

      //! player angle direction
      float mDeltaAngle;

      //! killed flag
      bool mKilled;

      //! player can be infected
      QPointer<PlayerDisease> mDisease;

      //! player's directions
      qint8 mDirections;
};

#endif // PLAYERINFO_H
