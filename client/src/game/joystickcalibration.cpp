// header
#include "joystickcalibration.h"

// game
#include "gamejoystickintegration.h"
#include "gamejoystickmapping.h"

// defines
#define QUEUE_MAX_SIZE 1000

#ifdef Q_OS_MAC
#include "stdlib.h"
#endif

/*
   concept:

      get a table of axis values

         axis0 axis1 axis2 axis3
         val   val   val   val
         val   val   val   val
         val   val   val   val
         val   val   val   val

      determine max per column

         => two max rows are one axis unit
*/


JoystickCalibration::JoystickCalibration(QObject *parent) :
   QObject(parent),
   mEnabled(false)
{
   reset();
}


void JoystickCalibration::setEnabled(bool enabled)
{
   mEnabled = enabled;

   reset();
   updateConnections();
}


bool JoystickCalibration::isEnabled() const
{
   return mEnabled;
}


void JoystickCalibration::processJoystickData(const JoystickInfo & joystickInfo)
{
   int axisCount = joystickInfo.getAxisValues().count();

   // resize list of queues if necessary
   if (mAxisData.size() != axisCount)
   {
      clearAxisData();

      for (int i = 0; i < axisCount; i++)
      {
         mAxisData.append(QQueue<int>());
      }
   }

   // check if it is actually required to add the current row
   // to the list of queues
   bool add = false;
   int sum = 0;

   QList<int> values = joystickInfo.getAxisValues();
   for (int i = 0; i < values.size(); i++)
   {
      sum += abs(values[i]);

      if (sum > 0)
      {
         add = true;
         break;
      }
   }

   if (add)
   {
      // copy values to queues
      int index = 0;
      foreach (int value, values)
      {
         mAxisData[index].push_back(value);
         index++;
      }

      // resize queues if their size exceeds max queue size
      for (int i = 0; i < mAxisData.size(); i++)
      {
         while (mAxisData[i].size() > QUEUE_MAX_SIZE)
            mAxisData[i].pop_front();
      }
   }
   else
   {
      // evaluate the retrieved data at this point *once*
      // and clear all axis data afterwards
      if (!mAxisData.isEmpty() && !mAxisData[0].isEmpty())
      {
         evaluate();
         emit axesCalibrated(getAxis1(), getAxis2());
      }

      clearAxisData();
   }
}


int* JoystickCalibration::getAxisPair()
{
   return mAxes;
}


void JoystickCalibration::reset()
{
   mAxes[0] = -1;
   mAxes[1] = -1;

   clearAxisData();
}


void JoystickCalibration::updateConnections()
{
   GameJoystickMapping* mapping =
      GameJoystickIntegration::getInstance(0)->getJoystickMapping();

   if (isEnabled())
   {
      connect(
         mapping,
         SIGNAL(dataReceived(JoystickInfo)),
         this,
         SLOT(processJoystickData(JoystickInfo))
      );
   }
   else
   {
      disconnect(
         mapping,
         SIGNAL(dataReceived(JoystickInfo)),
         this,
         SLOT(processJoystickData(JoystickInfo))
      );
   }
}


void JoystickCalibration::evaluate()
{
   if (!mAxisData.isEmpty())
   {
      // init
      int max1 = INT_MIN;
      int max2 = INT_MIN;

      int max1Index = -1;
      int max2Index = -1;

      int axisCount = mAxisData.size();
      int* sums = new int[axisCount];
      memset(sums, 0, sizeof(int)*axisCount);

      // sum up axis movement
      for (int rowIndex = 0; rowIndex < mAxisData[0].size(); rowIndex++)
      {
         for (int axisIndex = 0; axisIndex < mAxisData.size(); axisIndex++)
         {
            sums[axisIndex] += abs(mAxisData[axisIndex][rowIndex]);
         }
      }

      // assign maxima
      for (int i = 0; i < axisCount; i++)
      {
         if (i % 2)
         {
            if (sums[i] > max1)
            {
               max1 = sums[i];
               max1Index = i;
            }
         }
         else
         {
            if (sums[i] > max2)
            {
               max2 = sums[i];
               max2Index = i;
            }
         }
      }

      if (
            max1Index != -1
         && max2Index != -1
      )
      {
         if (max1Index < max2Index)
            setAxisPair(max1Index, max2Index);
         else
            setAxisPair(max2Index, max1Index);
      }

       delete[] sums;
   }
}


int JoystickCalibration::getAxis1() const
{
   return mAxes[0];
}


int JoystickCalibration::getAxis2() const
{
   return mAxes[1];
}


void JoystickCalibration::setAxisPair(int a, int b)
{
   mAxes[0] = a;
   mAxes[1] = b;

   qDebug("JoystickCalibration::setAxisPair: axis pair: %d, %d", a, b);
}


void JoystickCalibration::clearAxisData()
{
   for (int i = 0; i < mAxisData.size(); i++)
      mAxisData[i].clear();

   mAxisData.clear();
}


