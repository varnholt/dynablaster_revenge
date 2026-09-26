// header
#include "hosthistory.h"

#include "stringutils.h"

#include <algorithm>
#include <fstream>

// defines
#define HISTORY_FILE "history.dr"
#define HISTORY_MAX_ENTRIES 3


//-----------------------------------------------------------------------------
/*!
*/
HostHistory::HostHistory()
{
}


//-----------------------------------------------------------------------------
/*!
   \param host host to add to list
*/
void HostHistory::add(const std::string& host)
{
   if (!host.empty())
   {
      deserialize();

      std::vector<std::string> hosts;
      hosts.push_back(host);

      // add all entries from the original host list (skip duplicates,
      // do not exceed max size)
      for (const std::string& tmpHost : mHosts)
      {
         if (tmpHost != host)
         {
            if (hosts.size() < HISTORY_MAX_ENTRIES)
               hosts.push_back(tmpHost);
         }
      }

      mHosts = hosts;

      // write entries to disk
      serialize();
   }
}


//-----------------------------------------------------------------------------
/*!
   \return list of hosts
*/
std::vector<std::string> HostHistory::load(const std::string& selected)
{
   std::vector<std::string> hosts;

   deserialize();
   hosts = mHosts;

   std::erase(hosts, selected);
   hosts.insert(hosts.begin(), selected);

   return hosts;
}


//-----------------------------------------------------------------------------
/*!
*/
void HostHistory::serialize()
{
   std::ofstream file(HISTORY_FILE);
   if (file.is_open())
   {
      for (const std::string& host : mHosts)
      {
         file << host << "\n";
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HostHistory::deserialize()
{
   mHosts.clear();

   std::ifstream file(HISTORY_FILE);
   if (file.is_open())
   {
      std::string line;
      while (std::getline(file, line))
      {
          if (
               std::find(mHosts.begin(), mHosts.end(), line) == mHosts.end()
            && !StringUtils::trim(line).empty()
            && mHosts.size() < HISTORY_MAX_ENTRIES
          )
          {
             mHosts.push_back(line);
          }
      }
   }
}
