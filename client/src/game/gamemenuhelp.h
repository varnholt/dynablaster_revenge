#ifndef GAMEMENUHELP_H
#define GAMEMENUHELP_H

#include <QObject>

class GameMenuHelp : public QObject
{
   Q_OBJECT

   public:

      GameMenuHelp(QObject *parent = 0);

      bool isEnabled() const;


   public slots:

      void pageChanged(const QString &page);


   protected:

      bool mEnabled;
};

#endif // GAMEMENUHELP_H
