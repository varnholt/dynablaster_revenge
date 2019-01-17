#ifndef SERVERCONFIGURATION_H
#define SERVERCONFIGURATION_H

#include <QDataStream>

class ServerConfiguration
{
   public:

      //! constructor
      ServerConfiguration();

      //! setter for bomb tick time
      void setBombTickTime(int time);

      //! getter for bomb tick time
      int getBombTickTime() const;


   protected:

      //! bomb tick time
      int mBombTickTime;
};

QDataStream& operator<<(QDataStream& out, const ServerConfiguration& config);
QDataStream& operator>>(QDataStream& in, ServerConfiguration& config);

#endif // SERVERCONFIGURATION_H
