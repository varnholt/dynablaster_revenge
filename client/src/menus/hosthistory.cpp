// header
#include "hosthistory.h"

// Qt
#include <QFile>
#include <QTextStream>

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

      QStringList hosts;
      hosts.append(host);

      // add all entries from the original host list (skip duplicates,
      // do not exceed max size)
      foreach (const QString& tmpHost, mHosts)
      {
         if (tmpHost != host)
         {
            if (hosts.size() < HISTORY_MAX_ENTRIES)
               hosts.append(tmpHost);
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
QStringList HostHistory::load(const QString& selected)
{
   QStringList hosts;

   deserialize();
   hosts = mHosts;

   hosts.removeAll(selected);
   hosts.prepend(selected);

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
      foreach (const QString& host, mHosts)
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
               !mHosts.contains(line)
            && !line.trimmed().isEmpty()
            && mHosts.size() < HISTORY_MAX_ENTRIES
          )
          {
             mHosts.append(line);
          }
      }
   }
}

