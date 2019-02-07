#pragma once

int next(int c)
{
   int v= c % 3;
   if (v==2) return c-2; else return c+1;
}

int pre(int c)
{
   int v= c % 3;
   if (v==0) return c+2; else return c-1;
}


int mRight=0;
int mLeft=0;
int mBack=0;

FILE *f;

void Geometry::compress()
{
   int i,j,k,l;
   unsigned short *vindex;

   printf("compressing...\n");

   int nf= getIndexCount() / 3;
   int nv= getVertexCount();

   f= fopen("d:\\test.dat", "wb");

   /*
   vtxVisit= new int[nv];
   memset(vtxVisit, 0, sizeof(int)*nv);

   triVisit= new int[nf];
   memset(triVisit, 0, sizeof(int)*nf);
   */

   // for each vertex: find number of connected faces
   int *vtxNumConFaces= new int[nv];
   memset(vtxNumConFaces, 0, nv*sizeof(int));
   vindex= getIndices();
   for (i=0;i<nf*3;i++)
   {
      int index= vindex[i];
      vtxNumConFaces[index]++;
   }

   // memory management heuristic:
   // the maximum number of faces connected to a single vertex "i"
   // >= the maximum number of faces connected to two vertices (i1, i2)
   int maxv=0;
   for (i=0;i<nv;i++)
      if (vtxNumConFaces[i]>maxv) maxv= vtxNumConFaces[i];
   int *facelist= new int[maxv];

   // for each vertex: get list of connected face ids
   int** vtxConFaces= new int*[nv];
   for (i=0;i<nv;i++) vtxConFaces[i]= new int[vtxNumConFaces[i]];
   memset(vtxNumConFaces, 0, nv*sizeof(int));
   for (i=0;i<nf;i++)
   {
      for (j=0;j<3;j++)
      {
         int index= vindex[i*3+j];
         int num= vtxNumConFaces[index];
         vtxConFaces[index][num]= i;
         vtxNumConFaces[index]++;
      }
   }

   edgeNumConFaces= new int[nf*3];
   edgeConFaces= new int*[nf*3];

   // for each face-edge (vertex-pair i,i+1) get the number of connected faces
   for (i=0;i<nf;i++)
   {
      for (j=0;j<3;j++)
      {
         int edge= i*3+j;

         int i1, i2;
         i1= vindex[edge];
         if (j<2) i2= vindex[edge+1]; else i2= vindex[edge-2];

         int num1= vtxNumConFaces[i1];
         int num2= vtxNumConFaces[i2];

         int *faces1= vtxConFaces[i1];
         int *faces2= vtxConFaces[i2];

         int count=0;
         for (k=0;k<num1;k++)
         {
            int f1= faces1[k];
            if (f1!=i)
               for (l=0;l<num2;l++)
               {
                  int f2= faces2[l];
                  if (f1==f2)
                  {
                     facelist[count]= f1;
                     count++;
                  }
               }
         }

         edgeNumConFaces[edge]= count;
         //         if (count!=1) printf("%d/%d:%d  ", i,j,count);

         if (count>0)
         {
            int *faces= new int[count];
            for (k=0;k<count;k++)
            {
               int face= facelist[k];
               int corner;
               for (l=0;l<3;l++)
               {
                  int index= vindex[face*3+l];
                  if (index!=i1 && index!=i2) corner= l;
               }
               faces[k]= face*3+corner;
            }
            edgeConFaces[edge]= faces;
         }
         else
         {
            edgeConFaces[edge]= new int[1];
            edgeConFaces[edge][0]= edge;
         }
      }
   }

   int nIso=0;
   for (i=0;i<nf;i++)
   {
      if (edgeNumConFaces[i*3+0]==0 && edgeNumConFaces[i*3+1]==0 && edgeNumConFaces[i*3+2]==0) nIso++;
   }

   printf("isolated faces: %d \n", nIso);

   faceVisit= new int[nf];
   for (i=0;i<nf;i++) faceVisit[i]= 0;

   vtxVisit= new int[nv];
   for (i=0;i<nv;i++) vtxVisit[i]= -1;

   mNumVisited=0;

   mCache= new VCache(64);

   int c= 0;
   for (i=0;i<nf*3;i++) if (edgeNumConFaces[i]==0) c= i;
   //   while (edgeNumConFaces[c]==0) c++;
   traverse(c);

   printf("\nvisited faces: %d/%d \n", mNumVisited, nf);
   printf("%d %d %d \n", mLeft, mRight, mBack);

   int vis=0;
   for (i=0;i<nv;i++) if (vtxVisit[i]==1) vis++;
   printf("visited vertices: %d/%d \n", vis, nv);

}


void Geometry::traverse(int c)
{
   int f= c / 3;

   unsigned short *vindex= getIndices();
   int index= vindex[c];

   vtxVisit[index]= mNumVisited;

   faceVisit[f]=1;
   mNumVisited++;

   //   printf("%d,", f);

   int left= edgeConFaces[pre(c)][0];
   int right= edgeConFaces[c][0];

   bool b= false;
   if (!faceVisit[left/3])
   {
      traverse(left);
   }

   if (!faceVisit[right/3])
   {
      traverse(right);
   }
}
