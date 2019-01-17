#include "protobotinsults.h"


ProtoBotInsults::ProtoBotInsults(QObject *parent) :
   QObject(parent)
{
   mInsults << tr("You fight like a dairy farmer.");
   mInsults << tr("Soon you'll be wearing my sword like a shish-kabob!");
   mInsults << tr("You're no match for my brains, you poor fool.");
   mInsults << tr("This is the end for you, you gutter-crawling cur!");
   mInsults << tr("There are no words for how disgusting you are.");
   mInsults << tr("I'm not going to take your insolence sitting down!");
   mInsults << tr("People fall at my feet when they see me coming.");
   mInsults << tr("I've heard you are a contemptible sneak.");
   mInsults << tr("My handkerchief will wipe up your blood!");
   mInsults << tr("If your brother's like you, better to marry a pig.");
   mInsults << tr("Every word you say to me is stupid.");
   mInsults << tr("My last fight ended with my hands covered in blood.");
   mInsults << tr("You are a pain in the backside, sir!");
   mInsults << tr("My sword is famous all over the Caribbean!");
   mInsults << tr("There are no clever moves that can help you now.");
   mInsults << tr("I will milk every drop of blood from your body!");
   mInsults << tr("I hope you have a boat ready for a quick escape.");
   mInsults << tr("My name is feared in every dirty corner of this island!");

   shootAgain();
}


// neo-dimension.wikidot.com/somi:insult-swordfighting

void ProtoBotInsults::shootAgain()
{
   QTimer::singleShot(
      20000 + (qrand() % 60000),
      this,
      SLOT(insult())
   );
}



void ProtoBotInsults::insult()
{
   if (!mInsults.isEmpty())
   {
      emit sendMessage(mInsults[qrand() % (mInsults.size()-1)]);
      shootAgain();
   }
}


