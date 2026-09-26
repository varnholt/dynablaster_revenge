#ifndef PATH_H
#define PATH_H

#include "point.h"

#include <vector>


class Path
{
   public:

      //! constructor
      Path();

      //! one position was reached
      void positionReached();

      //! setter for points
      void setPoints(const std::vector<Point>&);


   protected:

      //! list of points
      std::vector<Point> mPoints;

};

#endif // PATH_H
