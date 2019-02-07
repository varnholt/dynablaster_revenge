#include "stdafx.h"
#include "omni.h"
#include "tools/stream.h"

Omni::Omni(Node *parent)
: Light(Node::idOmni, parent),
  mAttNear(0.0f),
  mAttFar(0.0f)
{
}


void Omni::load(Stream *stream)
{
//   mFlags= stream->getInt();

//   mAttNear= stream->getFloat();
//   mAttFar= stream->getFloat();

   Light::load(stream);
}

void Omni::write(Stream *stream)
{
   Light::write(stream);
}