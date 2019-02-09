// shape object
// contains a number of polylines

#pragma once

#include "node.h"
#include "tools/list.h"

class Stream;

class Shape : public Node
{
public:
   class PolyLine
   {
   public:
      PolyLine()
         : mFlags(0)
      {
      }

      bool closed()
      {
         return (mFlags & 1);
      }

      void load(Stream *stream)
      {
         mFlags= stream->getInt();

         int count= stream->getInt();
         while (count--) stream->getInt();

         mVertices.load(stream);
      }

      int getVertexCount() const
      {
         return mVertices.size();
      }

      const Vector& getVertex(int index) const
      {
         return mVertices[index];
      }

   private:
      int mFlags;
      Array<int>    mTypes;
      List<Vector> mVertices;
   };

           Shape(Node *parent = 0);
   virtual ~Shape();
   void    load(Stream *stream);
   void    write(Stream *stream);

   int     getPolyCount() const;
   PolyLine* getPoly(int count) const;

private:
   Array<PolyLine*> mPolys;
};
