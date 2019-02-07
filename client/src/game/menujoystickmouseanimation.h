#ifndef MENUJOYSTICKMOUSEANIMATION_H
#define MENUJOYSTICKMOUSEANIMATION_H

// framework
#include "frametimer.h"

// Qt
#include <QObject>
#include <QPoint>

class MenuJoystickMouseAnimation : public QObject
{
   Q_OBJECT

public:

   //! add a mouse animation
   static void add(const QPoint& dest);

   //! check if mouse animation is busy
   static bool isBusy();

   //! getter for last animation
   static MenuJoystickMouseAnimation* getLast();


signals:

   //! animation is done
   void done();


public slots:

   //! start animation
   void start();


protected slots:

   //! update cursor positon
   void update();

   //! remove animation
   void remove();


protected:

   //! constructor
   explicit MenuJoystickMouseAnimation(QObject *parent = 0);

   //! getter for start position
   const QPoint& getOrigin() const;

   //! setter for start position
   void setOrigin(const QPoint &value);

   //! getter for stop position
   const QPoint &getDestination() const;

   //! setter for stop position
   void setDestination(const QPoint &value);

   //! update timer
   FrameTimer mTimer;

   //! elapsed timer
   FrameTimer mElapsed;

   //! start position
   QPoint mOrigin;

   //! end position
   QPoint mDestination;

   //! static animation queue
   static QList<MenuJoystickMouseAnimation*> sAnimations;
};

#endif // MENUJOYSTICKMOUSEANIMATION_H
