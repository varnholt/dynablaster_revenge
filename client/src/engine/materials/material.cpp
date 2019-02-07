// reference implementation of a dummy material

#include "stdafx.h"
#include "material.h"
#include "textureslot.h"
#include "tools/stream.h"
#include "nodes/mesh.h"
#include "nodes/scenegraph.h"
#include "image/image.h"
#include "render/geometry.h"
#include "render/renderbuffer.h"
#include "render/vertexbuffer.h"
#include "renderdevice.h"
#include "render/texturepool.h"
#include "render/texture.h"
#include "image/imagepool.h"
#include "gldevice.h"


QFileSystemWatcher* Material::mWatcher= 0;
QMultiMap<QString,QPair<Texture*,QString> > Material::mWatchers;

Material::Buffer::Buffer()
: geo(0)
, vb(0)
{
}

Material::Buffer::Buffer(Geometry *g, VertexBuffer *v)
: geo(g)
, vb(v)
{
}

Material::Material(SceneGraph *scene, int id)
: mId(id)
, mInitialized(false)
, mPool(0)
, mDebug(0)
{
   mPool= new VertexBufferPool();
   if (scene)
      scene->addMaterial(this);
}

Material::~Material()
{
   if (mPool)
      delete mPool;
}

void Material::getBoundingRect(Vector& min, Vector& max, const Matrix&)
{
   max= min= Vector(0.0f, 0.0f, 0.0f);
}

Vector Material::getCenter2d(const Matrix&) const
{
   return Vector(0.0f);
}

const Array<Material::Buffer>& Material::getBuffers() const
{
   return mVB;
}

//! get geometry by given index
Geometry* Material::getGeometry(int index) const
{
   if (index>=0 && index<mVB.size())
      return mVB[index].geo;
   else
      return 0;
}

int Material::getDebug() const
{
   return mDebug;
}

void Material::setDebug(int v)
{
   mDebug= v;
}

int Material::geometryCount() const
{
   return mVB.size();
}

void Material::add(Geometry* geo)
{
   mGeometryQueue.add(geo);
}

int Material::size() const
{
   return mVB.size();
}

void Material::addTexture(Texture& texture, const char* filename, int flags)
{
//   Image* image= ImagePool::Instance()->getImage(filename);
   Image* image= new Image(filename);
   addTexture(texture, image, flags);
}

void Material::addWatcher(Texture* texture, Image* image)
{
   if (!mWatcher)
   {
      mWatcher= new QFileSystemWatcher();
      connect(mWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));

      QString prev;
      QMultiMap<QString,QPair<Texture*,QString> >::ConstIterator it;
      for (it= mWatchers.constBegin(); it!= mWatchers.constEnd(); it++)
      {
         if (it.key() != prev)
            mWatcher->addPath( it.key() );
         prev= it.key();
      }
   }

   QString path( (const char*)image->path() );
   QString filename( (const char*)image->filename() );
   mWatcher->addPath( path );
   mWatchers.insert(
      path,
      QPair<Texture*,QString>(texture, filename)
   );
}

void Material::fileChanged(const QString& path)
{
   delete mWatcher;
   mWatcher= 0;

   QMultiMap<QString,QPair<Texture*,QString> >::Iterator it;
   it= mWatchers.find( path );
   while (it != mWatchers.end() && it.key() == path)
   {
      Texture* texture= it.value().first;
      QString filename= it.value().second;
      it= mWatchers.erase(it);
      addTexture(*texture, qPrintable(filename));
   }
}

void Material::addTexture(Texture& texture, Image* image, int flags)
{
//   addWatcher(&texture, image);

   mImageQueue.add( image );
   mImageFlagsQueue.add( flags );
   mTextureIdQueue.add( &texture );
}

void Material::prepare()
{
   while (mGeometryQueue.size()>0)
   {
      Geometry* geo= mGeometryQueue.takeLast();
      addGeometry(geo);
   }

   while (mImageQueue.size()>0 && mTextureIdQueue.size()>0)
   {
      Image* image= mImageQueue.takeLast();
      int flags= mImageFlagsQueue.takeLast();
      Texture* texture= mTextureIdQueue.takeLast();
      *texture= TexturePool::Instance()->getTexture( image, flags );
      delete image;
   }
}

