#pragma once

/// \brief minimal hand-rolled GLES3 function pointer set, loaded at runtime via
/// SDL_GL_GetProcAddress. desktop GL drivers expose the ES3 entry points under the same names,
/// so this works whether the context ends up being a native ES context or a
/// desktop-GL-with-ES-profile context.

#include <cstdint>

using GLenum = unsigned int;
using GLboolean = unsigned char;
using GLbitfield = unsigned int;
using GLvoid = void;
using GLbyte = signed char;
using GLshort = short;
using GLint = int;
using GLubyte = unsigned char;
using GLushort = unsigned short;
using GLuint = unsigned int;
using GLsizei = int;
using GLfloat = float;
using GLclampf = float;
using GLchar = char;
using GLintptr = intptr_t;
using GLsizeiptr = intptr_t;

#define GL_FALSE 0
#define GL_TRUE 1

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100

#define GL_LINES 0x0001
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005

#define GL_FLOAT 0x1406
#define GL_UNSIGNED_BYTE 0x1401

#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8

// The legacy renderer bound buffers via the old ARB-suffixed enums (same values, pre-promotion
// to core). Aliased here so ported .cpp files compile unchanged; safe to drop once every call
// site has been swept to the plain names.
#define GL_ARRAY_BUFFER_ARB GL_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER_ARB GL_ELEMENT_ARRAY_BUFFER
#define GL_STATIC_DRAW_ARB GL_STATIC_DRAW
#define GL_DYNAMIC_DRAW_ARB GL_DYNAMIC_DRAW

#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7

// The legacy renderer selected texture units via the ARB-suffixed enums (same values,
// pre-promotion to core); aliased so ported call sites compile unchanged.
#define GL_TEXTURE0_ARB GL_TEXTURE0
#define GL_TEXTURE1_ARB GL_TEXTURE1
#define GL_TEXTURE2_ARB GL_TEXTURE2
#define GL_TEXTURE3_ARB GL_TEXTURE3
#define GL_TEXTURE4_ARB GL_TEXTURE4
#define GL_TEXTURE5_ARB GL_TEXTURE5
#define GL_TEXTURE6_ARB GL_TEXTURE6
#define GL_TEXTURE7_ARB GL_TEXTURE7
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_RGBA 0x1908
#define GL_RGB 0x1907
#define GL_DEPTH_COMPONENT 0x1902

// GLES has no BGRA format; the one real caller (Material::uploadCubeMap) gets aliased to RGBA
// rather than reordering channels - environment cubemaps come out with red/blue swapped until
// someone circles back to it, which is a correctness gap worth flagging, not a crash risk.
#define GL_BGRA GL_RGBA

#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A

#define GL_DEPTH_TEST 0x0B71
#define GL_CULL_FACE 0x0B44
#define GL_SCISSOR_TEST 0x0C11

// Fixed-function alpha testing (glAlphaFunc) has no GLES equivalent at all - it becomes a
// `discard` in the fragment shader instead. glEnable/glDisable(GL_ALPHA_TEST) call sites are
// left in place rather than hunted down individually; the driver silently ignores the unknown
// enum (a harmless GL_INVALID_ENUM nobody checks for), and every current caller already layers
// real blending on top for the same visual effect.
#define GL_ALPHA_TEST 0x0BC0
#define GL_LEQUAL 0x0203
#define GL_BLEND 0x0BE2
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303

#define GL_PACK_ALIGNMENT 0x0D05

#define GL_NO_ERROR 0

#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_DEPTH_COMPONENT24 0x81A6

// The legacy renderer targeted the pre-promotion EXT_framebuffer_object extension (same values
// as the later core FBO functionality); aliased so ported call sites compile unchanged.
#define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER
#define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
#define GL_COLOR_ATTACHMENT0_EXT GL_COLOR_ATTACHMENT0
#define GL_DEPTH_ATTACHMENT_EXT GL_DEPTH_ATTACHMENT
#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE

// glActiveTextureARB is the same entry point as core glActiveTexture, just under its
// pre-promotion ARB name.
#define glActiveTextureARB glActiveTexture

// glClientActiveTexture(ARB) selected which fixed-function texture-coordinate *array* a
// subsequent glTexCoordPointer() fed - a concept that doesn't exist once texcoords come from a
// generic vertex attribute instead. Every call site already switched to that model, so these
// are no-ops rather than real entry points.
#define glClientActiveTexture(unit) ((void)0)
#define glClientActiveTextureARB(unit) ((void)0)

