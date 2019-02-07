#include "playerobject.h"
#include "math/matrix.h"
#include "nodes/mesh.h"

PlayerObject::PlayerObject(int id, const QString& nick)
: mID(id)
, mNick(nick)
, mMaterial(0)
, mMesh(0)
, mPos(0,0,0)
, mRot(0)
, mSpeed(0.0f)
{
}

PlayerObject::~PlayerObject()
{
   if (mMesh)
      delete mMesh;
}

int PlayerObject::getID() const
{
	return mID;
}

const QString& PlayerObject::getNick() const
{
	return mNick;
}

void PlayerObject::setMaterial(Material *mat)
{
	mMaterial= mat;
}

Material* PlayerObject::getMaterial() const
{
	return mMaterial;
}

void PlayerObject::setMesh(Mesh *mesh)
{
	mMesh= mesh;
}

Mesh* PlayerObject::getMesh() const
{
   return mMesh;
}

void PlayerObject::setRotation(float rot)
{
	mRot= rot;
	update();
}

void PlayerObject::setPosition(float x, float y)
{
	mPos= Vector(x,-y,0);
	update();
}


void PlayerObject::update()
{
   Matrix pos= Matrix::position(mPos.x, mPos.y, 0.0f);
   Matrix mat= Matrix::rotateZ(mRot);
   Matrix scale= Matrix::scale(0.03, 0.03, 0.03);
	mMesh->setUserTransformable(true);
	mMesh->setTransform(mat * scale * pos);
}

void PlayerObject::setSpeed(float speed)
{
   mSpeed= speed;
}

float PlayerObject::getSpeed() const
{
   return mSpeed;
}
