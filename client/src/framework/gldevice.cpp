#include "stdafx.h"
#include "gldevice.h"
#include "tools/filestream.h"
#include "tools/profiling.h"

#ifndef Q_OS_MAC
#include <GL/glu.h>
#else
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#define GL_MAX_IMAGE_UNITS_EXT 0x8F38
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define glGetObjectParameteriv(x,y,z)  glGetObjectParameterivARB(x,y,z)
#define glGetInfoLog(w, x, y, z) glGetInfoLogARB(w, x, y, z)
#define glCreateProgramObject() glCreateProgramObjectARB()
#define glCreateShaderObject(x) glCreateShaderObjectARB(x)
#define glShaderSource(v, x, y, z) glShaderSourceARB(v, x, y, z)
#define glCompileShader(x) glCompileShaderARB(x)
#define glUseProgramObject(x) glUseProgramObjectARB(x)
#define glAttachObject(x,y) glAttachObjectARB(x,y)
#define glLinkProgram(x) glLinkProgramARB(x)
#define glGetUniformLocation(x, y) glGetUniformLocationARB(x,y)
#endif


#ifdef Q_OS_LINUX
#include "GL/glx.h"
#endif

#ifdef QT_VERSION
   #define PRINTF qDebug
#else
   #define PRINTF printf
#endif

typedef GLhandleARB ShaderProgramHandle;

// multi-texture
#ifdef WIN32
PFNGLACTIVETEXTUREARBPROC    glActiveTexture= NULL;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture= NULL;
PFNGLMULTITEXCOORD2FARBPROC  glMultiTexCoord2f= NULL;
PFNGLTEXIMAGE3DPROC          glTexImage3D = NULL;
PFNGLDRAWRANGEELEMENTSPROC   glDrawRangeElements= NULL;
#endif

#ifndef Q_OS_MAC
// blending
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;

// vertex buffer objects
PFNGLGENBUFFERSARBPROC    glGenBuffers = NULL;
PFNGLBINDBUFFERPROC       glBindBuffer = NULL;
PFNGLBUFFERDATAPROC       glBufferData = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffers = NULL;
PFNGLMAPBUFFERPROC        glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC      glUnmapBuffer= NULL;

// shader stuff
PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObject = NULL;
PFNGLSHADERSOURCEARBPROC         glShaderSource = NULL;
PFNGLCOMPILESHADERARBPROC        glCompileShader = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObject = NULL;
PFNGLATTACHOBJECTARBPROC         glAttachObject = NULL;
PFNGLLINKPROGRAMARBPROC          glLinkProgram = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObject = NULL;
PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfv = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv = NULL;
PFNGLGETINFOLOGARBPROC           glGetInfoLog= NULL;
PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocation = NULL;
PFNGLUNIFORM1IARBPROC            glUniform1i = NULL;
PFNGLUNIFORM1FARBPROC            glUniform1f = NULL;
PFNGLUNIFORM2FARBPROC            glUniform2f = NULL;
PFNGLUNIFORM3FARBPROC            glUniform3f = NULL;
PFNGLUNIFORM4FARBPROC            glUniform4f = NULL;
PFNGLUNIFORM1FVARBPROC           glUniform1fv = NULL;
PFNGLUNIFORM2FVARBPROC           glUniform2fv = NULL;
PFNGLUNIFORM3FVARBPROC           glUniform3fv = NULL;
PFNGLUNIFORM4FVARBPROC           glUniform4fv = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC     glUniformMatrix4fv = NULL;

// framebuffer objects
PFNGLGENFRAMEBUFFERSEXTPROC                  glGenFramebuffers = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC               glDeleteFramebuffers = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC                  glBindFramebuffer = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv = NULL;
PFNGLGENRENDERBUFFERSEXTPROC                 glGenRenderbuffers = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC              glDeleteRenderbuffers = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC              glRenderbufferStorage = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC          glFramebufferRenderbuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC             glFramebufferTexture2D = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC           glCheckFramebufferStatus = NULL;
PFNGLBINDRENDERBUFFEREXTPROC                 glBindRenderbuffer = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC   glRenderbufferStorageMultisample = NULL;
PFNGLBLITFRAMEBUFFERPROC                     glBlitFramebuffer = NULL;
PFNGLGETINTERNALFORMATIVPROC                 glGetInternalformativ = NULL;

