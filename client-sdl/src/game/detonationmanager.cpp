#include "detonationmanager.h"
#include "detonation.h"
#include "framework/gldevice.h"
#include "math/vector.h"
#include "math/vector4.h"
#include "image/image.h"
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace
{

float linear(float a, float b, float t)
{
   return a + (b-a)*t;
}

float fract( float x )
{
   return fmod(x, 1.0f);
}

float hash( float n )
{
   return fract(sin(n)*43758.5453f);
}

Vector vfloor( const Vector& x )
{
   Vector v;
   v.x= floorf(x.x);
   v.y= floorf(x.y);
   v.z= floorf(x.z);
   return v;
}

float mix(float x, float y, float a)
{
   return x + (y-x)*a;
}

Vector4 mix(const Vector4& x, const Vector4& y, float a)
{
   return x + (y-x)*a;
}

float frand(float min, float max)
{
   int x= rand() & 16384;
   return min + x * (max - min) / 16383.0f;
}

float noise( const Vector& x )
{
   Vector p = vfloor(x);

   Vector f = Vector( frand(0.0f, 1.0f), frand(0.0f, 1.0f), frand(0.0f, 1.0f) );

   float n = p.x + p.y*57.0f + 113.0f*p.z;

   float res =
      mix( mix( mix( hash(n+  0.0f), hash(n+  1.0f),f.x),
      mix( hash(n+ 57.0f), hash(n+ 58.0f),f.x),f.y),
      mix(mix( hash(n+113.0f), hash(n+114.0f),f.x),
      mix( hash(n+170.0f), hash(n+171.0f),f.x),f.y),f.z);

   return res;
}

Vector4 gradient(float x)
{
   const Vector4 c0 = Vector4(0.1f, 0.1f, 0.1f, 0.0f);   // transparent
   const Vector4 c1 = Vector4(0.1f, 0.1f, 0.1f, 0.6f);   // grey
   const Vector4 c2 = Vector4(0.1f, 0.1f, 0.1f, 0.0f);   // black
   const Vector4 c3 = Vector4(1.0f, 0.2f, 0.0f, 0.6f);   // red
   const Vector4 c4 = Vector4(1.2f, 0.5f, 0.3f, 1.0f);   // yellow
   const Vector4 c5 = Vector4(1.0f, 0.7f, 0.0f, 1.0f);   // yellow

   float t = fract(x*5.0f);

   Vector4 c;
   if (x < 0.0f)
      return c0;
   else if (x < 0.2f)
      c =  mix(c0, c1, t);
   else if (x < 0.4f)
      c = mix(c1, c2, t);
   else if (x < 0.6f)
      c = mix(c2, c3, t);
   else if (x < 0.8f)
      c = mix(c3, c4, t);
   else if (x < 1.0f)
      c = mix(c4, c5, t);
   else if (x >= 1.0f)
      return c5;

   return c;
}

}  // namespace

DetonationManager::DetonationManager()
: mTime(0.0f),
  mShader(0),
  mNoiseMap(0),
  mGradientMap(0),
  mParamTime(0),
  mParamCamPos(0),
  mParamTop(0),
  mParamBottom(0),
  mParamLeft(0),
  mParamRight(0),
  mParamBoundMin(0),
  mParamBoundMax(0),
  mParamNoiseMap(0),
  mParamGradientMap(0),
  mBoxVertexBuffer(0)
{
}

DetonationManager::~DetonationManager()
{
   clear();
}

void DetonationManager::clear()
{
   mDetonations.clear();
}

