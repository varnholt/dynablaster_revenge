#include "motionmixer.h"
#include "nodes/scenegraph.h"
#include "nodes/node.h"
#include "nodes/mesh.h"
#include "math/matrix.h"

// cmath
#include "math.h"

SceneGraph*             MotionMixer::mRefAnim = 0;
Array<BoneAnimPrecalc*> MotionMixer::mMatrixPrecalc;
float                   MotionMixer::mFrameStep= 50.0f;
int                     MotionMixer::mBoneCount= 0;
int                     MotionMixer::mAnimLength= 0;

MotionMixer::MotionMixer()
   : mAnim1(-1)
   , mAnim2(-1)
   , mAnim3(-1)
   , mWeight1(1.0f)
   , mWeight2(0.0f)
{
   for (int i=0; i<mBoneCount; i++)
   {
      Node* node= new Node(Node::idDummy, 0);
      mCurrentState.add( node );
   }
}

MotionMixer::~MotionMixer()
{
}

// final cleanup of static data
void MotionMixer::cleanup()
{
   if (mRefAnim)
      delete mRefAnim;

   for (int i=0; i<mMatrixPrecalc.size(); i++)
   {
      BoneAnimPrecalc* precalc= mMatrixPrecalc[i];
      for (int j=0; j<precalc->size(); j++)
      {
         Array<Matrix>* list= precalc->get(j);
         delete list;
      }
      delete precalc;
   }
}

Node* matchTreeNode(Node* treeRoot, Node* source)
{
   Array<String> nameList;
   while (source)
   {
      nameList.add( source->name() );
      source= source->parent();
   }

   for (int i=nameList.size()-2;i>=0 && treeRoot;)
   {
      const String& name= nameList[i];
      Node* child= treeRoot->getChild(name);
      if (child) 
      {
         treeRoot= child;
         i--;
      }
      else
         treeRoot= treeRoot->getChild(0);
   }

   return treeRoot;
}

int MotionMixer::addAnimation(const String& name)
{
   int index= mMatrixPrecalc.size();
   SceneGraph *scene= new SceneGraph();
   scene->load(name);

   if (!mRefAnim)
      mRefAnim= scene;

   // match nodes to reference animation
   const Array<Node*>& refNodes= mRefAnim->nodeList();
   Array<Node*> nodeMap( refNodes.size() );
   for (int i=0; i<refNodes.size(); i++)
   {
      Node* node= refNodes[i];

      Node* source= matchTreeNode(scene, node);
      if (!source)
      {
         printf("node match fail!\n");
         source= scene->getNode( node->name() );
         if (!source)
            source= refNodes[i];
      }
      nodeMap.add( source );
   }

   // find required bone nodes
   Array<Bone*> boneList;   // bone initial transform
   for (int i=0; i<scene->nodeList().size(); i++)
   {
      Node *node= scene->getNode(i);
      int nodeAnimLength= node->getAnimationLength();
      if (nodeAnimLength > mAnimLength)
         mAnimLength= nodeAnimLength;

      if (node->id() == Node::idMesh)
      {
         Mesh *mesh= (Mesh*)node;
         for (int p=0; p<mesh->getPartCount(); p++)
         {
            Geometry *geo= mesh->getPart(p);
            const List<Bone>& bones= geo->getBoneList();
            for (int b=0; b<bones.size(); b++)
            {
               Bone* bone= (Bone*)&bones[b];
               boneList.add( bone );
            }
         }
      }
   }

   int frames= (int)ceil(mAnimLength / mFrameStep);

   if (boneList.size() > mBoneCount)
      mBoneCount= boneList.size();

   // adjust length of existing animations
   for (int i=0; i<mMatrixPrecalc.size(); i++)
   {
      BoneAnimPrecalc* precalc= mMatrixPrecalc[i];
      Array<Matrix>* nodes= precalc->get( precalc->size()-1 );
      for (int n=precalc->size(); n<frames; n++)
      {
         Array<Matrix>* copy= new Array<Matrix>( boneList.size() );
         for (int j=0; j<nodes->size(); j++)
            copy->add( nodes->get(j) );
         precalc->add( copy );
      }
   }

   // precalc animation matrices for relevant nodes
   BoneAnimPrecalc* precalc= new BoneAnimPrecalc( frames );

   Matrix cam; // identity
   for (int fr=0; fr<=frames; fr++)
   {
      Array<Matrix> *anim= new Array<Matrix>( boneList.size() );

      float frame= fr*mFrameStep;

      // transform all scene nodes
      for (int i=0; i<scene->nodeList().size(); i++)
         scene->getNode(i)->transform(frame);

      // store relevant node matrices
      for (int i=0; i<boneList.size(); i++)
      {
         Bone* bone= boneList[i];
         Node* node= scene->getNode( bone->id() );
         anim->add( bone->transform() * node->getTransform() );
      }
      precalc->add( anim );
   }
   mMatrixPrecalc.add( precalc );

   // fix bone ids
   for (int i=0; i<boneList.size(); i++)
      boneList[i]->setId(i);

   if (mRefAnim != scene)
      delete scene;

   return index;
}

