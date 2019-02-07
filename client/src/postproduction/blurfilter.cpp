#include "blurfilter.h"
#include "framework/gldevice.h"
#include <math.h>

float filters[]={0.0f, 4.9f, 16.8f, 50.726f, 136.882f, 345.38f, 848.614f, 1412.286f, 1612.833f };
float factors[]= {4.9f, 8.125f, 12.075f, 15.875f, 19.82f, 23.96875f, 28.0f, 18.100f, 18.0f };


/*
9-tap kaiser/bessel filter -40db at fs/2
0.000000,
0.037396,
0.119231,
0.209928,
0.250000,
0.209928,
0.119231,
0.037396,
0.000000,
*/

static int modus= 0;

BlurFilter::BlurFilter(float radius)
: Filter("blur")
, mDownsample(0)
, mGauss(0)
, mParamOffsetX1(-1)
, mParamOffsetY1(-1)
, mParamClampU1(-1)
, mParamClampV1(-1)
, mParamTexture1(-1)
, mParamOffsetX2(-1)
, mParamOffsetY2(-1)
, mParamTexture2(-1)
, mParamClampU2(-1)
, mParamClampV2(-1)
, mParamRadius(-1)
, mParamKernel(-1)
, mRadius(radius)
, mAlpha(1.0f)
{
   mTemp[0]= 0;
   mTemp[1]= 0;

   const int size= 11;
   double coeff[size]= {
       0.011353,
       0.027530,
       0.048153,
       0.068867,
       0.084290,
       0.090000,
       0.084290,
       0.068867,
       0.048153,
       0.027530,
       0.011353 };

   double sum= 0.0f;
   for (int i=0;i<size;i++)
       sum+= coeff[i];

   for (int i=0;i<size;i++)
   {
       coeff[i]= coeff[i] / sum;
       // printf("BlurFilter::BlurFilter: %f, ", coeff[i]);
   }

   // printf("\n");

}

BlurFilter::~BlurFilter()
{
   delete mTemp[0];
   delete mTemp[1];
}

bool BlurFilter::init()
{
   mDownsample= activeDevice->loadShader("blur-vert.glsl", "blurprepass-simple-frag.glsl");
   mParamOffsetX1= activeDevice->getParameterIndex("texelOffsetX");
   mParamOffsetY1= activeDevice->getParameterIndex("texelOffsetY");
   mParamClampU1= activeDevice->getParameterIndex("clampU");
   mParamClampV1= activeDevice->getParameterIndex("clampV");
   mParamTexture1= activeDevice->getParameterIndex("texture");

   mGauss= activeDevice->loadShader("blur-vert.glsl", "blur-frag.glsl");
   mParamOffsetX2= activeDevice->getParameterIndex("texelOffsetX");
   mParamOffsetY2= activeDevice->getParameterIndex("texelOffsetY");
   mParamTexture2= activeDevice->getParameterIndex("texture");
   mParamClampU2=   activeDevice->getParameterIndex("clampU");
   mParamClampV2=   activeDevice->getParameterIndex("clampV");
   mParamRadius=   activeDevice->getParameterIndex("radius");
   mParamKernel=   activeDevice->getParameterIndex("kernel");

   return true;
}

void BlurFilter::setRadius(float radius)
{
   mRadius= radius;
}

void BlurFilter::setAlpha(float alpha)
{
   mAlpha = alpha;
}

unsigned int BlurFilter::downSample(unsigned int texture, int width, int height, int pass)
{
   float deltaU= 1.0f / mTemp[0]->width();
   float deltaV= 1.0f / mTemp[0]->height();

   activeDevice->setShader( mDownsample );
   activeDevice->bindSampler(mParamTexture1, 0);
   activeDevice->setParameter(mParamClampU1, deltaU * (width-0.5f));
   activeDevice->setParameter(mParamClampV1, deltaV * (height-0.5f));

   float border= 1.0f;
   downSamplePass(mTemp[0], width>>1, height, texture, width, height, deltaU, deltaV, deltaU*pass, 0.0f, border);
   downSamplePass(mTemp[1], width>>1, height>>1, mTemp[0]->texture(), width>>1, height, deltaU, deltaV, 0.0f, deltaV*pass, border);

   return mTemp[1]->texture();
}


