#include "spheregeometryvbo.h"

// engine
#include "framework/globaltime.h"
#include "framework/renderdevice.h"
#include "framework/gldevice.h"
#include "nodes/mesh.h"
#include "nodes/scenegraph.h"
#include "render/texturepool.h"
#include "math/vector.h"
#include "math/vector4.h"
// cmath
#include <math.h>


SphereGeometryVbo::SphereGeometryVbo(Geometry* geo)
 : GeometryVbo(),
   mShader(0)
{
   mGeometry = geo;
}


void SphereGeometryVbo::initialize()
{
   GeometryVbo::initialize();

   // init texture
   TexturePool* pool= TexturePool::Instance();
   mTexture = pool->getTexture("bomb");

   // init shader
   mShader = activeDevice->loadShader(
      "simplelight-vert.glsl",
      "simplelight-frag.glsl"
   );
}


void SphereGeometryVbo::initGlParameters()
{
   // glClearColor(0.0, 0.0, 0.0, 0.0);
   // glClear(GL_COLOR_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // activate shader
   activeDevice->setShader(mShader);

   glColor4f(1,1,1,1);
}


void SphereGeometryVbo::cleanupGlParameter()
{
   activeDevice->setShader(0);
   glColor4f(1,1,1,1);
}


void SphereGeometryVbo::draw(const Vector4& color)
{
   initGlParameters();

   glPushMatrix();

   glRotatef(GlobalTime::Instance()->getTime() * 10.0f, 0.0, 1.0, 0.0);
   glRotatef(23.5f, -1.0, 0.0, 0.0);

   /*
   float time = GlobalTime::Instance()->getTime();
   Matrix rotation;
   rotation = Matrix::rotateY(time * 0.1745f);   // rotate the earth around its own y axis
   rotation = rotation * Matrix::rotateX(-1.0f); // rotate the north pole towards the viewer
   rotation = rotation * Matrix::rotateY(1.0f);  // make earth rotate / instead of |
   */

   glBindTexture(GL_TEXTURE_2D, mTexture.getTexture());

   const Matrix& mat= mGeometry->getTransform();

   glColor4fv( color );
   glMultMatrixf(mat);
   // activeDevice->push( mat );

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexBuffer );
   glVertexPointer( 3, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)0 );
   glNormalPointer(GL_FLOAT, sizeof(Vertex3D), (GLvoid*)sizeof(Vector) );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)(sizeof(Vector)*2) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, mGeometry->getIndexCount(), GL_UNSIGNED_SHORT, 0 ); // render

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   // activeDevice->pop();

   glPopMatrix();

   cleanupGlParameter();
}

