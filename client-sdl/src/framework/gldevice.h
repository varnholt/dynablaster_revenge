#pragma once

// Materials call raw gl* functions directly (not just through activeDevice->), the same way
// they did when the legacy gldevice.h pulled in framework/glext.h for that exact reason - so
// this exposes the GLES3 declarations too, rather than making every material .cpp add its own
// include.
#include "../gles3.h"
#include "renderdevice.h"

#include <map>

/// \brief GLES3 replacement for the legacy desktop-GL/ARB-shader-object device.
///
/// The legacy implementation drove OpenGL's fixed-function matrix stack (glMatrixMode /
/// glFrustum / glMultMatrixf / glLoadMatrixf) and relied on shaders reading it back implicitly
/// through compatibility-profile built-ins (gl_ModelViewMatrix, gl_ProjectionMatrix,
/// gl_NormalMatrix, gl_Vertex, gl_MultiTexCoord0). None of that exists in GLES3, so this version
/// computes the same matrices on the CPU (using the engine's own Matrix class, which already had
/// frustum()/ortho() helpers) and uploads them as plain uniforms - "u_modelView", "u_projection",
/// "u_normalMatrix" and "u_modelViewProjection" - to whichever of those names a given shader
/// happens to declare. Materials keep calling activeDevice->push()/pop()/setCamera() exactly as
/// before; only the internals changed.
///
/// setMaterial() (fixed-function per-vertex lighting via glMaterialfv) and uploadTexture1D()
/// (GL_TEXTURE_1D, which GLES has no equivalent for) are both already dead in practice - every
/// call site is commented out except one, itself inside code that still needs its immediate-mode
/// draw calls ported - so they are stubs here rather than real implementations.
class GLDevice : public RenderDevice
{
public:
   bool init();

   void resize(int x, int y);
   void setViewPort(int x, int y, int width, int height);
   void getViewPort(int *x, int *y, int *width, int *height);
   void convertFromViewPort(int *x, int *y, int targetWidth, int targetHeight);
   void clear();

   void setPerspective(float fov, float aspect, float zNear = 1.0f, float zFar = 5000.0f);
   void setCamera(const Matrix& m, float fov, float zNear = 1.0f, float zFar = 1000.0f, bool perspective = true);
   void push(const Matrix& mat);
   void pop();

   /// \brief reads the combined projection*view matrix setCamera() last computed.
   /// replaces the legacy glGetFloatv(GL_PROJECTION_MATRIX, ...) readback a few materials used
   /// to pull the shadow camera's matrix out after a temporary setCamera() call.
   Matrix getProjectionMatrix() const
   {
      return mProjectionMatrix;
   }

   /// \brief saves the current projection matrix (single slot - callers don't nest this).
   /// pairs with popProjection() to protect the main camera's projection across a material's
   /// temporary setCamera() call for its own shadow camera, mirroring the legacy
   /// glMatrixMode(GL_PROJECTION)/glPushMatrix()/glPopMatrix() bracket.
   void pushProjection()
   {
      mSavedProjectionMatrix = mProjectionMatrix;
   }

   void popProjection()
   {
      mProjectionMatrix = mSavedProjectionMatrix;
   }

   /// \brief directly assigns the projection matrix, bypassing setCamera()'s 3D frustum/ortho
   /// computation - used by 2D screen-space rendering (the menu system) which builds its own
   /// pixel-space ortho matrix via Matrix::ortho() instead.
   void setProjectionMatrix(const Matrix& mat)
   {
      mProjectionMatrix = mat;
   }

   unsigned int createVertexBuffer(int size, bool dynamic = false);
   void allocateVertexBuffer(unsigned int buffer, int size, bool dyn = false);
   void* lockVertexBuffer(unsigned int handle, int size = 0);
   void  unlockVertexBuffer(unsigned int buf);

   unsigned int createIndexBuffer(int size, bool dyn = false);
   void allocateIndexBuffer(unsigned int buf, int size, bool dyn = false);
   void* lockIndexBuffer(unsigned int handle, int size = 0);
   void  unlockIndexBuffer(unsigned int buf);
   void  setCulling(bool state);
   void  setMaterial(const Vector& amb, const Vector& dif, const Vector& spc, float shine);

   void drawLine(Vector *v);

   unsigned int createTexture(void *data, int x, int y, int flags = 3);
   void deleteTexture(unsigned int textureId);
   void updateTexture(void *data, int x, int y, int flags);

   unsigned int uploadTexture1D(void *data, int x, int flags = 0);
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
   /// \brief a linked program plus the reserved-name uniform locations push()/setCamera() feed.
   struct ShaderInfo
   {
      unsigned int program = 0;
      int locModelView = -1;
      int locProjection = -1;
      int locNormalMatrix = -1;
      int locModelViewProjection = -1;
   };

   void uploadTransformUniforms();

   std::map<unsigned int, ShaderInfo> mShaderTable;
   unsigned int mShaderAllocIndex = 0;

   int mViewPortX = 0;
   int mViewPortY = 0;
   int mViewPortWidth = 0;
   int mViewPortHeight = 0;

   Matrix mProjectionMatrix;
   Matrix mSavedProjectionMatrix;
   Matrix mWorldTransform;

   // lock*Buffer() is always called right after the matching allocate*Buffer() with no size
   // argument of its own (mirroring the legacy glMapBuffer() call, which mapped the whole
   // buffer); glMapBufferRange() needs an explicit length, so the last allocated size is cached.
   int mLastVertexBufferSize = 0;
   int mLastIndexBufferSize = 0;
};
