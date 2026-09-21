#include "gldevice.h"

#include "../gles3.h"
#include "../tools/filestream.h"

#include <SDL3/SDL.h>

#include <vector>

namespace
{

GLuint compileStage(GLenum type, const char* source, int length, const char* filename)
{
   GLuint shader = glCreateShader(type);
   glShaderSource(shader, 1, &source, &length);
   glCompileShader(shader);

   GLint status = GL_FALSE;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
   if (status == GL_FALSE)
   {
      GLint log_length = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
      std::vector<char> log(static_cast<size_t>(log_length) + 1, '\0');
      glGetShaderInfoLog(shader, log_length, nullptr, log.data());
      SDL_Log("shader compile error in %s:\n%s", filename, log.data());
      glDeleteShader(shader);
      return 0;
   }

   return shader;
}

}  // namespace

bool GLDevice::init()
{
   mShaderAllocIndex = 0;

   glClearColor(0.1f, 0.1f, 0.12f, 1.0f);

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glClearDepthf(1.0f);
   glClearStencil(0);

   glEnable(GL_CULL_FACE);

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   mActive = true;

   return true;
}

void GLDevice::resize(int width, int height)
{
   mWidth = width;
   mHeight = height;
   setViewPort(0, 0, width, height);
}

void GLDevice::setViewPort(int x, int y, int width, int height)
{
   mViewPortX = x;
   mViewPortY = y;
   mViewPortWidth = width;
   mViewPortHeight = height;

   glViewport(x, y, width, height);
}

void GLDevice::getViewPort(int* x, int* y, int* width, int* height)
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

void GLDevice::convertFromViewPort(int* x, int* y, int targetWidth, int targetHeight)
{
   *x -= mViewPortX;
   *y -= mViewPortY;
   *x = (*x * targetWidth) / mViewPortWidth;
   *y = (*y * targetHeight) / mViewPortHeight;
}

void GLDevice::clear()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLDevice::clear(float r, float g, float b, float a)
{
   glClearColor(r, g, b, a);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
}

