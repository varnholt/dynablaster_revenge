#pragma once

// GLES3 port of client/src/game/animatedgamemessage.cpp.

#include "gamemessage.h"

#include "tools/array.h"
#include "menus/vertex.h"

class AnimatedGameMessage : public GameMessage
{
public:
   AnimatedGameMessage(QObject* parent = nullptr);

   void setVertices(const Array<Vertex>& vertices);
   const Array<Vertex>& getVertices() const;

   float getAlpha() const;

   virtual void initialize();

protected:
   float mAlpha;
   Array<Vertex> mVertices;
};
