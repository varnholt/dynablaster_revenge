#pragma once

#include "tools/array.h"
#include "tools/string.h"

class SceneGraph;
class Node;
class Mesh;
class Matrix;

typedef Array< Array<Matrix>* > BoneAnimPrecalc;

class MotionMixer
{
public:
   MotionMixer();
   ~MotionMixer();

   static void cleanup();
   static int  addAnimation(const String& name);
   static Mesh* getMesh(const String& name);
   
   void setAnimation(int anim1, int anim2, float weight1, int anim3, float weight2);
   void animate(float frame);
   Node* getNode( int index ) const;

private:
   static SceneGraph*             mRefAnim;
   static Array<BoneAnimPrecalc*> mMatrixPrecalc;

   static float                   mFrameStep;
   static int                     mBoneCount;
   static int                     mAnimLength;

   Array<Node*>       mCurrentState;

   int                mAnim1;
   int                mAnim2;
   int                mAnim3;
   float              mWeight1;
   float              mWeight2;
};
