// header
#include "videodebugging.h"

// Qt
#include <qgl.h>


//-----------------------------------------------------------------------------
/*!
*/
void VideoDebugging::debugProjectionMatrix()
{
   Matrix mat;
   glGetFloatv(GL_PROJECTION_MATRIX, mat.data());
   debugMatrix("projection", mat);
}


//-----------------------------------------------------------------------------
/*!
*/
void VideoDebugging::debugModelViewMatrix()
{
   Matrix mat;
   glGetFloatv(GL_MODELVIEW_MATRIX, mat.data());
   debugMatrix("modelview", mat);
}


//-----------------------------------------------------------------------------
/*!
   \param mat matrix to debug
*/
void VideoDebugging::debugMatrix(const QString& name, const Matrix &mat)
{
   qDebug("%s= {", qPrintable(name));
   qDebug("   %f %f %f %f", mat.xx, mat.xy, mat.xz, mat.xw);
   qDebug("   %f %f %f %f", mat.yx, mat.yy, mat.yz, mat.yw);
   qDebug("   %f %f %f %f", mat.zx, mat.zy, mat.zz, mat.zw);
   qDebug("   %f %f %f %f", mat.wx, mat.wy, mat.wz, mat.ww);
   qDebug("}");
}
