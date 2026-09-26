#ifndef PLAYER_H
#define PLAYER_H

#include "math/vector.h"
#include "constants.h"

#include <string>

class Material;
class Mesh;

class PlayerItem
{

public:

   PlayerItem(int id, const std::string& nick, Constants::Color color);
   ~PlayerItem();

   int getID() const;
   Constants::Color getColor() const;
   const std::string& getNick() const;

   void setMaterial(Material *mat);
   Material* getMaterial() const;

   void setMesh(Mesh *mesh);
   Mesh* getMesh() const;

   void setRotation(float rot);
   void setPosition(float x, float y);
   const Vector& getPosition() const;

   void setSpeed(float speed);
   float getSpeed() const;

   void animate(float time, float delta);
   void kill();
   void setKilled(bool isKilled);
   bool isKilled() const;
   void win();
   bool isWinner() const;

   void setFlash(float flash);


private:

   void update();

   int mID;
   Constants::Color mColor;
   std::string mNick;
   Mesh *mMesh;
   Material *mMaterial;
   Vector mPos;
   float mRot;
   float mSpeed;
   float mAnimBlend;
   float mStandBlend;
   bool  mKilled;
   bool  mWin;
   bool  mLeftFoot;
   float mFlash;
};

#endif
