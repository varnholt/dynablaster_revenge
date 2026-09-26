#ifndef WORDWRAP_H
#define WORDWRAP_H

// Qt
#include <QStringList>

#include <vector>


class WordWrap
{
public:

   //! word wrap a line
   static std::vector<QString> wrap(const QString& line, int allowedCharsPerLine);
};

#endif // WORDWRAP_H
