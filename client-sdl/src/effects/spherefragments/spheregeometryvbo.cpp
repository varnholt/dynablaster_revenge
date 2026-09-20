#include "spheregeometryvbo.h"

// engine
#include "framework/globaltime.h"
#include "gldevice.h"
#include "render/geometry.h"
#include "render/texturepool.h"
#include "math/vector.h"
#include "math/vector4.h"


SphereGeometryVbo::SphereGeometryVbo(Geometry* geo)
 : GeometryVbo(),
   mShader(0),
   mColorParam(-1)
{
   mGeometry = geo;
}


void SphereGeometryVbo::initialize()
{
   GeometryVbo::initialize();

   // init texture
   TexturePool* pool= TexturePool::Instance();
   mTexture = pool->getTexture("bomb");

   // init shader (data/effects/spherefragments/shaders variant - light position (30,15,-100),
   // matching the real engine's FileStream path precedence which prefers the spherefragments-
   // specific copy over the generic client/data/shaders one of the same name)
   mShader = activeDevice->loadShader(
      "simplelight-vert.glsl",
      "simplelight-frag.glsl"
   );
   mColorParam = activeDevice->getParameterIndex("u_color");
}


void SphereGeometryVbo::initGlParameters()
{
   // activate shader
   activeDevice->setShader(mShader);
}


void SphereGeometryVbo::cleanupGlParameter()
{
   activeDevice->setShader(0);
}


void SphereGeometryVbo::draw(const Vector4& color)
{
   initGlParameters();

   activeDevice->setParameter(mColorParam, color);

   Matrix rotation = Matrix::rotateY(GlobalTime::Instance()->getTime() * 10.0f * (3.14159265f / 180.0f));
   rotation = rotation * Matrix::rotateX(23.5f * (3.14159265f / 180.0f));

   glBindTexture(GL_TEXTURE_2D, mTexture.getTexture());

   const Matrix& mat= mGeometry->getTransform();

   activeDevice->push( mat * rotation );

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);

   glBindBuffer( GL_ARRAY_BUFFER, mVertexBuffer );
   glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)0 );
   glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)sizeof(Vector) );
   glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(sizeof(Vector)*2) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, mGeometry->getIndexCount(), GL_UNSIGNED_SHORT, 0 ); // render

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);

   activeDevice->pop();

   cleanupGlParameter();
}
