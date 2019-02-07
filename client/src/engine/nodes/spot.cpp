#include "stdafx.h"
#include "spot.h"
#include "tools/stream.h"

Spot::Spot(Node *parent)
: Light(Node::idSpot, parent)
, mShape(0)
, mHotspot(0.0f)
, mFallSize(0.0f)
{
}


void Spot::load(Stream *stream)
{
   mShape= stream->getInt();
   mHotspot= stream->getFloat();
   mFallSize= stream->getFloat();

   Light::load(stream);
}


void Spot::write(Stream *stream)
{
   stream->writeInt( mFlags );
   stream->writeFloat( mHotspot );
   stream->writeFloat( mFallSize );

   Light::write(stream);
}

