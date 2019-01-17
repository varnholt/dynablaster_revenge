#include <QStringList>

bool checkQtVersion( int major, int minor, QString* returnVersion )
{
   QString versionText = qVersion();
   if (returnVersion)
      *returnVersion= versionText;

   QStringList list= versionText.split('.');
   QList<int> version;
   foreach (const QString& text, list)
   {
      bool ok= false;
      int val= text.toInt(&ok);
      if (ok)
         version.append( val );
   }

   if (version.size() > 1)
   {
      // check major version
      if (version[0] < major)
         return false;
      else if (version[0] == major)
      {
         if (version[1] < minor)
            return false;
      }
      else if (version[0] > major)
      {
         return false;
      }
   }

   return true;
}
