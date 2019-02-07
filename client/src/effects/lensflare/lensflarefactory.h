#pragma once

// Qt
#include <QGLWidget>

// framework
#include "framework/frametimer.h"

// math
#include "math/vector.h"

// forward declarations
class RenderDevice;
class LensFlare;

class LensFlareFactory : public QObject
{
    Q_OBJECT
    
public:

   //! constructor
   LensFlareFactory();

   //! destructor
   ~LensFlareFactory();

   //! singleton getter
   static LensFlareFactory* getInstance();

   //! initialize gl context
   void initialize();

   //! paint
   void draw();

   //! activate flare
   bool activate(const QString& key);

   //! getter for flare groups
   QList<QString> getKeys() const;


protected:

   //! draw lines around file
   void drawField();

   //! draw sun
   void drawSun();

   //! hightlight 0x0
   void drawCenter();

   //! highlight the flare line
   void drawFlareLine(float x, float y);

   //! draw the sun's 2d coords
   void drawSun2dCoords(float x, float y);

   //! generate sun 2d coords
   void make2d(
      float aspectY,
      float& x,
      float& y,
      float &xTranslate,
      float &yTranslate
   );

   //! list of animations
   QMap<QString, LensFlare*> mLensFlares;

   //! active lensflare
   LensFlare* mActive;

   //! render device
   RenderDevice* mDevice;

   //! static instance of lensflare factory
   static LensFlareFactory* sLensFlareFactory;
};

