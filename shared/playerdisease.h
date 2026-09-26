#ifndef PLAYERDISEASE_H
#define PLAYERDISEASE_H

// Qt
#include <QElapsedTimer>
#include <QObject>
#include <QSet>

// constants
#include "constants.h"
#include "signal.h"

#include <functional>
#include <vector>

class PlayerDisease : public QObject
{
   Q_OBJECT

public:
   //! constructor
   PlayerDisease(QObject* parent = 0);

   //! destructor - runs the destroy callbacks below (some callers delete this directly
   //! rather than going through abort(), see project_full_qt_removal_scope memory)
   virtual ~PlayerDisease();

   //! run an arbitrary callback right before this object is destroyed - used to disconnect
   //! Signal<> subscriptions registered against a longer-lived signal (Game's own signals live
   //! for the whole match, a disease doesn't), since Signal<> has no automatic
   //! disconnect-on-destroy the way Qt's own connect() did
   void addDestroyCallback(std::function<void()> callback);

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
   void applyAutofire(int8_t& keysPressed);

   //! apply keyboard invert
   void applyKeyboardInvert(int8_t& keysPressed);

public slots:

   //! abort infection
   void abort();

public:
   // Signal<> replacement for PlayerDisease's former Qt signal (see
   // project_full_qt_removal_scope memory).

   //! disease stopped
   Signal<> stoppedSignal;

protected:
   //! disease type
   Constants::SkullType mType;

   //! disease duration
   int mDuration;

   //! active time
   QElapsedTimer mActiveTime;

   //! player id
   int mPlayerId;

   //! all skulls that are supported/enabled
   static QSet<Constants::SkullType> sSupportedSkulls;

   //! skull cube setup
   static QList<Constants::SkullType> sCubeFaces;

   //! run in the destructor - see addDestroyCallback()
   std::vector<std::function<void()>> mDestroyCallbacks;
};

#endif  // PLAYERDISEASE_H
