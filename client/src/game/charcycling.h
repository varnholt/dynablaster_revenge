#ifndef CHARCYCLING_H
#define CHARCYCLING_H

#include <QString>

class CharCycling
{
   public:

      CharCycling();

      void up();
      void down();
      void reset();

      void setChar(const QChar& c);
      const QChar getChar() const;

      QString modify(const QString& input, int cursor);

      void setCharIndex(int);

      int getCharIndex() const;


   protected:

      QString mChars;

      int mCharIndex;
};

#endif // CHARCYCLING_H
