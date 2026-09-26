#include "wordwrap.h"

#include "stringutils.h"



//-----------------------------------------------------------------------------
/*!
   \param line line to wrap
   \param allowed_chars_per_line allowed chars per line
   \return wrapped lines
*/
std::vector<std::string> WordWrap::wrap(const std::string& line, int allowed_chars_per_line)
{
   const std::string trimmed = StringUtils::trim(line);
   std::vector<std::string> split = StringUtils::split(trimmed, ' ');

   // chop large chunks
   for (size_t i = 0; i < split.size(); i++)
   {
      const std::string test = split[i];

      if (static_cast<int>(test.size()) > allowed_chars_per_line)
      {
         const std::string a = test.substr(0, static_cast<size_t>(allowed_chars_per_line));
         const std::string b = test.substr(static_cast<size_t>(allowed_chars_per_line) + 1);

         split.erase(split.begin() + static_cast<long>(i));
         split.insert(split.begin() + static_cast<long>(i), a);
         split.insert(split.begin() + static_cast<long>(i) + 1, b);
      }
   }

   std::vector<std::string> result;
   std::string tmp_line;

   // iterate through every word
   for (size_t i = 0; i < split.size(); i++)
   {
      const std::string& next_word = split[i];

      if (static_cast<int>(tmp_line.size() + next_word.size() + 1) < allowed_chars_per_line)
      {
         tmp_line.append(" ");
         tmp_line.append(next_word);
      }
      else
      {
         result.push_back(StringUtils::trim(tmp_line));
         tmp_line = next_word;
      }
   }

   if (!tmp_line.empty())
   {
      result.push_back(StringUtils::trim(tmp_line));
   }

   return result;
}
