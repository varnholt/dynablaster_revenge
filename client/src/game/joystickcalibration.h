#ifndef JOYSTICKCALIBRATION_H
#define JOYSTICKCALIBRATION_H

// Qt
#include <QList>
#include <QObject>
#include <QQueue>

// game
#include "joystick/joystickinfo.h"


class JoystickCalibration : public QObject
{
   Q_OBJECT

public:

   JoystickCalibration(QObject *parent = 0);

   void setEnabled(bool enabled);

   bool isEnabled() const;

   void processJoystickData(const JoystickInfo & joystickInfo);

   int* getAxisPair();

   void evaluate();

   int getAxis1() const;

   int getAxis2() const;


signals:

   void axesCalibrated(int axis1, int axis2);
   

public slots:
   
protected:

   void reset();

   void updateConnections();

   void setAxisPair(int a, int b);

   void clearAxisData();

   bool mEnabled;

   QList< QQueue<int> > mAxisData;

   int mAxes[2];
};

#endif // JOYSTICKCALIBRATION_H

