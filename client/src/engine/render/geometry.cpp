#include "stdafx.h"
#include "geometry.h"
#include "nodes/node.h"
#include "vcache.h"
#include "math/vector.h"
//#include "edgebreaker.h"
#include "vcache.h"
//#include "meshcompress.h"
#include "tools/profiling.h"
#include "uv.h"
#include "nodes/scenegraph.h"
#include "nodes/mesh.h"
#include "animation/motionmixer.h"

static int mCurrentGeoID= 0;
static Matrix mIdentity;


Geometry::Geometry(Node *parent)
: Referenced()
, mID(mCurrentGeoID++)
, mParent(parent)
, mVisible(true)
, mMatID(0)
, mVtxMap(0)
{
}

Geometry::Geometry(const Geometry* geo)
: Referenced( *geo )
, mID( geo->getID() )
, mParent( geo->getParent() )
, mVisible( geo->isVisible() )
, mMatID(-1)
{
   mIndices= geo->getIndicesList();     
   mVertice= geo->getVertexList();
   mColor= geo->getColorList();
   mNormal= geo->getNormalList();
   mUV= geo->getUVList();
   mBones= geo->getBoneList();
   mEdge= geo->getEdgeList();
   
   mVtxMap= (int*)geo->getVertexMap();
}

Geometry::~Geometry()
{
}

void Geometry::copy(const Geometry& geo)
{
   mIndices.copy( geo.getIndicesList() );     
   mVertice.copy( geo.getVertexList() ); 
   mColor.copy( geo.getColorList() );
   mNormal.copy( geo.getNormalList() );
   // copy uv channels
   mUV.init( geo.getUVList().size() );
   for (int i=0; i<geo.getUVList().size(); i++)
   {
      UVChannel chn;
      const UVChannel& other= geo.getUVList()[i];
      chn.copy( other );
      mUV.add(chn);
   }

   mBones.copy( geo.getBoneList() );
   mEdge.copy( geo.getEdgeList() );

   mVtxMap= 0;
}

int Geometry::getID() const
{
   return mID;
}

bool Geometry::isVisible() const
{
   return mVisible;
}

void Geometry::setVisible(bool visible)
{
   mVisible= visible;
}

void Geometry::calcBoundingBox(Vector &min, Vector &max)
{
   min= max= mVertice[0];
   for (int i=0; i<mVertice.size(); i++)
   {
      const Vector& v= mVertice[i];
      max.maximum( v );
      min.minimum( v );
   }
}

void Geometry::createQuad(float x, float y)
{
   int i;
   static unsigned short idx[6]= {0,2,1, 0,3,2 };
   static UV uvs[4]= { UV(0.0f, 1.0f), UV(1.0f, 1.0f), UV(1.0f, 0.0f), UV(0.0f, 0.0f) };
   static Vector vtx[4]= { Vector(0.0f,0.0f,0.0f), Vector(x,0.0f,0.0f), Vector(x,y,0.0f), Vector(0.0f,y,0.0f) };

   for (i=0;i<6;i++)
      mIndices.add(idx[i]);

   for (i=0;i<4;i++)
   {
      mVertice.add(vtx[i]);
      mNormal.add( Vector(0.0f, 0.0f, 1.0f) );
      mColor.add( Vector(1.0f, 1.0f, 1.0f) );
   }

   mUV.add( UVChannel(1, uvs, 4) );

   mVtxMap= new int[4];
   for (i=0;i<4;i++) 
      mVtxMap[i]= i;

}

void Geometry::createCube(float scale)
{
   int i;

   static unsigned short tris[12*3]= {
      0,1,2,  1,3,2, // front
      2,3,6,  3,7,6, // right
      4,6,5,  5,6,7, // back
      0,2,4,  2,6,4, // top
      0,4,5,  0,5,1, // left
      1,5,7,  3,1,7  // bottom
   };

   static Vector vtx[8]= {
      Vector(-1.0f, -1.0f, -1.0f),
      Vector(-1.0f,  1.0f, -1.0f),
      Vector( 1.0f, -1.0f, -1.0f),
      Vector( 1.0f,  1.0f, -1.0f),

      Vector(-1.0f, -1.0f,  1.0f),
      Vector(-1.0f,  1.0f,  1.0f),
      Vector( 1.0f, -1.0f,  1.0f),
      Vector( 1.0f,  1.0f,  1.0f)
   };

   for (i=0;i<12*3;i++)
      mIndices.add(tris[i]);

   for (i=0;i<8;i++)
      mVertice.add(vtx[i]*scale);

   mVtxMap= new int[8];
   for (i=0;i<8;i++)
      mVtxMap[i]= i;
}