void GLDevice::setPerspective(float scale, float aspect, float zNear, float zFar)
{
   const float ymin = -zNear * scale;
   const float ymax = -ymin;
   const float xmax = ymax * aspect;
   const float xmin = ymin * aspect;

   mProjectionMatrix = Matrix::frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void GLDevice::setCamera(const Matrix& mat, float fov, float zNear, float zFar, bool perspective)
{
   if (perspective)
   {
      setPerspective(fov, static_cast<float>(mViewPortWidth) / mViewPortHeight, zNear, zFar);
   }
   else
   {
      const float x = 2.8f / fov;
      const float y = x * 9.0f / 16.0f;
      mProjectionMatrix = Matrix::ortho(-x, x, -y, y, zNear, zFar);
   }

   // the legacy renderer folded the camera matrix into the projection slot (glMultMatrixf right
   // after glFrustum/glOrtho, while modelview stayed at identity until the next push()) rather
   // than the modelview slot - preserved here so push()'s world transform keeps the same
   // "already positions relative to the camera" meaning the old fixed-function pipeline gave it.
   mProjectionMatrix = mat * mProjectionMatrix;
   mWorldTransform.identity();
}

void GLDevice::push(const Matrix& mat)
{
   mWorldTransform = mat;
   uploadTransformUniforms();
}

void GLDevice::pop()
{
   mWorldTransform.identity();
}

void GLDevice::uploadTransformUniforms()
{
   const auto it = mShaderTable.find(mCurShader);
   if (it == mShaderTable.end())
      return;

   const ShaderInfo& info = it->second;

   if (info.locModelView >= 0)
      glUniformMatrix4fv(info.locModelView, 1, GL_FALSE, mWorldTransform.data());

   if (info.locProjection >= 0)
      glUniformMatrix4fv(info.locProjection, 1, GL_FALSE, mProjectionMatrix.data());

   if (info.locModelViewProjection >= 0)
   {
      const Matrix mvp = mWorldTransform * mProjectionMatrix;
      glUniformMatrix4fv(info.locModelViewProjection, 1, GL_FALSE, mvp.data());
   }

   if (info.locNormalMatrix >= 0)
   {
      // uploaded as a mat4 uniform; shaders read the rotation part via mat3(u_normalMatrix),
      // which sidesteps needing a dedicated 4x4-to-3x3 packing helper on the C++ side.
      const Matrix normalMatrix = mWorldTransform.get3x3().adjointTranspose();
      glUniformMatrix4fv(info.locNormalMatrix, 1, GL_FALSE, normalMatrix.data());
   }
}

unsigned int GLDevice::createBuffer()
{
   unsigned int buf = 0;
   glGenBuffers(1, &buf);
   return buf;
}

void GLDevice::deleteBuffer(unsigned int buffer)
{
   glDeleteBuffers(1, &buffer);
}

unsigned int GLDevice::createVertexBuffer(int size, bool dyn)
{
   const unsigned int buf = createBuffer();
   allocateVertexBuffer(buf, size, dyn);
   return buf;
}

void GLDevice::allocateVertexBuffer(unsigned int buf, int size, bool dyn)
{
   glBindBuffer(GL_ARRAY_BUFFER, buf);
   glBufferData(GL_ARRAY_BUFFER, size, nullptr, dyn ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
   mLastVertexBufferSize = size;
}

void* GLDevice::lockVertexBuffer(unsigned int buf, int size)
{
   glBindBuffer(GL_ARRAY_BUFFER, buf);
   return glMapBufferRange(GL_ARRAY_BUFFER, 0, size != 0 ? size : mLastVertexBufferSize, GL_MAP_WRITE_BIT);
}

void GLDevice::unlockVertexBuffer(unsigned int buf)
{
   glBindBuffer(GL_ARRAY_BUFFER, buf);
   glUnmapBuffer(GL_ARRAY_BUFFER);
}

unsigned int GLDevice::createIndexBuffer(int size, bool dyn)
{
   const unsigned int buf = createBuffer();
   allocateIndexBuffer(buf, size, dyn);
   return buf;
}

void GLDevice::allocateIndexBuffer(unsigned int buf, int size, bool dyn)
{
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, dyn ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
   mLastIndexBufferSize = size;
}

void* GLDevice::lockIndexBuffer(unsigned int buf, int size)
{
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
   return glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, size != 0 ? size : mLastIndexBufferSize, GL_MAP_WRITE_BIT);
}

void GLDevice::unlockIndexBuffer(unsigned int buf)
{
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
   glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void GLDevice::setCulling(bool state)
{
   if (state)
      glEnable(GL_CULL_FACE);
   else
      glDisable(GL_CULL_FACE);
}

void GLDevice::setMaterial(const Vector&, const Vector&, const Vector&, float)
{
   // fixed-function per-vertex lighting (glMaterialfv) has no GLES equivalent. The one real
   // caller (GouraudShading) still needs its whole render() path ported off immediate mode; at
   // that point ambient/diffuse/specular become explicit shader uniforms instead.
}

void GLDevice::drawLine(Vector*)
{
   // the legacy implementation was already fully commented out upstream - no live callers.
}

unsigned int GLDevice::createTexture(void* data, int x, int y, int flags)
{
   GLuint tex = 0;
   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);
   updateTexture(data, x, y, flags);
   return tex;
}

void GLDevice::deleteTexture(unsigned int textureId)
{
   GLuint tex = textureId;
   glDeleteTextures(1, &tex);
}

void GLDevice::updateTexture(void* data, int x, int y, int flags)
{
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (flags & 1) ? GL_LINEAR : GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & 2) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

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

   int level = 0;

   // legacy asset data is laid out BGRA (it was uploaded through desktop GL's GL_BGRA format);
   // GLES has no guaranteed BGRA format, so the channel swap happens once here up front instead,
   // and the mip-chain averaging below (which only sums same-position byte lanes) carries it
   // through every level unchanged.
   auto* temp = new unsigned int[static_cast<size_t>(x) * y];
   const auto* src = static_cast<const unsigned int*>(data);
   for (int i = 0; i < x * y; ++i)
   {
      const unsigned int p = src[i];
      const unsigned int a = (p >> 24) & 0xff;
      const unsigned int r = (p >> 16) & 0xff;
      const unsigned int g = (p >> 8) & 0xff;
      const unsigned int b = p & 0xff;
      temp[i] = (a << 24) | (b << 16) | (g << 8) | r;
   }

   do
   {
      if (x == 0)
         x = 1;
      if (y == 0)
         y = 1;

      glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp);

      const int nx = (x >> 1) == 0 ? 1 : (x >> 1);
      const int ny = (y >> 1) == 0 ? 1 : (y >> 1);

      unsigned int* dst = temp;
      unsigned int* src1 = temp;

      for (int i = 0; i < ny; ++i)
      {
         unsigned int* src2 = (ny > 1) ? src1 + x : src1;

         for (int j = 0; j < nx; ++j)
         {
            const unsigned int c1 = *src1++;
            const unsigned int c2 = *src1++;
            const unsigned int c3 = *src2++;
            const unsigned int c4 = *src2++;

            const unsigned int a = ((c1 >> 24 & 0xff) + (c2 >> 24 & 0xff) + (c3 >> 24 & 0xff) + (c4 >> 24 & 0xff)) >> 2;
            const unsigned int r = ((c1 >> 16 & 0xff) + (c2 >> 16 & 0xff) + (c3 >> 16 & 0xff) + (c4 >> 16 & 0xff)) >> 2;
            const unsigned int g = ((c1 >> 8 & 0xff) + (c2 >> 8 & 0xff) + (c3 >> 8 & 0xff) + (c4 >> 8 & 0xff)) >> 2;
            const unsigned int b = ((c1 & 0xff) + (c2 & 0xff) + (c3 & 0xff) + (c4 & 0xff)) >> 2;

            *dst++ = (a << 24) | (r << 16) | (g << 8) | b;
         }
         src1 += (x << 1) - (nx << 1);
      }

      x >>= 1;
      y >>= 1;
      ++level;
   } while ((x != 0 || y != 0) && (flags & 2));

   delete[] temp;
}

