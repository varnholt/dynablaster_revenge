// header
#include "serverconfiguration.h"

// shared
#include "binaryreader.h"
#include "binarywriter.h"

//-----------------------------------------------------------------------------
/*!
 */
ServerConfiguration::ServerConfiguration() : mBombTickTime(0)
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
BinaryWriter& operator<<(BinaryWriter& out, const ServerConfiguration& config)
{
   out << config.getBombTickTime();
   return out;
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream in
   \param config server configuration reference
*/
BinaryReader& operator>>(BinaryReader& in, ServerConfiguration& config)
{
   int bombTickTime = 0;

   in >> bombTickTime;

   config.setBombTickTime(bombTickTime);

   return in;
}