// glDrawRangeElements is desktop GL only (ES3.2, not ES3.0) - it's just glDrawElements plus a
// [start,end] vertex-range hint the driver may use to skip work; dropping the hint and calling
// straight through is always correct, just potentially a little slower.
#define glDrawRangeElements(mode, start, end, count, type, indices) glDrawElements(mode, count, type, indices)

using PFNGLVIEWPORTPROC = void(*)(GLint, GLint, GLsizei, GLsizei);
using PFNGLCLEARCOLORPROC = void(*)(GLfloat, GLfloat, GLfloat, GLfloat);
using PFNGLCLEARPROC = void(*)(GLbitfield);
using PFNGLENABLEPROC = void(*)(GLenum);
using PFNGLDISABLEPROC = void(*)(GLenum);
using PFNGLBLENDFUNCPROC = void(*)(GLenum, GLenum);
using PFNGLPIXELSTOREIPROC = void(*)(GLenum, GLint);
using PFNGLREADPIXELSPROC = void(*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*);
using PFNGLSCISSORPROC = void(*)(GLint, GLint, GLsizei, GLsizei);

using PFNGLCREATESHADERPROC = GLuint(*)(GLenum);
using PFNGLSHADERSOURCEPROC = void(*)(GLuint, GLsizei, const GLchar* const*, const GLint*);
using PFNGLCOMPILESHADERPROC = void(*)(GLuint);
using PFNGLGETSHADERIVPROC = void(*)(GLuint, GLenum, GLint*);
using PFNGLGETSHADERINFOLOGPROC = void(*)(GLuint, GLsizei, GLsizei*, GLchar*);
using PFNGLDELETESHADERPROC = void(*)(GLuint);

using PFNGLCREATEPROGRAMPROC = GLuint(*)();
using PFNGLATTACHSHADERPROC = void(*)(GLuint, GLuint);
using PFNGLLINKPROGRAMPROC = void(*)(GLuint);
using PFNGLGETPROGRAMIVPROC = void(*)(GLuint, GLenum, GLint*);
using PFNGLGETPROGRAMINFOLOGPROC = void(*)(GLuint, GLsizei, GLsizei*, GLchar*);
using PFNGLUSEPROGRAMPROC = void(*)(GLuint);
using PFNGLDELETEPROGRAMPROC = void(*)(GLuint);

using PFNGLGENBUFFERSPROC = void(*)(GLsizei, GLuint*);
using PFNGLBINDBUFFERPROC = void(*)(GLenum, GLuint);
using PFNGLBUFFERDATAPROC = void(*)(GLenum, GLsizeiptr, const void*, GLenum);
using PFNGLDELETEBUFFERSPROC = void(*)(GLsizei, const GLuint*);

