#pragma once

#include "node.h"
#include "tools/list.h"
#include "../render/geometry.h"

class Stream;
class MotionMixer;

class Mesh : public Node
{
public:
                Mesh(Node *parent = 0);
                Mesh(const Mesh&, Node* parent = 0);
   virtual      ~Mesh();

   void         copy(const Mesh& mesh);

   void         load(Stream *stream);
   void         write(Stream *stream);

   int          getPartCount() const;
   void         add(Geometry *geo);
   Geometry*    getPart(int index) const;

   void         update(Node **nodelist);

   void         setAnimationFrame(float frame);
   float        getAnimationFrame() const;
   Node*        getSkeleton() const;
   void         setSkeleton(Node *node);

   MotionMixer* getMotionMixer() const;
   void         setMotionMixer(MotionMixer* mixer);

   void         transform(float frame);

   unsigned int getRenderFlags() const;
   void         setRenderFlags(unsigned int flags);
   float        getRenderParameter(int index) const;
   void         setRenderParameter(int index, float param);

   void         createBoxMapping(bool unwrap, const Vector& min, const Vector& max, const Matrix& tm = Matrix());

protected:
   Array<Geometry*> mGeometry;
   Node*            mSkeleton;
   MotionMixer*     mMotionMixer;
   float            mAnimFrame;
   unsigned int     mRenderFlags;
   float            mRenderParameter[4];
};
