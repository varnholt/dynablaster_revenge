#include "path.h"


//-----------------------------------------------------------------------------
/*!
*/
Path::Path()
{
}


//-----------------------------------------------------------------------------
/*!
   \param points points to set
*/
void Path::setPoints(const QList<Point>& points)
{
   mPoints = points;
}


//-----------------------------------------------------------------------------
/*!
*/
void Path::positionReached()
{
   mPoints.pop_front();
}
