#ifndef VCACHE_H
#define VCACHE_H

// cache simulation
class VCache
{
private:
   unsigned short *mData;
   int mSize;

public:
   VCache(int size)
   {
      mData= new unsigned short[size];
      mSize= size;
      for (int i=0;i<size;i++) mData[i]=0xffff;
   }

   ~VCache()
   {
      delete[] mData;
   }

   bool add(unsigned short index)
   {
      int j;

      for (j=0;j<mSize;j++) if (mData[j]==index) return true;

      // kick oldest element
      for (j=0;j<mSize-1;j++) mData[j]= mData[j+1]; 

      // add new index
      mData[mSize-1]= index;

      return false;
   }

   bool exist(unsigned short index)
   {
      int j;
      for (j=0;j<mSize;j++) if (mData[j]==index) return true;
      return false;
   }

   unsigned short get(int index)
   {
      return mData[index];
   }

   int size()
   {
      return mSize;
   }

   unsigned short* data()
   {
      return mData;
   }
};


#endif
