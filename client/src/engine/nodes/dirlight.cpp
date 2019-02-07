#include "stdafx.h"
#include "dirlight.h"

DirLight::DirLight(Node *parent)
   : Light(Node::idDir, parent)
{
}

