#include "shaderpool.h"

#include "gldevice.h"

#include <QGLShaderProgram>
#include <QFile>

QMap<QString, QGLShaderProgram*> ShaderPool::mPrograms;


//-----------------------------------------------------------------------------
/*!
*/
ShaderPool::ShaderPool()
{
}


//-----------------------------------------------------------------------------
/*!
   \param vshFile name of the vertex shader file
   \param pshFile name of the pixel shader file
*/
void ShaderPool::registerShaderFromFile(
   const QString& vshFile,
   const QString& pshFile
)
{
   QGLShaderProgram* program = new QGLShaderProgram();
   bool success = false;

   QGLShaderProgram::hasOpenGLShaderPrograms();

   if (!QFile::exists(vshFile))
   {
      qDebug(
         "ShaderPool::registerShaderFromFile: %s not found",
         qPrintable(vshFile)
      );
   }

   if (!QFile::exists(pshFile))
   {
      qDebug(
         "ShaderPool::registerShaderFromFile: %s not found",
         qPrintable(pshFile)
      );
   }

   if (program->addShaderFromSourceFile(QGLShader::Vertex, vshFile))
   {
      if (!pshFile.isEmpty())
      {
         if (program->addShaderFromSourceFile(QGLShader::Fragment, pshFile))
         {
            success = program->link();
         }
         else
         {
            qDebug(
               "ShaderPool::registerShaderFromFile: addShaderFromSourceFile failed %s",
               qPrintable(program->log())
            );
         }
      }
      else
      {
         success = program->link();
      }
   }
   else
   {
      qDebug(
         "ShaderPool::registerShaderFromFile: addShaderFromSourceFile failed: %s",
         qPrintable(program->log())
      );
   }

   if (success)
   {
      mPrograms.insert(vshFile, program);
   }
   else
   {
      qDebug(
         "ShaderPool::registerShaderFromFile: %s",
         qPrintable(program->log())
      );
   }
}


//-----------------------------------------------------------------------------
/*!
   \param vshFile name of the vertex shader file
   \param pshFile name of the pixel shader file
*/
void ShaderPool::registerShaderFromSource(
   const QString& name,
   const QString& vshData,
   const QString& pshData
)
{
   QGLShaderProgram* program = new QGLShaderProgram();
   bool success = false;

   if (program->addShaderFromSourceCode(QGLShader::Vertex, vshData))
   {
      if (!pshData.isEmpty())
      {
         if (program->addShaderFromSourceCode(QGLShader::Fragment, pshData))
         {
            success = program->link();
         }
      }
   }

   if (success)
   {
      mPrograms.insert(name, program);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param name name of the program
   \return ptr to shader program
*/
QGLShaderProgram* ShaderPool::getProgram(const QString& name)
{
   return mPrograms[name];
}


