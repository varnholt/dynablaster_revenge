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
void Path::setPoints(const QList<QPoint>& points)
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