// perform horizontal or vertical pass of downsampling
void BlurFilter::downSamplePass(FrameBuffer* dst, int dstWidth, int dstHeight, unsigned int texture, int srcWidth, int srcHeight, float deltaU, float deltaV, float texelU, float texelV, float border)
{
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   activeDevice->setParameter(mParamOffsetX1, texelU);
   activeDevice->setParameter(mParamOffsetY1, texelV);

   dst->bind();

   float w= dstWidth + border;
   float h= dstHeight + border;

   // fix texture coordinate to match additional border radius
   float tu1= 0.0f;
   float tv1= 0.0f;
   float tu2= (srcWidth+border*srcWidth/dstWidth)*deltaU;
   float tv2= (srcHeight+border*srcHeight/dstHeight)*deltaV;

   glBegin(GL_QUADS);
   glTexCoord2f(tu1,tv1); glVertex3f( 0, 0, 0);
   glTexCoord2f(tu2,tv1); glVertex3f( w, 0, 0);
   glTexCoord2f(tu2,tv2); glVertex3f( w, h, 0);
   glTexCoord2f(tu1,tv2); glVertex3f( 0, h, 0);
   glEnd();

   dst->unbind();
}

// gauss passes
unsigned int BlurFilter::gauss(float radius, unsigned int texture, int width, int height)
{
   float deltaU= 1.0f / mTemp[0]->width();
   float deltaV= 1.0f / mTemp[0]->height();

   int size= (int)ceil(radius);
   if (size > 31) size= 31;

   float kernel[32];
   const double scale=  -4.0/(radius*radius);
   float sum= 0.0f;
   kernel[0]= 1.0f;
   for (int i=1;i<=size;i++)
   {
      float f= pow(2.718281828459045, i*i*scale);
      sum += f;
      kernel[i]= f;
   }
   for (int i=size+1; i<32; i++)
       kernel[i]= 0.0f;

   float t= 1.0f / (sum*2.0f+1.0f);
   for (int i=0;i<=size;i++)
      kernel[i] *= t;

   activeDevice->setShader( mGauss );
   activeDevice->setParameter(mParamKernel, kernel, 32);
   activeDevice->setParameter(mParamRadius, size + 0.5);
   activeDevice->bindSampler(mParamTexture2, 0);

   activeDevice->setParameter(mParamClampU2, deltaU * (width-0.5f));
   activeDevice->setParameter(mParamClampV2, deltaV * (height-0.5f));

   gaussPass(mTemp[0], width, height, texture, width, height, deltaU, deltaV, deltaU, 0.0f );
   gaussPass(mTemp[1], width, height, mTemp[0]->texture(), width, height, deltaU, deltaV, 0.0f, deltaV );

   return mTemp[1]->texture();
}

void BlurFilter::gaussPass(FrameBuffer* dst, int dstWidth, int dstHeight, unsigned int texture, int srcWidth, int srcHeight, float deltaU, float deltaV, float texelU, float texelV)
{
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   activeDevice->setParameter(mParamOffsetX2, texelU);
   activeDevice->setParameter(mParamOffsetY2, texelV);

   dst->bind();

   float w= dstWidth;
   float h= dstHeight;

   float tu1= 0;
   float tv1= 0;
   float tu2= srcWidth*deltaU;
   float tv2= srcHeight*deltaV;

   glBegin(GL_QUADS);
   glTexCoord2f(tu1,tv1); glVertex3f( 0, 0, 0);
   glTexCoord2f(tu2,tv1); glVertex3f( w, 0, 0);
   glTexCoord2f(tu2,tv2); glVertex3f( w, h, 0);
   glTexCoord2f(tu1,tv2); glVertex3f( 0, h, 0);
   glEnd();

   dst->unbind();

}


void BlurFilter::draw(int dstWidth, int dstHeight, unsigned int texture, int srcWidth, int srcHeight)
{
   activeDevice->setShader( 0 );

   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   int w= dstWidth;
   int h= dstHeight;

   float tu1= 0.0f;
   float tv1= 0.0f;
   float tu2= (float)srcWidth / w;
   float tv2= (float)srcHeight / h;

   glColor4f(1,1,1,mAlpha);
   glBegin(GL_QUADS);
   glTexCoord2f(tu1,tv1); glVertex3f( 0, 0, 0);
   glTexCoord2f(tu2,tv1); glVertex3f( w, 0, 0);
   glTexCoord2f(tu2,tv2); glVertex3f( w, h, 0);
   glTexCoord2f(tu1,tv2); glVertex3f( 0, h, 0);
   glEnd();
}


