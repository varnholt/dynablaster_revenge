#include "stdafx.h"
#include "scenegraph.h"
#include "node.h"
#include "mesh.h"
#include "dummy.h"
#include "camera.h"
#include "omni.h"

#include "tools/filestream.h"
#include "tools/profiling.h"
#include "materials/materialfactory.h"
#include "materials/material.h"

#include "gldevice.h"

#include <QMutexLocker>
#include <math.h>

SceneGraph* SceneGraph::mInstance = 0;

// construct empty graph
SceneGraph::SceneGraph()
 : Node(Node::idRoot),
   mCamera(NULL),
   mAnimBegin(0),
   mAnimEnd(0),
   mDepth(0),
   mFlags(0),
   mMaterialStartIndex(0),
   mNodeStartIndex(0)
{
}


SceneGraph::~SceneGraph()
{
   while (mMaterials.size() > 0)
   {
      Material* mat= mMaterials.takeLast();
      delete mat;
   }

   while (mNodes.size() > 0)
   {
      Node* node= mNodes.takeLast();
      delete node;
   }
}

SceneGraph* SceneGraph::instance()
{
   return mInstance;
}

const Matrix& SceneGraph::getGlobalTransform() const
{
   return mGlobalTransform;
}

void SceneGraph::setGlobalTransform(const Matrix& matrix)
{
   mGlobalTransform= matrix;
}

int SceneGraph::getMaterialStartIndex() const
{
   QMutexLocker lock(&mMutex);
   return mMaterialStartIndex;
}

void SceneGraph::setMaterialStartIndex(int index)
{
   QMutexLocker lock(&mMutex);
   mMaterialStartIndex= index;
}

int SceneGraph::getMaterialCount() const
{
   return mMaterials.size();
}

Material* SceneGraph::getMaterial(int index) const
{
   return mMaterials[index];
}

int SceneGraph::getNodeStartIndex() const
{
   QMutexLocker lock(&mMutex);
   return mNodeStartIndex;
}

void SceneGraph::setNodeStartIndex(int index)
{
   QMutexLocker lock(&mMutex);
   mNodeStartIndex= index;
}

const Array<Node*>& SceneGraph::nodeList() const
{
   QMutexLocker lock(&mMutex);
   return mNodes;
}

// get frame-number of end-of-animation
int SceneGraph::getLastFrame() const
{
   return mAnimEnd;
}


// get number of nodes
int SceneGraph::size()
{
   QMutexLocker lock(&mMutex);
   return mNodes.size();
}

void SceneGraph::addNode(Node *node)
{
   QMutexLocker lock(&mMutex);
	mNodes.add(node);
}

void SceneGraph::removeNode(Node *node)
{
   QMutexLocker lock(&mMutex);
   mNodes.remove(node);
}

// get node by index id
Node* SceneGraph::getNode(int index)
{
   QMutexLocker lock(&mMutex);
  return mNodes.get(index);
}

// get node by index id
Node* SceneGraph::getNode(const String& name)
{
   QMutexLocker lock(&mMutex);
   int num= mNodes.size();
   for (int i=0;i<num;i++)
   {
      Node *node= mNodes.get(i);
      if (node->name() == name) 
         return node;
   }
   return 0;
}


// get current camera node
Node* SceneGraph::getCamera()
{
   QMutexLocker lock(&mMutex);
   return (Node*)mCamera;
}


// set n-th camera
void SceneGraph::setCamera(Node *cam)
{
   QMutexLocker lock(&mMutex);
   if (cam && cam->id() != idCamera)
      cam= 0;
   mCamera= (Camera*)cam;
}


int SceneGraph::loadHeader(Stream *stream)
{
   mInstance= this;

   Chunk chunk(stream);
   if (chunk.id() != 0x33424a48) return 0; // wtf?!

   String comment;
   comment.load(stream);
   mAnimBegin= chunk.getInt();
   mAnimEnd= chunk.getInt();

   /*int nodes=*/ chunk.getInt();

   chunk.skip();

   return 1;
}

