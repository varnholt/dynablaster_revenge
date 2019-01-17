// header
#include "errorpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "Error"

//-----------------------------------------------------------------------------
/*!
   \param errorType error type
   \param message error message
*/
ErrorPacket::ErrorPacket(Constants::ErrorType errorType, const QString &message)
   : Packet(Packet::ERROR),
     mErrorType(errorType),
     mErrorMessage(message)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
*/
ErrorPacket::ErrorPacket()
   : Packet(Packet::ERROR),
     mErrorType(Constants::ErrorDefault)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void ErrorPacket::enqueue(QDataStream & out)
{
   // write members
   out << mErrorType;
   out << mErrorMessage;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void ErrorPacket::dequeue(QDataStream & in)
{
   // read members
   int errorType = 0;
   in >> errorType;
   in >> mErrorMessage;

   mErrorType = (Constants::ErrorType)errorType;
}


//-----------------------------------------------------------------------------
/*!
   \return error type
*/
Constants::ErrorType ErrorPacket::getErrorType() const
{
   return mErrorType;
}


//-----------------------------------------------------------------------------
/*!
   \param errorType error type
*/
void ErrorPacket::setErrorType(Constants::ErrorType errorType)
{
   mErrorType = errorType;
}



//-----------------------------------------------------------------------------
/*!
   \return error message
*/
const QString &ErrorPacket::getErrorMessage() const
{
   return mErrorMessage;
}


//-----------------------------------------------------------------------------
/*!
   \param message error message
*/
void ErrorPacket::setErrorMessage(const QString &message)
{
   mErrorMessage = message;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void ErrorPacket::debug()
{
   // debug output login response
   qDebug(
      "ErrorPacket:debug: type: %d, message: %s",
      mErrorType,
      qPrintable(mErrorMessage)
   );
}


