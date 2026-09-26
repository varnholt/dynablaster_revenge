#ifndef ERRORPACKET_H
#define ERRORPACKET_H

#include <string>

// base
#include "packet.h"

// shared
#include "constants.h"

class ErrorPacket : public Packet
{
public:
   //! write constructor
   ErrorPacket(Constants::ErrorType errorType, const std::string& message);

   //! read constructor
   ErrorPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for error type
   Constants::ErrorType getErrorType() const;

   //! setter for error type
   void setErrorType(Constants::ErrorType errorType);

   //! getter for error message
   const std::string& getErrorMessage() const;

   //! setter for error message
   void setErrorMessage(const std::string& message);

protected:
   //! error type
   Constants::ErrorType mErrorType;

   //! error message
   std::string mErrorMessage;
};

#endif  // ERRORPACKET_H