PFNGLACTIVESTENCILFACEEXTPROC    glActiveStencilFace = NULL;
PFNGLSTENCILOPSEPARATEATIPROC    glStencilOpSeparateATI= NULL;
PFNGLSTENCILFUNCSEPARATEATIPROC  glStencilFuncSeparateATI= NULL;
#endif
static bool missingExtensions= false;

QString extTest;

QString GLDevice::getErrorText()
{
    return extTest;
}

void* getProcAddress(const char *name, bool optional = false)
{
   void *ptr= 0;
#ifdef WIN32
   ptr= (void*)wglGetProcAddress(name);
#endif

#ifdef Q_OS_LINUX
   ptr= (void*)glXGetProcAddressARB((GLubyte*)name);
#endif

#ifdef Q_OS_MAC
   ptr=(void*)glutGetProcAddress(name);
#endif


   if (!ptr)
   {
      PRINTF("%s does not exist!\n", name);

      int len= strlen(name);
      if (len>3)
      {
        char temp[256];
        if (strcmp(name+len-3, "ARB") == 0)
        {
           memcpy(temp, name, len-3);
           temp[len-3]= 0;
           memcpy(temp + len-3, "EXT", 4);
           return getProcAddress(temp, optional);
        }
        else if (strcmp(name+len-3, "EXT") == 0)
        {
           memcpy(temp, name, len-3);
           temp[len-3]= 0;
           return getProcAddress(temp, optional);
        }
        else
        {
           if (!optional)
           {
              missingExtensions= true;
              extTest += QString("'%1' does not exist!\n").arg( QString(name) );
           }
        }
      }

   }

   return ptr;
}

void GLDevice::setSwapInterval(int interval)
{
#ifdef WIN32
   typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);
   PFNWGLSWAPINTERVALEXTPROC wglSwapInterval= (PFNWGLSWAPINTERVALEXTPROC)getProcAddress("wglSwapIntervalEXT");
   if (wglSwapInterval)
      wglSwapInterval(interval);
#endif

#ifdef Q_OS_LINUX
/*
   typedef int (*PFNGLXSWAPINTERVALEXTPROC)(Display* dpy, GLXDrawable drawable, int interval);
   PFNGLXSWAPINTERVALEXTPROC glxSwapInterval = (PFNGLXSWAPINTERVALEXTPROC)getProcAddress("glXSwapIntervalEXT");
   if (glxSwapInterval)
      glxSwapInterval(mDisplay, mWin, 1);
*/
#endif

// TODO: macOS
}

bool testFrameBuffer()
{
   bool result= true;
   unsigned int target= 0;
   unsigned int testColor= 0;
   unsigned int testDepth= 0;
   int width= 640;
   int height= 480;

   glGenFramebuffers(1, &target);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, target);

   // create texture
   glGenTextures(1, &testColor);
   glBindTexture(GL_TEXTURE_2D, testColor);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // _MIPMAP_LINEAR
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   glBindTexture(GL_TEXTURE_2D, 0);


   // create depth texture
   glGenTextures(1, &testDepth);
   glBindTexture(GL_TEXTURE_2D, testDepth);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // _MIPMAP_LINEAR
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, testColor, 0);
   glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, testDepth, 0);

   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
      result= false;

   if (target)
      glDeleteFramebuffers(1, &target);

   if (testDepth)
      glDeleteTextures(1, &testDepth);

   if (testColor)
      glDeleteTextures(1, &testColor);

   return result;
}

bool GLDevice::init()
{
   mTotalTextureMemory= 0;
   mTotalVertexMemory= 0;
   for (int i=0;i<256;i++) keys[i]=0;
   mShaderAllocIndex= 0;

   if (!initExtensions())
   {
      PRINTF("required extensions not available!\n");
      return false;
   }

   // test framebuffer
   if (!testFrameBuffer())
   {
      extTest += QString("Render to depth texture is not possible!\n");
      return false;
   }


   int texunits;
   glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &texunits);
   PRINTF("max texture units: %d \n", texunits);

    int fragUniforms;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &fragUniforms);

   int maxFragmentTextureUmageUnits;
   glGetIntegerv(GL_MAX_IMAGE_UNITS_EXT, &maxFragmentTextureUmageUnits);

   int maxVertexTextureImageUnits;
   glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureImageUnits);
   PRINTF("vertex texture units: %d \n", maxVertexTextureImageUnits);
   int maxCombinedTextureImageUnits;
   glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits);
   PRINTF("combined texture units: %d \n", maxCombinedTextureImageUnits);

   int texsize;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texsize);
   PRINTF("max texture size: %d \n", texsize);

   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glColor4f(1,1,1,1);

