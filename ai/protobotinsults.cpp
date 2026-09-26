#include "protobotinsults.h"

#include <QRandomGenerator>

ProtoBotInsults::ProtoBotInsults(QObject *parent) :
   QObject(parent)
{
   mInsults.push_back(tr("You fight like a dairy farmer."));
   mInsults.push_back(tr("Soon you'll be wearing my sword like a shish-kabob!"));
   mInsults.push_back(tr("You're no match for my brains, you poor fool."));
   mInsults.push_back(tr("This is the end for you, you gutter-crawling cur!"));
   mInsults.push_back(tr("There are no words for how disgusting you are."));
   mInsults.push_back(tr("I'm not going to take your insolence sitting down!"));
   mInsults.push_back(tr("People fall at my feet when they see me coming."));
   mInsults.push_back(tr("I've heard you are a contemptible sneak."));
   mInsults.push_back(tr("My handkerchief will wipe up your blood!"));
   mInsults.push_back(tr("If your brother's like you, better to marry a pig."));
   mInsults.push_back(tr("Every word you say to me is stupid."));
   mInsults.push_back(tr("My last fight ended with my hands covered in blood."));
   mInsults.push_back(tr("You are a pain in the backside, sir!"));
   mInsults.push_back(tr("My sword is famous all over the Caribbean!"));
   mInsults.push_back(tr("There are no clever moves that can help you now."));
   mInsults.push_back(tr("I will milk every drop of blood from your body!"));
   mInsults.push_back(tr("I hope you have a boat ready for a quick escape."));
   mInsults.push_back(tr("My name is feared in every dirty corner of this island!"));

   shootAgain();
}


// neo-dimension.wikidot.com/somi:insult-swordfighting

void ProtoBotInsults::shootAgain()
{
   QTimer::singleShot(
      20000 + QRandomGenerator::global()->bounded(60000),
      this,
      SLOT(insult())
   );
}



void ProtoBotInsults::insult()
{
   if (!mInsults.empty())
   {
      emit sendMessage(mInsults[QRandomGenerator::global()->bounded(mInsults.size() - 1)]);
      shootAgain();
   }
}


