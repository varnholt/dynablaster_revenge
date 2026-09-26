#ifndef HOSTHISTORY_H
#define HOSTHISTORY_H

// Qt
#include <QString>

#include <vector>


class HostHistory
{

public:

   //! constructor
   HostHistory();

   //! add an entry
   void add(const QString& host);

   //! get all entries
   std::vector<QString> load(const QString& selected = QString());


protected:

   //! serialize entries
   void serialize();

   //! deserialize entries
   void deserialize();

   //! list of hosts
   std::vector<QString> mHosts;

};

#endif // HOSTHISTORY_H
