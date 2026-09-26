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
void Path::setPoints(const std::vector<Point>& points)
{
   mPoints = points;
}


//-----------------------------------------------------------------------------
/*!
*/
void Path::positionReached()
{
   mPoints.erase(mPoints.begin());
}
