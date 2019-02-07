#include "detonationmanager.h"
#include "detonation.h"
#include "framework/gldevice.h"
#include "math/vector.h"
#include "math/vector4.h"
#include "image/image.h"
#include <math.h>

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
  mParamGradientMap(0)
{
}

DetonationManager::~DetonationManager()
{
   clear();
}

void DetonationManager::clear()
{
   QList<Detonation*>::Iterator it;
   for (it=mDetonations.begin(); it!=mDetonations.end(); )
   {
      Detonation *det= *it;
      it= mDetonations.erase(it);
      delete det;
   }
}

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
   return fract(sin(n)*43758.5453);
}

Vector floor( const Vector& x )
{
   Vector v;
   v.x= floor(x.x);
   v.y= floor(x.y);
   v.z= floor(x.z);
   return v;
}

Vector fract( const Vector& x )
{
   Vector v;
   v.x= fract(x.x);
   v.y= fract(x.y);
   v.z= fract(x.z);
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
   Vector p = floor(x);

   Vector f = Vector( frand(0.0f, 1.0f), frand(0.0f, 1.0f), frand(0.0f, 1.0f) );

   float n = p.x + p.y*57.0 + 113.0*p.z;

   float res = //frand(-0.5, 1.0);
      mix( mix( mix( hash(n+  0.0), hash(n+  1.0),f.x),
      mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
      mix(mix( hash(n+113.0), hash(n+114.0),f.x),
      mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);

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

   float t = fract(x*5.0);
   
   Vector4 c;
   if (x < 0.0)
      return c0;
   else if (x < 0.2)
      c =  mix(c0, c1, t);
   else if (x < 0.4)
      c = mix(c1, c2, t);
   else if (x < 0.6)
      c = mix(c2, c3, t);
   else if (x < 0.8)
      c = mix(c3, c4, t);		
   else if (x < 1.0)
      c = mix(c4, c5, t);		
   else if (x >= 1.0)
      return c5;

   return c;
}

void DetonationManager::init()
{
   int size;
   GLDevice* dev= (GLDevice*)activeDevice;
/*
   size= 32;
   unsigned int *gradMap= new unsigned int[size];
   for (int i=0; i<size; i++)
   {
      Vector4 c= gradient(i / (float)size);
      
      c.w *= 0.5;
      c.x *= c.w;
      c.y *= c.w;
      c.z *= c.w;

      unsigned int color= c.rgba();
      gradMap[i]= color;
   }
   savetga("palette.tga", gradMap, size, 1);
*/

   Image palette("detonationpalette");
   glGenTextures(1, &mGradientMap);
   glBindTexture(GL_TEXTURE_1D, mGradientMap);
   glTexImage1D(
      GL_TEXTURE_1D,
      0,
      GL_RGBA,
      palette.getWidth(),
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      palette.getData()
      );
   glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   size= 32;
   float mi=1000.0,ma=-1000.0;
   unsigned char *noiseMap= new unsigned char[size*size*size];
   for (int z=0;z<size;z++)
   {
      const float scale= 1.0 / size;
      for (int y=0;y<size;y++)
      {
         for (int x=0;x<size;x++)
         {
            Vector p(x*scale, y*scale, z*scale);
            float n1= noise(p);
            if (n1 > ma) ma= n1;
            if (n1 < mi) mi= n1;
            noiseMap[(z*size+y)*size+x]= n1*127.0+128.0;
         }
      }
   }

   // printf("noise range: %f..%f \n", mi, ma);

   glGenTextures(1, &mNoiseMap);
   glBindTexture(GL_TEXTURE_3D, mNoiseMap);
   glTexImage3D(
      GL_TEXTURE_3D,
      0,
      GL_ALPHA,
      size,
      size,
      size,
      0,
      GL_ALPHA,
      GL_UNSIGNED_BYTE,
      noiseMap
      );
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
   glBindTexture(GL_TEXTURE_3D, 0);
   delete[] noiseMap;


   // load shader
   mShader= dev->loadShader("flame-vert.glsl", "flame-frag.glsl");

   mParamTime= dev->getParameterIndex("time");
   mParamCamPos= dev->getParameterIndex("campos");
   mParamNoiseMap= dev->getParameterIndex("noisemap");
   mParamGradientMap= dev->getParameterIndex("gradientmap");
   mParamTop= dev->getParameterIndex("top");
   mParamBottom= dev->getParameterIndex("bottom");
   mParamLeft= dev->getParameterIndex("left");
   mParamRight= dev->getParameterIndex("right");
   mParamBoundMin= dev->getParameterIndex("boundmin");
   mParamBoundMax= dev->getParameterIndex("boundmax");

}

void DetonationManager::addDetonation(int x, int y, int top, int bottom, int left, int right)
{
   Detonation *det= new Detonation(x,y+1, left, right, top, bottom);
   det->setStartTime(mTime);
   mDetonations.append( det );
}

void DetonationManager::update(float time)
{
   mTime= time;
   QList<Detonation*>::Iterator it;
   for (it=mDetonations.begin(); it!=mDetonations.end(); )
   {
      Detonation *det= *it;
      if (det->elapsed(time) > 2.0)
      {
         it= mDetonations.erase(it);
         delete det;
      }
      else
         it++;
   }
}

void DetonationManager::drawBox(float x, float y, float z, float left, float right, float bottom, float top, int sides)
{
   static unsigned char tris[5*3*2]= {
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

   glBegin(GL_TRIANGLES);

   for (int i=0; i<5; i++)
   {
      if (sides&1)
      for (int tri=0;tri<6;tri++)
      {
         const int index= tris[i*6+tri];
         Vector v(index&1, index>>1&1, index>>2&1);

         glNormal3f( 
            linear(left, right, v.x),
            linear(bottom, top, v.y),
            v.z-0.5 );
         glVertex3f( v.x*x, v.y*y, v.z*z );
      }
      sides>>=1;
   }

   glEnd();
}

void DetonationManager::drawExplosion(Detonation *det, float time)
{
   GLDevice* dev= (GLDevice*)activeDevice;

   time= det->elapsed(time);
   dev->setParameter(mParamTime, time);

   float x= det->getX();
   float y= det->getY();

   float top= det->getUp();
   float bottom= det->getDown();
   float left= det->getLeft();
   float right= det->getRight();

   dev->setParameter(mParamTop, top);
   dev->setParameter(mParamBottom, bottom);
   dev->setParameter(mParamLeft, left);
   dev->setParameter(mParamRight, right);

//   glDepthMask(GL_FALSE);

   // top
   glPushMatrix();
   glTranslatef(x,-y+1,0.0f);
   drawBox(1.0, top, 1.0, -0.5, 0.5, 0.5, top+0.5, 1|4|8); // top !bottom left right
   glPopMatrix();

   // center
   glPushMatrix();
   glTranslatef(x,-y,0.0f);
   drawBox(1.0, 1.0, 1.0, -0.5, 0.5, -0.5,0.5, 0); // !top !bottom !left !right
   glPopMatrix();

   // left
   glPushMatrix();
   glTranslatef(x-left,-y,0.0f);
   drawBox(left, 1.0, 1.0, -left-0.5, -0.5, -0.5,0.5, 1|2|4); // top bottom left !right
   glPopMatrix();

   // right
   glPushMatrix();
   glTranslatef(x+1,-y,0.0f);
   drawBox(right, 1.0, 1.0, 0.5, right+0.5, -0.5,0.5, 1|2|8); // top bottom !left right
   glPopMatrix();

   // bottom
   glPushMatrix();
   glTranslatef(x,-y-bottom,0.0f);
   drawBox(1.0, bottom, 1.0, -0.5, 0.5, -bottom-0.5, -0.5, 2|4|8); // !top bottom left right
   glPopMatrix();
}

void DetonationManager::render()
{
   GLDevice* dev= (GLDevice*)activeDevice;

   Matrix projMat;
   glGetFloatv(GL_PROJECTION_MATRIX, projMat.data());
   projMat= projMat.invert();
   Vector camPos= projMat.translation();

   glEnable(GL_BLEND);

   // set shader
   dev->setShader(mShader);
   dev->setParameter(mParamCamPos, camPos);

   glActiveTexture(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_3D);
   glBindTexture(GL_TEXTURE_3D, mNoiseMap);
   dev->bindSampler(mParamNoiseMap, 0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_1D);
   glBindTexture(GL_TEXTURE_1D, mGradientMap);
   dev->bindSampler(mParamGradientMap, 1);

   foreach (Detonation *det, mDetonations)
   {
      drawExplosion(det, mTime);
   }

   dev->setShader(0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_1D);

   glActiveTexture(GL_TEXTURE0_ARB);
   glDisable(GL_TEXTURE_3D);
   glEnable(GL_TEXTURE_2D);

   glDisable(GL_BLEND);
}

