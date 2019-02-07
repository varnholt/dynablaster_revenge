#include "stdafx.h"
#include "dummy.h"
#include "tools/stream.h"

Dummy::Dummy(Node *parent)
: Node(Node::idDummy, parent)
{
}

Dummy::~Dummy()
{
}

void Dummy::load(Stream *stream)
{
   Node::load(stream);

   // read tracks
   Chunk anim(stream);
   mPosTrack.load(&anim);
   mRotTrack.load(&anim);
   mScaleTrack.load(&anim);
   mVisTrack.load(&anim);
   mFlipTrack.load(&anim);
   anim.skip();
}

void Dummy::write(Stream *stream)
{
   Node::write(stream);

   // write tracks
   Chunk anim(stream, 2000, "Animation");
   mPosTrack.write(&anim);
   mRotTrack.write(&anim);
   mScaleTrack.write(&anim);
   mVisTrack.write(&anim);
   mFlipTrack.write(&anim);
}
