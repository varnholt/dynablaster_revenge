#include "stdafx.h"
#include "shape.h"
#include "tools/stream.h"

Shape::Shape(Node *parent)
: Node(Node::idShape, parent)
{
}

Shape::~Shape()
{
}

void Shape::load(Stream *stream)
{
   Node::load(stream);

   int numPolys= stream->getInt();
   mPolys.init(numPolys);
   for (int i=0;i<numPolys;i++)
   {
      Chunk polyChunk(stream);
      PolyLine* poly= new PolyLine();
      poly->load(&polyChunk);
      mPolys.add(poly);
      polyChunk.skip();
   }

   // read tracks
   Chunk anim(stream);
   mPosTrack.load(&anim);
   mRotTrack.load(&anim);
   mScaleTrack.load(&anim);
   mVisTrack.load(&anim);
   mFlipTrack.load(&anim);
   anim.skip();
}

void Shape::write(Stream *stream)
{
   Node::write(stream);

   // todo:
   // write poly lines

   // write tracks
   Chunk anim(stream, 2000, "Animation");
   mPosTrack.write(&anim);
   mRotTrack.write(&anim);
   mScaleTrack.write(&anim);
   mVisTrack.write(&anim);
   mFlipTrack.write(&anim);
}

int Shape::getPolyCount() const
{
   return mPolys.size();
}

Shape::PolyLine* Shape::getPoly(int index) const
{
   return mPolys[index];
}
