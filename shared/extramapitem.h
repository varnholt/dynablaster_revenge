#ifndef EXTRAMAPITEM_H
#define EXTRAMAPITEM_H

// base
#include "mapitem.h"

// shared
#include "constants.h"

// Qt
#include <QElapsedTimer>
#include <QList>

// forward declarations
class ExtraMapItemCreatedPacket;

class ExtraMapItem : public MapItem
{
   public:

      //! constructor
      ExtraMapItem(
         int id,
         Constants::ExtraType type,
         int x,
         int y
      );

      //! construct from packet
      ExtraMapItem(ExtraMapItemCreatedPacket* packet);

      //! destructor
      virtual ~ExtraMapItem();

      //! getter for extra type
      Constants::ExtraType getExtraType();

      //! initialize start time
      void initializeStartTime();

      //! getter elapsed time
      float getElapsedTime() const;

      //! setter for skull faces
      void setSkullFaces(const QList<Constants::SkullType>& faces);

      //! getter for skull faces
      QList<Constants::SkullType> getSkullFaces() const;


   private:

      //! extra type
      Constants::ExtraType mExtraType;

      //! time since extra map item is visible
      QElapsedTimer mStartTime;

      //! skull face setup
      QList<Constants::SkullType> mSkullFaces;
};

#endif // EXTRAMAPITEM_H