const int* Geometry::getVertexMap() const
{
	return mVtxMap;
}

const FaceList& Geometry::getIndicesList() const
{
	return mIndices;
}

const List<Vector>& Geometry::getVertexList() const
{
	return mVertice;  
}

const List<Vector>& Geometry::getColorList() const
{
	return mColor;
}

const List<Vector>& Geometry::getNormalList() const
{
	return mNormal;
}

const List<UVChannel>& Geometry::getUVList() const
{
	return mUV;
}

const List<Bone>& Geometry::getBoneList() const
{
	return mBones;
}

const Array<Edge>& Geometry::getEdgeList() const
{
	return mEdge;
}

int Geometry::createEdges()
{
   int i,j;
   unsigned short edge[3];     // matching edge-vertices
   unsigned short *poly;       // pointer to vertex-indices
   int numtri= mIndices.size()/3; // number of triangles (3 indices per triangle)
   int nv= mVertice.size();    // number of vertices
   int *vmap= mVtxMap;
   int idx;
   int nov= 0;

   double time= getCpuTick();

   for (i=0;i<nv;i++) if (vmap[i]>nov) nov= vmap[i];
   nov++;

   // calculate polygon-normals
   Vector *nrm= new Vector[numtri];
   poly= mIndices.data();
   for (i=0;i<numtri;i++)
   {
      const Vector &v1= mVertice[*poly++];
      const Vector &v2= mVertice[*poly++];
      const Vector &v3= mVertice[*poly++];
      nrm[i]= (v2-v1) % (v3-v1);
      nrm[i].normalize();
   }

   IndexList *vtxConFaces= new IndexList[nov]; // for each vertex: list of connected faces
   poly= mIndices.data();
   for (i=0;i<numtri;i++)
   {
      for (j=0;j<3;j++)
      {
         idx= vmap[*poly++];
         vtxConFaces[idx].add(i);
      }
   }

   IndexList *faceConFaces= new IndexList[numtri]; // for each face: list of connected faces
   poly= mIndices.data();
   for (i=0;i<numtri;i++)
   {
      IndexList *facelist= &faceConFaces[i]; // get list of faces (currently) connected to face i
      for (j=0;j<3;j++)
      {
         // faces connected to current vertex j of this triangle
         idx= *poly++;
         IndexList& list= vtxConFaces[vmap[idx]];
         facelist->merge(list);
      }
   }

   IndexList *vcon= new IndexList[nov]; // for each vertex: list of connected vertices, thus building an edge

   mEdge.init(numtri*3); // maximum number of edges is 3 per triangle
   poly= mIndices.data();
   for (i=0;i<numtri;i++,poly+=3)
   {
      int va1= poly[0];
      int va2= poly[1];
      int va3= poly[2];

      int p1= vmap[va1];
      int p2= vmap[va2];
      int p3= vmap[va3];

      IndexList *facelist= &faceConFaces[i];
      for (j=0;j<facelist->size();j++)
      {
         int faceid= facelist->get(j);
         unsigned short *test= mIndices.data() + faceid*3;
         int count=0;

         int vb1= test[0];
         int vb2= test[1];
         int vb3= test[2];

         if (vmap[vb1]==p1 || vmap[vb2]==p1 || vmap[vb3]==p1) edge[count++]= va1;
         if (vmap[vb1]==p2 || vmap[vb2]==p2 || vmap[vb3]==p2) edge[count++]= va2;
         if (vmap[vb1]==p3 || vmap[vb2]==p3 || vmap[vb3]==p3) edge[count++]= va3;

         if (count==2)
         {
            if ( (edge[0]==va1 && edge[1]==va3) ||
                 (edge[0]==va2 && edge[1]==va1) ||
                 (edge[0]==va3 && edge[1]==va2) )
            {
               unsigned short t= edge[2];
               edge[2]= edge[0];
               edge[0]= edge[1];
               edge[1]= t;
            }

            // if not already connected
            if (!vcon[vmap[edge[0]]].find(vmap[edge[1]]) && !vcon[vmap[edge[1]]].find(vmap[edge[0]]))
            {
               vcon[vmap[edge[0]]].add(vmap[edge[1]]);
               vcon[vmap[edge[1]]].add(vmap[edge[0]]);

               // compare assigned normals
               int n1,n2,k;
               for (k=0;k<3;k++)
               {
                  int idx= test[k];
                  if (vmap[edge[0]]== vmap[idx]) n1= idx;
                  if (vmap[edge[1]]== vmap[idx]) n2= idx;
               }

               int flags= 1;
               if ( (mNormal[n1] == mNormal[edge[0]]) && (mNormal[n2] == mNormal[edge[1]])) flags=0;

               // faces are coplanar? -> edge cannot be visible!
//               if ( flags==0 && nrm[i]*nrm[faceid] < 0.9999f ) continue;

               // get "unused" vertex from each triangle
               int vc;
               if (vmap[edge[0]]!=vmap[va1] && vmap[edge[1]]!=vmap[va1]) vc= va1;
               if (vmap[edge[0]]!=vmap[va2] && vmap[edge[1]]!=vmap[va2]) vc= va2;
               if (vmap[edge[0]]!=vmap[va3] && vmap[edge[1]]!=vmap[va3]) vc= va3;

               int vd;
               if (vmap[edge[0]]!=vmap[vb1] && vmap[edge[1]]!=vmap[vb1]) vd= vb1;
               if (vmap[edge[0]]!=vmap[vb2] && vmap[edge[1]]!=vmap[vb2]) vd= vb2;
               if (vmap[edge[0]]!=vmap[vb3] && vmap[edge[1]]!=vmap[vb3]) vd= vb3;

               Edge e;
               e.i1= edge[0];
               e.i2= edge[1];
               e.i3= vc;
               e.i4= vd;
               e.f1= i;
               e.f2= faceid;
               e.flags= flags;
               mEdge.add(e);
            }
         }
      }
   }

   int add=0;
   poly= mIndices.data();
   for (i=0;i<numtri;i++,poly+=3)
   {
      int index[5]={poly[0],poly[1],poly[2],poly[0],poly[1]};

      for (j=0;j<3;j++)
      {
         int i1= index[j];
         int i2= index[j+1];
         // int vc= index[j+2];
         // if not already connected
         if (!vcon[vmap[i1]].find(vmap[i2]) && !vcon[vmap[i2]].find(vmap[i1]))
         {
            vcon[vmap[i1]].add(vmap[i2]);
            vcon[vmap[i2]].add(vmap[i1]);
/*
            Edge e;
            e.i1= i1;
            e.i2= i2;
            e.i3= vc;
            e.i4= vc;
            e.f1= i;
            e.f2= i;
            e.flags= 0;
            mEdge.add(e);
*/
            add++;
         }
      }
   }

   time= getCpuTick() - time;
//   printf("edge performance: %f \n", time);
//   printf("unconnected edges: %d \n", add);

   delete[] vcon;
   delete[] nrm;
//   delete vmap;

   return mEdge.size();
}

