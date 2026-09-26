// header
#include "hosthistory.h"

// Qt
#include <QFile>
#include <QTextStream>

#include <algorithm>

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
void HostHistory::add(const QString &host)
{
   if (!host.isEmpty())
   {
      deserialize();

      std::vector<QString> hosts;
      hosts.push_back(host);

      // add all entries from the original host list (skip duplicates,
      // do not exceed max size)
      for (const QString& tmpHost : mHosts)
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
std::vector<QString> HostHistory::load(const QString& selected)
{
   std::vector<QString> hosts;

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
   QFile file(HISTORY_FILE);
   if (file.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      QTextStream out(&file);
      for (const QString& host : mHosts)
      {
         out << host << "\n";
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HostHistory::deserialize()
{
   mHosts.clear();

   QFile file(HISTORY_FILE);
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream in(&file);
      while (!in.atEnd())
      {
          QString line = in.readLine();

          if (
               std::find(mHosts.begin(), mHosts.end(), line) == mHosts.end()
            && !line.trimmed().isEmpty()
            && mHosts.size() < HISTORY_MAX_ENTRIES
          )
          {
             mHosts.push_back(line);
          }
      }
   }
}