//   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);;

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);                   // The Type Of Depth Test To Do
   glClearDepth(1.0);                      // Enables Clearing Of The Depth Buffer
   glClearStencil(0x0);

   glEnable(GL_CULL_FACE);
   glActiveTexture(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);                // Enable Texture Mapping
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glShadeModel(GL_SMOOTH);                // Enables Smooth Color Shading

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_ALPHA_TEST);
   glAlphaFunc(GL_GREATER, 0.001f);
   glLineWidth(1.0);

//   glEnable(GL_POLYGON_OFFSET_LINE);
//   glPolygonOffset(1.0, -10.0);

   mActive= true;

   return true;
}




void GLDevice::resize(int width, int height)
{
   mWidth= width;
   mHeight= height;
   setViewPort(0,0,width,height);
}


void GLDevice::setViewPort(int x, int y, int width, int height)
{
   mViewPortX = x;
   mViewPortY = y;
   mViewPortWidth = width;
   mViewPortHeight = height;

   glViewport(x, y, width, height);
}


void GLDevice::getViewPort(int *x, int *y, int *width, int *height)
{
   if (x)
      *x = mViewPortX;
   if (y)
      *y = mViewPortY;
   if (width)
      *width = mViewPortWidth;
   if (height)
      *height = mViewPortHeight;
}


void GLDevice::convertFromViewPort(int *x, int *y, int targetWidth, int targetHeight)
{
   *x -= mViewPortX;
   *y -= mViewPortY;
   *x = ((*x * targetWidth)  / mViewPortWidth)  /*- mViewPortX*/;
   *y = ((*y * targetHeight) / mViewPortHeight) /*- mViewPortY*/;
}


