#ifndef BOTCONSTANTS_H
#define BOTCONSTANTS_H

class BotConstants
{

   public:
      BotConstants();


};

// it is import to have the keyboard correction epsilon smaller than the
// "field reached precision" -> so the bot doesn't think he missed a field
// after the keyboard correction was applied
#define FIELD_REACHED_PRECISION     0.15f
#define KEYBOARD_CORRECTION_EPSILON 0.1f

#define DEBUG_MAPITEMS              false
#define DEBUG_PATH                  false
#define DEBUG_SCORES                false
#define DEBUG_KEYSPRESSED           false
#define DEBUG_POSSIBLE_ACTIONS      false
#define DEBUG_EXECUTED_ACTIONS      false
#define DEBUG_CURRENT_HAZARDOUS     false
#define DEBUG_ESCAPE_PATH           false
#define DEBUG_WALK_ACTION           false
#define DEBUG_BOMB_DROP             false


#endif // BOTCONSTANTS_H
