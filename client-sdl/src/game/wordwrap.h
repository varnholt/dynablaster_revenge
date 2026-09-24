#ifndef WORDWRAP_H
#define WORDWRAP_H

// Qt
#include <QStringList>


class WordWrap
{
public:

   //! word wrap a line
   static QList<QString> wrap(const QString& line, int allowedCharsPerLine);
};

#endif // WORDWRAP_H
