#pragma once

#include <QList>

class Detonation;

class DetonationManager
{
public:
   DetonationManager();
   virtual ~DetonationManager();

   void init();
   void clear();
   void addDetonation(int x, int y, int top, int bottom, int left, int right);

   void update(float time);
   void render();

private:
   void drawExplosion(Detonation *det, float time);
   void drawBox(float x, float y, float z, float left, float right, float bottom, float top, int sides);

   float              mTime;
   unsigned int mShader;

   unsigned int mNoiseMap;
   unsigned int mGradientMap;

   int mParamTime;
   int mParamCamPos;
   int mParamTop;
   int mParamBottom;
   int mParamLeft;
   int mParamRight;
   int mParamBoundMin;
   int mParamBoundMax;
   int mParamNoiseMap;
   int mParamGradientMap;
   QList<Detonation*> mDetonations;
};
