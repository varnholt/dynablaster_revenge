// node-class
// abstract base class of all scene-objects

#pragma once

#include "tools/streamable.h"
#include "tools/array.h"
#include "tools/objectname.h"
#include "math/vector.h"
#include "math/matrix.h"

#include "animation/postrack.h"
#include "animation/rottrack.h"
#include "animation/scaletrack.h"
#include "animation/valtrack.h"
#include "animation/vistrack.h"
#include "animation/bakedtransformation.h"

class Node;
class SceneGraph;

class Node : public Streamable, public ObjectName
{
public:
   enum ID
   {
	   idRoot = -1,
	   idNone =  0,
	   idMesh =  1,
	   idDummy = 2,
	   idShape = 3,
	   idCamera = 4,
	   idDir    = 9,
      idOmni   = 10,
	   idSpot   = 11,
      idAnimMesh = 32
   };

                 Node(ID id, Node *parent = 0);
                 Node(const Node& node, Node* parent = 0);
   virtual       ~Node();
   ID            id() const;                   // return the node id (object type)
   Node*         parent() const;               // get parent node
   SceneGraph*   getRoot() const;
   int           getDepth() const;
   bool          visible() const;
   void          setVisible(bool visible);
   void          addChild(Node* node);
   int           getChildCount() const;
   Node*         getChild(int index) const;
   Node*         getChild(const String& name) const;
   void          setParent(Node *parent);      // link to parent obj. includes this to parent's children
   bool          getUserTransformable() const;
   void          setUserTransformable(bool);
   bool          hasSkinning() const;
   float         getFrame() const;
   void          setFrame(float frame);
   int           getAnimationLength() const;
   void          bakeAnimationTrack(float step);
   const BakedTransformation& getBakedAnimation() const;

   virtual void  load(Stream *stream);    // load object from stream
   virtual void  write(Stream *stream);

   virtual void  transform(float frame);    // calc transformation at frame

   void          setTransform(const Matrix& m);
   const Matrix& getTransform() const;
   Vector        getPosition() const;

   const PosTrack&   getPositionTrack() const;
	const RotTrack&   getRotationTrack() const;
	const ScaleTrack& getScaleTrack() const;
	const ValTrack&   getFlipTrack() const;
	const VisTrack&   getVisibilityTrack() const;

protected:
	// node-head
	ID          mID;       // pseudo-rtti to identify the object type
	Node        *mParent;  // pointer to parent object
   Array<Node*> mChilds;  // list of child objects

	Matrix      mTransform;
	bool        mHasSkinning;
   bool        mVisible;

	bool        mUserTransform;
   float       mFrame;

	// tracks
	PosTrack    mPosTrack;   // position track
	RotTrack    mRotTrack;   // rotation track
	ScaleTrack  mScaleTrack; // scale track
	ValTrack    mFlipTrack;  // deprecated!
	VisTrack    mVisTrack;   // visibility track
   BakedTransformation mBake;
};
