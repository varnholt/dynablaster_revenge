#ifndef BITMAPFONT_H
#define BITMAPFONT_H

// framework
#include "tools/array.h"
#include "render/texture.h"
#include "vertex.h"

class BitmapFont
{
public:

   struct Parameter
   {
      char c;
      unsigned short x;
      unsigned short y;
      unsigned short width;
      unsigned short height;
      short basecolumn;
      short baseline;
      unsigned short space;
   };

   BitmapFont(
         const char *filename,
         Parameter* description,
         float size = 1.0f,
         float spacing= 0.0f,
         float distanceRadius= 0.0f,
         float outlineRed = 0.0f,
         float outlineGreen = 0.0f,
         float outlineBlue = 0.0f,
         float outlineAlpha = 1.0f,
         float outlineRadius = 0.15f,
         float softRadius = 0.06f,
         float thickness = 0.0f
   );

   ~BitmapFont();

   bool isCharAvailable(char c) const;
   void setOutlineColor(float r, float g, float b, float a);
   void getOutlineColor(float& r, float& g, float& b, float& a);
   float buildVertices(float size, const char* text, float x, float y, float centerWidth= -1.0f, float centerHeight = -1.0f);
   void draw(const Array<Vertex>& vertices);
   void draw();
   unsigned int getTexture();
   const Array<Vertex>& getVertices() const;

   //! getter cursor dimensions
   void getCursor(
      float scale,
      int cursorPosition,
      float& left,
      float& right,
      float& top,
      float& bottom
   );

   Parameter* getCharParameter(char c) const;

private:

   unsigned int   mShader;
   Texture        mTexture;
   int            mParamTexture;
   int            mParamOutlineColor;
   int            mParamSoftRadius;
   int            mParamOutlineRadius;
   int            mParamThickness;
   int            mParamSampleOffset;

   Parameter      *mDescription;
   float          mSize;
   float          mSpacing;
   float          mRadius;
   float          mScaleU;
   float          mScaleV;
   float          mOutlineRed;
   float          mOutlineGreen;
   float          mOutlineBlue;
   float          mOutlineAlpha;
   float          mOutlineRadius;
   float          mSoftRadius;
   float          mThickness;

   float          mBaseline;
   float          mBaseColumn;
   mutable Array<Vertex> mVertices;
};

#endif