void sortBonesByID(Bone *bones, int l, int r)
{
   Bone tmp;
   if(r>l)
   {
      int i=l-1;
      int j=r;
      while (i<j)
      {
         while(bones[++i].id() < bones[r].id());
         while(bones[--j].id() > bones[r].id() && j>i);
         if (i<j)
         {
            tmp= bones[i];
            bones[i]= bones[j];
            bones[j]= tmp;
         }
      }
      tmp=bones[i];
      bones[i]=bones[r];
      bones[r]=tmp;

      sortBonesByID(bones, l, i-1);
      sortBonesByID(bones, i+1, r);
   }
}






void Geometry::load(Stream *stream)
{
   Chunk chunk(stream);

   SceneGraph *scene= SceneGraph::instance();
   mMatID= chunk.getInt();
   if (mMatID>=0 && scene) mMatID += scene->getMaterialStartIndex();

   mVertice << chunk;


/*
   Vector vmin( 100000, 100000, 100000);
   Vector vmax(-100000,-100000,-100000);

   for (int i=0;i<mVertice.size();i++)
   {
      const Vector& v= mVertice[i];
      if (v.x < vmin.x) vmin.x= v.x;
      if (v.y < vmin.y) vmin.y= v.y;
      if (v.z < vmin.z) vmin.z= v.z;

      if (v.x > vmax.x) vmax.x= v.x;
      if (v.y > vmax.y) vmax.y= v.y;
      if (v.z > vmax.z) vmax.z= v.z;
   }

   printf("bounding box: %f, %f, %f - %f,%f,%f \n", vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z);
*/

   mNormal << chunk;

   if (mNormal.size() == 0)
      calcNormals();

   mColor << chunk;

   mUV << chunk;
//   printf(" - uv channels: %d \n", mUV.size());

   mIndices << chunk;

   // bones
   mBones << chunk;

//   printf(" - bones: %d \n", mBones.size());

   chunk.skip();
//   createEdges();
}

