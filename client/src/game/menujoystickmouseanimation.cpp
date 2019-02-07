// header
#include "menujoystickmouseanimation.h"

// Qt
#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QWidget>

// cmath
#include <math.h>

// duration in ms
#define DURATION 400.0f

// static
QList<MenuJoystickMouseAnimation*> MenuJoystickMouseAnimation::sAnimations;

//-----------------------------------------------------------------------------
/*!
   \param start start position
   \param stop stop position
*/
void MenuJoystickMouseAnimation::add(const QPoint &dest)
{
   MenuJoystickMouseAnimation* animation = new MenuJoystickMouseAnimation();
   animation->setDestination(dest);

   sAnimations.push_back(animation);

   connect(
      animation,
      SIGNAL(done()),
      animation,
      SLOT(remove())
   );

   // initiate the first animation
   if (sAnimations.size() == 1)
      animation->start();
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if animation is busy
*/
bool MenuJoystickMouseAnimation::isBusy()
{
   return !sAnimations.isEmpty();
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to last animation
*/
MenuJoystickMouseAnimation *MenuJoystickMouseAnimation::getLast()
{
   return sAnimations.last();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickMouseAnimation::remove()
{
   if (!sAnimations.isEmpty())
   {
      MenuJoystickMouseAnimation* animation =
         sAnimations.takeFirst();

      animation->deleteLater();
   }

   // initiate subsequent animations
   if (!sAnimations.isEmpty())
   {
      MenuJoystickMouseAnimation* next = sAnimations.first();

      if (next)
         next->start();
   }
}


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
MenuJoystickMouseAnimation::MenuJoystickMouseAnimation(QObject *parent)
 : QObject(parent)
{
   connect(
      &mTimer,
      SIGNAL(timeout()),
      this,
      SLOT(update())
   );
}


//-----------------------------------------------------------------------------
/*!
   \return start point
*/
const QPoint &MenuJoystickMouseAnimation::getOrigin() const
{
   return mOrigin;
}


//-----------------------------------------------------------------------------
/*!
   \param value start point
*/
void MenuJoystickMouseAnimation::setOrigin(const QPoint &value)
{
   mOrigin = value;
}


//-----------------------------------------------------------------------------
/*!
   \return stop point
*/
const QPoint& MenuJoystickMouseAnimation::getDestination() const
{
   return mDestination;
}


//-----------------------------------------------------------------------------
/*!
   \param value stop point
*/
void MenuJoystickMouseAnimation::setDestination(const QPoint &value)
{
   mDestination = value;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickMouseAnimation::start()
{
   setOrigin(QCursor::pos());

   mElapsed.start();
   mTimer.start(16);
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickMouseAnimation::update()
{
   // distance and speed setup
   float maxManhattanLength = 200.0f;
   float distance = (getOrigin() - getDestination()).manhattanLength();
   float inverseSpeed = qMin(1.0f, distance / maxManhattanLength);
   float duration = DURATION * inverseSpeed;
   float elapsed = mElapsed.elapsed();

   // interpolation
   float normed = qMin(1.0f, elapsed / duration);   
   float val = 0.5f * ( 1.0f + cos(M_PI * normed) );
   float a = val;
   float b = 1.0f - val;

   QPoint next = (a * getOrigin()) + (b * getDestination());
   QCursor::setPos(next);

   // exit condition
   if (elapsed >= duration)
   {
      emit done();
   }
}

