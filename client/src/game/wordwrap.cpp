#include "wordwrap.h"



//-----------------------------------------------------------------------------
/*!
   \param line line to wrap
   \param allowedCharsPerLine allowed chars per line
   \return wrapped lines
*/
QList<QString> WordWrap::wrap(
   const QString &line,
   int allowedCharsPerLine
)
{
   QString trimmed = line.trimmed();
   QList<QString> result;

   QStringList split = trimmed.split(" ");

   QString tmpLine;
   QString nextWord;

   // chuck large chunks
   for (int i = 0; i < split.length(); i++)
   {
      QString test = split.at(i);

      if (test.length() > allowedCharsPerLine)
      {
         QString a = test.left(allowedCharsPerLine);
         QString b = test.mid(allowedCharsPerLine + 1);

         split.removeAt(i);
         split.insert(i, a);
         split.insert(i + 1, b);
      }
   }

   // iterate through every word
   for (int i = 0; i < split.size(); i++)
   {
      nextWord = split.at(i);

      if (tmpLine.size() + nextWord.length() + 1 < allowedCharsPerLine)
      {
         tmpLine.append(" ");
         tmpLine.append(nextWord);
      }
      else
      {
         result.append(tmpLine.trimmed());
         tmpLine = nextWord;
      }
   }

   if (!tmpLine.isEmpty())
      result.append(tmpLine.trimmed());

   return result;
}
