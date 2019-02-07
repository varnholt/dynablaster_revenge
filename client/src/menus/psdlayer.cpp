#include "psdlayer.h"
#include "image/image.h"
#include "framework/gldevice.h"
#include "render/texturepool.h"

PSDLayer::PSDLayer()
 : mLayer(0)
 , mTexture()
 , mVertexBuffer(0)
 , mIndexBuffer(0)
 , mRed(1.0f)
 , mGreen(1.0f)
 , mBlue(1.0f)
 , mOpacity(0.0f)
 , mU(0.0f)
 , mV(0.0f)
{
}

PSDLayer::PSDLayer(PSD::Layer* layer, float z, bool unwrap)
 : mLayer(layer)
 , mTexture()
 , mVertexBuffer(0)
 , mIndexBuffer(0)
 , mRed(1.0f)
 , mGreen(1.0f)
 , mBlue(1.0f)
 , mOpacity(layer->getOpacity()/255.0)
{
   int w= layer->getWidth();
   int h= layer->getHeight();

   // next power of two
   int tw= log2( w );
   int th= log2( h );

   tw= w;
   th= h;

   Image* image= new Image(tw, th);
   Image *src= layer->getImage();

   if (!unwrap)
   {
      image->scaled(*src);
   }
   else
   {
      image->copy(0,0, *src, true);
   }

   mTexture= TexturePool::Instance()->getTexture(image, TexturePool::Trilinear | TexturePool::Clamp);

   delete image;

   mU= (float)w / tw;
   mV= (float)h / th;

   // create vertexbuffer
   mVertexBuffer= activeDevice->createVertexBuffer(4 * sizeof(Vertex));
   Vertex *vtx= (Vertex*)activeDevice->lockVertexBuffer(mVertexBuffer);
   *vtx++= Vertex(0, 0, z,  0,  0 );
   *vtx++= Vertex(0, h, z,  0,  mV);
   *vtx++= Vertex(w, 0, z,  mU, 0 );
   *vtx++= Vertex(w, h, z,  mU, mV);
   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // create indexbuffer
   mIndexBuffer= activeDevice->createIndexBuffer(6 * sizeof(unsigned short));
   unsigned short *idx= (unsigned short*)activeDevice->lockVertexBuffer(mIndexBuffer);
   *idx++= 0;  *idx++= 1;  *idx++= 2;
   *idx++= 1;  *idx++= 3;  *idx++= 2;
   activeDevice->unlockIndexBuffer(mIndexBuffer);
}

PSDLayer::~PSDLayer()
{
}

PSD::Layer* PSDLayer::getLayer() const
{
   return mLayer;
}

float PSDLayer::getU() const
{
   return mU;
}

float PSDLayer::getV() const
{
   return mV;
}

int PSDLayer::getWidth() const
{
   return mLayer->getWidth();
}

int PSDLayer::getHeight() const
{
   return mLayer->getHeight();
}

int PSDLayer::getLeft() const
{
   return mLayer->getLeft();
}

int PSDLayer::getRight() const
{
   return getLeft() + getWidth();
}

int PSDLayer::getTop() const
{
   return mLayer->getTop();
}

int PSDLayer::getBottom() const
{
   return mLayer->getTop() + mLayer->getHeight();
}

int PSDLayer::log2(int x) const
{
   int r= 1;
   while (x>0)
   {
      x>>=1;
      r<<=1;
   }
   return r;
}

unsigned int PSDLayer::getTexture() const
{
   return mTexture.getTexture();
}

unsigned int PSDLayer::getVertexBuffer() const
{
   return mVertexBuffer;
}

unsigned int PSDLayer::getIndexBuffer() const
{
   return mIndexBuffer;
}

float PSDLayer::getOpacity() const
{
   return mOpacity;
}

void PSDLayer::setOpacity(float opacity)
{
   mOpacity= opacity;
}

void PSDLayer::setColor(float r, float g, float b)
{
   mRed= r;
   mGreen= g;
   mBlue= b;
}

void PSDLayer::render(float x, float y, float alpha)
{
   // set opacity
   glColor4f(mRed, mGreen, mBlue, mOpacity * alpha);

   glPushMatrix();
   glTranslatef(mLayer->getLeft()+x, mLayer->getTop()+y, 0.0f);

   glBindTexture(GL_TEXTURE_2D, mTexture);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexBuffer );
   glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(3*sizeof(float)) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glPopMatrix();
}


void PSDLayer::renderScaled(
   float x,
   float y,
   float alpha,
   float scaleX,
   float scaleY,
   float scaleZ
)
{
   // set opacity
   glColor4f(mRed, mGreen, mBlue, mOpacity * alpha);

   glPushMatrix();
   glTranslatef(mLayer->getLeft()+x, mLayer->getTop()+y, 0.0f);
   glScalef(scaleX, scaleY, scaleZ);

   glBindTexture(GL_TEXTURE_2D, mTexture);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexBuffer );
   glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(3*sizeof(float)) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glPopMatrix();
}
