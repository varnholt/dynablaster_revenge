#pragma once

class Vector4;

class DuplicateAlpha
{

public:

   DuplicateAlpha();

   void process(unsigned int texture, const Vector4& color);

private:
   unsigned int mShader;
   int mColorParam;
};
