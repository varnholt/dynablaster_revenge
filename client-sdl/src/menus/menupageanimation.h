#pragma once

#include "signal.h"

/// \brief GLES3 port of client/src/menus/menupageanimation.cpp.
/// setShaderProgram()/getShaderProgram() (QGLShaderProgram*) are dropped - dead even upstream:
/// the one derived class that touched mProgram (MenuPageFadeAnimation) had that code already
/// commented out, in favor of the alpha uniform MenuDrawable sets directly via GLDevice.
class MenuPageAnimation
{
public:
   MenuPageAnimation();

   virtual ~MenuPageAnimation();

   virtual void initialize();

   virtual void start() = 0;

   virtual void animate() = 0;

   Signal<> stoppedSignal;
};
