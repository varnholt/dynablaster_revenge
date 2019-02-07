// template track class
// holds a list of animation keys

#pragma once

#include "tools/stream.h"
#include "tools/list.h"
#include "tools/chunk.h"
#include <stdio.h>

template <class KeyClass> class Track : public List<KeyClass>
{
public:
   enum Type
   {
      idUndefined = 0,
      idValue = 2001,
      idPosition = 2002,
      idRotation = 2003,
      idScale = 2004,
      idVisibility= 2005,
      idVertexMorph = 2010
   };

   Track(Type type, const String& name = String())
   : List<KeyClass>()
   , mType(type)
   , mName(name)
   , mCurKey(0)
   {
      if (mName.isEmpty())
      {
         switch (mType)
         {
            case idUndefined: mName= "Undefined"; break;
            case idValue: mName= "Value"; break;
            case idPosition: mName= "Position"; break;
            case idRotation: mName= "Rotation"; break;
            case idScale: mName= "Scale"; break;
            case idVisibility: mName= "Visible"; break;
            case idVertexMorph: mName= "VertexMorph"; break;
            default: mName= "Undefined"; break;
         }
      }
   }

   virtual ~Track()
   {
   }

   void addKey(const KeyClass& key)
   {
      this->add(key);
   }

   int getAnimationLength() const
   {
      int count= this->size();
      if (count > 0)
         return this->mData[count-1].time();
      else
         return 0;
   }

   virtual void load(Stream *stream)
   {
      Chunk track(stream);

      if (track.id() == mType)
      {
         List<KeyClass>::load(&track);
      }
      else
      {
         printf("wrong track!\n");
      }

      track.skip();
   }

   String trackName(int id)
   {
      return 0;
   }

   virtual void write(Stream *stream)
   {
      Chunk track(stream, mType, mName);

      List<KeyClass>::write(&track);
   }

   float interpolate(float time)
   {
      while (mCurKey>0 && time<key(mCurKey).time()) mCurKey--;
      while (mCurKey<Array<KeyClass>::size()-1 && time>=key(mCurKey+1).time()) mCurKey++;

      int len= nextKey().time() - prevKey().time();
      float prog= time - prevKey().time();

      float frac= prog / len;

      return frac;
   }

   KeyClass& prevKey() const
   {
      return Array<KeyClass>::mData[mCurKey];
   }

   KeyClass& nextKey() const
   {
      return Array<KeyClass>::mData[mCurKey+1];
   }

   KeyClass& key(int index) const
   {
      return Array<KeyClass>::mData[index];
   }

protected:
   Type mType;
   String mName;
   int mCurKey;
};
