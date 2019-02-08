// scene graph

#pragma once

#include "tools/array.h"
#include "node.h"
#include <QMutex>

class Stream;
class Material;
class Camera;
class Mesh;

class MaterialFactory;

class SceneGraph : public Node
{
public:
             SceneGraph();                // construct
   virtual   ~SceneGraph();               // destruct

   static SceneGraph* instance();

   int       load(const String& name, MaterialFactory* materials = 0, Node* parent= 0);      // load the scene
   void      write(const String& name);

   int       size();                      // get total number of nodes
   void      addNode(Node *node);
   void      removeNode(Node *node);
   Node*     getNode(int id);             // find node by id
   Node*     getNode(const String& name);         // find node by name

   void      prepare();
   void      render(float frame = 0.0f, const Matrix &shake = Matrix());         // recursively render all node of the scene graph
   void      exportOBJ(float frame, Stream* stream, int& vertexNum);
   Matrix    setupCamera(const Matrix& shake = Matrix());
   void      setCamera(Node *node);       // set active camera
   Node*     getCamera();                 // get active camera
   int       getLastFrame() const;        // get last frame of animation
   void      setFlags(int flags);

   Material* getMaterial(int index) const;      // get material by number
   int       getMaterialCount() const;
   void      addMaterial(Material *mat);
   void      removeMaterial(Material *mat);
   const Array<Node*>& nodeList() const;

   int       getMaterialStartIndex() const;
   int       getNodeStartIndex() const;
  
   const Matrix& getGlobalTransform() const;
   void      setGlobalTransform(const Matrix& matrix);

   void      exportOBJ(const String& name);

private:
   int       loadHeader(Stream*);          // load global information
   void      writeHeader(Stream *stream);

   void      loadMaterials(Stream*, MaterialFactory*);  // load list of materials
   void      writeMaterials(Stream *stream);

   void      loadNode(Stream*,Node*);      // recursive stream-traversal (tree)
   void      writeNode(Stream *stream, Node *parent);

   void      linkMaterials(Mesh *mesh);    // add mesh to used materials
   void      clean(Node*);                 // recursively remove all nodes
   int       processLights();
   void      setMaterialStartIndex(int index);
   void      setNodeStartIndex(int index);

private:
   mutable QMutex   mMutex;
	Array<Node*>     mNodes;       // array containing all nodes to avoid tree traversal
	Array<Material*> mMaterials;   // materials
	Camera*          mCamera;      // camera (currently active)
	int              mAnimBegin;
	int              mAnimEnd;
	int              mDepth;       // debug shit
	int              mFlags;

   int              mMaterialStartIndex;
   int              mNodeStartIndex;
   Matrix           mGlobalTransform;
   Array<Node*>     mShapes;

   static SceneGraph* mInstance;
};
