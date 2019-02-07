// header
#include "menupagescrollimageitem.h"

// framework
#include "clipper.h"
#include "globaltime.h"
#include "gldevice.h"
#include "shaderpool.h"

// Qt
#include <QGLContext>

// cmath
#include <math.h>

// defines
#define SCROLLTIME 36.0f
#define LIM_0 (0.0f          * SCROLLTIME)
#define LIM_1 (0.13f         * SCROLLTIME)
#define LIM_2 (0.3333333333f * SCROLLTIME)
#define LIM_3 (0.6666666666f * SCROLLTIME)
#define LIM_4 (0.87f         * SCROLLTIME)
#define LIM_5 (1.0f          * SCROLLTIME)
#define LIM_2_POS_OFFSET 0.0f
#define LIM_2_POS_LENGTH 0.2f
#define LIM_3_POS_OFFSET 0.2f
#define LIM_3_POS_LENGTH 0.6f
#define LIM_4_POS_OFFSET 0.8f
#define LIM_4_POS_LENGTH 0.2f



//-----------------------------------------------------------------------------
/*!
*/
MenuPageScrollImageItem::MenuPageScrollImageItem()
 : mClipper(0),
   mLayer(0),
   mY(0.0f),
   mStartTime(0.0f),
   mAnimationTime(0.0f),
   mMoveUp(false),
   mRelativeTimePrevious(0.0f)
{
   mPageItemType = PageItemTypeScrollImage;
}


//-----------------------------------------------------------------------------
/*!
*/
MenuPageScrollImageItem::~MenuPageScrollImageItem()
{
   delete mClipper;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageScrollImageItem::initialize()
{
   MenuPageItem::initialize();

   PSDLayer* boundingRect = getInactiveLayer();

   // use layer for clipping
   mClipper = new Clipper(
      boundingRect->getLeft(),
      boundingRect->getTop(),
      boundingRect->getRight(),
      boundingRect->getBottom()
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageScrollImageItem::draw()
{
   // set opacity
   glColor4ub(
      255,
      255,
      255,
      255
   );

   /*
        +------------+ layer top
        |            |
        |            |
     +--|- - - - - - |--+ page top
     |  |            |  |
     +--|            |--+
     |  |            |  |
     |  |            |  |
     +--|            |--+
     |  |            |  |
     +--|- - - - - - |--+ page bottom
        |            |
        |            |
        +------------+ layer bottom

   */

   PSDLayer* layer= getActiveLayer();

   float offset = mY * (layer->getHeight() - mClipper->getHeight());

   // clip image to reference layer
   mClipper->enable();
   layer->render(0.0f, -offset); // move up to scroll down
   mClipper->disable();

}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageScrollImageItem::reset()
{
   mRelativeTimePrevious = 0.0f;
   mY = 0.0f;
   mMoveUp = false;
   mStartTime = GlobalTime::Instance()->getTime();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageScrollImageItem::animate(float /*time*/)
{
   mAnimationTime = GlobalTime::Instance()->getTime();

   float relativeTime = fmod(mAnimationTime - mStartTime, LIM_5);

   // flip if fmod limit reached
   if (relativeTime < mRelativeTimePrevious)
      mMoveUp = !mMoveUp;

   mRelativeTimePrevious = relativeTime;

   float pos = 0.0f;

   if (relativeTime < LIM_1)
   {
      pos = 0.0f;
   }
   else if (relativeTime < LIM_2)
   {
      // normalize from 0..1
      float val = (relativeTime - LIM_1) / (LIM_2 - LIM_1);

      // normalize to pi/2
      val *= (float)M_PI_2;
      val = 1.0f - cos(val);

      val *= LIM_2_POS_LENGTH;
      val += LIM_2_POS_OFFSET;

      pos = val;
   }
   else if (relativeTime < LIM_3)
   {
      // normalize from 0..1
      float val = (relativeTime - LIM_2) / (LIM_3 - LIM_2);

      val *= LIM_3_POS_LENGTH;
      val += LIM_3_POS_OFFSET;

      pos = val;
   }
   else if (relativeTime < LIM_4)
   {
      // normalize from 0..1
      float val = (relativeTime - LIM_3) / (LIM_4 - LIM_3);

      // normalize to pi/2
      val *= (float)M_PI_2;
      val = sin(val);

      val *= LIM_4_POS_LENGTH;
      val += LIM_4_POS_OFFSET;

      pos = val;
   }
   else
   {
      pos = 1.0f;
   }

   mY = mMoveUp ? (1.0f - pos) : pos;
}


