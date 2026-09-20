#include "defaultshader.h"
#include "framework/gldevice.h"

namespace
{
unsigned int gShader = 0;
int gAlphaParam = -1;

unsigned int gBlitShader = 0;
int gBlitAlphaParam = -1;
}

unsigned int getDefaultMenuShader()
{
   if (gShader == 0)
   {
      gShader = activeDevice->loadShader(
         "data/shaders/texalpha-vert.glsl",
         "data/shaders/texalpha-frag.glsl"
      );
      gAlphaParam = activeDevice->getParameterIndex("alpha");
   }

   return gShader;
}

int getDefaultMenuShaderAlphaParam()
{
   getDefaultMenuShader();
   return gAlphaParam;
}

unsigned int getFramebufferBlitShader()
{
   if (gBlitShader == 0)
   {
      gBlitShader = activeDevice->loadShader(
         "data/shaders/texalphaignore-vert.glsl",
         "data/shaders/texalphaignore-frag.glsl"
      );
      gBlitAlphaParam = activeDevice->getParameterIndex("alpha");
   }

   return gBlitShader;
}

int getFramebufferBlitShaderAlphaParam()
{
   getFramebufferBlitShader();
   return gBlitAlphaParam;
}