void Geometry::write(Stream *stream)
{
   Chunk chunk(stream, 100, "Geometry");

   chunk.writeInt(mMatID);

   mVertice >> chunk;
   mNormal >> chunk;
   mColor >> chunk;
   mUV >> chunk;
   mIndices >> chunk;
   mBones >> chunk;
}

const Matrix& Geometry::getTransform() const
{
   if (mParent)
   {
      return mParent->getTransform();
   }
   return mIdentity;
}


int Geometry::getMaterial() const
{
   return mMatID;
}


Node* Geometry::getParent() const
{
   return mParent;
}


void Geometry::setParent(Node *node)
{
   mParent= node;
}


int Geometry::getIndexCount() const
{
   return mIndices.size();
}


unsigned short* Geometry::getIndices() const
{
   return mIndices.data();
}

int Geometry::getEdgeCount() const
{
   return mEdge.size();
}


Edge* Geometry::getEdges() const
{
   return (Edge*)mEdge.data();
}


int Geometry::getVertexCount() const
{
   return mVertice.size();
}


Vector* Geometry::getVertices() const
{
   return mVertice.data();
}

Vector* Geometry::getNormals() const
{
   return mNormal.data();
}

Vector* Geometry::getColors() const
{
   return mColor.data();
}


UV* Geometry::getUV(int channel) const
{
   int num= mUV.size();
   for (int i=0;i<num;i++)
   {
      const UVChannel& chn= mUV[i];
      if (chn.id() == channel)
         return chn.data();
   }
   return 0;
}


int Geometry::getBoneCount() const
{
   return mBones.size();
}


Bone* Geometry::getBones() const
{
   return mBones.data();
}

const Bone& Geometry::getBone(int index) const
{
   return mBones[index];
}


void Geometry::calcNormals()
{
   mNormal.init( mVertice.size() );

   for (int i=0;i<mVertice.size();i++)
      mNormal.add( Vector(0.0f, 0.0f, 0.0f) );

   unsigned short *index= mIndices.data();

   for (int i=0;i<mIndices.size();i+=3)
   {
      unsigned short i1= *index++;
      unsigned short i2= *index++;
      unsigned short i3= *index++;

      const Vector& v1= mVertice[i1];
      const Vector& v2= mVertice[i2];
      const Vector& v3= mVertice[i3];

      Vector normal= (v2-v1) % (v3-v1);

      mNormal[i1]+=normal;
      mNormal[i2]+=normal;
      mNormal[i3]+=normal;
   }

   for (int i=0;i<mNormal.size();i++)
      mNormal[i].normalize();
}

