#pragma once

// GLES3 port of client/src/game/gameplayernamedisplay.cpp.

#include "framework/frametimer.h"
#include "math/vector.h"
#include "render/texture.h"

#include <map>
#include <string>
#include <vector>

class BitmapFont;
class PlayerItem;

class GamePlayerNameDisplay : public QObject
{
   Q_OBJECT

public:
   explicit GamePlayerNameDisplay(QObject* parent = nullptr);

   bool isActive() const;

public slots:
   void setPlayerData(std::map<int, PlayerItem*>& players);
   void draw() const;
   void drawPlayTexts() const;
   void drawArrow() const;
   void start();
   void initialize();

private:
   void initGlParameters() const;
   void cleanupGlParameters() const;
   float computeFontAlpha() const;
   float computeArrowAlpha() const;

   FrameTimer mActiveTime;
   std::vector<Vector> mPositions;
   std::vector<std::string> mNames;
   BitmapFont* mFont;
   bool mShowArrow;
   Texture mArrowTexture;
   Vector mArrowPosition;
   unsigned int mArrowShader;
   unsigned int mArrowVertexBuffer;
   unsigned int mArrowIndexBuffer;
   int mArrowParamTexture;
   int mArrowParamAlpha;
};