void BlurFilter::begin()
{
   FrameBuffer* fb= FrameBuffer::Instance();

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0, fb->width(), 0, fb->height(), -1, 1);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
}


void BlurFilter::end()
{
   // restore projection
   glPopMatrix();

   // restore modelview
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


float match(unsigned int *data1, unsigned int* data2, int w, int h)
{
   int error= 0;
   int count= 0;
   for (int i=0; i<w*h; i++)
   {
      unsigned int c1= data1[i];
      unsigned int c2= data2[i];

      int a1= (c1>>24&255);
      int r1= (c1>>16&255);
      int g1= (c1>>8&255);
      int b1= (c1&255);

      int a2= (c2>>24&255);
      int r2= (c2>>16&255);
      int g2= (c2>>8&255);
      int b2= (c2&255);

      if ( (r1==255 && g1==255 && b1==255) || (r1==0 && g1==0 && b1==0) )
          continue;

      int a= a2 - a1;
      int r= r2 - r1;
      int g= g2 - g1;
      int b= b2 - b1;

      error += abs(a) + abs(r) + abs(g) + abs(b);
      count++;
   }
   if (count==0)
       count=1;

   return (float)error / count;
}

void BlurFilter::process(unsigned int texture, float, float)
{
   begin();

   FrameBuffer* fb= FrameBuffer::Instance();

   FrameBuffer::push();

   // create buffers
   for (int i=0; i<2; i++)
   {
      if (!mTemp[i] || mTemp[i]->resolutionChanged(fb->width(), fb->height()))
      {
         if (mTemp[i])
            delete mTemp[i];
         mTemp[i]= new FrameBuffer( fb->width(), fb->height(), 1, false );
      }
   }

   int width= fb->width();
   int height= fb->height();

   // overlap window to compensate blur radius

   int prepasses= 0;
   float gaussRadius= mRadius;
   while (gaussRadius > filters[prepasses+1] && prepasses<7) prepasses++;
   gaussRadius= (gaussRadius - filters[prepasses]) * factors[prepasses] / (filters[prepasses+1] - filters[prepasses]);

   // downsampling passes
   for (int pass=1; pass<=prepasses; pass++)
   {
      texture= downSample(texture, width, height, pass);
      width>>=1;
      height>>=1;
   }

   // gauss pass with remaining radius
   texture= gauss(gaussRadius, texture, width, height);

   FrameBuffer::pop();

   draw(fb->width(), fb->height(), texture, width, height );

   end();
}







void BlurFilter::matchFilters(unsigned int texture)
{
   begin();

//   modus^=1;

   FrameBuffer* fb= FrameBuffer::Instance();

   FrameBuffer::push();

   // create buffers
   for (int i=0; i<2; i++)
   {
      if (!mTemp[i] || mTemp[i]->resolutionChanged(fb->width(), fb->height()))
      {
         if (mTemp[i])
            delete mTemp[i];
         mTemp[i]= new FrameBuffer( fb->width(), fb->height(), 1, false );
      }
   }

   int width= fb->width();
   int height= fb->height();

   // overlap window to compensate blur radius

   int prepasses= 8;

   /*
      int prepasses= 0;
      while (gaussRadius > filters[prepasses+1] && prepasses<5)
      {
         prepasses++;
      }
      gaussRadius -= filters[prepasses];
   */

   if (modus == 0)
   {
      unsigned int source= texture;

      for (int pass=1; pass<=prepasses; pass++)
      {
         texture= downSample(texture, width, height, pass);
         width>>=1;
         height>>=1;
      }

//      texture= gauss(gaussRadius, texture, width, height);




      // stretch into original buffer

      unsigned int* data1= new unsigned int[fb->width() * fb->height()];
      unsigned int* data2= new unsigned int[fb->width() * fb->height()];

      fb->bind();
      draw(fb->width(), fb->height(), texture, width, height );
      glReadPixels(0,0, fb->width(), fb->height(), GL_BGRA, GL_UNSIGNED_BYTE, data1);

      float low= filters[prepasses-1];
      float dir= 10.0f;

      float bestrad= 0.0f;
      float best= 100000000.0f;
      float prev= 100000000.0f;

      while (fabs(dir) > 0.001)
      {
         texture= gauss(low, source, fb->width(), fb->height());
         fb->bind();
         draw(fb->width(), fb->height(), texture, fb->width(), fb->height() );

         glReadPixels(0,0, fb->width(), fb->height(), GL_BGRA, GL_UNSIGNED_BYTE, data2);

         float m= match(data1, data2, fb->width(), fb->height());
         if (m < best)
         {
            bestrad= low;
            best= m;
         }

         // if current match is worse than previous: change direction and use smaller step size
         if (m >= prev)
         {
            low= bestrad;
            dir = -dir*0.5f;
         }

         low += dir;

         prev= m;

      }

      delete[] data1;
      delete[] data2;
   }

   if (modus==1)
   {
      // gauss pass
      float gaussRadius= filters[prepasses+1];
      texture= gauss(gaussRadius, texture, width, height);
   }

   // stretch into original buffer
//   FrameBuffer::pop();

//   draw(fb->width(), fb->height(), texture, width, height );
   FrameBuffer::pop();

   end();
}


void BlurFilter::matchInterpolation(unsigned int texture)
{
   begin();

//   modus^=1;

   FrameBuffer* fb= FrameBuffer::Instance();

   FrameBuffer::push();

   // create buffers
   for (int i=0; i<2; i++)
   {
      if (!mTemp[i] || mTemp[i]->resolutionChanged(fb->width(), fb->height()))
      {
         if (mTemp[i])
            delete mTemp[i];
         mTemp[i]= new FrameBuffer( fb->width(), fb->height(), 1, false );
      }
   }

   int width= fb->width();
   int height= fb->height();

   // overlap window to compensate blur radius

//   for (int prepasses=1; prepasses<8; prepasses++)
   {
      int prepasses= 7;
      unsigned int source= texture;

      int pass= 1;
      for (pass=1; pass<=prepasses; pass++)
      {
         texture= downSample(texture, width, height, pass);
         width>>=1;
         height>>=1;
      }

      source= mTemp[1]->copyTexture();

      unsigned int* data1= new unsigned int[fb->width() * fb->height()];
      unsigned int* data2= new unsigned int[fb->width() * fb->height()];

      texture= downSample(texture, width, height, pass);

      fb->bind();
      draw(fb->width(), fb->height(), texture, width>>1, height>>1 );
      glReadPixels(0,0, fb->width(), fb->height(), GL_BGRA, GL_UNSIGNED_BYTE, data1);

//      texture= gauss(gaussRadius, texture, width, height);

      // stretch into original buffer


      float low= 0.0f;
      float curr= 1.0f;
      float high= 100000.0f;
      float dir= 1.0f;

      float bestrad= 0.0f;
      float best= 100000000.0f;

      while (fabs(dir) > 0.001)
      {
         texture= gauss(curr, source, width, height);

         fb->bind();
         draw(fb->width(), fb->height(), texture, width, height );
         glReadPixels(0,0, fb->width(), fb->height(), GL_BGRA, GL_UNSIGNED_BYTE, data2);

         float m= match(data1, data2, fb->width(), fb->height());
         if (m < best)
         {
            if (dir < 0.0)
               high= curr;
            else
               low= curr;
            bestrad= curr;
            best= m;
         }

         // if current match is worse than previous: change direction and use smaller step size
         if (curr > high || curr < low || m > (best+0.1f))
         {
             if (dir > 0.0)
             {
                if (curr < high)
                   high= curr;
             }
             else
             {
                 if (curr < high)
                    low= curr;
             }

             curr= bestrad;
             dir = -dir*0.5f;
             m= best;
         }

         curr += dir;
      }

      delete[] data1;
      delete[] data2;
   }

   // stretch into original buffer
//   FrameBuffer::pop();

//   draw(fb->width(), fb->height(), texture, width, height );
   FrameBuffer::pop();

   end();
}
