// header
#include "bitmapfont.h"
#include "defaultshader.h"
#include "framework/gldevice.h"
#include "image/image.h"
#include "image/imagepool.h"
#include "math/matrix.h"
#include "math/vector4.h"
#include "render/texturepool.h"

BitmapFont::BitmapFont(
   const char* filename,
   Parameter* description,
   float size,
   float spacing,
   float distanceRadius,
   float outlineRed,
   float outlineGreen,
   float outlineBlue,
   float outlineAlpha,
   float outlineRadius,
   float softRadius,
   float thickness
)
    : mTexture(0),
      mDescription(description),
      mSize(size),
      mSpacing(spacing),
      mRadius(distanceRadius),
      mScaleU(0.0f),
      mScaleV(0.0f),
      mOutlineRed(outlineRed),
      mOutlineGreen(outlineGreen),
      mOutlineBlue(outlineBlue),
      mOutlineAlpha(outlineAlpha),
      mOutlineRadius(outlineRadius),
      mSoftRadius(softRadius),
      mThickness(thickness),
      mColorRed(1.0f),
      mColorGreen(1.0f),
      mColorBlue(1.0f),
      mColorAlpha(1.0f),
      mBaseline(0.0f),
      mBaseColumn(0.0f),
      mVertexBuffer(0)
{
   mShader = activeDevice->loadShader("fontoutlines-vert.glsl", "fontoutlines-frag.glsl");
   mParamTexture = activeDevice->getParameterIndex("distanceMap");
   mParamOutlineColor = activeDevice->getParameterIndex("outlineColor");
   mParamColor = activeDevice->getParameterIndex("color");

   mParamSoftRadius = activeDevice->getParameterIndex("aaRadius");
   mParamOutlineRadius = activeDevice->getParameterIndex("outlineRadius");
   mParamThickness = activeDevice->getParameterIndex("threshold");
   mParamSampleOffset = activeDevice->getParameterIndex("sampleOffset");

   Image* image = ImagePool::Instance()->getImage(filename);
   mTexture = TexturePool::Instance()->getTexture(image, TexturePool::Linear | TexturePool::Clamp);
   mScaleU = 1.0f / image->getWidth();
   mScaleV = 1.0f / image->getHeight();
}

BitmapFont::~BitmapFont()
{
}

bool BitmapFont::isCharAvailable(char c) const
{
   return getCharParameter(c) != 0;
}

void BitmapFont::setOutlineColor(float r, float g, float b, float a)
{
   mOutlineRed = r;
   mOutlineGreen = g;
   mOutlineBlue = b;
   mOutlineAlpha = a;
}

void BitmapFont::getOutlineColor(float& r, float& g, float& b, float& a)
{
   r = mOutlineRed;
   g = mOutlineGreen;
   b = mOutlineBlue;
   a = mOutlineAlpha;
}

void BitmapFont::setColor(float r, float g, float b, float a)
{
   mColorRed = r;
   mColorGreen = g;
   mColorBlue = b;
   mColorAlpha = a;
}

BitmapFont::Parameter* BitmapFont::getCharParameter(char c) const
{
   if (c >= 0)
      return &mDescription[(unsigned char)c];
   else
      return 0;
}

float BitmapFont::buildVertices(float size, const char* text, float x, float y, float centerWidth, float centerHeight)
{
   mVertices.clear();

   // remember text position for cursor
   mBaseColumn = x;
   mBaseline = y;

   size *= mSize;

   x -= mRadius * size;

   if (centerWidth >= 0.0f)
   {
      float width = 0.0f;
      int index = 0;
      while (text[index])
      {
         Parameter* param = getCharParameter(text[index]);
         if (param)
            width += (param->space + mSpacing);
         index++;
      }
      width -= mSpacing;  // remove last spacing

      x += (centerWidth - (width + mRadius * 2) * size) * 0.5f;
   }

   if (centerHeight >= 0.0f)
   {
      float height = centerHeight;
      Parameter* param = getCharParameter('M');
      if (param)
         height = (param->height - mRadius * 2) * size;
      y -= (centerHeight - height) * 0.5f;
   }

   while (*text)
   {
      char c = *text++;
      Parameter* param = getCharParameter(c);

      float xLeft = x - (param->basecolumn) * size;
      float xRight = x - (param->basecolumn - param->width) * size;
      float yTop = y - (param->baseline) * size;
      float yBottom = y - (param->baseline + param->height) * size;

      mVertices.add(Vertex(xLeft, yTop, param->x * mScaleU, (param->y + param->height) * mScaleV));

      mVertices.add(Vertex(xRight, yTop, (param->x + param->width) * mScaleU, (param->y + param->height) * mScaleV));

      mVertices.add(Vertex(xRight, yBottom, (param->x + param->width) * mScaleU, param->y * mScaleV));

      mVertices.add(Vertex(xLeft, yBottom, param->x * mScaleU, param->y * mScaleV));

      x += (param->space + mSpacing) * size;
   }

   return x;
}

