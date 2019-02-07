#pragma once

// Qt
#include <QString>

// framework
#include "math/matrix.h"

class VideoDebugging
{
   public:

      //! debug projection matrix
      static void debugProjectionMatrix();

      //! debug modelview matrix
      static void debugModelViewMatrix();

      //! debug given matrix
      static void debugMatrix(const QString &name, const Matrix& mat);
};