void DetonationManager::init()
{
   GLDevice* dev = static_cast<GLDevice*>(activeDevice);

   // gradient palette - GLES3 has no GL_TEXTURE_1D, uploaded as a 2D texture with height 1
   // instead (sampled at v=0.5 in flame-frag.glsl).
   Image palette("detonationpalette");

   glGenTextures(1, &mGradientMap);
   glBindTexture(GL_TEXTURE_2D, mGradientMap);
   glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      palette.getWidth(),
      1,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      palette.getData()
   );
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   // procedural noise volume - GLES3 has native GL_TEXTURE_3D support, ported as-is except the
   // internal format: GL_ALPHA isn't part of GLES3's texImage3D format table, R8 is the modern
   // single-channel equivalent (read back via .r instead of .a in the shader).
   const int size = 32;
   std::vector<unsigned char> noiseMap(static_cast<size_t>(size) * size * size);
   for (int z = 0; z < size; z++)
   {
      const float scale = 1.0f / size;
      for (int y = 0; y < size; y++)
      {
         for (int x = 0; x < size; x++)
         {
            Vector p(x*scale, y*scale, z*scale);
            float n1 = noise(p);
            noiseMap[static_cast<size_t>((z*size+y)*size+x)] = static_cast<unsigned char>(n1*127.0f+128.0f);
         }
      }
   }

   glGenTextures(1, &mNoiseMap);
   glBindTexture(GL_TEXTURE_3D, mNoiseMap);
   glTexImage3D(
      GL_TEXTURE_3D,
      0,
      GL_R8,
      size,
      size,
      size,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      noiseMap.data()
   );
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
   glBindTexture(GL_TEXTURE_3D, 0);

   // load shader
   mShader = dev->loadShader("flame-vert.glsl", "flame-frag.glsl");

   mParamTime = dev->getParameterIndex("time");
   mParamCamPos = dev->getParameterIndex("campos");
   mParamNoiseMap = dev->getParameterIndex("noisemap");
   mParamGradientMap = dev->getParameterIndex("gradientmap");
   mParamTop = dev->getParameterIndex("top");
   mParamBottom = dev->getParameterIndex("bottom");
   mParamLeft = dev->getParameterIndex("left");
   mParamRight = dev->getParameterIndex("right");
   mParamBoundMin = dev->getParameterIndex("boundmin");
   mParamBoundMax = dev->getParameterIndex("boundmax");
}

void DetonationManager::addDetonation(int x, int y, int top, int bottom, int left, int right)
{
   auto det = std::make_unique<Detonation>(x, y + 1, left, right, top, bottom);
   det->setStartTime(mTime);
   mDetonations.push_back(std::move(det));
}

void DetonationManager::update(float time)
{
   mTime= time;
   std::vector<std::unique_ptr<Detonation>>::iterator it;
   for (it=mDetonations.begin(); it!=mDetonations.end(); )
   {
      Detonation *det= it->get();
      if (det->elapsed(time) > 2.0f)
      {
         it= mDetonations.erase(it);
      }
      else
         it++;
   }
}

void DetonationManager::drawBox(float x, float y, float z, float left, float right, float bottom, float top, int sides)
{
   static const unsigned char tris[5*3*2]= {
      2,6,7,  3,2,7, // back   +y
      0,1,4,  1,5,4, // front  -y
      0,4,6,  0,6,2, // left   -x
      1,3,5,  3,7,5, // right  +x
      4,5,6,  6,5,7  // top    +z
   };

   sides|=16;

   Vector boxmin(left, bottom, -0.5f);
   Vector boxmax(right, top, 0.5f);
   if ((sides&1)==0) boxmax.y++;
   if ((sides&2)==0) boxmin.y--;
   if ((sides&4)==0) boxmin.x--;
   if ((sides&8)==0) boxmax.x++;

   activeDevice->setParameter(mParamBoundMin, boxmin);
   activeDevice->setParameter(mParamBoundMax, boxmax);

   // position (3) + normal (3) per vertex, built on the CPU and uploaded as a dynamic attribute-
   // array buffer instead of glBegin(GL_TRIANGLES)/glVertex3f/glNormal3f immediate mode.
   std::vector<float> vertices;
   vertices.reserve(30 * 6);

   int sideBits = sides;
   for (int i=0; i<5; i++)
   {
      if (sideBits&1)
      {
         for (int tri=0;tri<6;tri++)
         {
            const int index= tris[i*6+tri];
            Vector v(static_cast<float>(index&1), static_cast<float>(index>>1&1), static_cast<float>(index>>2&1));

            vertices.push_back(v.x*x);
            vertices.push_back(v.y*y);
            vertices.push_back(v.z*z);

            vertices.push_back(linear(left, right, v.x));
            vertices.push_back(linear(bottom, top, v.y));
            vertices.push_back(v.z-0.5f);
         }
      }
      sideBits>>=1;
   }

   if (vertices.empty())
      return;

   const int byteSize = static_cast<int>(vertices.size() * sizeof(float));

   if (mBoxVertexBuffer == 0)
      mBoxVertexBuffer = activeDevice->createVertexBuffer(byteSize, true);
   else
      activeDevice->allocateVertexBuffer(mBoxVertexBuffer, byteSize, true);

   void* dst = activeDevice->lockVertexBuffer(mBoxVertexBuffer, byteSize);
   memcpy(dst, vertices.data(), static_cast<size_t>(byteSize));
   activeDevice->unlockVertexBuffer(mBoxVertexBuffer);

   glBindBuffer(GL_ARRAY_BUFFER, mBoxVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (GLvoid*)0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (GLvoid*)(sizeof(float)*3));

   glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}

