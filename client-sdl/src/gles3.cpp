#include "gles3.h"

#include <SDL3/SDL.h>

PFNGLVIEWPORTPROC glViewport;
PFNGLCLEARCOLORPROC glClearColor;
PFNGLCLEARPROC glClear;
PFNGLENABLEPROC glEnable;
PFNGLDISABLEPROC glDisable;
PFNGLBLENDFUNCPROC glBlendFunc;
PFNGLPIXELSTOREIPROC glPixelStorei;
PFNGLREADPIXELSPROC glReadPixels;
PFNGLSCISSORPROC glScissor;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;

PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLDEPTHMASKPROC glDepthMask;

PFNGLGENTEXTURESPROC glGenTextures;
PFNGLBINDTEXTUREPROC glBindTexture;
PFNGLTEXIMAGE2DPROC glTexImage2D;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLTEXPARAMETERIPROC glTexParameteri;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D;

PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

PFNGLDRAWARRAYSPROC glDrawArrays;
PFNGLDRAWELEMENTSPROC glDrawElements;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;

PFNGLDEPTHFUNCPROC glDepthFunc;
PFNGLCLEARDEPTHFPROC glClearDepthf;
PFNGLCLEARSTENCILPROC glClearStencil;
PFNGLTEXPARAMETERFPROC glTexParameterf;
PFNGLGETERRORPROC glGetError;
PFNGLGETSTRINGPROC glGetString;

PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;

namespace
{
template <typename T>
bool load(T& fn, const char* name)
{
   fn = reinterpret_cast<T>(SDL_GL_GetProcAddress(name));
   return fn != nullptr;
}
}  // namespace

bool loadGles3Functions()
{
   bool ok = true;

   ok &= load(glViewport, "glViewport");
   ok &= load(glClearColor, "glClearColor");
   ok &= load(glClear, "glClear");
   ok &= load(glEnable, "glEnable");
   ok &= load(glDisable, "glDisable");
   ok &= load(glBlendFunc, "glBlendFunc");
   ok &= load(glPixelStorei, "glPixelStorei");
   ok &= load(glReadPixels, "glReadPixels");
   ok &= load(glScissor, "glScissor");

   ok &= load(glCreateShader, "glCreateShader");
   ok &= load(glShaderSource, "glShaderSource");
   ok &= load(glCompileShader, "glCompileShader");
   ok &= load(glGetShaderiv, "glGetShaderiv");
   ok &= load(glGetShaderInfoLog, "glGetShaderInfoLog");
   ok &= load(glDeleteShader, "glDeleteShader");

   ok &= load(glCreateProgram, "glCreateProgram");
   ok &= load(glAttachShader, "glAttachShader");
   ok &= load(glLinkProgram, "glLinkProgram");
   ok &= load(glGetProgramiv, "glGetProgramiv");
   ok &= load(glGetProgramInfoLog, "glGetProgramInfoLog");
   ok &= load(glUseProgram, "glUseProgram");
   ok &= load(glDeleteProgram, "glDeleteProgram");

   ok &= load(glGenBuffers, "glGenBuffers");
   ok &= load(glBindBuffer, "glBindBuffer");
   ok &= load(glBufferData, "glBufferData");
   ok &= load(glDeleteBuffers, "glDeleteBuffers");

   ok &= load(glGenVertexArrays, "glGenVertexArrays");
   ok &= load(glBindVertexArray, "glBindVertexArray");
   ok &= load(glDeleteVertexArrays, "glDeleteVertexArrays");
   ok &= load(glVertexAttribPointer, "glVertexAttribPointer");
   ok &= load(glEnableVertexAttribArray, "glEnableVertexAttribArray");
   ok &= load(glDisableVertexAttribArray, "glDisableVertexAttribArray");
   ok &= load(glDepthMask, "glDepthMask");

   ok &= load(glGenTextures, "glGenTextures");
   ok &= load(glBindTexture, "glBindTexture");
   ok &= load(glTexImage2D, "glTexImage2D");
   ok &= load(glTexImage3D, "glTexImage3D");
   ok &= load(glTexParameteri, "glTexParameteri");
   ok &= load(glDeleteTextures, "glDeleteTextures");
   ok &= load(glActiveTexture, "glActiveTexture");
   ok &= load(glCopyTexImage2D, "glCopyTexImage2D");

   ok &= load(glGetUniformLocation, "glGetUniformLocation");
   ok &= load(glUniform1i, "glUniform1i");
   ok &= load(glUniform1f, "glUniform1f");
   ok &= load(glUniform1fv, "glUniform1fv");
   ok &= load(glUniform2fv, "glUniform2fv");
   ok &= load(glUniform3fv, "glUniform3fv");
   ok &= load(glUniform4fv, "glUniform4fv");
   ok &= load(glUniformMatrix4fv, "glUniformMatrix4fv");

   ok &= load(glDrawArrays, "glDrawArrays");
   ok &= load(glDrawElements, "glDrawElements");
   ok &= load(glGetAttribLocation, "glGetAttribLocation");

   ok &= load(glDepthFunc, "glDepthFunc");
   ok &= load(glClearDepthf, "glClearDepthf");
   ok &= load(glClearStencil, "glClearStencil");
   ok &= load(glTexParameterf, "glTexParameterf");
   ok &= load(glGetError, "glGetError");
   ok &= load(glGetString, "glGetString");

   ok &= load(glMapBufferRange, "glMapBufferRange");
   ok &= load(glUnmapBuffer, "glUnmapBuffer");

   ok &= load(glGenFramebuffers, "glGenFramebuffers");
   ok &= load(glBindFramebuffer, "glBindFramebuffer");
   ok &= load(glDeleteFramebuffers, "glDeleteFramebuffers");
   ok &= load(glFramebufferTexture2D, "glFramebufferTexture2D");
   ok &= load(glCheckFramebufferStatus, "glCheckFramebufferStatus");
   ok &= load(glGenRenderbuffers, "glGenRenderbuffers");
   ok &= load(glBindRenderbuffer, "glBindRenderbuffer");
   ok &= load(glDeleteRenderbuffers, "glDeleteRenderbuffers");
   ok &= load(glRenderbufferStorage, "glRenderbufferStorage");
   ok &= load(glFramebufferRenderbuffer, "glFramebufferRenderbuffer");

   return ok;
}
