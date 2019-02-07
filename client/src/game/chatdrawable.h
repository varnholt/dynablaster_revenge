#ifndef CHATDRAWABLE_H
#define CHATDRAWABLE_H

#include <QObject>

#include "drawable.h"

class ChatDrawable : public QObject, public Drawable
{
    Q_OBJECT

public:

   ChatDrawable(RenderDevice*);

   void initializeGL();
   void paintGL();

signals:

public slots:

};

#endif // CHATDRAWABLE_H
