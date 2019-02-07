// header
#include "bitmapfont.h"
#include "gldevice.h"
#include "image/imagepool.h"
#include "render/texturepool.h"
#include "math/vector4.h"

BitmapFont::BitmapFont(
      const char *filename,
      Parameter *description,
      float size,
      float spacing,
      float distanceRadius,
      float outlineRed,
      float outlineGreen,
      float outlineBlue,
      float outlineAlpha,
      float outlineRadius,
      float softRadius,
      float thickness )
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
   mBaseline(0.0f),
   mBaseColumn(0.0f)
{
   mShader= activeDevice->loadShader("fontoutlines-vert.glsl", "fontoutlines-frag.glsl");
   mParamTexture= activeDevice->getParameterIndex("distanceMap");
   mParamOutlineColor= activeDevice->getParameterIndex("outlineColor");

   mParamSoftRadius= activeDevice->getParameterIndex("aaRadius");
   mParamOutlineRadius= activeDevice->getParameterIndex("outlineRadius");
   mParamThickness= activeDevice->getParameterIndex("threshold");
   mParamSampleOffset= activeDevice->getParameterIndex("sampleOffset");

   Image *image= ImagePool::Instance()->getImage(filename);
   mTexture= TexturePool::Instance()->getTexture(image, TexturePool::Linear | TexturePool::Clamp);
   mScaleU= 1.0f / image->getWidth();
   mScaleV= 1.0f / image->getHeight();
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


void BitmapFont::getOutlineColor(float &r, float &g, float &b, float &a)
{
   r = mOutlineRed;
   g = mOutlineGreen;
   b = mOutlineBlue;
   a = mOutlineAlpha;
}


BitmapFont::Parameter* BitmapFont::getCharParameter(char c) const
{
   if (c>=0)
      return &mDescription[(unsigned char)c];
   else
      return 0;
/*
   int l= 0;
   int h= mCharCount-1;
   int m;

   do {
      m = (l + h) >> 1;
      if (c > mDescription[m].c)
         l= m+1;
      else
         h= m-1;
   } while ((mDescription[m].c != c) && (l <= h));

   return &mDescription[m];
*/
}


float BitmapFont::buildVertices(float size, const char *text, float x, float y, float centerWidth, float centerHeight)
{
   mVertices.clear();

   // remember text position for cursor
   mBaseColumn= x;
   mBaseline= y;

   size*=mSize;

   x -= mRadius * size;
//   y += mRadius * size;


   if (centerWidth >= 0.0f)
   {
      float width= 0.0f;
      int index= 0;
      while (text[index])
      {
         Parameter *param= getCharParameter(text[index]);
         if (param)
            width += (param->space + mSpacing);
         index++;
      }
      width-=mSpacing; // remove last spacing

      x+= (centerWidth - (width+mRadius*2) * size) * 0.5f;
   }

   if (centerHeight >= 0.0f)
   {
      float height= centerHeight;
      Parameter *param= getCharParameter('M');
      if (param)
         height= (param->height - mRadius*2) * size;
      y -= (centerHeight - height) * 0.5f;
   }

   while (*text)
   {
      char c= *text++;
      Parameter *param= getCharParameter(c);

      float xLeft   = x - (param->basecolumn) * size;
      float xRight  = x - (param->basecolumn - param->width) * size;
      float yTop    = y - (param->baseline) * size;
      float yBottom = y - (param->baseline + param->height) * size;

      mVertices.add(
         Vertex(
            xLeft,
            yTop,
            param->x * mScaleU,
            (param->y + param->height) * mScaleV
         )
      );

      mVertices.add(
         Vertex(
            xRight,
            yTop,
            (param->x + param->width) * mScaleU,
            (param->y + param->height) * mScaleV
         )
      );

      mVertices.add(
         Vertex(
            xRight,
            yBottom,
            (param->x + param->width) * mScaleU,
            param->y * mScaleV
         )
      );

      mVertices.add(
         Vertex(
            xLeft,
            yBottom,
            param->x * mScaleU,
            param->y * mScaleV
         )
      );

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
   if (vertices.size()<4)
      return;

   activeDevice->setShader( mShader );

   float sampleOffset= (mVertices[1].u - mVertices[0].u) / (mVertices[1].x - mVertices[0].x);

   glBindTexture(GL_TEXTURE_2D, mTexture);
   activeDevice->bindSampler(mParamTexture, 0);
   activeDevice->setParameter(mParamOutlineColor, Vector4(mOutlineRed, mOutlineGreen, mOutlineBlue, mOutlineAlpha));
   activeDevice->setParameter(mParamSoftRadius, mSoftRadius);
   activeDevice->setParameter(mParamOutlineRadius, mOutlineRadius);
   activeDevice->setParameter(mParamThickness, mThickness);
   activeDevice->setParameter(mParamSampleOffset, sampleOffset * 1.0f);

   glBegin(GL_QUADS);

   for (int i = 0; i < vertices.size(); i++)
   {
      const Vertex& vtx= vertices[i];
      glTexCoord2f( vtx.u, vtx.v );
      glVertex3f( vtx.x, vtx.y, 0.0f );
   }

   glEnd();

   activeDevice->setShader( 0 );
}



unsigned int BitmapFont::getTexture()
{
   return mTexture;
}


const Array<Vertex>& BitmapFont::getVertices() const
{
   return mVertices;
}


void BitmapFont::getCursor(
   float size,
   int cursorPos,
   float& left,
   float& right,
   float& top,
   float& bottom
)
{
   size*=mSize;
   cursorPos*=4;

   if (mVertices.size() < 4)
   {
      // empty string
      Parameter *param= getCharParameter('M');
      left= mBaseColumn;
      right= left +  param->space * size + mRadius*size*2;

      top    = mBaseColumn - (param->baseline) * size;
      bottom = mBaseline - (param->baseline + param->height) * size;
   }
   else
   if (cursorPos >= mVertices.size())
   {
      // cursor at end of text
      Parameter *param= getCharParameter('M');
      cursorPos= mVertices.size()-4;

      left= mVertices[cursorPos+1].x - (mRadius*size*2) + mSpacing*size;
      right= left +  param->space * size + mRadius*size*2;
   }
   else
   {
      left= mVertices[cursorPos].x;
      right= mVertices[cursorPos+1].x;
   }

//   top= mVertices[cursorPos].y;
//   bottom= mVertices[cursorPos+2].y;

   Parameter *param= getCharParameter('M');
   top    = mBaseline - (param->baseline) * size;
   bottom = mBaseline - (param->baseline + param->height) * size;

   top -= mRadius*size;
   bottom += mRadius*size;

   left += mRadius*size;
   right -= mRadius*size;

/*
   left   = mCursorX - param->basecolumn;
   right  = mCursorX - param->basecolumn + param->width  * mSize;
   top    = mYOffset + mCursorY - param->baseline;
   bottom = mYOffset + mCursorY - param->baseline   - param->height * mSize;
*/
}