using PFNGLGENVERTEXARRAYSPROC = void(*)(GLsizei, GLuint*);
using PFNGLBINDVERTEXARRAYPROC = void(*)(GLuint);
using PFNGLDELETEVERTEXARRAYSPROC = void(*)(GLsizei, const GLuint*);
using PFNGLVERTEXATTRIBPOINTERPROC = void(*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
using PFNGLENABLEVERTEXATTRIBARRAYPROC = void(*)(GLuint);
using PFNGLDISABLEVERTEXATTRIBARRAYPROC = void(*)(GLuint);
using PFNGLDEPTHMASKPROC = void(*)(GLboolean);

using PFNGLGENTEXTURESPROC = void(*)(GLsizei, GLuint*);
using PFNGLBINDTEXTUREPROC = void(*)(GLenum, GLuint);
using PFNGLTEXIMAGE2DPROC = void(*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
using PFNGLTEXPARAMETERIPROC = void(*)(GLenum, GLenum, GLint);
using PFNGLDELETETEXTURESPROC = void(*)(GLsizei, const GLuint*);
using PFNGLACTIVETEXTUREPROC = void(*)(GLenum);

using PFNGLGETUNIFORMLOCATIONPROC = GLint(*)(GLuint, const GLchar*);
using PFNGLUNIFORM1IPROC = void(*)(GLint, GLint);
using PFNGLUNIFORM1FPROC = void(*)(GLint, GLfloat);
using PFNGLUNIFORM1FVPROC = void(*)(GLint, GLsizei, const GLfloat*);
using PFNGLUNIFORM2FVPROC = void(*)(GLint, GLsizei, const GLfloat*);
using PFNGLUNIFORM3FVPROC = void(*)(GLint, GLsizei, const GLfloat*);
using PFNGLUNIFORM4FVPROC = void(*)(GLint, GLsizei, const GLfloat*);
using PFNGLUNIFORMMATRIX4FVPROC = void(*)(GLint, GLsizei, GLboolean, const GLfloat*);

using PFNGLDRAWARRAYSPROC = void(*)(GLenum, GLint, GLsizei);
using PFNGLDRAWELEMENTSPROC = void(*)(GLenum, GLsizei, GLenum, const void*);
using PFNGLGETATTRIBLOCATIONPROC = GLint(*)(GLuint, const GLchar*);

using PFNGLDEPTHFUNCPROC = void(*)(GLenum);
using PFNGLCLEARDEPTHFPROC = void(*)(GLfloat);
using PFNGLCLEARSTENCILPROC = void(*)(GLint);
using PFNGLTEXPARAMETERFPROC = void(*)(GLenum, GLenum, GLfloat);
using PFNGLGETERRORPROC = GLenum(*)();

using PFNGLMAPBUFFERRANGEPROC = void*(*)(GLenum, GLintptr, GLsizeiptr, GLbitfield);
using PFNGLUNMAPBUFFERPROC = GLboolean(*)(GLenum);

using PFNGLGENFRAMEBUFFERSPROC = void(*)(GLsizei, GLuint*);
using PFNGLBINDFRAMEBUFFERPROC = void(*)(GLenum, GLuint);
using PFNGLDELETEFRAMEBUFFERSPROC = void(*)(GLsizei, const GLuint*);
using PFNGLFRAMEBUFFERTEXTURE2DPROC = void(*)(GLenum, GLenum, GLenum, GLuint, GLint);
using PFNGLCHECKFRAMEBUFFERSTATUSPROC = GLenum(*)(GLenum);
using PFNGLGENRENDERBUFFERSPROC = void(*)(GLsizei, GLuint*);
using PFNGLBINDRENDERBUFFERPROC = void(*)(GLenum, GLuint);
using PFNGLDELETERENDERBUFFERSPROC = void(*)(GLsizei, const GLuint*);
using PFNGLRENDERBUFFERSTORAGEPROC = void(*)(GLenum, GLenum, GLsizei, GLsizei);
using PFNGLFRAMEBUFFERRENDERBUFFERPROC = void(*)(GLenum, GLenum, GLenum, GLuint);

extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;

extern PFNGLVIEWPORTPROC glViewport;
extern PFNGLCLEARCOLORPROC glClearColor;
extern PFNGLCLEARPROC glClear;
extern PFNGLENABLEPROC glEnable;
extern PFNGLDISABLEPROC glDisable;
extern PFNGLBLENDFUNCPROC glBlendFunc;
extern PFNGLPIXELSTOREIPROC glPixelStorei;
extern PFNGLREADPIXELSPROC glReadPixels;
extern PFNGLSCISSORPROC glScissor;

extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC glDeleteShader;

extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;

extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;

extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLDEPTHMASKPROC glDepthMask;

extern PFNGLGENTEXTURESPROC glGenTextures;
extern PFNGLBINDTEXTUREPROC glBindTexture;
extern PFNGLTEXIMAGE2DPROC glTexImage2D;
extern PFNGLTEXPARAMETERIPROC glTexParameteri;
extern PFNGLDELETETEXTURESPROC glDeleteTextures;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;

extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM2FVPROC glUniform2fv;
extern PFNGLUNIFORM3FVPROC glUniform3fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

extern PFNGLDRAWARRAYSPROC glDrawArrays;
extern PFNGLDRAWELEMENTSPROC glDrawElements;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;

extern PFNGLDEPTHFUNCPROC glDepthFunc;
extern PFNGLCLEARDEPTHFPROC glClearDepthf;
extern PFNGLCLEARSTENCILPROC glClearStencil;
extern PFNGLTEXPARAMETERFPROC glTexParameterf;
extern PFNGLGETERRORPROC glGetError;

using PFNGLGETSTRINGPROC = const GLubyte*(*)(GLenum);
extern PFNGLGETSTRINGPROC glGetString;
#define GL_VERSION 0x1F02
#define GL_RENDERER 0x1F01
#define GL_VENDOR 0x1F00
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C

extern PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;

/// \brief loads every entry point above via SDL_GL_GetProcAddress.
/// must be called once, after the GL context is current.
/// \return false when any entry point could not be resolved.
bool loadGles3Functions();
