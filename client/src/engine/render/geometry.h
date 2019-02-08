// geometry class
// "geometry" is one part of a mesh,
// it is directly linked to it's originating mesh-node by a parent pointer, but it's not a scenegraph-node by it's own.
// a single mesh may be cut down to several geometry objects when using multiple materials or when exceeding the vertexlimit of 2^16

#pragma once

#include "tools/referenced.h"
#include "tools/list.h"
#include "uvchannel.h"
#include "edge.h"
#include "bone.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "indexlist.h"
#include "facelist.h"
#include "animation/morphtrack.h"

class Stream;
class Node;

class Geometry : public Referenced
{
public:
   Geometry(Node *parent);
   Geometry(const Geometry* geo);
   virtual ~Geometry();

   void                    copy(const Geometry& geo);

   void                    createQuad(float x, float y);
   void                    createCube(float scale);

	const FaceList&         getIndicesList() const;
	const List<Vector>&     getVertexList() const;
	const List<Vector>&     getColorList() const;
	const List<Vector>&     getNormalList() const;
	const List<UVChannel>&  getUVList() const;
	const List<Bone>&       getBoneList() const;
	const Array<Edge>&      getEdgeList() const;
	const int*              getVertexMap() const;

   int                     getID() const;                //! get unique id
   bool                    isVisible() const;            //! get visibility
   void                    setVisible(bool visible);     //! set visibility
   bool                    isMorphing() const;           //! get morph status
   void                    setMorphFrame(float frame);   //! interpolate vertex/normal data to given frame

   void                    load(Stream *stream);         //! load geometry from stream
   void                    write(Stream *stream);

   void                    setParent(Node *node);        //! link geometry to originating mesh-node
   Node*                   getParent() const;            //! get originating mesh

   int                     getIndexCount() const;        //! get number of indices (triangles*3)
   int                     getVertexCount() const;       //! get number of vertices
   int                     getBoneCount() const;         //! get number of weighted vertices
   int                     getEdgeCount() const;         //! get number of edges

   unsigned short*         getIndices() const;           //! get pointer to index data

   Edge*                   getEdges() const;             //! get pointer to edge data

   Vector*                 getVertices() const;          // get pointer to vertex-position data
   Array<Vector>           getSkinVertices() const;      // get skinned vertices
   Bone*                   getBones() const;             // get pointer to vertex-weight data
   const Bone&             getBone(int index) const;
   Vector*                 getNormals() const;           // get pointer to vertex-normal data
   Vector*                 getColors() const;            // get pointer to vertex-color data
   UV*                     getUV(int channel) const;     // get pointer to a vertex-texcoord set "channel"
   int                     getMaterial() const;          // get Material ID
   const Matrix&           getTransform() const;         // get transformation matrix from mesh
   
   void                    calcBoundingBox(Vector &min, Vector &max);

   int                     morphTargetCount() const;
   const List<Vector>&     processMorphTrack(float time);

   void                    createBoxMapping(bool unwrap, const Vector& min, const Vector& max, const Matrix& tm= Matrix(), const Matrix& gizmo= Matrix());

private:
   int                     createEdges();
   void                    calcNormals();

   int                     mID;                          //! unique id for this geometry
   Node                    *mParent;                     //! link to mesh node
   bool                    mVisible;
   int                     mMatID;                       //! material id

   FaceList                mIndices;                     //! list of polygon indices
   List<Vector>            mVertice;                     //! vertex position
   List<Vector>            mColor;                       //! vertex colors
   List<Vector>            mNormal;                      //! vertex normals
   List<UVChannel>         mUV;                          //! list of uv-channels (can be empty)
   List<Bone>              mBones;                       //! vertex weights
   MorphTrack              mMorphTrack;
   Array<Edge>             mEdge;
   int                     *mVtxMap;                     //! vertex identity map
};
