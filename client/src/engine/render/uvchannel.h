#pragma once

#include "uv.h"
#include "tools/list.h"
#include "tools/stream.h"

class UVChannel : public Streamable
{
public:
  UVChannel();
  UVChannel(int id, UV* uv, int size);
  virtual ~UVChannel();

  void load(Stream *stream);
  void write(Stream *stream);
  void copy(const UVChannel& other);
  const List<UV>& getUV() const;

  int id() const;

  UV* data() const;

private:
   int mID;
   List<UV> mUV;
};
