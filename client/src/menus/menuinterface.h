#ifndef MENUINTERFACE_H
#define MENUINTERFACE_H

#include <QObject>

class MenuInterface : public QObject
{
    Q_OBJECT

public:

   explicit MenuInterface(QObject *parent = 0);

};

#endif // MENUINTERFACE_H
