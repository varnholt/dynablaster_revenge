#ifndef ERRORPACKET_H
#define ERRORPACKET_H

// base
#include "packet.h"

// shared
#include "constants.h"


class ErrorPacket : public Packet
{
   public:

      //! write constructor
      ErrorPacket(Constants::ErrorType errorType, const QString& message);

      //! read constructor
      ErrorPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for error type
      Constants::ErrorType getErrorType() const;

      //! setter for error type
      void setErrorType(Constants::ErrorType errorType);

      //! getter for error message
      const QString& getErrorMessage() const;

      //! setter for error message
      void setErrorMessage(const QString& message);



   protected:

      //! error type
      Constants::ErrorType mErrorType;

      //! error message
      QString mErrorMessage;
};

#endif // ERRORPACKET_H
