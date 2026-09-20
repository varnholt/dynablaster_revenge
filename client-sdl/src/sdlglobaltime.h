#pragma once

#include "framework/globaltime.h"

/// \brief SDL-backed GlobalTime provider for the standalone SDL entry point.
///
/// The original game got GlobalTime from MainDrawable (a QGLWidget subclass, see
/// client/src/framework/maindrawable.h/.cpp) - one of the many things tied to the QGLWidget
/// window itself. Since that widget isn't part of this port, materials that need a running
/// clock (DisplacementMaterial's flag-wave animation) need a concrete GlobalTime instance from
/// somewhere; this is that instance for main.cpp.
class SdlGlobalTime : public GlobalTime
{
public:
   void update();
   float getTime() const override;

private:
   float mTime = 0.0f;
};