void DetonationManager::drawExplosion(Detonation *det, float time)
{
   GLDevice* dev= static_cast<GLDevice*>(activeDevice);

   time= det->elapsed(time);
   dev->setParameter(mParamTime, time);

   float x= static_cast<float>(det->getX());
   float y= static_cast<float>(det->getY());

   float top= static_cast<float>(det->getUp());
   float bottom= static_cast<float>(det->getDown());
   float left= static_cast<float>(det->getLeft());
   float right= static_cast<float>(det->getRight());

   dev->setParameter(mParamTop, top);
   dev->setParameter(mParamBottom, bottom);
   dev->setParameter(mParamLeft, left);
   dev->setParameter(mParamRight, right);

   // top
   activeDevice->push(Matrix::position(x,-y+1,0.0f));
   drawBox(1.0f, top, 1.0f, -0.5f, 0.5f, 0.5f, top+0.5f, 1|4|8); // top !bottom left right
   activeDevice->pop();

   // center
   activeDevice->push(Matrix::position(x,-y,0.0f));
   drawBox(1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f,0.5f, 0); // !top !bottom !left !right
   activeDevice->pop();

   // left
   activeDevice->push(Matrix::position(x-left,-y,0.0f));
   drawBox(left, 1.0f, 1.0f, -left-0.5f, -0.5f, -0.5f,0.5f, 1|2|4); // top bottom left !right
   activeDevice->pop();

   // right
   activeDevice->push(Matrix::position(x+1,-y,0.0f));
   drawBox(right, 1.0f, 1.0f, 0.5f, right+0.5f, -0.5f,0.5f, 1|2|8); // top bottom !left right
   activeDevice->pop();

   // bottom
   activeDevice->push(Matrix::position(x,-y-bottom,0.0f));
   drawBox(1.0f, bottom, 1.0f, -0.5f, 0.5f, -bottom-0.5f, -0.5f, 2|4|8); // !top bottom left right
   activeDevice->pop();
}

void DetonationManager::render()
{
   if (mDetonations.empty())
      return;

   GLDevice* dev= static_cast<GLDevice*>(activeDevice);

   // replaces the legacy glGetFloatv(GL_PROJECTION_MATRIX, ...) readback - see
   // GLDevice::getProjectionMatrix()'s own doc comment for why this is the right replacement
   // (this engine's "projection" matrix already carries view*projection combined).
   Matrix projMat = dev->getProjectionMatrix();
   projMat = projMat.invert();
   Vector camPos = projMat.translation();

   glEnable(GL_BLEND);

   dev->setShader(mShader);
   dev->setParameter(mParamCamPos, camPos);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_3D, mNoiseMap);
   dev->bindSampler(mParamNoiseMap, 0);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, mGradientMap);
   dev->bindSampler(mParamGradientMap, 1);

   for (const auto& det : mDetonations)
   {
      drawExplosion(det.get(), mTime);
   }

   dev->setShader(0);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, 0);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_3D, 0);

   glDisable(GL_BLEND);
}
