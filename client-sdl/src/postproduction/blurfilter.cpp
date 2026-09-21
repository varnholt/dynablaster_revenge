#include "blurfilter.h"
#include <math.h>
#include "gldevice.h"
#include "math/matrix.h"

float filters[] = {0.0f, 4.9f, 16.8f, 50.726f, 136.882f, 345.38f, 848.614f, 1412.286f, 1612.833f};
float factors[] = {4.9f, 8.125f, 12.075f, 15.875f, 19.82f, 23.96875f, 28.0f, 18.100f, 18.0f};

BlurFilter::BlurFilter(float radius)
    : Filter("blur"),
      mDownsample(0),
      mGauss(0),
      mBlit(0),
      mParamBlitTexture(-1),
      mParamBlitAlpha(-1),
      mParamOffsetX1(-1),
      mParamOffsetY1(-1),
      mParamClampU1(-1),
      mParamClampV1(-1),
      mParamTexture1(-1),
      mParamOffsetX2(-1),
      mParamOffsetY2(-1),
      mParamTexture2(-1),
      mParamClampU2(-1),
      mParamClampV2(-1),
      mParamRadius(-1),
      mParamKernel(-1),
      mRadius(radius),
      mAlpha(1.0f)
{
   mTemp[0] = 0;
   mTemp[1] = 0;
}

BlurFilter::~BlurFilter()
{
   delete mTemp[0];
   delete mTemp[1];
}

bool BlurFilter::init()
{
   mDownsample = activeDevice->loadShader("blur-vert.glsl", "blurprepass-simple-frag.glsl");
   mParamOffsetX1 = activeDevice->getParameterIndex("texelOffsetX");
   mParamOffsetY1 = activeDevice->getParameterIndex("texelOffsetY");
   mParamClampU1 = activeDevice->getParameterIndex("clampU");
   mParamClampV1 = activeDevice->getParameterIndex("clampV");
   mParamTexture1 = activeDevice->getParameterIndex("texture0");

   mGauss = activeDevice->loadShader("blur-vert.glsl", "blur-frag.glsl");
   mParamOffsetX2 = activeDevice->getParameterIndex("texelOffsetX");
   mParamOffsetY2 = activeDevice->getParameterIndex("texelOffsetY");
   mParamTexture2 = activeDevice->getParameterIndex("texture0");
   mParamClampU2 = activeDevice->getParameterIndex("clampU");
   mParamClampV2 = activeDevice->getParameterIndex("clampV");
   mParamRadius = activeDevice->getParameterIndex("radius");
   mParamKernel = activeDevice->getParameterIndex("kernel");

   mBlit = activeDevice->loadShader("blur-blit-vert.glsl", "blur-blit-frag.glsl");
   mParamBlitTexture = activeDevice->getParameterIndex("texture0");
   mParamBlitAlpha = activeDevice->getParameterIndex("alpha");

   return true;
}

void BlurFilter::setRadius(float radius)
{
   mRadius = radius;
}

void BlurFilter::setAlpha(float alpha)
{
   mAlpha = alpha;
}

unsigned int BlurFilter::downSample(unsigned int texture, int width, int height, int pass)
{
   float deltaU = 1.0f / mTemp[0]->width();
   float deltaV = 1.0f / mTemp[0]->height();

   activeDevice->setShader(mDownsample);
   activeDevice->bindSampler(mParamTexture1, 0);
   activeDevice->setParameter(mParamClampU1, deltaU * (width - 0.5f));
   activeDevice->setParameter(mParamClampV1, deltaV * (height - 0.5f));

   float border = 1.0f;
   downSamplePass(mTemp[0], width >> 1, height, texture, width, height, deltaU, deltaV, deltaU * pass, 0.0f, border);
   downSamplePass(mTemp[1], width >> 1, height >> 1, mTemp[0]->texture(), width >> 1, height, deltaU, deltaV, 0.0f, deltaV * pass, border);

   return mTemp[1]->texture();
}

// perform horizontal or vertical pass of downsampling
void BlurFilter::downSamplePass(
   FrameBuffer* dst,
   int dstWidth,
   int dstHeight,
   unsigned int texture,
   int srcWidth,
   int srcHeight,
   float deltaU,
   float deltaV,
   float texelU,
   float texelV,
   float border
)
{
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   activeDevice->setParameter(mParamOffsetX1, texelU);
   activeDevice->setParameter(mParamOffsetY1, texelV);

   dst->bind();

   float w = dstWidth + border;
   float h = dstHeight + border;

   // fix texture coordinate to match additional border radius
   float tu1 = 0.0f;
   float tv1 = 0.0f;
   float tu2 = (srcWidth + border * srcWidth / dstWidth) * deltaU;
   float tv2 = (srcHeight + border * srcHeight / dstHeight) * deltaV;

   static_cast<GLDevice*>(activeDevice)
      ->setProjectionMatrix(Matrix::ortho(0.0f, (float)dst->width(), 0.0f, (float)dst->height(), -1.0f, 1.0f));
   activeDevice->push(Matrix());
   mQuad.drawRect(0.0f, 0.0f, w, h, tu1, tv1, tu2, tv2);

   dst->unbind();
}

