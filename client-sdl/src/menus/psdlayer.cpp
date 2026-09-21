#include "psdlayer.h"
#include "framework/gldevice.h"
#include "image/image.h"
#include "math/matrix.h"
#include "render/texturepool.h"

PSDLayer::PSDLayer() : mLayer(0), mTexture(), mVertexBuffer(0), mIndexBuffer(0), mOpacity(0.0f), mU(0.0f), mV(0.0f)
{
}

PSDLayer::PSDLayer(PSD::Layer* layer, float z, bool unwrap)
    : mLayer(layer), mTexture(), mVertexBuffer(0), mIndexBuffer(0), mOpacity(layer->getOpacity() / 255.0)
{
   int w = layer->getWidth();
   int h = layer->getHeight();

   // the original code computed a next-power-of-two size (log2) here, then immediately
   // overwrote it with the exact size on the next two lines - dead code even upstream, since
   // whatever hardware constraint once required power-of-two textures is long gone. Dropped.
   int tw = w;
   int th = h;

   Image* image = new Image(tw, th);
   Image* src = layer->getImage();

   if (!unwrap)
   {
      image->scaled(*src);
   }
   else
   {
      image->copy(0, 0, *src, true);
   }

   mTexture = TexturePool::Instance()->getTexture(image, TexturePool::Trilinear | TexturePool::Clamp);

   delete image;

   mU = (float)w / tw;
   mV = (float)h / th;

   // create vertexbuffer
   mVertexBuffer = activeDevice->createVertexBuffer(4 * sizeof(Vertex));
   Vertex* vtx = (Vertex*)activeDevice->lockVertexBuffer(mVertexBuffer);
   *vtx++ = Vertex(0, 0, z, 0, 0);
   *vtx++ = Vertex(0, h, z, 0, mV);
   *vtx++ = Vertex(w, 0, z, mU, 0);
   *vtx++ = Vertex(w, h, z, mU, mV);
   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // create indexbuffer
   mIndexBuffer = activeDevice->createIndexBuffer(6 * sizeof(unsigned short));
   unsigned short* idx = (unsigned short*)activeDevice->lockIndexBuffer(mIndexBuffer);
   *idx++ = 0;
   *idx++ = 1;
   *idx++ = 2;
   *idx++ = 1;
   *idx++ = 3;
   *idx++ = 2;
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
   mOpacity = opacity;
}

void PSDLayer::render(float x, float y, float alpha)
{
   // the legacy fixed-function draw (glPushMatrix/glTranslatef, glColor4f for opacity,
   // glEnableClientState/glVertexPointer/glTexCoordPointer) becomes a plain attribute-array
   // draw through the shared texalphaignore shader, with the translation folded into a world
   // matrix uploaded the same way every other draw call in the engine uploads one.
   Matrix world;
   world.translate(Vector(mLayer->getLeft() + x, mLayer->getTop() + y, 0.0f));
   activeDevice->push(world);

   glBindTexture(GL_TEXTURE_2D, mTexture);

   activeDevice->setParameter(activeDevice->getParameterIndex("alpha"), mOpacity * alpha);

   glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(float)));

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   activeDevice->pop();
}
