#include "stdafx.h"
#include "light.h"

Light::Light(Node::ID id, Node *parent)
: Node(id, parent),
  mFlags(0),
  mMapSize(0),
  mAttStart(0.0f),
  mAttEnd(0.0f)
{
}

Light::~Light()
{
}

void Light::transform(float time)
{
   Node::transform(time);
   mColor= mColTrack.get(time);
}


void Light::load(Stream *stream)
{
   mFlags= stream->getInt();
//   mMapSize= stream->getInt();

   mAttStart= stream->getFloat();
   mAttEnd= stream->getFloat();

   // load exclude list
   Chunk list(stream);
   int size= list.getInt();
   mExclude.init(size);
   for (int i=0;i<size;i++) mExclude.add( list.getInt() );
   list.skip();

   Chunk anim(&list);
   mColTrack.load(&anim);
   mPosTrack.load(&anim);
   mRotTrack.load(&anim);
   mScaleTrack.load(&anim);
   mVisTrack.load(&anim);
   mFlipTrack.load(&anim);
   anim.skip();
}

void Light::write(Stream *stream)
{
   // load exclude list
   {
   Chunk list(stream, 500, "Light");
  
   list.writeInt( mExclude.size() );
   for (int i=0; i<mExclude.size(); i++) 
      list.writeInt( mExclude[i] );
   }

   Chunk anim(stream, 2000, "Animation");
   mColTrack.write(&anim);
   mPosTrack.write(&anim);
   mRotTrack.write(&anim);
   mScaleTrack.write(&anim);
   mVisTrack.write(&anim);
   mFlipTrack.write(&anim);
}


Vector Light::getColor() const
{
   return mColor;
}
