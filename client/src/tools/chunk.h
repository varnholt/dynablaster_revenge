// chunk class (scene-graph traversal helper-class)
// a chunk holds one node of the scene graph inside the .hjb file-format
// the chunk starts at the current stream position and contains at least an id to describe the node type (mesh,light,etc),
// name and size of the node.
// since the chunk-size and stream-position are known, a chunk can be skipped at any time, no matter how much data of the
// actual stream have already been read inside the chunk, guiding the stream to the start of the next chunk.

#pragma once

#include "stream.h"
#include "objectname.h"
#include "array.h"
#include "string.h"

class Chunk : public Stream, public ObjectName
{
public:
   enum AccessMode
   {
      chunkRead = 0,
      chunkWrite= 1,
   };

   Chunk(Stream *stream);
   Chunk(Stream *stream, int id, const String& name);
   ~Chunk();

   void        getData(void *src, int size);
   void        writeData(void *src, int size);

   int         id() const;

   int         dataLeft() const;
   void        skip();

private:
   Stream*          mStream;
   AccessMode       mMode;
   int              mID;
   int              mSize;
   int              mChunkPos;

   char*            mBuffer;
   Array<char*>     mBuffers;
   int              mBufferPos;
};

