#include "stdafx.h"
#include "camera.h"
#include "tools/stream.h"

Camera::Camera(Node *parent)
: Node(Node::idCamera, parent)
, mFov(1.0f)
, mNear(1.0f)
, mFar(1000.0f)
, mPerspectiveMode(true)
{
}

Camera::~Camera()
{
}

void Camera::load(Stream *stream)
{
   Node::load(stream);

   // read tracks
   Chunk anim(stream);
   mFovTrack.load(&anim);
//   mNearTrack.load(&anim);
//   mFarTrack.load(&anim);
   mPosTrack.load(&anim);
   mRotTrack.load(&anim);
   mScaleTrack.load(&anim);
   mVisTrack.load(&anim);
   mFlipTrack.load(&anim);
   anim.skip();
}

void Camera::write(Stream *stream)
{
   Node::write(stream);

   // read tracks
   Chunk anim(stream, 2000, "Animation");
   mFovTrack.write(&anim);
//   mNearTrack.write(&anim);
//   mFarTrack.write(&anim);

   mPosTrack.write(&anim);
   mRotTrack.write(&anim);
   mScaleTrack.write(&anim);
   mVisTrack.write(&anim);
   mFlipTrack.write(&anim);
}

void Camera::transform(float time)
{
   Node::transform(time);
   if (!mUserTransform)
      mFov= mFovTrack.get(time);

//   mFov= mFovTrack.get(time);
//   mNear= mNearTrack.get(time);
//   mFar= mFarTrack.get(time);
}

float Camera::getFOV() const
{
   return mFov;
}

float Camera::getNear() const
{
   return mNear;
}

void Camera::setNear(float znear)
{
   mNear= znear;
}

float Camera::getFar() const
{
   return mFar;
}

void Camera::setFar(float zfar)
{
   mFar= zfar;
}

void Camera::setPerspectiveMode(bool mode)
{
   mPerspectiveMode= mode;
}

bool Camera::getPerspectiveMode() const
{
    return mPerspectiveMode;
}

void Camera::lookAt(const Vector& pos, const Vector& target, const Vector& up)
{
   Matrix mat= Matrix::lookAt(pos, target, up);

   setTransform( mat.invert() );
}

