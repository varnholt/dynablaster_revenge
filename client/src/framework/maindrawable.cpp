// header
#include "maindrawable.h"

// Qt
#include <QMutexLocker>


QGLFormat MainDrawable::mFormat;
MainDrawable* MainDrawable::mInstance= 0;


class SharedGLWidget : public QGLWidget
{
   public:

      SharedGLWidget(QGLWidget* shared);
      void paintEvent(QPaintEvent *);
      void resizeEvent(QResizeEvent *);
};


SharedGLWidget::SharedGLWidget(QGLWidget* shared)
 : QGLWidget(QGLFormat::defaultFormat(), 0, shared)
{
}


void SharedGLWidget::paintEvent(QPaintEvent * /*e*/)
{
}


void SharedGLWidget::resizeEvent(QResizeEvent * /*e*/)
{
}


MainDrawable::MainDrawable(QWidget *parent, const QGLFormat& format)
 : QGLWidget(format, parent),
   mShared(0),
   mTime(0.0f)
{
   setCursor( QCursor(Qt::BlankCursor) );
   mInstance= this;
   mShared= new SharedGLWidget( this );
}


MainDrawable::~MainDrawable()
{
   delete mShared;
   mShared = 0;
}


MainDrawable* MainDrawable::getInstance()
{
   return mInstance;
}


QGLWidget* MainDrawable::getSharedContext() const
{
   return mShared;
}


float MainDrawable::getTime() const
{
   return mTime;
}


void MainDrawable::setTime(float time)
{
   mTime= time;
}


void MainDrawable::addDrawable(Drawable *drawable)
{
   mDrawables.append( drawable );
}


void MainDrawable::removeDrawable(Drawable *drawable)
{
   mDrawables.removeAll( drawable );
}


void MainDrawable::setFormat(const QGLFormat& format)
{
   mFormat= format;
}

