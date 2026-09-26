#ifndef PROTOBOTINSULTS_H
#define PROTOBOTINSULTS_H

#include <string>
#include <vector>

// Qt
#include <QObject>

class ProtoBotInsults : public QObject
{
   Q_OBJECT

public:

   explicit ProtoBotInsults(QObject *parent = 0);
   
signals:

   void sendMessage(const std::string&);

   
protected slots:

   void shootAgain();
   void insult();

protected:

   std::vector<std::string> mInsults;
   
};

#endif // PROTOBOTINSULTS_H