void SceneGraph::writeHeader(Stream *stream)
{
   Chunk chunk(stream, 0x33424a48, "Header");

   String comment= "Hfr";
   comment.write(&chunk);
   chunk.writeInt(mAnimBegin);
   chunk.writeInt(mAnimEnd);

   chunk.writeInt(mNodes.size());
}

void SceneGraph::loadMaterials(Stream *stream, MaterialFactory* materials)
{
   Chunk mats(stream);

   int num= mats.getInt();

   setMaterialStartIndex( mMaterials.size() );

   if (materials)
   {
      for (int i=0; i<num; i++)
      {
         Chunk chunk(&mats);

         Material* mat= materials->createMaterial(this, chunk.id());

         if (mat)
         {
            mat->load(&chunk);
            mat->setName( chunk.name() );
         }

         chunk.skip();           // skip to next chunk
      }
   }

   mats.skip();
}

void SceneGraph::writeMaterials(Stream *stream)
{
   Chunk chunk(stream, 1000, "Materials");

   chunk.writeInt( mMaterials.size() );

   for (int i=0; i<mMaterials.size(); i++)
   {
   	Material *mat= mMaterials[i];
      mat->write(&chunk);
   }
}

void SceneGraph::addMaterial(Material *mat)
{
   QMutexLocker lock(&mMutex);
   mMaterials.add(mat);
}

void SceneGraph::removeMaterial(Material *mat)
{
   QMutexLocker lock(&mMutex);
   mMaterials.remove(mat);
}

void SceneGraph::linkMaterials(Mesh *mesh)
{ 
   QMutexLocker lock(&mMutex);
   int num= mesh->getPartCount();
   for (int i=0;i<num;i++)
   {
      Geometry *geo= mesh->getPart(i);
      int matid= geo->getMaterial();
      if (matid>=0 && matid<mMaterials.size())
      {
         Material *mat= mMaterials[matid];
         if (mat)
            mat->add(geo);
      }
   }
}

void SceneGraph::loadNode(Stream *stream, Node *parent)
{
   int id=0;

   mDepth++; // increase indent with every recursion (-> child), for debug text only.

   while (id!=0xffff)
   {
      // get new chunk from stream
      Chunk *chunk= new Chunk(stream);
      id= chunk->id();

      // stop recursion when terminator chunk is found
      if (id!=0xffff)
      {
//         // print name and id of node
//         for (int i=0;i<mDepth;i++) printf(" ");
//         printf("%s [%d] (%d) \n", (const char*)chunk->name(), id, mNodes.size());
//         for (int i=0;i<mDepth;i++) printf(" ");

         // create new node based on chunk id
         Node *node= 0;
         switch(id)
         {
            case Node::idMesh:      node= new Mesh(parent);   break;
            case Node::idDummy:     node= new Dummy(parent);  break;
            case Node::idCamera:    node= new Camera(parent); break;
            case Node::idOmni:      node= new Omni(parent); break;
            default:                node= new Node(Node::idNone, parent); break;
         }

//         addNode(node);    // add node to nodelist
         node->setName(chunk->name());
         node->load(stream);      // load the node

         if (!mCamera && node->id() == Node::idCamera)
            mCamera= (Camera*)node;

         // link mesh to materials
         if (id==Node::idMesh)
         {
            linkMaterials((Mesh*)node);
         }

         chunk->skip();           // skip to next chunk
         delete chunk;            // delete chunk before recursing deeper to save fragmentation

         // printf("\r");

         loadNode(stream, node);  // recursive load child nodes
      }
      else
         delete chunk;
   }

   mDepth--;
}

void SceneGraph::writeNode(Stream *stream, Node *parent)
{
   // int id=0;

   {
      Chunk chunk(stream, parent->id(), parent->name());
      parent->write(&chunk);
   }

   for (int i=0; i<parent->getChildCount(); i++)
   {
      writeNode( stream, parent->getChild(i) );
   }

   stream->writeInt(0xffff);
}

