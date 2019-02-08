#ifndef RENDERDEVICE_H
#define RENDERDEVICE_H

#include "tools/singleton.h"
#include "math/vector.h"
#include "math/vector2.h"
#include "math/vector4.h"
#include "math/matrix.h"

class Stream;

class RenderDevice : public Singleton<RenderDevice>
{
public:
                 RenderDevice();
   virtual       ~RenderDevice();

   void          setKey(int num, int state);
   int           getKey(int num);

   virtual bool  abort();
   virtual void  setAbort();
   virtual bool  active();
   virtual void  setActive(bool state);
   virtual Matrix getCameraMatrix() const;

   virtual bool  init() = 0;
   virtual void  clear() = 0;
   virtual void  resize(int x, int y) = 0;
   virtual void  setViewPort(int x, int y, int width, int height) = 0;
   virtual void  getViewPort(int *x, int *y, int *width, int *height) = 0;
   virtual void  convertFromViewPort(int *x, int *y, int targetWidth, int targetHeight) = 0;

   virtual void  setPerspective(float fov, float aspect, float znear, float zfar) = 0;
   virtual void  setCamera(const Matrix& m, float fov, float zNear, float zFar, bool perspective = false) = 0;
   virtual void  push(const Matrix& mat) = 0;
   virtual void  pop() = 0;

   virtual unsigned int createVertexBuffer(int size, bool dynamic=false) = 0;
   virtual void allocateVertexBuffer(unsigned int buffer, int size, bool dyn=false) = 0;
   virtual void* lockVertexBuffer(unsigned int handle, int size=0) = 0;
   virtual void  unlockVertexBuffer(unsigned int buf) = 0;

   virtual unsigned int createIndexBuffer(int size, bool dyn=false) = 0;
   virtual void allocateIndexBuffer(unsigned int buf, int size, bool dyn=false) = 0;
   virtual void* lockIndexBuffer(unsigned int handle, int size=0) = 0;
   virtual void  unlockIndexBuffer(unsigned int buf) = 0;

   virtual void  setCulling(bool state) = 0;
   virtual void  setMaterial(const Vector& amb, const Vector& dif, const Vector& spc, float shine) = 0;

   virtual void drawLine(Vector *v) = 0;
   virtual unsigned int createTexture(void *data, int x, int y, int flags=3) = 0;
   virtual void deleteTexture(unsigned int textureId) = 0;
   virtual void updateTexture(void *data, int x, int y, int flags) = 0;
   virtual unsigned int uploadTexture1D(void *data, int x, int flags=0) = 0;
   virtual unsigned int loadShader(const char *vname, const char *pname) = 0;
   virtual void setShader(unsigned int shader) = 0;
   virtual int getParameterIndex(const char *name) = 0;
   virtual void bindSampler(int pos, int unit) = 0;
   virtual void setParameter(int pos, float* data, int size) = 0;
   virtual void setParameter(int pos, const Vector& p) = 0;
   virtual void setParameter(int pos, const Vector2& p) = 0;
   virtual void setParameter(int pos, const Vector4& p) = 0;
   virtual void setParameter(int pos, const Matrix& p) = 0;
   virtual void setParameter(int pos, const Matrix* p, int count) = 0;
   virtual void setParameter(int pos, float f) = 0;


   virtual unsigned int createBuffer() = 0;
   virtual void deleteBuffer(unsigned int buffer) = 0;

   virtual void setSwapInterval(int interval) = 0;

   virtual float getWidth() const;
   virtual float getHeight() const;

   void setBorder(int left, int top, int right, int bottom);
   int getBorderLeft() const;
   int getBorderBottom() const;

protected:
   int        mWidth, mHeight;
   float      mAspect;
   int        mActive;
   int        mAbort;
   int        mTime;
   Matrix     mCamera;
   unsigned int mCurShader;
   int        keys[256]; // keyboard state
   int        mBorderLeft, mBorderTop,mBorderRight, mBorderBottom;
};

extern RenderDevice *activeDevice;

#endif
