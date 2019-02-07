#ifndef REFERENCED_H
#define REFERENCED_H

class Referenced
{
public:
   Referenced();
   Referenced(const Referenced& r);
   Referenced(const Referenced* r);
   Referenced(Referenced* r);
   virtual ~Referenced();

   void addRef() const;       //! add reference
   bool deref();              //! remove reference
   bool copyRef();            //! copy reference if required

   int getRefCount() const;  //! get number of referencing objects
   int* getRef() const;       //! get reference pointer

protected:
   int *mReferences;
};

#endif