void Geometry::createBoxMapping(bool, const Vector& min, const Vector& max, const Matrix& transform, const Matrix& gizmo)
{
   UV* uv= getUV(1);

   Vector *verts= new Vector[mVertice.size()];

   for (int i=0; i<mVertice.size(); i++)
   {
      verts[i]= gizmo * (transform * mVertice[i]);
   }

   for (int i=0; i<mIndices.size(); i+=3)
   {
      int i1= mIndices[i];
      int i2= mIndices[i+1];
      int i3= mIndices[i+2];

      Vector v1= verts[i1];
      Vector v2= verts[i2];
      Vector v3= verts[i3];

      Vector fn= (v2-v1) % (v3-v1);
      int absIndex= fn.absMaxIndex();

      float su= 1.0f / 3.0f;
      float sv= 1.0f / 4.0f;

      switch (absIndex)
      {
         case 2:
            if (fn.z >= 0.0) // top *
            {
               for (int j=0; j<3; j++)
               {
                  int index= mIndices[i+j];
                  Vector vtx= verts[index];
                  float u= uv[index].u;
                  float v= uv[index].v;
                  v= (max.x - vtx.x) / (max.x - min.x);
                  u= (max.y - vtx.y) / (max.y - min.y);
                  uv[index].u= u*su + su*1;
                  uv[index].v= v*sv;
               }
            }
            else // bottom *
            {
               for (int j=0; j<3; j++)
               {
                  int index= mIndices[i+j];
                  Vector vtx= verts[index];
                  float u= uv[index].u;
                  float v= uv[index].v;
                  v= (vtx.x - min.x) / (max.x - min.x);
                  u= (vtx.y - min.y) / (max.y - min.y);
                  uv[index].u= u*su + su*1;
                  uv[index].v= v*sv + sv*2;
               }
            }
            break;

         case 1:
            if (fn.y >= 0.0) // right *
            {
               for (int j=0; j<3; j++)
               {
                  int index= mIndices[i+j];
                  Vector vtx= verts[index];
                  float u= uv[index].u;
                  float v= uv[index].v;
                  u= (max.x - vtx.x) / (max.x - min.x);
                  v= (max.z - vtx.z) / (max.z - min.z);
                  uv[index].u= u*su;
                  uv[index].v= v*sv + sv*1;
               }
            }
            else // left*
            {
               for (int j=0; j<3; j++)
               {
                  int index= mIndices[i+j];
                  Vector vtx= verts[index];
                  float u= uv[index].u;
                  float v= uv[index].v;
                  u= (vtx.x - min.x) / (max.x - min.x);
                  v= (max.z - vtx.z) / (max.z - min.z);
                  uv[index].u= u*su + su*2;
                  uv[index].v= v*sv + sv*1;
               }
            }
            break;

         case 0:
            if (fn.x >= 0.0) // back*
            {
               for (int j=0; j<3; j++)
               {
                  int index= mIndices[i+j];
                  Vector vtx= verts[index];
                  float u= uv[index].u;
                  float v= uv[index].v;
                  u= (vtx.y - min.y) / (max.y - min.y);
                  v= (max.z - vtx.z) / (max.z - min.z);
                  uv[index].u= u*su + su*1;
                  uv[index].v= v*sv + sv*3;
               }
            }
            else // front*
            {
               for (int j=0; j<3; j++)
               {
                  int index= mIndices[i+j];
                  Vector vtx= verts[index];
                  float u= uv[index].u;
                  float v= uv[index].v;
                  v= (max.z - vtx.z) / (max.z - min.z);
                  u= (max.y - vtx.y) / (max.y - min.y);
                  uv[index].u= u*su + su*1;
                  uv[index].v= v*sv + sv*1;
               }
            }
            break;
      }
   }
   delete[] verts;
}


Array<Vector> Geometry::getSkinVertices() const
{

   Mesh* mesh= (Mesh*)mParent;
   MotionMixer *mixer= mesh->getMotionMixer();

   int nv= getVertexCount();
   Array<Vector> list( nv );

   Vector* vtx= getVertices();
   Vector* vtemp= list.data();
   memset(vtemp, 0, sizeof(Vector)*nv);

   for (int b=0;b<getBoneCount();b++)
   {
      const Bone& bone= getBone(b);

      Node *node= mixer->getNode( bone.id() );

//         Matrix bm= bone.transform() * node->getTransform();
      const Matrix& bm= node->getTransform();
//      Matrix nbm= bm.normalize();

      Weight* weights= bone.weights();
      for (int v=0;v<bone.count();v++)
      {
         int idx= weights[v].id();
         float f= weights[v].weight();

         vtemp[idx] += (bm * vtx[idx]) * f;
//         ntemp[idx] += (nbm * nrm[idx]) * f;
      }
   }

   return list;
}

