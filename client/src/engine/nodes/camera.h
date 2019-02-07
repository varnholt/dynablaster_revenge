// camera object
// the camera's transformation defines the view matrix (additionally requires fov scaling)

#pragma once

#include "node.h"
#include "animation/valtrack.h"

class Stream;

class Camera : public Node
{
public:
           Camera(Node *parent = 0);
   virtual ~Camera();
   void    load(Stream *stream);
   void    write(Stream *stream);

   void    transform(float time);
   float   getFOV() const;
   void    setFOV(float fov);
   float   getNear() const;
   void    setNear(float znear);
   float   getFar() const;
   void    setFar(float zfar);
   void    lookAt(const Vector& pos, const Vector& target, const Vector& up= Vector(0,0,1));
   bool    getPerspectiveMode() const;
   void    setPerspectiveMode(bool mode);

private:
   float    mFov;
   float    mNear;
   float    mFar;
   bool     mPerspectiveMode;
   ValTrack mFovTrack;  // fov track
   ValTrack mNearTrack; // near track
   ValTrack mFarTrack;  // far track
};

