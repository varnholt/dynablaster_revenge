#ifndef PROTOBOTINSULTS_H
#define PROTOBOTINSULTS_H

// Qt
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>

class ProtoBotInsults : public QObject
{
   Q_OBJECT

public:

   explicit ProtoBotInsults(QObject *parent = 0);
   
signals:

   void sendMessage(const QString&);

   
protected slots:

   void shootAgain();
   void insult();

protected:

   QList<QString> mInsults;
   
};

#endif // PROTOBOTINSULTS_H
