/***************************************************************************
                              startalersfactory
 ---------------------------------------------------------------------------
    project              : startalers
    begin                : dec 2013
    copyright            : (C) 2013 by mueslee
    author               : mueslee
    email                : nope
***************************************************************************/


#pragma once

// Qt
#include <QGLWidget>
#include <QTimer>

// framework
#include "framework/frametimer.h"

// math
#include "math/vector.h"

// todo remove
#include "globaltime.h"

// shared
#include "constants.h"

// forward declarations
class RenderDevice;
class StarTalers;

class StarTalersFactory : public QObject
{
    Q_OBJECT
    
public:

   //! constructor
   StarTalersFactory(QObject *parent = 0);

   //! destructor
   ~StarTalersFactory();

   //! initialize gl context
   void initialize();

   //! paint
   void update(float dt);

   //! add a new animation at given position
   void add(float x, float y, Constants::ExtraType extra);

   //! initialize colors
   void initColors();

   //! get color by extra type
   const Vector& getColor(Constants::ExtraType);


protected slots:

   //! unit testing
   void unitTest1();


protected:

   //! initialize gl states
   void initGlStates();

   //! cleanup gl states
   void cleanUpGlStates();

   //! setup camera
   void setupCamera();

   //! draw lines around file
   void drawField();

   //! draw timer
   FrameTimer mUpdatePaintTimerNonWorking;

   //! list of animations
   QList<StarTalers*> mAnimations;

   //! unit test timer
   QTimer mUnitTestTimer;


   // colors

   //! color bomb
   Vector mColorBomb;

   //! color flame
   Vector mColorFlame;

   //! color speedup
   Vector mColorSpeedUp;

   //! color kick
   Vector mColorKick;

   //! default color
   Vector mColorDefault;
};