bool GLDevice::initExtensions()
{
   PRINTF("extensions:\n");
   // char *ext= (char*)glGetString(GL_EXTENSIONS);
   /*
   while (*ext!=0)
   {
      char *temp=ext;
      while (*temp!=0 && *temp!=32) temp++;
      if (*temp==0)
      {
         PRINTF("%s\n", ext);
         ext=temp;
      }
      else
      {
         *temp=0;
         PRINTF("%s\n", ext);
         ext= temp+1;
      }
   }
*/

   missingExtensions= false;

   // multi-texturing
#ifdef WIN32
   glClientActiveTexture= (PFNGLCLIENTACTIVETEXTUREPROC)getProcAddress("glClientActiveTextureARB");
   glActiveTexture =      (PFNGLACTIVETEXTUREARBPROC)   getProcAddress("glActiveTextureARB");
   glMultiTexCoord2f =    (PFNGLMULTITEXCOORD2FARBPROC) getProcAddress("glMultiTexCoord2fARB");
   glTexImage3D =         (PFNGLTEXIMAGE3DPROC)         getProcAddress("glTexImage3DEXT");
   glDrawRangeElements =  (PFNGLDRAWRANGEELEMENTSPROC)  getProcAddress("glDrawRangeElementsEXT");

#endif

//   glBlendFuncSeparate= (PFNGLBLENDFUNCSEPARATEPROC)getProcAddress("glBlendFuncSeparateEXT");
#ifndef Q_OS_MAC
   // vertex buffer objects
   glGenBuffers =    (PFNGLGENBUFFERSARBPROC)    getProcAddress("glGenBuffersARB");
   glBindBuffer =    (PFNGLBINDBUFFERPROC)    getProcAddress("glBindBufferARB");
   glBufferData =    (PFNGLBUFFERDATAPROC)    getProcAddress("glBufferDataARB");
   glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC) getProcAddress("glDeleteBuffersARB");
   glMapBuffer =     (PFNGLMAPBUFFERPROC)        getProcAddress("glMapBufferARB");
   glUnmapBuffer =   (PFNGLUNMAPBUFFERPROC)      getProcAddress("glUnmapBufferARB");

   // shader stuff
   glCreateShaderObject =   (PFNGLCREATESHADEROBJECTARBPROC)  getProcAddress("glCreateShaderObjectARB");
   glShaderSource =         (PFNGLSHADERSOURCEARBPROC)        getProcAddress("glShaderSourceARB");
   glCompileShader =        (PFNGLCOMPILESHADERARBPROC)       getProcAddress("glCompileShaderARB");
   glCreateProgramObject =  (PFNGLCREATEPROGRAMOBJECTARBPROC) getProcAddress("glCreateProgramObjectARB");
   glAttachObject =         (PFNGLATTACHOBJECTARBPROC)        getProcAddress("glAttachObjectARB");
   glLinkProgram =          (PFNGLLINKPROGRAMARBPROC)         getProcAddress("glLinkProgramARB");
   glUseProgramObject =     (PFNGLUSEPROGRAMOBJECTARBPROC)    getProcAddress("glUseProgramObjectARB");
   glGetObjectParameterfv = (PFNGLGETOBJECTPARAMETERFVARBPROC)getProcAddress("glGetObjectParameterfvARB");
   glGetObjectParameteriv = (PFNGLGETOBJECTPARAMETERIVARBPROC)getProcAddress("glGetObjectParameterivARB");
   glGetUniformLocation=    (PFNGLGETUNIFORMLOCATIONARBPROC)  getProcAddress("glGetUniformLocationARB");
   glUniform1i =            (PFNGLUNIFORM1IARBPROC)           getProcAddress("glUniform1iARB");
   glUniform1f =            (PFNGLUNIFORM1FARBPROC)           getProcAddress("glUniform1fARB");
   glUniform2f =            (PFNGLUNIFORM2FARBPROC)           getProcAddress("glUniform2fARB");
   glUniform3f =            (PFNGLUNIFORM3FARBPROC)           getProcAddress("glUniform3fARB");
   glUniform4f =            (PFNGLUNIFORM4FARBPROC)           getProcAddress("glUniform4fARB");
   glUniform1fv =           (PFNGLUNIFORM1FVARBPROC)          getProcAddress("glUniform1fvARB");
   glUniform2fv =           (PFNGLUNIFORM2FVARBPROC)          getProcAddress("glUniform2fvARB");
   glUniform3fv =           (PFNGLUNIFORM3FVARBPROC)          getProcAddress("glUniform3fvARB");
   glUniform4fv =           (PFNGLUNIFORM4FVARBPROC)          getProcAddress("glUniform4fvARB");
   glUniformMatrix4fv =     (PFNGLUNIFORMMATRIX4FVARBPROC)    getProcAddress("glUniformMatrix4fvARB");

   glGetInfoLog=            (PFNGLGETINFOLOGARBPROC)          getProcAddress("glGetInfoLogARB");

   glGenFramebuffers=         (PFNGLGENFRAMEBUFFERSEXTPROC)          getProcAddress("glGenFramebuffersEXT");
   glDeleteFramebuffers=      (PFNGLDELETEFRAMEBUFFERSEXTPROC)       getProcAddress("glDeleteFramebuffersEXT");
   glBindFramebuffer=         (PFNGLBINDFRAMEBUFFEREXTPROC)          getProcAddress("glBindFramebufferEXT");
   glGetRenderbufferParameteriv=(PFNGLGETRENDERBUFFERPARAMETERIVPROC)getProcAddress("glGetRenderbufferParameterivEXT");
   glGenRenderbuffers=        (PFNGLGENRENDERBUFFERSEXTPROC)         getProcAddress("glGenRenderbuffersEXT");
   glDeleteRenderbuffers=     (PFNGLDELETERENDERBUFFERSEXTPROC)      getProcAddress("glDeleteRenderbuffersEXT");

   glRenderbufferStorage=     (PFNGLRENDERBUFFERSTORAGEEXTPROC)      getProcAddress("glRenderbufferStorageEXT");
   glFramebufferRenderbuffer= (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)  getProcAddress("glFramebufferRenderbufferEXT");
   glFramebufferTexture2D=    (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)     getProcAddress("glFramebufferTexture2DEXT");
   glCheckFramebufferStatus=  (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)   getProcAddress("glCheckFramebufferStatusEXT");
   glBindRenderbuffer=        (PFNGLBINDRENDERBUFFEREXTPROC)         getProcAddress("glBindRenderbufferEXT");
   glBlitFramebuffer=         (PFNGLBLITFRAMEBUFFERPROC)             getProcAddress("glBlitFramebufferEXT");
   glRenderbufferStorageMultisample= (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)getProcAddress("glRenderbufferStorageMultisampleEXT", true);
   glGetInternalformativ =    (PFNGLGETINTERNALFORMATIVPROC)         getProcAddress("glGetInternalformativ", true);
