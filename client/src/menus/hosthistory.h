#ifndef HOSTHISTORY_H
#define HOSTHISTORY_H

// Qt
#include <QStringList>


class HostHistory
{

public:

   //! constructor
   HostHistory();

   //! add an entry
   void add(const QString& host);

   //! get all entries
   QStringList load(const QString& selected = QString::null);


protected:

   //! serialize entries
   void serialize();

   //! deserialize entries
   void deserialize();

   //! list of hosts
   QStringList mHosts;

};

#endif // HOSTHISTORY_H