unsigned int GLDevice::uploadTexture1D(void*, int, int)
{
   // GLES has no 1D textures. The only caller (cartoonshading's gradient-map lookup) already
   // has this call commented out upstream, so this is kept purely to satisfy the interface.
   return 0;
}

unsigned int GLDevice::loadShader(const char* vname, const char* pname)
{
   FileStream stream;
   GLuint vertexShader = 0;
   GLuint fragmentShader = 0;

   if (stream.open(vname))
   {
      const int size = stream.size();
      std::vector<char> source(static_cast<size_t>(size));
      stream.getData(source.data(), size);
      stream.close();
      vertexShader = compileStage(GL_VERTEX_SHADER, source.data(), size, vname);
   }
   else
   {
      SDL_Log("shader file not found: %s", vname);
   }

   if (stream.open(pname))
   {
      const int size = stream.size();
      std::vector<char> source(static_cast<size_t>(size));
      stream.getData(source.data(), size);
      stream.close();
      fragmentShader = compileStage(GL_FRAGMENT_SHADER, source.data(), size, pname);
   }
   else
   {
      SDL_Log("shader file not found: %s", pname);
   }

   ShaderInfo info;
   info.program = glCreateProgram();

   if (vertexShader != 0)
      glAttachShader(info.program, vertexShader);
   if (fragmentShader != 0)
      glAttachShader(info.program, fragmentShader);

   glLinkProgram(info.program);

   GLint link_status = GL_FALSE;
   glGetProgramiv(info.program, GL_LINK_STATUS, &link_status);
   if (link_status == GL_FALSE)
   {
      GLint log_length = 0;
      glGetProgramiv(info.program, GL_INFO_LOG_LENGTH, &log_length);
      std::vector<char> log(static_cast<size_t>(log_length) + 1, '\0');
      glGetProgramInfoLog(info.program, log_length, nullptr, log.data());
      SDL_Log("shader link error (%s / %s):\n%s", vname, pname, log.data());
   }

   if (vertexShader != 0)
      glDeleteShader(vertexShader);
   if (fragmentShader != 0)
      glDeleteShader(fragmentShader);

   info.locModelView = glGetUniformLocation(info.program, "u_modelView");
   info.locProjection = glGetUniformLocation(info.program, "u_projection");
   info.locNormalMatrix = glGetUniformLocation(info.program, "u_normalMatrix");
   info.locModelViewProjection = glGetUniformLocation(info.program, "u_modelViewProjection");

   const unsigned int shader = ++mShaderAllocIndex;
   mShaderTable[shader] = info;
   setShader(shader);

   return shader;
}

void GLDevice::setShader(unsigned int shader)
{
   if (mCurShader != shader)
   {
      mCurShader = shader;
      const auto it = mShaderTable.find(shader);
      glUseProgram(it != mShaderTable.end() ? it->second.program : 0);
   }
}

int GLDevice::getParameterIndex(const char* name)
{
   const auto it = mShaderTable.find(mCurShader);
   if (it == mShaderTable.end())
      return -1;

   return glGetUniformLocation(it->second.program, name);
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
   glUniformMatrix4fv(pos, 1, GL_FALSE, mat.data());
}

void GLDevice::setParameter(int pos, const Matrix* mat, int count)
{
   glUniformMatrix4fv(pos, count, GL_FALSE, mat->data());
}

void GLDevice::setParameter(int pos, float f)
{
   glUniform1f(pos, f);
}

void GLDevice::setSwapInterval(int interval)
{
   SDL_GL_SetSwapInterval(interval);
}