int SceneGraph::load(const String& name, MaterialFactory* materials, Node* parent)
{
   int i;
   FileStream stream;

   mInstance = this;
   if (parent==0)
      parent= (Node*)this;

   if (!stream.open(name)) return 0;
   if (!loadHeader(&stream)) return 0;

   int count= mNodes.size();

   setName(name);

   loadMaterials(&stream, materials);

   setNodeStartIndex( mNodes.size() );
   mDepth=0;
   loadNode(&stream, parent);

/*
   {
      Chunk layers(&stream);
      layers.skip();
   }
*/

   // printf("done\n");

   // find skeleton root nodes for all meshes
   for (i=count;i<mNodes.size();i++)
   {
      Node *node= mNodes[i];
      if (node->id() == idMesh)
      {
         Mesh *mesh= (Mesh*)node;

         int minDepth= 0xffff;
         Node *skeleton= 0;
         for (int i=0;i<mesh->getPartCount();i++)
         {
            Geometry *geo= mesh->getPart(i);
            for (int j=0;j<geo->getBoneCount();j++)
            {
               const Bone& bone= geo->getBone(j);
               Node *node= mNodes[ bone.id() ];
               int depth= node->getDepth();

               if (depth<minDepth)
               {
                  minDepth= depth;
                  skeleton= node;
               }
            }
         }

         if (skeleton)
         {
            while (skeleton && skeleton->parent() && skeleton->parent()->id()==idDummy)
               skeleton= skeleton->parent();
            mesh->setSkeleton(skeleton);
         }
      }
   }

   // find skeleton root nodes for all meshes
   for (i=count;i<mNodes.size();i++)
   {
      Node *node= mNodes[i];
      node->transform(0.0f);
   }

   mInstance = 0;

   return 1;
}


void SceneGraph::write(const String& name)
{
   // int i;
   FileStream stream;

   mInstance = this;

   if (!stream.open(name, true)) return;
   
   writeHeader(&stream);

   writeMaterials(&stream);

   for (int i=0; i<getChildCount(); i++)
      writeNode(&stream, getChild(i));

   stream.writeInt(0xffff);
   stream.close();
}


//! export scenegraph to obj
void SceneGraph::exportOBJ(const String& name)
{
   // int i;
   FILE *file= fopen(name.data(), "wb");

   int indexOffset= 1; // indices start with 1 (not 0)
   for (int obj=0; obj<getChildCount(); obj++)
   {
      Node* node= getChild( obj );
      if (node && node->id()==Node::idMesh)
      {
         Mesh* mesh= (Mesh*)node;
         int partCount= mesh->getPartCount();
         for (int part=0; part<partCount; part++)
         {
            Geometry* geo= mesh->getPart(part);
            int vertexCount= geo->getVertexCount();
            int indexCount= geo->getIndexCount();

            Vector* vtx= geo->getVertices();
            Vector* nrm= geo->getNormals();
            UV* texcoords= geo->getUV(1);
            unsigned short* indices= geo->getIndices();

            // write object info comment
            fprintf(file, "# object: %s-%d\n", (const char*)node->name(), part);
            fprintf(file, "# vertices: %d\n", vertexCount);
            fprintf(file, "# triangles: %d\n", indexCount/3);

            // write vertices
            fprintf(file, "\n");
            const Matrix& tm= geo->getTransform();
            for (int i=0; i<vertexCount; i++)
            {
               Vector v= tm * vtx[i];
               fprintf(file, "v %.09f %.09f %.09f\n", v.x, v.y, v.z); // flip y/z !
            }

            // write normals
            fprintf(file, "\n");
            for (int i=0; i<vertexCount; i++)
            {
               const Vector& n= nrm[i];
               fprintf(file, "vn %f %f %f\n", n.x, n.y, n.z);
            }

            // write uv channel
            fprintf(file, "\n");
            for (int i=0; i<vertexCount; i++)
            {
               if (texcoords)
                  fprintf(file, "vt %f %f 0.0\n", texcoords[i].u, 1.0f-texcoords[i].v);
               else
                  fprintf(file, "vt 0.0 0.0 0.0\n");
            }

            // write triangles - indices start with 1 (not 0)
            fprintf(file, "\n");
            fprintf(file, "g %s-%d \n", (const char*)node->name(), part);
            fprintf(file, "s off \n");
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
                  fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
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
               }
            }
            fprintf(file, "\n\n");

            indexOffset+=vertexCount;
         }
      }
   }

   fclose(file);
}

