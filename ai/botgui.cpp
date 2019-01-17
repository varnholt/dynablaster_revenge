#include "botgui.h"

// Qt
#include <QApplication>
#include <QLayout>
#include <QPushButton>

BotGui::BotGui(QWidget *parent) :
   QWidget(parent)
{
   QPushButton* addButton = new QPushButton(tr("add bot"), this);
   QPushButton* quitButton = new QPushButton(tr("quit"), this);

   QHBoxLayout *mainLayout = new QHBoxLayout();

   mainLayout->addWidget(addButton);
   mainLayout->addWidget(quitButton);

   setLayout(mainLayout);

   connect(
      addButton,
      SIGNAL(clicked()),
      this,
      SLOT(add())
   );

   connect(
      quitButton,
      SIGNAL(clicked()),
      qApp,
      SLOT(quit())
   );
}


void BotGui::add()
{
   mFactory.add();
}
