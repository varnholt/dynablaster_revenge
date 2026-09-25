#pragma once

// GLES3 port of client/src/game/gameplayernamedisplay.cpp.

#include <QList>
#include <QMap>
#include <QString>

#include "framework/frametimer.h"
#include "math/vector.h"
#include "render/texture.h"

class BitmapFont;
class PlayerItem;

class GamePlayerNameDisplay : public QObject
{
   Q_OBJECT

public:
   explicit GamePlayerNameDisplay(QObject* parent = nullptr);

   bool isActive() const;

public slots:
   void setPlayerData(QMap<int, PlayerItem*>& players);
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
   QList<Vector> mPositions;
   QList<QString> mNames;
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
