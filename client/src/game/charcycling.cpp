#include "charcycling.h"


CharCycling::CharCycling()
{
   mChars = " ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.:_-";
}


void CharCycling::up()
{
   mCharIndex++;
}


void CharCycling::down()
{
   mCharIndex--;

   if (mCharIndex < 0)
      mCharIndex = mChars.length() - 1;
}


void CharCycling::reset()
{
   mCharIndex = 0;
}


void CharCycling::setChar(const QChar &c)
{
   for (int i = 0; i < mChars.size(); i++)
   {
      if (mChars.at(i).toLower() == c.toLower())
      {
         setCharIndex(i);
         break;
      }
   }
}


const QChar CharCycling::getChar() const
{
   return mChars.at( mCharIndex % mChars.size() );
}


QString CharCycling::modify(const QString &input, int cursor)
{
   QString alteredText;

   // left half
   int maxLeft = qMin(input.length(), cursor);
   for (int i = 0; i < maxLeft; i++)
      alteredText.push_back(input.at(i));

   // modified char
   alteredText.push_back(getChar());

   // right half
   for (int i = cursor + 1; i < input.length(); i++)
      alteredText.push_back(input.at(i));

   return alteredText;
}


void CharCycling::setCharIndex(int index)
{
   mCharIndex = index;
}


int CharCycling::getCharIndex() const
{
   return mCharIndex;
}