void SceneGraph::exportOBJ(float /*frame*/, Stream* stream, int& vertexNum)
{
   // render() needs to be called first!
   int num= mMaterials.size();
   for (int i=0;i<num;i++)
      mMaterials[i]->exportOBJ(stream, vertexNum);
}

int SceneGraph::processLights()
{
   int count=0;

   glDisable(GL_LIGHTING);

   return 0;
/*
   for (i=0;i<mNodes.size();i++)
   {
      Node *node= mNodes.get(i);
      if (node->id() & ID_LIGHT)
      {
         int nr= GL_LIGHT0 + count;

         Light *light= (Light*)node;

         Vector p= light->getPosition();
         Vector c= light->getColor();

//         printf("%d: %f,%f,%f \n", count, c.x, c.y, c.z);

         glEnable(nr);
         float pos[4]={p.x, p.y, p.z, 1.0};
         float amb[4]={1.0, 1.0, 1.0, 1.0};
         float col[4]={c.x, c.y, c.z, 1.0};
//         float spc[4]={1,1,1,1};

         glLightfv(nr, GL_POSITION,             pos);
         glLightfv(nr, GL_AMBIENT,              amb);
         glLightfv(nr, GL_DIFFUSE,              col);
         glLightfv(nr, GL_SPECULAR,             col);
         glLightf(nr,  GL_CONSTANT_ATTENUATION, 1.0000f);
         glLightf(nr,  GL_LINEAR_ATTENUATION,   0.0f);
         glLightf(nr,  GL_QUADRATIC_ATTENUATION,0.0f);

         count++;
      }
   }

   for (i=count;i<8;i++) glDisable(GL_LIGHT0 + i);

   if (count>0) glEnable(GL_LIGHTING);
*/

   return count;
}


Matrix SceneGraph::setupCamera(const Matrix& shake)
{
   Matrix cam;
   float fov= 1.0f;
   float znear= 1.0f;
   float zfar= 500.0f;
   bool persp= true;
   if (mCamera)
   {
      cam= mCamera->getTransform().getView();
      fov= mCamera->getFOV();
      fov= tan(fov*0.5)*0.75;
      znear= mCamera->getNear();
      zfar= mCamera->getFar();
      cam= mGlobalTransform * cam * shake;
      persp= mCamera->getPerspectiveMode();
   }
   else
   {
      cam= mGlobalTransform * shake;
   }
   activeDevice->setCamera(cam, fov, znear, zfar, persp);
   return cam;
}

void SceneGraph::prepare()
{
   int num= mMaterials.size();
   for (int i=0;i<num;i++) mMaterials[i]->prepare();
}

void SceneGraph::render(float frame, const Matrix& shake)
{
   QMutexLocker lock(&mMutex);

//   glFinish();

//   double t1= getCpuTick();

   int i;

   // no camera: no render.
//   if (!mCamera) return;

   // call "transform" all nodes
   for (i=0;i<mNodes.size();i++)
   {
      Node* node= mNodes[i];
      if (node)
         node->transform(frame);
   }

   // get view matrix from camera node
   Matrix cam= setupCamera(shake);

//   cam.print();

   // tell renderdevice about position, target & fov

   processLights();


   // process all materials
   int num= mMaterials.size();
   for (i=0;i<num;i++) mMaterials[i]->update(frame, mNodes.data(), cam);

//   glFinish();
//   double t2= getCpuTick();

   for (i=0;i<num;i++) mMaterials[i]->renderDiffuse();

//   glFinish();

//   double t3= getCpuTick();

//   printf("%s:  transform: %f  render: %f \n", (const char*)mName, (t2-t1)/1000000.0, (t3-t2)/1000000.0);

//   mOutlines->renderDiffuse();

   // draw debug information
//   if (mFlags & 1)
//      renderBones();
}


void SceneGraph::setFlags(int flags)
{
   QMutexLocker lock(&mMutex);
   mFlags= flags;
}
