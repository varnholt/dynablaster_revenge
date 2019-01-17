#ifndef BOTGUI_H
#define BOTGUI_H

// Qt
#include <QWidget>

// bot
#include "botfactory.h"

class BotGui : public QWidget
{
   Q_OBJECT
public:
   explicit BotGui(QWidget *parent = 0);
   
signals:
   
public slots:

protected slots:

   void add();

protected:

   BotFactory mFactory;
};

#endif // BOTGUI_H
