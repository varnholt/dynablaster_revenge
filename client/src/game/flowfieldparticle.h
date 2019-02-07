#ifndef FLOWFIELDPARTICLE_H
#define FLOWFIELDPARTICLE_H

#include "math/vector.h"

// Qt
#include <QColor>

class FlowFieldParticle
{

public:

    //! constructor
    FlowFieldParticle();

    //! particle position
    Vector mPosition;

    float mAlpha;

    float mFade;

    //! particle color
    QRgb mColor;
};

#endif // FLOWFIELDPARTICLE_H