void MotionMixer::setAnimation(int anim1, int anim2, float weight1, int anim3, float weight2)
{
   mAnim1= anim1;
   mAnim2= anim2;
   mAnim3= anim3;
   mWeight1= weight1;
   mWeight2= weight2;
}

void MotionMixer::animate(float frame)
{
   Matrix m;

   if (mAnim1<0 || mAnim2<0)
      return;

   BoneAnimPrecalc* anim1= mMatrixPrecalc[mAnim1];
   BoneAnimPrecalc* anim2= mMatrixPrecalc[mAnim2];

   frame/=50.0f;
   if (frame<0.0)
      frame= 0.0f;
   int index= floor(frame);
   if (index>anim1->size()-2)
      index=anim1->size()-2;
   float t= (index+1)-frame;

   Array<Matrix>* a11= anim1->get(index);
   Array<Matrix>* a12= anim1->get(index+1);

   Array<Matrix>* a21= anim2->get(index);
   Array<Matrix>* a22= anim2->get(index+1);

   for (int i=0; i<mCurrentState.size(); i++)
   {

      Matrix m1= Matrix::blend( a11->get(i), a12->get(i), t);
      Matrix m2= Matrix::blend( a21->get(i), a22->get(i), t);
      m= Matrix::blend( m1, m2, mWeight1 );
      mCurrentState[i]->setTransform( m );
   }

   if (mWeight2 > 0.0f && mAnim3>=0)
   {
      BoneAnimPrecalc* anim3= mMatrixPrecalc[mAnim3];

      Array<Matrix>* a31= anim3->get(index);
      Array<Matrix>* a32= anim3->get(index+1);

      for (int i=0; i<mCurrentState.size(); i++)
      {
         const Matrix& m1= mCurrentState[i]->getTransform();
         Matrix m3= Matrix::blend( a31->get(i), a32->get(i), t);
         m= Matrix::blend( m1, m3, mWeight2 );
         mCurrentState[i]->setTransform( m );
      }
   }

/*
   const Array<Node*>& src1= mAnimations[mAnim1]->nodeList();
   for (int i=0; i<src1.size(); i++)
      src1[i]->transform(frame);

   const Array<Node*>& src2= mAnimations[mAnim2]->nodeList();
   for (int i=0; i<src2.size(); i++)
      src2[i]->transform(frame);

   const Array<Node*>& src3= mAnimations[mAnim3]->nodeList();
   for (int i=0; i<src3.size(); i++)
      src3[i]->transform(frame);

   Array<Node*>* map1= mNodeMap[mAnim1];
   Array<Node*>* map2= mNodeMap[mAnim2];

   for (int i=0; i<mCurrentState.size(); i++)
   {
      Node* n1= map1->get(i);
      Node* n2= map2->get(i);

//      printf("%s %s \n", (const char*)n1->name(), (const char*)n2->name());
      const Matrix& m1= n1->getTransform();
      const Matrix& m2= n2->getTransform();// * Matrix::scale(0.01f, 0.01f, 0.01f);
      m= Matrix::blend( m1, m2, mWeight1 );

      mCurrentState[i]->setTransform( m );
   }

   if (mWeight2 > 0.0f)
   {
      Array<Node*>* map3= mNodeMap[mAnim3];
      for (int i=0; i<mCurrentState.size(); i++)
      {
         Node* n3= map3->get(i);
         const Matrix& m1= mCurrentState[i]->getTransform();// * Matrix::scale(0.01f, 0.01f, 0.01f);
         const Matrix& m3= n3->getTransform();// * Matrix::scale(0.01f, 0.01f, 0.01f);
         m= Matrix::blend( m1, m3, mWeight2 );
         mCurrentState[i]->setTransform( m );
      }
   }
*/
}

Mesh* MotionMixer::getMesh(const String& name)
{
   if (mRefAnim)
      return (Mesh*)mRefAnim->getNode( name );
   else
      return 0;
}

Node* MotionMixer::getNode( int index ) const
{
   return mCurrentState[index];
}