void Material::begin()
{
   if (!mInitialized)
   {
      init();
      mInitialized= true;
   }

   prepare();
}

unsigned int Material::uploadMap(const Image& image, int flags)
{
   return activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight(), flags);
}


void Material::updateMap(unsigned int texture, const Image& image, int flags)
{
   glBindTexture(GL_TEXTURE_2D, texture);

   return activeDevice->updateTexture(image.getData(), image.getWidth(), image.getHeight(), flags);
}


unsigned int Material::uploadCubeMap(const Image& image)
{
	unsigned int tex;

   int x= image.getWidth();
   int y= image.getHeight();

	if (x/3 != y/4)
	{
		printf("expected cubemap as vertical cross!\n");
		return 0;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int size= y >> 2;

	unsigned int target[6]={
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	unsigned char pos[6]={0x12, 0x10, 0x01, 0x21, 0x11, 0x31};

   for (int s=0;s<6;s++)
	{
		int yp= (pos[s]>>4&3)*size;
		int xp= (pos[s]&3)*size;

      Image temp(size, size);

		for (int i=0;i<size;i++)
		{
			unsigned int *src= image.getScanline(yp+i) + xp;
         unsigned int *dst;
			if (s==5) // back-side must be flipped
         {
            dst= temp.getScanline(size-i-1);
				for (int j=0;j<size;j++) dst[j]= src[size-1-j];
         }
			else
         {
            dst= temp.getScanline(i);
				for (int j=0;j<size;j++) dst[j]= src[j];
         }
		}

      int level=0;
      do {
		   glTexImage2D(
            target[s], 
            level, 
            GL_RGBA, 
            temp.getWidth(), 
            temp.getHeight(),
            0, 
            GL_BGRA, 
            GL_UNSIGNED_BYTE, 
            temp.getData()
         );
         temp= temp.downsample();
         level++;
      } while (temp.getWidth()>0 && temp.getHeight()>0);
	}

	return (unsigned int)tex;
}


bool Material::getCulling()
{
   return (bool) ((mFlags & 1)==0);
}


void Material::load(Stream *stream)
{
   int buffers= stream->getInt();
   mVB.init(buffers);
   mAmbient.load(stream);
   mDiffuse.load(stream);
   mSpecular.load(stream);
   mShininess= stream->getFloat();
   mStrength= stream->getFloat();
   mSelfIllum= stream->getFloat();
   mIOR= stream->getFloat();
   mOpacity= stream->getFloat();
   mFlags= stream->getInt();
   
   int numMaps= stream->getInt();
   mSlots.init(numMaps);
   for (int i=0;i<numMaps;i++)
   {
      TextureSlot *slot= new TextureSlot(stream);
      stream->skip(72);
      mSlots.add(slot);
   }
}

void Material::write(Stream *stream)
{
   Chunk chunk(stream, mId, name());

   chunk.writeInt( mVB.size() );
   mAmbient.write(&chunk);
   mDiffuse.write(&chunk);
   mSpecular.write(&chunk);
   chunk.writeFloat(mShininess);
   chunk.writeFloat(mStrength);
   chunk.writeFloat(mSelfIllum);
   chunk.writeFloat(mIOR);
   chunk.writeFloat(mOpacity);
   chunk.writeInt(mFlags);

   chunk.writeInt( mSlots.size() );
   for (int i=0; i<mSlots.size(); i++)
   {
      TextureSlot *slot= mSlots[i];
      slot->write(&chunk);
   }
}

TextureSlot* Material::getTextureSlot(int index) const
{
   if (index>=0 && index<mSlots.size())
      return mSlots[index];
   else
      return 0;
}

void Material::addMesh(Mesh *mesh)
{
   if (mesh && mesh->id() == Node::idMesh)
   {
      for (int i=0;i<mesh->getPartCount();i++)
         add(mesh->getPart(i));
   }
}

void Material::clear()
{
   mGeometryQueue.clear();
   mVB.clear();
}

void Material::removeMesh(Mesh *mesh)
{
   for (int i=0; i<mesh->getPartCount(); i++)
   {
      Geometry *geo= mesh->getPart(i);
      mGeometryQueue.remove(geo);
   }

	for (int i=0;i<mVB.size();)
	{
//		VertexBuffer *vb= mVB[i].vb;
		Geometry *geo= mVB[i].geo;
		if (geo->getParent() == mesh)
			mVB.erase(i);
		else
			i++;
	}
}

void Material::update(float /*frame*/, Node **nodelist, const Matrix&)
{
   for (int i=0;i<mVB.size();i++)
      mVB[i].vb->update(nodelist);
}


void Material::renderDiffuse()
{
   for (int i=0;i<mVB.size();i++)
   {
      /*
      VertexBuffer *vb= mVB[i];

      activeDevice->push(vb->getTransform());
      activeDevice->render(vb);
      activeDevice->pop();
      */
   }
}


static int dummyCounter= 1;

void Material::exportGeo(
      Stream* stream,
      const String& name,
      const Matrix& tm,
      Vector* vtx,
      Vector* nrm,
      UV* texcoords,
      int vertexCount,
      unsigned short* indices,
      int indexCount,
      int indexOffset )
{
   char tmp[256];
   // write object info comment
   if (!name.isEmpty())
      sprintf(tmp, "# object: %s\n", (const char*)name);
   else
      sprintf(tmp, "# object: dummy-%d\n", dummyCounter++);

   stream->writeData(tmp, strlen(tmp));
   sprintf(tmp, "# vertices: %d\n", vertexCount);
   stream->writeData(tmp, strlen(tmp));
   sprintf(tmp, "# triangles: %d\n", indexCount/3);
   stream->writeData(tmp, strlen(tmp));

      // write vertices
   stream->writeChar('\n');
   for (int i=0; i<vertexCount; i++)
   {
      Vector v= tm * vtx[i];
      sprintf(tmp, "v %.09f %.09f %.09f\n", -v.x, v.z, v.y); // flip y/z !
      stream->writeData(tmp, strlen(tmp));
   }

   // write normals
   stream->writeChar('\n');
   for (int i=0; i<vertexCount; i++)
   {
      const Vector& n= nrm[i];
      sprintf(tmp, "vn %f %f %f\n", -n.x, n.z, n.y);
      stream->writeData(tmp, strlen(tmp));
   }

   // write uv channel
   stream->writeChar('\n');
   for (int i=0; i<vertexCount; i++)
   {
      if (texcoords)
         sprintf(tmp, "vt %f %f 0.0\n", texcoords[i].u, 1.0f-texcoords[i].v);
      else
         sprintf(tmp, "vt 0.0 0.0 0.0\n");
      stream->writeData(tmp, strlen(tmp));
   }

   // write triangles - indices start with 1 (not 0)
   stream->writeChar('\n');
   sprintf(tmp, "g %s \n", (const char*)name);
   stream->writeData(tmp, strlen(tmp));
   sprintf(tmp, "s off \n");
   stream->writeData(tmp, strlen(tmp));
   for (int i=0; i<indexCount; i+=3)
   {
      bool valid= true;
      // flip normal because of y/z-issue
      int i1= indices[i];
      int i2= indices[i+1];
      int i3= indices[i+2];
      if (i1<0 || i1>=vertexCount || i2<0 || i2>=vertexCount || i3<0 || i3>=vertexCount)
         valid= false;

      if (valid)
      {
         sprintf(tmp, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
            i1+indexOffset,
            i1+indexOffset,
            i1+indexOffset,
            i2+indexOffset,
            i2+indexOffset,
            i2+indexOffset,
            i3+indexOffset,
            i3+indexOffset,
            i3+indexOffset
         );
         stream->writeData(tmp, strlen(tmp));
      }
   }
   stream->writeChar('\n');
   stream->writeChar('\n');
}

void Material::exportOBJ(Stream* stream, int& indexOffset)
{
   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      Geometry *geo= mVB[i].geo;

      if (!geo->isVisible())
         continue;

      exportGeo(
         stream,
         geo->getParent()->name(),
         geo->getTransform(),
         geo->getVertices(),
         geo->getNormals(),
         geo->getUV(1),
         geo->getVertexCount(),
         geo->getIndices(),
         geo->getIndexCount(),
         indexOffset
      );

      indexOffset += geo->getVertexCount();
   }
}
