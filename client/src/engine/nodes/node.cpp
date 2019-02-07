#include "stdafx.h"
#include "node.h"
#include "scenegraph.h"
#include "tools/stream.h"

Node::Node(Node::ID id, Node *parent)
: mID(id)
, mParent(0)
, mHasSkinning(false)
, mVisible(true)
, mUserTransform(false)
, mFrame(0.0f)
, mBake()
{
   setParent( parent );
}

Node::Node(const Node& node, Node* parent)
: ObjectName(node)
, mID(node.id())
, mParent(0)
, mHasSkinning(node.hasSkinning())
, mUserTransform(node.getUserTransformable())
, mFrame(node.getFrame())
, mPosTrack(node.getPositionTrack())
, mRotTrack(node.getRotationTrack())
, mScaleTrack(node.getScaleTrack())
, mFlipTrack(node.getFlipTrack())
, mVisTrack(node.getVisibilityTrack())
, mBake(node.getBakedAnimation())
{
   if (parent)
      setParent( parent );
   else
      setParent( node.parent() );
}

Node::~Node()
{
   SceneGraph *scene= getRoot();
   if (scene && scene != this)
      scene->removeNode(this);
}

// get root node (scene)
SceneGraph* Node::getRoot() const
{
   // get root node
   Node *root= (Node*)this;
   while (root->parent())
      root= root->parent();

   // remove node from scene
   if (root->id() == Node::idRoot)
      return (SceneGraph*)root;
   else
      return 0;
}

Node::ID Node::id() const
{
   return mID;
}

void Node::addChild(Node* node)
{
   mChilds.add(node);
   Node *root= (Node*)this;
   while (root && root->id()!=idRoot)
      root= root->parent();
   if (root)
   {
      SceneGraph *scene= (SceneGraph*)root;
      scene->addNode(node);
   }
}

int Node::getChildCount() const
{
   return mChilds.size();
}

Node* Node::getChild(int index) const
{
   if (index >= 0 && index < mChilds.size())
      return mChilds[index];
   else
      return 0;
}

Node* Node::getChild(const String& name) const
{
   for (int i=0; i<mChilds.size(); i++)
   {
      if (mChilds[i]->name() == name)
         return mChilds[i];
   }
   return 0;
}

bool Node::visible() const
{
   return mVisible;
}

void Node::setVisible(bool visible)
{
   mVisible= visible;
}

bool Node::hasSkinning() const
{
   return mHasSkinning;
}

float Node::getFrame() const
{
   return mFrame;
}

void Node::setFrame(float frame)
{
   mFrame= frame;
}

int Node::getAnimationLength() const
{
   int time= 0;
   if (time < mPosTrack.getAnimationLength())
      time= mPosTrack.getAnimationLength();

   if (time < mRotTrack.getAnimationLength())
      time= mRotTrack.getAnimationLength();

   if (time < mScaleTrack.getAnimationLength())
      time= mScaleTrack.getAnimationLength();

   return time;
}

void Node::setParent(Node *parent)
{
   mParent= parent;
   if (parent)
      parent->addChild(this);
}

Node* Node::parent() const
{
   return mParent;
}


int Node::getDepth() const
{
   int depth= 0;
   Node *node= (Node*)this;
   while (node)
   {
      node= node->parent();
      depth++;
   }
   return depth;
}


void Node::load(Stream*)
{
}

void Node::setTransform(const Matrix& m)
{
   mTransform= m;
}

const Matrix& Node::getTransform() const
{
   return mTransform;
}

void Node::write(Stream*)
{
}


Vector Node::getPosition() const
{
   return mTransform.translation();
}

const PosTrack& Node::getPositionTrack() const
{
   return mPosTrack;
}

const RotTrack& Node::getRotationTrack() const
{
   return mRotTrack;
}

const ScaleTrack& Node::getScaleTrack() const
{
   return mScaleTrack;
}

const ValTrack& Node::getFlipTrack() const
{
   return mFlipTrack;
}

const VisTrack& Node::getVisibilityTrack() const
{
   return mVisTrack;
}

void Node::transform(float time)
{
   // in user-transform mode the matrix has already been set from outside
   if (mUserTransform)
      return;

   time += getFrame();

   if (mBake.size() == 0)
   {
      Vector pos= mPosTrack.get(time);
      Quat   rot= mRotTrack.get(time);
      Matrix stm= mScaleTrack.get(time);
      float  flip= mFlipTrack.get(time);
      if (flip<0) flip=-1; else flip=1;

      flip=1;

      Matrix ptm;
      ptm.translate(pos);

      Matrix rtm(rot);

      Matrix ftm( Vector(flip,flip,flip) );

   //   if (mHasSkinning) printf("%s has skinning  ", name());
      if (!mUserTransform)
      {
         mTransform= stm * rtm * ftm * ptm;
   //   mTransform= rtm * ptm;

         if (parent())
            mTransform= mTransform * parent()->getTransform();
      }
   }
   else
   {
      mTransform= mBake.interpolate(time) * parent()->getTransform();
   }
}


void Node::setUserTransformable(bool state)
{
   mUserTransform= state;
}

bool Node::getUserTransformable() const
{
   return mUserTransform;
}

const BakedTransformation& Node::getBakedAnimation() const
{
   return mBake;
}

void Node::bakeAnimationTrack(float step)
{
   mBake= BakedTransformation(this, step);
}
