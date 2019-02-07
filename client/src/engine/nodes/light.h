// light object
// all lights have a colortrack and an exclude list (list of object-ids which are not affected by a light)

#pragma once

#include "node.h"
#include "render/idlist.h"
#include "animation/postrack.h"

class Stream;

class Light : public Node
{
public:
                Light(Node::ID id, Node *parent = 0);
   virtual      ~Light();
   Vector       getColor() const;
   virtual void transform(float frame);
   virtual void load(Stream *stream);
   virtual void write(Stream *stream);

protected:
   int        mFlags;
   int        mMapSize;
   float      mAttStart;
   float      mAttEnd;
   Vector     mColor;
   PosTrack   mColTrack;
   IDList     mExclude;
};
