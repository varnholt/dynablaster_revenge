#include "bombsocketgeometryvbo.h"

// engine
#include "framework/globaltime.h"
#include "gldevice.h"
#include "math/vector.h"
#include "math/vector2.h"
#include "math/vector4.h"
#include "render/geometry.h"
#include "render/texturepool.h"

BombSocketGeometryVbo::BombSocketGeometryVbo(Geometry* geo) : GeometryVbo(), mShader(0), mFresnel(-1), mColorParam(-1)
{
   mGeometry = geo;
}

void BombSocketGeometryVbo::initialize()
{
   GeometryVbo::initialize();

   // init texture
   TexturePool* pool = TexturePool::Instance();
   mTexture = pool->getTexture("metal_fire");

   // init shader
   mShader = activeDevice->loadShader("socketlight-vert.glsl", "socketlight-frag.glsl");
   mFresnel = activeDevice->getParameterIndex("fresnelFactor");
   mColorParam = activeDevice->getParameterIndex("u_color");
}

void BombSocketGeometryVbo::initGlParameters()
{
   // activate shader
   activeDevice->setShader(mShader);
   activeDevice->setParameter(mFresnel, Vector2(1.3f, 8.0f));
}

void BombSocketGeometryVbo::cleanupGlParameter()
{
   activeDevice->setShader(0);
}

void BombSocketGeometryVbo::draw(const Vector4& color)
{
   initGlParameters();

   activeDevice->setParameter(mColorParam, color);

   float time = GlobalTime::Instance()->getTime();
   Matrix rotation;
   rotation = Matrix::rotateY(time * 0.1745f);    // rotate the earth around its own y axis
   rotation = rotation * Matrix::rotateX(-1.0f);  // rotate the north pole towards the viewer
   rotation = rotation * Matrix::rotateY(1.0f);   // make earth rotate / instead of |

   glBindTexture(GL_TEXTURE_2D, mTexture.getTexture());

   const Matrix& mat = mGeometry->getTransform();

   activeDevice->push(mat * rotation);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);

   glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)sizeof(Vector));
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(sizeof(Vector) * 2));

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
   glDrawElements(GL_TRIANGLES, mGeometry->getIndexCount(), GL_UNSIGNED_SHORT, 0);  // render

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);

   activeDevice->pop();

   cleanupGlParameter();
}
