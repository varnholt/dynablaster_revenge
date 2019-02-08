#include "stdafx.h"
#include "morphkey.h"

MorphKey::MorphKey()
: KeyBase(0)
{
}

int MorphKey::getVertexCount() const
{
   return mVertices.size();
}

const List<Vector>& MorphKey::getVertices() const
{
   return mVertices;
}

const List<Vector>& MorphKey::getNormals() const
{
   return mNormals;
}

void MorphKey::load(Stream *stream)
{
   KeyBase::load(stream);
   mVertices << *stream;
}

void MorphKey::write(Stream *stream)
{
   KeyBase::write(stream);
   mVertices >> *stream;
}

void MorphKey::calculateNormals(const Array<unsigned short>& indexBuffer)
{
   int i;
   const int vcount= mVertices.size();
   const int icount= indexBuffer.size();
   mNormals.init( vcount );
   for (i=0; i<vcount; i++)
      mNormals[i].set(0.0f, 0.0f, 0.0f);

   for (i=0; i<icount; i+=3)
   {
      const int i1= indexBuffer[i];
      const int i2= indexBuffer[i+1];
      const int i3= indexBuffer[i+2];

      const Vector& v1= mVertices[i1];
      const Vector& v2= mVertices[i2];
      const Vector& v3= mVertices[i3];

      Vector n= (v2-v1) % (v3-v1);
      n.normalize();

      mNormals[i1] += n;
      mNormals[i2] += n;
      mNormals[i3] += n;
   }

   for (i=0; i<vcount; i++)
      mNormals[i].normalize();
}

