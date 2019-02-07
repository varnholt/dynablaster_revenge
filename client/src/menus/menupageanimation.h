#ifndef MENUPAGEANIMATION_H
#define MENUPAGEANIMATION_H

#include <QObject>

class QGLContext;
class QGLShaderProgram;

class MenuPageAnimation : public QObject
{
   Q_OBJECT

   public:

      MenuPageAnimation();

      ~MenuPageAnimation();

      void setShaderProgram(QGLShaderProgram* program);

      QGLShaderProgram* getShaderProgram();

      virtual void initialize();


   public slots:

      virtual void start() = 0;

      virtual void animate() = 0;


   signals:

      void stopped();


   protected:

      QGLShaderProgram* mProgram;


};

#endif // MENUPAGEANIMATION_H
