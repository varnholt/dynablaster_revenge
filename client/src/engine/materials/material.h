#pragma once

#include "tools/array.h"
#include "tools/map.h"
#include "tools/string.h"
#include "tools/objectname.h"
#include "math/vector.h"
#include "render/vertexbufferpool.h"
#include "render/texture.h"

#include <QObject>
#include <QMap>
#include <QPair>
#include <QFileSystemWatcher>

class Stream;
class Node;
class Mesh;
class Geometry;
class VertexBufferPool;
class TextureSlot;
class RenderBuffer;
class Mesh;
class Image;
class Texture;

#define MAP_AMBIENT     1
#define MAP_DIFFUSE     2
#define MAP_SPCOLOR     4
#define MAP_SPLEVEL     8
#define MAP_GLOSS      16
#define MAP_SELFILLUM  32
#define MAP_OPACITY    64
#define MAP_FILTER    128
#define MAP_BUMP      256
#define MAP_REFLECT   512
#define MAP_REFRACT  1024
#define MAP_DISPLACE 2048

class SceneGraph;
class Matrix;
class UV;

class Material : public QObject, public ObjectName
{
   Q_OBJECT

public:

   class Buffer
   {
   public:
      Buffer();
      Buffer(Geometry*, VertexBuffer*);

      Geometry *geo;
      VertexBuffer *vb;
   };

	Material(SceneGraph *scene, int id);
	virtual ~Material();

   virtual void init() = 0;
   virtual void begin();
   virtual void end() = 0;
   virtual void exportOBJ(Stream* stream, int& vertexNum);
   virtual void exportGeo(Stream* stream, const String& name, const Matrix& tm, Vector* vtx, Vector* nrm, UV* uv, int vertexCount, unsigned short* indices, int indexCount, int indexOffset);
   virtual void addGeometry(Geometry* geo) = 0;
   Geometry* getGeometry(int index) const;

	virtual void  load(Stream *stream);
   virtual void  write(Stream *stream);

   virtual void getBoundingRect(Vector& min, Vector& max, const Matrix& projMat);
   virtual Vector getCenter2d(const Matrix& projMat) const;

   void prepare();

   void add(Geometry *geo);
   int  size() const;
   void clear();

   bool          getCulling();
   int           geometryCount() const;
   const Array<Buffer>& getBuffers() const;

   void addTexture(Texture& texture, Image* image, int flags = 1|2|4);
   void addTexture(Texture& texture, const char* filename, int flags = 1|2|4);

   virtual void  update(float frame, Node **nodelist, const Matrix& cam);
   virtual void  renderDiffuse();
	virtual void  addMesh(Mesh*);
   virtual void  removeMesh(Mesh*);

   TextureSlot* getTextureSlot(int index) const;

   int getDebug() const;
   void setDebug(int v);

   static unsigned int uploadCubeMap(const Image& image);
   static unsigned int uploadMap(const Image& image, int flags = 1);
   static void updateMap(unsigned int texture, const Image& image, int flags = 1);

private slots:
   void fileChanged(const QString& path);

private:
   void addWatcher(Texture* texture, Image *image);

protected:
   int                 mId;
   bool                mInitialized;
   VertexBufferPool*   mPool;
   Array<Buffer>       mVB;
   Vector              mAmbient;
   Vector              mDiffuse;
   Vector              mSpecular;
   float               mShininess;
   float               mOpacity;
   float               mSelfIllum;
   float               mStrength;
   float               mIOR;
   int                 mFlags;
   Array<TextureSlot*> mSlots;
   Array<Geometry*>    mGeometryQueue;
   Array<Image*>       mImageQueue;
   Array<int>          mImageFlagsQueue;
   Array<Texture*>     mTextureIdQueue;
   int                 mDebug;

   static QMultiMap<QString,QPair<Texture*,QString> > mWatchers;
   static QFileSystemWatcher* mWatcher;
};