#endif
//   glActiveStencilFace=     (PFNGLACTIVESTENCILFACEEXTPROC)   getProcAddress("glActiveStencilFaceEXT");
//   glStencilOpSeparateATI=     (PFNGLSTENCILOPSEPARATEATIPROC)   getProcAddress("glStencilOpSeparateATI");
//   glStencilFuncSeparateATI=   (PFNGLSTENCILFUNCSEPARATEATIPROC) getProcAddress("glStencilFuncSeparateATI");

   return !missingExtensions;
}


// abstract render functions
void GLDevice::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


unsigned int GLDevice::createTexture(void *data, int x, int y, int flags)
{
   GLuint tex;

   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);

   updateTexture(data, x, y, flags);

   return (unsigned int)tex;
}

void GLDevice::deleteTexture(unsigned int textureId)
{
   glDeleteTextures(1, &textureId);
}

void GLDevice::updateTexture(void *data, int x, int y, int flags)
{
   if (flags & 1)
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   else
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

   if (flags & 2)
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
   else
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);


   if (flags & 4)
   {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   }
   else
   {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   }

   int level=0;

   // PRINTF("uploading %dx%d texture\n", x,y);

   unsigned int* temp= new unsigned int[x*y];
   memcpy(temp, data, x*y*4);

   do
   {
      if (x==0) x=1;
      if (y==0) y=1;

      glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, x, y, 0, GL_BGRA, GL_UNSIGNED_BYTE, temp);
//      glTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, x, y, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
      mTotalTextureMemory+= x*y*4;

      int nx= x>>1; if (nx==0) nx=1;
      int ny= y>>1; if (ny==0) ny=1;

      unsigned int *dst= (unsigned int *)temp;
      unsigned int *src1= (unsigned int *)temp;

      for (int i=0; i<ny; i++)
      {
         unsigned int *src2;
         if (ny>1)
            src2= src1 + x;
         else
            src2= src1; // downsampling a single scanline

         for (int j=0;j<nx;j++)
         {
            unsigned int c1= *src1++;
            unsigned int c2= *src1++;
            unsigned int c3= *src2++;
            unsigned int c4= *src2++;

            int a= ((c1>>24&0xff) + (c2>>24&0xff) + (c3>>24&0xff) + (c4>>24&0xff)) >> 2;
            int r= ((c1>>16&0xff) + (c2>>16&0xff) + (c3>>16&0xff) + (c4>>16&0xff)) >> 2;
            int g= ((c1>> 8&0xff) + (c2>> 8&0xff) + (c3>> 8&0xff) + (c4>> 8&0xff)) >> 2;
            int b= ((c1    &0xff) + (c2    &0xff) + (c3    &0xff) + (c4    &0xff)) >> 2;

            *dst++= (a<<24)|(r<<16)|(g<<8)|b;
         }
         src1+=(x<<1)-(nx<<1);
      }

      x >>= 1;
      y >>= 1;

      level++;
   } while ( (x!=0 || y!=0) && (flags & 2));

   delete[] temp;

   // PRINTF(" - %d mipmaps generated (total memory consumed: %dMB)\n", level-1, mTotalTextureMemory>>20);
}

unsigned int GLDevice::uploadTexture1D(void *data, int x, int flags)
{
   GLuint tex;

   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_1D, tex);

   if (flags & 1)
      glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   else
      glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

   if (flags & 2)
      glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   else
      glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, x, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

   return (unsigned int)tex;
}


void GLDevice::setPerspective(float scale, float aspect, float zNear, float zFar)
{
   float ymin = -zNear * scale;
   float ymax = -ymin;

   float xmax = ymax * aspect;
   float xmin = ymin * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}


void GLDevice::setCamera(const Matrix& mat, float fov, float zNear, float zFar, bool perspective)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
//   glScalef(0.25,0.25,0.25);

