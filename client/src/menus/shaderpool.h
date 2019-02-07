#ifndef SHADERPOOL_H
#define SHADERPOOL_H

#include <QMap>
#include <QString>

class QGLShaderProgram;
class QGLContext;
class RenderDevice;

class ShaderPool
{
   public:

      ShaderPool();

      static void registerShaderFromFile(
         const QString& vshName,
         const QString& pshName = QString()
      );

      static void registerShaderFromSource(
         const QString& name,
         const QString& vshData,
         const QString& pshData = QString()
      );

      static QGLShaderProgram* getProgram(const QString& name);


   private:

      static QMap<QString, QGLShaderProgram*> mPrograms;

};

#endif // SHADERPOOL_H
