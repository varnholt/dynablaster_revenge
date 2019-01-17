// header
#include "protobotmemory.h"


//-----------------------------------------------------------------------------
/*!
*/
ProtoBotMemory::ProtoBotMemory()
 : mExtraX(0),
   mExtraY(0),
   mBombStoneX(0),
   mBombStoneY(0),
   mBombStoneCount(0)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBotMemory::reset()
{
   invalidateExtraPosition();
   invalidateBombStonePosition();
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBotMemory::invalidateExtraPosition()
{
   mExtraX = -1;
   mExtraY = -1;
}


//-----------------------------------------------------------------------------
/*!
   \param x extra x position
   \param y extra y position
*/
void ProtoBotMemory::setExtraPosition(int x, int y)
{
   mExtraX = x;
   mExtraY = y;
}



//-----------------------------------------------------------------------------
/*!
   \return \c true if extra position is valid
*/
bool ProtoBotMemory::isExtraPositionValid() const
{
   return mExtraX != -1;
}


//-----------------------------------------------------------------------------
/*!
   \return extra x position
*/
int ProtoBotMemory::getExtraPositionX() const
{
   return mExtraX;
}


//-----------------------------------------------------------------------------
/*!
   \return extra y position
*/
int ProtoBotMemory::getExtraPositionY() const
{
   return mExtraY;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBotMemory::invalidateBombStonePosition()
{
   mBombStoneX = -1;
   mBombStoneY = -1;
   mBombStoneCount = -1;
}


//-----------------------------------------------------------------------------
/*!
   \param x bomb stone position x
   \param y bomb stone position y
*/
void ProtoBotMemory::setBombStonePosition(int x, int y)
{
   mBombStoneX = x;
   mBombStoneY = y;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if bomb stone is valid
*/
bool ProtoBotMemory::isBombStonePositionValid() const
{
   return mBombStoneX != -1;
}


//-----------------------------------------------------------------------------
/*!
   \return bomb stone position x
*/
int ProtoBotMemory::getBombStonePositionX() const
{
   return mBombStoneX;
}


//-----------------------------------------------------------------------------
/*!
   \return bomb stone position y
*/
int ProtoBotMemory::getBombStonePositionY() const
{
   return mBombStoneY;
}


//-----------------------------------------------------------------------------
/*!
   \param count bomb bomb stone count
*/
void ProtoBotMemory::setBombStoneCount(int count)
{
   mBombStoneCount = count;
}


//-----------------------------------------------------------------------------
/*!
   \return bomb stone neighbor count
*/
int ProtoBotMemory::getBombStoneCount() const
{
   return mBombStoneCount;
}
