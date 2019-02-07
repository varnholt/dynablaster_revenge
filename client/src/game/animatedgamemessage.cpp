#include "animatedgamemessage.h"

AnimatedGameMessage::AnimatedGameMessage(QObject* parent)
   : GameMessage(parent),
     mAlpha(1.0f)
{
}


void AnimatedGameMessage::setVertices(const Array<Vertex>& vertices)
{
   mVertices.copy(vertices);
}


const Array<Vertex>& AnimatedGameMessage::getVertices() const
{
   return mVertices;
}


float AnimatedGameMessage::getAlpha() const
{
   return mAlpha;
}


void AnimatedGameMessage::initialize()
{
   GameMessage::initialize();
}
