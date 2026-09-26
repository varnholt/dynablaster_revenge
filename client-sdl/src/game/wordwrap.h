#ifndef WORDWRAP_H
#define WORDWRAP_H

#include <string>
#include <vector>


class WordWrap
{
public:

   //! word wrap a line
   static std::vector<std::string> wrap(const std::string& line, int allowed_chars_per_line);
};

#endif // WORDWRAP_H
