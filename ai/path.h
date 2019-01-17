#ifndef PATH_H
#define PATH_H

#include <QList>
#include <QPoint>


class Path
{
   public:

      //! constructor
      Path();

      //! one position was reached
      void positionReached();

      //! setter for points
      void setPoints(const QList<QPoint>&);


   protected:

      //! list of points
      QList<QPoint> mPoints;

};

#endif // PATH_H
