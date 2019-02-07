#ifndef EXTRAWIDGET_H
#define EXTRAWIDGET_H

// Qt
#include <QObject>
#include <QTimer>

// framework
#include "framework/frametimer.h"

// forward declarations
class RenderDevice;
class RibbonAnimation;

class RibbonAnimationFactory : public QObject
{
    Q_OBJECT
    
public:

   //! constructor
   RibbonAnimationFactory();

   //! destructor
   ~RibbonAnimationFactory();

   //! initialize gl context
   void initialize();

   //! paint
   void update(float dt);

   //! add a new animation at given position
   void add(float x, float y);


protected slots:

   //! unit testing
   void unitTest1();


protected:

   //! draw lines around file
   void drawField();

   void setupCamera();

   void clearScreen();

   void initGlStates();

   void cleanUpGlStates();

//   //! delta between paintGl calls
//   FrameTimer mDelta;

//   //! draw timer
//   FrameTimer mUpdatePaintTimerNonWorking;

   //! list of animations
   QList<RibbonAnimation*> mAnimations;

   //! unit test timer
   QTimer mUnitTestTimer;
};

#endif // EXTRAWIDGET_H