// gauss passes
unsigned int BlurFilter::gauss(float radius, unsigned int texture, int width, int height)
{
   float deltaU = 1.0f / mTemp[0]->width();
   float deltaV = 1.0f / mTemp[0]->height();

   int size = (int)ceil(radius);
   if (size > 31)
      size = 31;

   float kernel[32];
   const double scale = -4.0 / (radius * radius);
   float sum = 0.0f;
   kernel[0] = 1.0f;
   for (int i = 1; i <= size; i++)
   {
      float f = (float)pow(2.718281828459045, i * i * scale);
      sum += f;
      kernel[i] = f;
   }
   for (int i = size + 1; i < 32; i++)
      kernel[i] = 0.0f;

   float t = 1.0f / (sum * 2.0f + 1.0f);
   for (int i = 0; i <= size; i++)
      kernel[i] *= t;

   activeDevice->setShader(mGauss);
   activeDevice->setParameter(mParamKernel, kernel, 32);
   activeDevice->setParameter(mParamRadius, size + 0.5f);
   activeDevice->bindSampler(mParamTexture2, 0);

   activeDevice->setParameter(mParamClampU2, deltaU * (width - 0.5f));
   activeDevice->setParameter(mParamClampV2, deltaV * (height - 0.5f));

   gaussPass(mTemp[0], width, height, texture, width, height, deltaU, deltaV, deltaU, 0.0f);
   gaussPass(mTemp[1], width, height, mTemp[0]->texture(), width, height, deltaU, deltaV, 0.0f, deltaV);

   return mTemp[1]->texture();
}

void BlurFilter::gaussPass(
   FrameBuffer* dst,
   int dstWidth,
   int dstHeight,
   unsigned int texture,
   int srcWidth,
   int srcHeight,
   float deltaU,
   float deltaV,
   float texelU,
   float texelV
)
{
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   activeDevice->setParameter(mParamOffsetX2, texelU);
   activeDevice->setParameter(mParamOffsetY2, texelV);

   dst->bind();

   float w = (float)dstWidth;
   float h = (float)dstHeight;

   float tu1 = 0.0f;
   float tv1 = 0.0f;
   float tu2 = srcWidth * deltaU;
   float tv2 = srcHeight * deltaV;

   static_cast<GLDevice*>(activeDevice)
      ->setProjectionMatrix(Matrix::ortho(0.0f, (float)dst->width(), 0.0f, (float)dst->height(), -1.0f, 1.0f));
   activeDevice->push(Matrix());
   mQuad.drawRect(0.0f, 0.0f, w, h, tu1, tv1, tu2, tv2);

   dst->unbind();
}

void BlurFilter::draw(int dstWidth, int dstHeight, unsigned int texture, int srcWidth, int srcHeight)
{
   activeDevice->setShader(mBlit);
   activeDevice->setParameter(mParamBlitAlpha, mAlpha);
   activeDevice->bindSampler(mParamBlitTexture, 0);

   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   int w = dstWidth;
   int h = dstHeight;

   float tu1 = 0.0f;
   float tv1 = 0.0f;
   float tu2 = (float)srcWidth / w;
   float tv2 = (float)srcHeight / h;

   FrameBuffer* fb = FrameBuffer::Instance();
   int fbWidth = fb ? fb->width() : w;
   int fbHeight = fb ? fb->height() : h;

   static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(Matrix::ortho(0.0f, (float)fbWidth, 0.0f, (float)fbHeight, -1.0f, 1.0f));
   activeDevice->push(Matrix());
   mQuad.drawRect(0.0f, 0.0f, (float)w, (float)h, tu1, tv1, tu2, tv2);

   activeDevice->setShader(0);
}

void BlurFilter::begin()
{
   static_cast<GLDevice*>(activeDevice)->pushProjection();

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
}

void BlurFilter::end()
{
   static_cast<GLDevice*>(activeDevice)->popProjection();

   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}

void BlurFilter::process(unsigned int texture, float, float)
{
   begin();

   FrameBuffer* fb = FrameBuffer::Instance();

   FrameBuffer::push();

   // create buffers
   for (int i = 0; i < 2; i++)
   {
      if (!mTemp[i] || mTemp[i]->resolutionChanged(fb->width(), fb->height()))
      {
         if (mTemp[i])
            delete mTemp[i];
         mTemp[i] = new FrameBuffer(fb->width(), fb->height(), 1, FrameBuffer::NoDepthBuffer);
      }
   }

   int width = fb->width();
   int height = fb->height();

   // overlap window to compensate blur radius

   int prepasses = 0;
   float gaussRadius = mRadius;
   while (gaussRadius > filters[prepasses + 1] && prepasses < 7)
      prepasses++;
   gaussRadius = (gaussRadius - filters[prepasses]) * factors[prepasses] / (filters[prepasses + 1] - filters[prepasses]);

   // downsampling passes
   for (int pass = 1; pass <= prepasses; pass++)
   {
      texture = downSample(texture, width, height, pass);
      width >>= 1;
      height >>= 1;
   }

   // gauss pass with remaining radius
   texture = gauss(gaussRadius, texture, width, height);

   FrameBuffer::pop();

   draw(fb->width(), fb->height(), texture, width, height);

   end();
}
