#ifndef JOYSTICKMAPPER_H
#define JOYSTICKMAPPER_H

#include <QObject>
#include <QMap>

#include "constants.h"
#include "joystickconstants.h"

class JoystickMapper : public QObject
{
   Q_OBJECT

public:

   static JoystickMapper* getInstance();

   QString getHatString(JoystickConstants::Hat) const;

   //Qt::Key getFromString(const QString&) const;


signals:


public slots:


protected:

   JoystickMapper(QObject *parent = 0);

   void initMaps();


protected:


   QMap<JoystickConstants::Hat, QString> mHatMap;

   static JoystickMapper* sInstance;
};

#endif // JOYSTICKMAPPER_H

