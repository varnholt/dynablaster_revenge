#ifndef GLDEVICE_H
#define GLDEVICE_H

#include <QtOpenGL/qgl.h>
#include "framework/glext.h"
#include "renderdevice.h"

// multi texturing
#ifdef WIN32
extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
extern PFNGLACTIVETEXTUREARBPROC    glActiveTexture;
extern PFNGLMULTITEXCOORD2FARBPROC  glMultiTexCoord2f;
extern PFNGLTEXIMAGE3DPROC          glTexImage3D;
extern PFNGLDRAWRANGEELEMENTSPROC   glDrawRangeElements;
#endif

#ifndef Q_OS_MAC

extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;

// vertex buffer objects
extern PFNGLGENBUFFERSARBPROC    glGenBuffers;
extern PFNGLBINDBUFFERARBPROC    glBindBuffer;
extern PFNGLBUFFERDATAARBPROC    glBufferData;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
extern PFNGLMAPBUFFERPROC        glMapBuffer;
extern PFNGLUNMAPBUFFERPROC      glUnmapBuffer;

// shader stuff
extern PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObject;
extern PFNGLSHADERSOURCEARBPROC         glShaderSource;
extern PFNGLCOMPILESHADERARBPROC        glCompileShader;
extern PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObject;
extern PFNGLATTACHOBJECTARBPROC         glAttachObject;
extern PFNGLLINKPROGRAMARBPROC          glLinkProgram;
extern PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObject;
extern PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfv;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv;
extern PFNGLGETINFOLOGARBPROC           glGetInfoLog;
extern PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocation;
extern PFNGLUNIFORM1IARBPROC            glUniform1i;
extern PFNGLUNIFORM1FARBPROC            glUniform1f;
extern PFNGLUNIFORM2FARBPROC            glUniform2f;
extern PFNGLUNIFORM3FARBPROC            glUniform3f;
extern PFNGLUNIFORM4FARBPROC            glUniform4f;
extern PFNGLUNIFORM1FVARBPROC           glUniform1fv;
extern PFNGLUNIFORM2FVARBPROC           glUniform2fv;
extern PFNGLUNIFORM3FVARBPROC           glUniform3fv;
extern PFNGLUNIFORM4FVARBPROC           glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVARBPROC     glUniformMatrix4fv;

// framebuffer objects
extern PFNGLGENFRAMEBUFFERSEXTPROC                  glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC               glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFEREXTPROC                  glBindFramebuffer;
extern PFNGLGENRENDERBUFFERSEXTPROC                 glGenRenderbuffers;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv;
extern PFNGLDELETERENDERBUFFERSEXTPROC              glDeleteRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC              glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC          glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC             glFramebufferTexture2D;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC           glCheckFramebufferStatus;
extern PFNGLBINDRENDERBUFFEREXTPROC                 glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC   glRenderbufferStorageMultisample;
extern PFNGLBLITFRAMEBUFFERPROC                     glBlitFramebuffer;
extern PFNGLGETINTERNALFORMATIVPROC                 glGetInternalformativ;
#endif

class GLDevice : public RenderDevice
{
private:
   int        mTempVertex;
   int        mTempIndex;
   bool       initExtensions();
   unsigned int mTexture;

   std::map<unsigned int, GLhandleARB>  mShaderTable;
   unsigned int                         mShaderAllocIndex;

public:
   bool init();

   static QString getErrorText();

   void resize(int x, int y);
   void setViewPort(int x, int y, int width, int height);
   void getViewPort(int *x, int *y, int *width, int *height);
   void convertFromViewPort(int *x, int *y, int targetWidth, int targetHeight);
   void clear();

   void setPerspective(float fov, float aspect, float zNear= 1.0f, float zFar= 5000.0f);
   void setCamera(const Matrix& m, float fov, float zNear = 1.0f, float zFar = 1000.0f, bool perspective = true);
   void push(const Matrix& mat);
   void pop();
   
   unsigned int createVertexBuffer(int size, bool dynamic=false);
   void allocateVertexBuffer(unsigned int buffer, int size, bool dyn=false);
   void* lockVertexBuffer(unsigned int handle, int size=0);
   void  unlockVertexBuffer(unsigned int buf);
   
   unsigned int createIndexBuffer(int size, bool dyn=false);
   void allocateIndexBuffer(unsigned int buf, int size, bool dyn=false);
   void* lockIndexBuffer(unsigned int handle, int size=0);
   void  unlockIndexBuffer(unsigned int buf);
   void  setCulling(bool state);
   void  setMaterial(const Vector& amb, const Vector& dif, const Vector& spc, float shine);
   
   // internals
   void drawLine(Vector *v);

   unsigned int createTexture(void *data, int x, int y, int flags=3);
   void deleteTexture(unsigned int textureId);
   void updateTexture(void *data, int x, int y, int flags);

   unsigned int uploadTexture1D(void *data, int x, int flags=0);
   unsigned int loadShader(const char *vname, const char *pname);
   void setShader(unsigned int shader);
   int getParameterIndex(const char *name);
   void bindSampler(int pos, int unit);
   void setParameter(int pos, float* data, int size);
   void setParameter(int pos, const Vector& vector);
   void setParameter(int pos, const Vector2& vector);
   void setParameter(int pos, const Vector4& vector);
   void setParameter(int pos, const Matrix& mat);
   void setParameter(int pos, const Matrix* mat, int count);
   void setParameter(int pos, float f);

   unsigned int createBuffer();
   void deleteBuffer(unsigned int buffer);

   void setSwapInterval(int interval);

private:
   bool checkShaderError(const char *filename, GLhandleARB shader);

   int mViewPortX;
   int mViewPortY;
   int mViewPortWidth;
   int mViewPortHeight;
   unsigned int mTotalTextureMemory;
   unsigned int mTotalVertexMemory;
};

#endif
