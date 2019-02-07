#ifndef STARFIELDBACKGROUND_H
#define STARFIELDBACKGROUND_H

#include "render/texture.h"


class StarFieldBackground
{
   public:

      StarFieldBackground();

      void begin();

      void end();

      void draw();

   protected:

      Texture mTexture;

};

#endif // STARFIELDBACKGROUND_H
