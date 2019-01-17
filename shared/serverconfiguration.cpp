// header
#include "serverconfiguration.h"


//-----------------------------------------------------------------------------
/*!
*/
ServerConfiguration::ServerConfiguration()
 : mBombTickTime(0)
{
}


//-----------------------------------------------------------------------------
/*!
   \param time bomb tick time
*/
void ServerConfiguration::setBombTickTime(int time)
{
   mBombTickTime = time;
}


//-----------------------------------------------------------------------------
/*!
   \return bomb tick time
*/
int ServerConfiguration::getBombTickTime() const
{
   return mBombTickTime;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream out
   \param config server configuration reference
*/
QDataStream& operator<<(QDataStream& out, const ServerConfiguration& config)
{
   out << config.getBombTickTime();
   return out;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream in
   \param config server configuration reference
*/
QDataStream& operator>>(QDataStream& in, ServerConfiguration& config)
{
   int bombTickTime = 0;

   in >> bombTickTime;

   config.setBombTickTime(bombTickTime);

   return in;
}

