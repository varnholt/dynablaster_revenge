#include "extra.h"

#include <math.h>

#ifdef Q_OS_MAC
#include "stdlib.h"
#endif

Extra::Extra(Constants::ExtraType type, Geometry *object, float x, float y)
: Mesh()
, mType(type)
{
   Geometry *geo= new Geometry(this);
   geo->copy( object );
   add( geo );
   setUserTransformable(true);

   mPosition.identity();
   mPosition.translate( Vector(x+0.5,-y-0.5f,0.0f) );
   setTransform(mPosition);

   mTimeOffset= (rand() & 4095) * M_PI / 2048.0f;
}

Extra::~Extra()
{
}

void Extra::animate(float time)
{
   float a= (float)sin(mTimeOffset+time*0.04) * 0.9f;
   float s= (float)sin(mTimeOffset+time*0.2) * 0.2f;

   Matrix scale= Matrix::scale(0.9f-s, 1.0f, 0.9f+s);
   Matrix rot= Matrix::rotateZ(a);
   setTransform( scale * rot * mPosition );
}
