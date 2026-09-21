#pragma once

#include <QObject>

/// \brief GLES3 port of client/src/menus/menupageanimation.cpp.
/// setShaderProgram()/getShaderProgram() (QGLShaderProgram*) are dropped - dead even upstream:
/// the one derived class that touched mProgram (MenuPageFadeAnimation) had that code already
/// commented out, in favor of the alpha uniform MenuDrawable sets directly via GLDevice.
class MenuPageAnimation : public QObject
{
   Q_OBJECT

public:
   MenuPageAnimation();

   ~MenuPageAnimation();

   virtual void initialize();

public slots:

   virtual void start() = 0;

   virtual void animate() = 0;

signals:

   void stopped();
};
