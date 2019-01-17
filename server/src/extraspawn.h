#ifndef EXTRASPAWN_H
#define EXTRASPAWN_H

// Qt
#include <QObject>
#include <QTimer>

// forward declarations
class Map;

class ExtraSpawn : public QObject
{
   Q_OBJECT

public:

   //! constructor
   explicit ExtraSpawn(QObject *parent = 0);

   //! check if spawning is enabled
   bool isEnabled() const;


signals:

   //! time to spawn an extra
   void spawn();


public slots:

   //! reset spawn state
   void reset();

   //! check if spawn needs to be activated
   void update();

   //! setter for enabled flag (completely enable or disable extra spawning)
   void setEnabled(bool enabled);

   //! setter for map
   void setMap(Map* map);


protected slots:

   //! activate spawn timer
   void activateSpawnTimer();


protected:

   //! check if extra is available
   bool isExtraAvailable() const;

   //! check if spawn timer is already active
   bool isSpawnTimerActive() const;

   //! getter for map
   Map* getMap() const;

   //! spawning is enabled
   bool mEnabled;

   //! spawn timer
   QTimer mSpawnTimer;

   //! game's map
   Map* mMap;
};

#endif // EXTRASPAWN_H
