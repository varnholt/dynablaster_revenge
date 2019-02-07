#pragma once

class IndexList : public Array<int>
{
public:

   void sort()
   {
      sort(0, mCount-1);
   }

   bool find(int it)
   {
      int l= 0;
      int r= mCount-1;
      while (l<=r)
      {
         int m= (l+r)>>1;
         if (mData[m] > it)
            r=m-1;
         else if (mData[m] < it)
            l=m+1;
         else
            return true;
      }
      return false;
   }

   // add indices (which are not yet contained) from "list" to this
   int merge(const IndexList& list)
   {
      int cursize= size();
      int num= list.size();
      for (int i=0; i<num; i++)
      {
         int idx= list.get(i);
         if (!find(idx)) add(idx);
      }
      return size() - cursize;
   }

private:
   void sort(int l, int r)
   {
      if(r>l)
      {
         int tmp;
         int i=l-1, j=r;
         while (i<j)
         {
            while(mData[++i] > mData[r]) ;
            while(mData[--j] < mData[r] && j>i) ;
            if (i<j)
            {
               tmp= mData[i];
               mData[i]= mData[j];
               mData[j]= tmp;
            }
         }
         tmp=mData[i];
         mData[i]=mData[r];
         mData[r]=tmp;

         sort(l, i-1);
         sort(i+1, r);
      }
   }
};
