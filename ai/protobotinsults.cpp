#include "protobotinsults.h"

#include <QRandomGenerator>

#include "timer.h"

ProtoBotInsults::ProtoBotInsults(QObject *parent) :
   QObject(parent)
{
   mInsults.push_back("You fight like a dairy farmer.");
   mInsults.push_back("Soon you'll be wearing my sword like a shish-kabob!");
   mInsults.push_back("You're no match for my brains, you poor fool.");
   mInsults.push_back("This is the end for you, you gutter-crawling cur!");
   mInsults.push_back("There are no words for how disgusting you are.");
   mInsults.push_back("I'm not going to take your insolence sitting down!");
   mInsults.push_back("People fall at my feet when they see me coming.");
   mInsults.push_back("I've heard you are a contemptible sneak.");
   mInsults.push_back("My handkerchief will wipe up your blood!");
   mInsults.push_back("If your brother's like you, better to marry a pig.");
   mInsults.push_back("Every word you say to me is stupid.");
   mInsults.push_back("My last fight ended with my hands covered in blood.");
   mInsults.push_back("You are a pain in the backside, sir!");
   mInsults.push_back("My sword is famous all over the Caribbean!");
   mInsults.push_back("There are no clever moves that can help you now.");
   mInsults.push_back("I will milk every drop of blood from your body!");
   mInsults.push_back("I hope you have a boat ready for a quick escape.");
   mInsults.push_back("My name is feared in every dirty corner of this island!");

   shootAgain();
}


// neo-dimension.wikidot.com/somi:insult-swordfighting

void ProtoBotInsults::shootAgain()
{
   Timer::singleShot(20000 + QRandomGenerator::global()->bounded(60000), [this]() { insult(); });
}



void ProtoBotInsults::insult()
{
   if (!mInsults.empty())
   {
      emit sendMessage(mInsults[QRandomGenerator::global()->bounded(mInsults.size() - 1)]);
      shootAgain();
   }
}


