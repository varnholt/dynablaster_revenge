// header
#include "botbombmapitem.h"


//-----------------------------------------------------------------------------
/*!
   \param playerId player id
   \param flames number of flames
   \param id mapitem layout id
   \param x x position
   \param y y position
*/
BotBombMapItem::BotBombMapItem(
   int playerId,
   int flames,
   int id,
   int x,
   int y
)
   : BombMapItem(
        playerId,
        flames,
        id,
        x,
        y
     )
{
   mDropTime = QTime::currentTime();
}



//-----------------------------------------------------------------------------
/*!
   \return drop time
*/
const QTime &BotBombMapItem::getDropTime() const
{
   return mDropTime;
}


//-----------------------------------------------------------------------------
/*!
   \param flames flame count
*/
void BotBombMapItem::setFlameCount(int flames)
{
   mFlames = flames;
}


