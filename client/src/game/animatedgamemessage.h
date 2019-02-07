#ifndef ANIMATEDGAMEMESSAGE_H
#define ANIMATEDGAMEMESSAGE_H


#include "gamemessage.h"

#include "tools/array.h"
#include "vertex.h"

class AnimatedGameMessage : public GameMessage
{
public:

    AnimatedGameMessage(QObject* parent = 0);

    //! setter for vertices
    void setVertices(const Array<Vertex>& vertices);

    //! getter for ptr to vertex array
    const Array<Vertex>& getVertices() const;

    //! getter for the alpha value
    float getAlpha() const;

    //! initialize message
    virtual void initialize();


protected:

    float mAlpha;
    Array<Vertex> mVertices;
};

#endif // ANIMATEDGAMEMESSAGE_H
