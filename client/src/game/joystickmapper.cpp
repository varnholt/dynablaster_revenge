#include "joystickmapper.h"

JoystickMapper* JoystickMapper::sInstance = 0;


JoystickMapper::JoystickMapper(QObject *parent) :
   QObject(parent)
{
   initMaps();
}


void JoystickMapper::initMaps()
{
   mHatMap.insert(JoystickConstants::HatCentered,  "HatCentered" );
   mHatMap.insert(JoystickConstants::HatUp,        "HatUp"       );
   mHatMap.insert(JoystickConstants::HatRight,     "HatRight"    );
   mHatMap.insert(JoystickConstants::HatDown,      "HatDown"     );
   mHatMap.insert(JoystickConstants::HatLeft,      "HatLeft"     );
   mHatMap.insert(JoystickConstants::HatRightUp,   "HatRightUp"  );
   mHatMap.insert(JoystickConstants::HatRightDown, "HatRightDown");
   mHatMap.insert(JoystickConstants::HatLeftUp,    "HatLeftUp"   );
   mHatMap.insert(JoystickConstants::HatLeftDown , "HatLeftDown" );
}


JoystickMapper *JoystickMapper::getInstance()
{
   if (!sInstance)
      sInstance = new JoystickMapper();

   return sInstance;
}


QString JoystickMapper::getHatString(JoystickConstants::Hat key) const
{
   QString str;

   QMap<JoystickConstants::Hat, QString>::const_iterator iter =
      mHatMap.constFind(key);

   if (iter != mHatMap.constEnd())
      str = iter.value();

   return str;
}


//Qt::Key JoystickMapper::getFromString(const QString & string) const
//{
//   Qt::Key key = Qt::Key_PageUp;

//   if (mKeyMap.values().contains(string))

//   {
//      key = mKeyMap.key(string);
//   }

//   return key;
//}