//   gluOrtho2D(0.0, 800.0, 0.0, 600.0) ;
//   glOrtho(-100,100,-100,100, zNear, zFar);

   if (perspective)
      setPerspective(fov, (float)mViewPortWidth / mViewPortHeight, zNear, zFar);
   else
   {
      // todo: do fov
      float x= 2.8f / fov;
      float y= x * 9.0f / 16.0f;
      glOrtho(-x,x,-y,y, zNear, zFar);
   }

   glMultMatrixf( mat );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void GLDevice::push(const Matrix& m)
{
   glPushMatrix();
   glLoadMatrixf( m );
}


void GLDevice::pop()
{
   glPopMatrix();
}

int scale=0;

int faker=0;

void GLDevice::drawLine(Vector*)
{
/*
   unsigned short indices[2]={0,1};

   Vector *dst= (Vector*)lockVertexBuffer(mTempVertex);
   memcpy(dst, v, sizeof(Vector)*2);
   unlockVertexBuffer(mTempVertex);

   unsigned short *index= (unsigned short*)lockIndexBuffer(mTempIndex);
   memcpy(index, v, sizeof(short)*2);
   unlockIndexBuffer(mTempIndex);

   glEnableClientState(GL_VERTEX_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mTempVertex );
   glVertexPointer( 3, GL_FLOAT, 0, NULL );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mTempIndex );
   glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, NULL );

   glDisableClientState(GL_VERTEX_ARRAY);
*/
}

unsigned int GLDevice::createBuffer()
{
   unsigned int buf = 0;
   glGenBuffers( 1, &buf);
   return buf;
}

void GLDevice::deleteBuffer(unsigned int buffer)
{
   glDeleteBuffers(1, &buffer);
}


unsigned int GLDevice::createVertexBuffer(int size, bool dyn)
{
   unsigned int buf = createBuffer();
   allocateVertexBuffer(buf, size, dyn);
   return buf;
}

void GLDevice::allocateVertexBuffer(unsigned int buf, int size, bool dyn)
{
   glBindBuffer( GL_ARRAY_BUFFER_ARB, buf );
   if (!dyn)
      glBufferData( GL_ARRAY_BUFFER_ARB, size, NULL, GL_STATIC_DRAW_ARB );
   else
      glBufferData( GL_ARRAY_BUFFER_ARB, size, NULL, GL_DYNAMIC_DRAW_ARB );
   mTotalVertexMemory += size;
}

void* GLDevice::lockVertexBuffer(unsigned int buf, int /*size*/)
{
   void *ptr= NULL;

   glBindBuffer( GL_ARRAY_BUFFER_ARB, buf );

   ptr= glMapBuffer( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY );

   return ptr;
}

void GLDevice::unlockVertexBuffer(unsigned int /*buf*/)
{
//   glBindBuffer( GL_ARRAY_BUFFER_ARB, buf );
   glUnmapBuffer( GL_ARRAY_BUFFER_ARB );
}

unsigned int GLDevice::createIndexBuffer(int size, bool dyn)
{
   unsigned int buf = createBuffer();

   allocateIndexBuffer(buf, size, dyn);

   return buf;
}

void GLDevice::allocateIndexBuffer(unsigned int buf, int size, bool dyn)
{
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, buf );
   if (!dyn)
      glBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, size, NULL, GL_STATIC_DRAW_ARB );
   else
      glBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, size, NULL, GL_DYNAMIC_DRAW_ARB );
   mTotalVertexMemory += size;
}

void* GLDevice::lockIndexBuffer(unsigned int buf, int /*size*/)
{
   void *ptr= NULL;

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, buf );

   ptr= glMapBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY );

   return ptr;
}


void GLDevice::unlockIndexBuffer(unsigned int buf)
{
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, buf );
   glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
}


void GLDevice::setCulling(bool state)
{
   if (state)
      glEnable(GL_CULL_FACE);
   else
      glDisable(GL_CULL_FACE);
}

void GLDevice::setMaterial(const Vector& amb, const Vector& dif, const Vector& spc, float shine)
{
   float a[4]={amb.x, amb.y, amb.z, 1.0f};
   float d[4]={dif.x, dif.y, dif.z, 1.0f};
   float s[4]={spc.x, spc.y, spc.z, 1.0f};

   glMaterialfv(GL_FRONT, GL_AMBIENT,   a);
   glMaterialfv(GL_FRONT, GL_DIFFUSE,   d);
   glMaterialfv(GL_FRONT, GL_SPECULAR,  s);
   glMaterialfv(GL_FRONT, GL_SHININESS, &shine);
}

