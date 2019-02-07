#ifndef HEADLESSINTEGRATION_H
#define HEADLESSINTEGRATION_H

// Qt
#include <QObject>

// forward declarations
class HeadlessClient;


class HeadlessIntegration : public QObject
{
    Q_OBJECT

public:

   //! constructor
   explicit HeadlessIntegration(QObject *parent = 0);

   //! destructor
   virtual ~HeadlessIntegration();

   //! getter for instance count
   static int getInstanceCount();

   //! getter for client
   HeadlessClient* getClient() const;

   //! setter for client
   void setClient(HeadlessClient *value);

   //! initialize
   void initialize();

   //! show joysticks
   void debugJoysticks();

   //! getter for joystick id
   int getJoystickId() const;

   //! setter for joystick id
   void setJoystickId(int value);

   //! getter for game id
   int getGameId() const;

   //! setter for game id
   void setGameId(int value);

   //! check if player is a headless player
   static bool isHeadlessPlayer(int id);


protected slots:

   void joinGame();


protected:

   //! client
   HeadlessClient* mClient;

   //! joystick id
   int mJoystickId;

   //! game id
   int mGameId;

   //! instances
   static QList<HeadlessIntegration*> sInstances;
};

#endif // HEADLESSINTEGRATION_H
