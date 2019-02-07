// chunk class (scene-graph traversal helper-class)
// a chunk holds one node of the scene graph inside the .hjb file-format
// the chunk starts at the current stream position and contains at least an id to describe the node type (mesh,light,etc),
// name and size of the node.
// since the chunk-size and stream-position are known, a chunk can be skipped at any time, no matter how much data of the
// actual stream have already been read inside the chunk, guiding the stream to the start of the next chunk.

#ifndef CHUNK_H
#define CHUNK_H

class Stream;

class Chunk
{
public:
   Chunk(Stream *stream);
   ~Chunk();
   void  skip();
   int   id() const;
   const char* name() const;

private:
  int    mID;
  char   *mName;
  int    mSize;
  Stream *mStream;
  int    mChunkPos;
};

#endif
