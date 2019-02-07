#include "bombsocketgeometryvbo.h"

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


BombSocketGeometryVbo::BombSocketGeometryVbo(Geometry* geo)
 : GeometryVbo(),
   mShader(0)
{
   mGeometry = geo;
}


void BombSocketGeometryVbo::initialize()
{
   GeometryVbo::initialize();

   // init texture
   TexturePool* pool= TexturePool::Instance();
   mTexture = pool->getTexture("metal_fire");

   // init shader
   mShader = activeDevice->loadShader(
      "socketlight-vert.glsl",
      "socketlight-frag.glsl"
   );
   mFresnel= activeDevice->getParameterIndex("fresnelFactor");
}


void BombSocketGeometryVbo::initGlParameters()
{
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // activate shader
   activeDevice->setShader(mShader);
   activeDevice->setParameter(mFresnel, Vector2(1.3f, 8.0f));

   glColor4f(1,1,1,1);
}


void BombSocketGeometryVbo::cleanupGlParameter()
{
   activeDevice->setShader(0);
   glColor4f(1,1,1,1);
}


void BombSocketGeometryVbo::draw(const Vector4& color)
{
   initGlParameters();

   glPushMatrix();

//   Matrix rotation;
//   rotation = Matrix::rotateX(-1.0f);           // rotate the north pole towards the viewer
//   rotation = rotation * Matrix::rotateY(1.0f); // make earth rotate / instead of |

   float time = GlobalTime::Instance()->getTime();
   Matrix rotation;
   rotation = Matrix::rotateY(time * 0.1745f);   // rotate the earth around its own y axis
   rotation = rotation * Matrix::rotateX(-1.0f); // rotate the north pole towards the viewer
   rotation = rotation * Matrix::rotateY(1.0f);  // make earth rotate / instead of |

   glBindTexture(GL_TEXTURE_2D, mTexture.getTexture());

   const Matrix& mat= mGeometry->getTransform();

   glColor4fv( color );
   glMultMatrixf( mat * rotation);

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

   glPopMatrix();

   cleanupGlParameter();
}

