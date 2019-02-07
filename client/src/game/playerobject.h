#ifndef PLAYEROBJECT_H
#define PLAYEROBJECT_H

#include <QString>
#include "math/vector.h"

class Material;
class Mesh;

class PlayerObject
{
public:
	PlayerObject(int id, const QString& nick);
	~PlayerObject();

	int getID() const;
	const QString& getNick() const;

	void setMaterial(Material *mat);
	Material* getMaterial() const;

	void setMesh(Mesh *mesh);
   Mesh* getMesh() const;

	void setRotation(float rot);
	void setPosition(float x, float y);

   void setSpeed(float speed);
   float getSpeed() const;

private:
	void update();

	int mID;
	QString mNick;
	Mesh *mMesh;
	Material *mMaterial;
	Vector mPos;
	float mRot;
   float mSpeed;
};

#endif