void BitmapFont::draw()
{
   draw(mVertices);
}

void BitmapFont::draw(const Array<Vertex>& vertices)
{
   const int quadCount = vertices.size() / 4;
   if (quadCount <= 0)
      return;

   activeDevice->setShader(mShader);

   // GLDevice::push() is what actually uploads u_modelViewProjection into whichever shader
   // program is currently bound (see gldevice.cpp - it's per-program uniform storage, not a
   // global). Without this call, fontoutlines' u_modelViewProjection is never set at all and
   // stays at GLSL's zero-initialized default, collapsing every glyph vertex to a degenerate
   // point that gets clipped - this was the actual cause of BitmapFont-drawn text never
   // producing a single visible pixel anywhere in this port (see project memory). Text vertices
   // are already baked in absolute page-space, so identity world (the page's ortho projection,
   // already set for the whole page-render pass, supplies the rest).
   activeDevice->push(Matrix());

   float sampleOffset = (vertices[1].u - vertices[0].u) / (vertices[1].x - vertices[0].x);

   glBindTexture(GL_TEXTURE_2D, mTexture);
   activeDevice->bindSampler(mParamTexture, 0);
   activeDevice->setParameter(mParamOutlineColor, Vector4(mOutlineRed, mOutlineGreen, mOutlineBlue, mOutlineAlpha));
   activeDevice->setParameter(mParamColor, Vector4(mColorRed, mColorGreen, mColorBlue, mColorAlpha));
   activeDevice->setParameter(mParamSoftRadius, mSoftRadius);
   activeDevice->setParameter(mParamOutlineRadius, mOutlineRadius);
   activeDevice->setParameter(mParamThickness, mThickness);
   activeDevice->setParameter(mParamSampleOffset, sampleOffset * 1.0f);

   // GLES3 has no GL_QUADS - each 4-vertex quad becomes 2 triangles (0,1,2 / 0,2,3), rebuilt
   // into a plain interleaved (x,y,u,v) buffer every draw call since the text changes every
   // frame anyway.
   Array<float> data;
   static const int order[6] = {0, 1, 2, 0, 2, 3};
   for (int q = 0; q < quadCount; q++)
   {
      for (int i = 0; i < 6; i++)
      {
         const Vertex& v = vertices[q * 4 + order[i]];
         data.add(v.x);
         data.add(v.y);
         data.add(v.u);
         data.add(v.v);
      }
   }

   const int size = data.size() * static_cast<int>(sizeof(float));
   if (mVertexBuffer == 0)
      mVertexBuffer = activeDevice->createVertexBuffer(size, true);
   else
      activeDevice->allocateVertexBuffer(mVertexBuffer, size, true);

   float* dst = (float*)activeDevice->lockVertexBuffer(mVertexBuffer, size);
   for (int i = 0; i < data.size(); i++)
      dst[i] = data[i];
   activeDevice->unlockVertexBuffer(mVertexBuffer);

   glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)(sizeof(float) * 2));

   glDrawArrays(GL_TRIANGLES, 0, quadCount * 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   activeDevice->pop();

   // restore the shared menu shader rather than "no shader" - see defaultshader.h. Whichever
   // page item draws next (most are plain PSDLayer-textured quads) relies on it already being
   // bound, the same way MenuDrawable bound it before this whole page-render pass started.
   activeDevice->setShader(getDefaultMenuShader());
}

unsigned int BitmapFont::getTexture()
{
   return mTexture;
}

void BitmapFont::getCursor(float size, int cursorPos, float& left, float& right, float& top, float& bottom)
{
   size *= mSize;
   cursorPos *= 4;

   if (mVertices.size() < 4)
   {
      // empty string
      Parameter* param = getCharParameter('M');
      left = mBaseColumn;
      right = left + param->space * size + mRadius * size * 2;

      top = mBaseColumn - (param->baseline) * size;
      bottom = mBaseline - (param->baseline + param->height) * size;
   }
   else if (cursorPos >= mVertices.size())
   {
      // cursor at end of text
      Parameter* param = getCharParameter('M');
      cursorPos = mVertices.size() - 4;

      left = mVertices[cursorPos + 1].x - (mRadius * size * 2) + mSpacing * size;
      right = left + param->space * size + mRadius * size * 2;
   }
   else
   {
      left = mVertices[cursorPos].x;
      right = mVertices[cursorPos + 1].x;
   }

   Parameter* param = getCharParameter('M');
   top = mBaseline - (param->baseline) * size;
   bottom = mBaseline - (param->baseline + param->height) * size;

   top -= mRadius * size;
   bottom += mRadius * size;

   left += mRadius * size;
   right -= mRadius * size;
}
