#include "menupageanimation.h"

#include <QGLShaderProgram>

MenuPageAnimation::MenuPageAnimation()
   : QObject(),
     mProgram(0)
{
}

MenuPageAnimation::~MenuPageAnimation()
{
   mProgram = 0;
}


void MenuPageAnimation::setShaderProgram(QGLShaderProgram* program)
{
   mProgram = program;
}


void MenuPageAnimation::initialize()
{
}



QGLShaderProgram* MenuPageAnimation::getShaderProgram()
{
   return mProgram;
}
