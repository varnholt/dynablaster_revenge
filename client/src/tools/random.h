#pragma once

#include <stdlib.h>

static const float invRange= 1.0f / 16383.0f;

inline float frand(float max)
{
    return (rand() & 16383) * invRange * max;
}

inline float frands(float min, float max)
{
    return min + (rand() & 16383) * invRange * (max - min);
}
