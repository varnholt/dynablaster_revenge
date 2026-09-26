#ifndef HOSTHISTORY_H
#define HOSTHISTORY_H

#include <string>
#include <vector>


class HostHistory
{

public:

   //! constructor
   HostHistory();

   //! add an entry
   void add(const std::string& host);

   //! get all entries
   std::vector<std::string> load(const std::string& selected = std::string());


protected:

   //! serialize entries
   void serialize();

   //! deserialize entries
   void deserialize();

   //! list of hosts
   std::vector<std::string> mHosts;

};

#endif // HOSTHISTORY_H
