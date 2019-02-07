#ifndef MAINDRAWABLE_H
#define MAINDRAWABLE_H

#include "framework/globaltime.h"
#include <QList>
#include <QGLWidget>
#include <QMutex>

class Drawable;
class SharedGLWidget;
class FrameBuffer;

class MainDrawable : public QGLWidget, public GlobalTime
{
   Q_OBJECT

public:
   MainDrawable();
   MainDrawable(QWidget *parent, const QGLFormat& format);
   virtual ~MainDrawable();

   static void setFormat(const QGLFormat& format);
   static MainDrawable* getInstance();

   virtual FrameBuffer* getRenderBuffer(int id= 0) const = 0;
   virtual void animate() = 0;

   QGLWidget* getSharedContext() const;
   void addDrawable(Drawable *drawable);
   void removeDrawable(Drawable *drawable);

   float getTime() const;
   void setTime(float time);

protected:
   QMutex               mMutex;
   SharedGLWidget*      mShared;
   static QGLFormat     mFormat;
   QList<Drawable*>     mDrawables;
   float                mTime;
   static MainDrawable* mInstance;
};

#endif
