#ifndef PATH_H
#define PATH_H

#include <QList>

#include "point.h"


class Path
{
   public:

      //! constructor
      Path();

      //! one position was reached
      void positionReached();

      //! setter for points
      void setPoints(const QList<Point>&);


   protected:

      //! list of points
      QList<Point> mPoints;

};

#endif // PATH_H