/*
void setShaders() {

char *vs = NULL,*fs = NULL,*fs2 = NULL;

v = glCreateShaderObject(GL_VERTEX_SHADER_ARB);


vs = textFileRead("toon.vert");

const char * vv = vs;

glShaderSource(v, 1, &vv,NULL);

free(vs);

glCompileShader(v);

p = glCreateProgramObject();
glAttachObject(p,v);

glLinkProgram(p);
glUseProgramObject(p);
}


*/


bool GLDevice::checkShaderError(const char *filename, GLhandleARB shader)
{
   GLcharARB* infoLog;
   GLint      infoLogLength;
   GLint      compileStatus;
   bool       error= false;

   glGetObjectParameteriv(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);

   if (compileStatus != GL_TRUE)
      error= true;

   glGetObjectParameteriv(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
   if (infoLogLength>1)
   {
      infoLog = (GLcharARB *) malloc(infoLogLength);

      glGetInfoLog(shader, infoLogLength, NULL, infoLog);

      qDebug("shader compile error in %s:\n%s\n\n", filename, infoLog);
      free(infoLog);
   }

   return error;
}

unsigned int GLDevice::loadShader(const char *vname, const char *pname)
{
   bool fileNotFound= false;

   FileStream* stream= new FileStream();

   ShaderProgramHandle prog= glCreateProgramObject();

   if (stream->open(vname))
   {
      int size= stream->size();
      char* source= new char[size];
      stream->getData(source, size);
      stream->close();
      GLhandleARB shader = glCreateShaderObject( GL_VERTEX_SHADER_ARB );
      glShaderSource( shader, 1, (const char**)&source, &size);
      glCompileShader(shader);

      if (checkShaderError(vname, shader))
         return 0;

      glAttachObject(prog,shader);
   }
   else
   {
      fileNotFound= true;
   }

   if (stream->open(pname))
   {
      int size= stream->size();
      char* source= new char[size];
      stream->getData(source, size);
      stream->close();
      GLhandleARB shader = glCreateShaderObject( GL_FRAGMENT_SHADER_ARB );
      glShaderSource( shader, 1, (const char**)&source, &size);
      glCompileShader(shader);

      if (checkShaderError(pname, shader))
         return 0;

      glAttachObject(prog,shader);
   }
   else
   {
      fileNotFound= true;
   }

   glLinkProgram(prog);

   delete stream;

   if (fileNotFound)
   {
      qDebug("shader file not found!");
   }

   unsigned int shader = mShaderAllocIndex++;
   mShaderTable[shader] = prog;
   setShader(shader);

   return shader;
}


void GLDevice::setShader(unsigned int shader)
{
   if (mCurShader != shader)
   {
      mCurShader= shader;
      ShaderProgramHandle prog = mShaderTable[mCurShader];
      glUseProgramObject(prog);
   }
}

int GLDevice::getParameterIndex(const char *name)
{
   ShaderProgramHandle prog = mShaderTable[mCurShader];
   int pos= glGetUniformLocation(prog, name);
   GLenum error = glGetError();
   if (error!=GL_NO_ERROR)
   {
      PRINTF("fail [%d]: get '%s' in shader %d \n", error, name, mCurShader);
   }

   return pos;
}

void GLDevice::bindSampler(int pos, int unit)
{
   glUniform1i(pos, unit);
}

void GLDevice::setParameter(int pos, float* data, int size)
{
   glUniform1fv(pos, size, data);
}

void GLDevice::setParameter(int pos, const Vector4& vector)
{
   glUniform4fv(pos, 1, vector.data());
}

void GLDevice::setParameter(int pos, const Vector& vector)
{
   glUniform3fv(pos, 1, vector.data());
}

void GLDevice::setParameter(int pos, const Vector2& vector)
{
   glUniform2fv(pos, 1, vector.data());
}

void GLDevice::setParameter(int pos, const Matrix& mat)
{
    glUniformMatrix4fv(pos, 1, false, mat.data());
}

void GLDevice::setParameter(int pos, const Matrix* mat, int count)
{
    glUniformMatrix4fv(pos, count, false, mat->data());
}

void GLDevice::setParameter(int pos, float f)
{
   glUniform1f(pos, f);
}
