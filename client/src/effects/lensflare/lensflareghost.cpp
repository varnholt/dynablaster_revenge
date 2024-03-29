// header
#include "lensflareghost.h"

// Qt
#include <QFile>
#include <QStringList>
#include <QTextStream>


LensFlareGhost::LensFlareGhost()
 : mSize(0.0f),
   mRayPosition(0.0f),
   mOffsetY(0.0f),
   mTileRow(0),
   mTileColumn(0),
   mWidth(0),
   mHeight(0),
   mAngle(0.0f),
   mSpeed(0.0f)
{
   // use dummy data for now
   mSize = 0.2f;
   mRayPosition = 0.5f;
   mOffsetY = -0.05f;
   mTileRow = 0;
   mTileColumn = 0;
   mColor = Vector(1.0f, 1.0f, 1.0f);
}


float LensFlareGhost::getSize() const
{
   return mSize;
}


void LensFlareGhost::setSize(float value)
{
   mSize = value;
}


float LensFlareGhost::getRayPosition() const
{
   return mRayPosition;
}


void LensFlareGhost::setRayPosition(float value)
{
   mRayPosition = value;
}


float LensFlareGhost::getOffsetY() const
{
   return mOffsetY;
}


void LensFlareGhost::setOffsetY(float value)
{
   mOffsetY = value;
}


int LensFlareGhost::getTileRow() const
{
   return mTileRow;
}


void LensFlareGhost::setTileRow(int value)
{
   mTileRow = value;
}


int LensFlareGhost::getTileColumn() const
{
   return mTileColumn;
}


void LensFlareGhost::setTileColumn(int value)
{
   mTileColumn = value;
}


Vector LensFlareGhost::getColor() const
{
   return mColor;
}


void LensFlareGhost::setColor(const Vector &value)
{
   mColor = value;
}


QList<LensFlareGhost> LensFlareGhost::readGhostStrip(const QString &fileName)
{
   QList<LensFlareGhost> strip;

   QFile file(fileName);

   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream in(&file);

      while (!in.atEnd())
      {
         QString line = in.readLine();

         QStringList elements = line.split(";");

         if (elements.size() >= 8)
         {
            bool ok = false;
            float x = 0.0f;
            float y = 0.0f;
            int col = 0;
            int row = 0;
            float size = 0.0;
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;
            int width = 1;
            int height = 1;
            float angle = 0.0f;
            float speed = 0.0f;

            x = elements[0].toFloat(&ok);
            if (!ok)
               continue;

            y = elements[1].toFloat(&ok);
            if (!ok)
               continue;

            col = elements[2].toInt(&ok);
            if (!ok)
               continue;

            row = elements[3].toInt(&ok);
            if (!ok)
               continue;

            size = elements[4].toFloat();
            if (!ok)
               continue;

            r = elements[5].toInt(&ok, 16)/255.0f;
            if (!ok)
               continue;

            g = elements[6].toInt(&ok, 16)/255.0f;
            if (!ok)
               continue;

            b = elements[7].toInt(&ok, 16)/255.0f;
            if (!ok)
               continue;

            if (elements.size() > 8)
            {
               int val = elements.at(8).toInt(&ok);

               if (ok)
               {
                  width = val;
                  height = val;
               }
            }

            if (elements.size() > 9)
            {
               int val = elements.at(9).toInt(&ok);

               if (ok)
               {
                  height = val;
               }
            }

            if (elements.size() > 10)
            {
               float val = elements.at(10).toFloat(&ok);

               if (ok)
               {
                  angle = val;
               }
            }

            if (elements.size() > 11)
            {
               float val = elements.at(11).toFloat(&ok);

               if (ok)
               {
                  speed = val;
               }
            }

            LensFlareGhost ghost;
            ghost.setRayPosition(x);
            ghost.setOffsetY(y);
            ghost.setTileColumn(col);
            ghost.setTileRow(row);
            ghost.setSize(size);
            ghost.setColor(Vector(r,g,b));
            ghost.setWidth(width);
            ghost.setHeight(height);
            ghost.setAngle(angle);
            ghost.setSpeed(speed);

            strip.append(ghost);
         }
      }
   }

   return strip;
}


float LensFlareGhost::getAngle() const
{
   return mAngle;
}


void LensFlareGhost::setAngle(float value)
{
   mAngle = value;
}


float LensFlareGhost::getSpeed() const
{
   return mSpeed;
}


void LensFlareGhost::setSpeed(float value)
{
   mSpeed = value;
}


int LensFlareGhost::getHeight() const
{
   return mHeight;
}

void LensFlareGhost::setHeight(int value)
{
   mHeight = value;
}


int LensFlareGhost::getWidth() const
{
   return mWidth;
}


void LensFlareGhost::setWidth(int fields)
{
   mWidth = fields;
}


