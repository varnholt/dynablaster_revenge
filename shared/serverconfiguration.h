#ifndef SERVERCONFIGURATION_H
#define SERVERCONFIGURATION_H

class BinaryWriter;
class BinaryReader;

class ServerConfiguration
{
public:
   //! constructor
   ServerConfiguration();

   //! setter for bomb tick time
   void setBombTickTime(int time);

   //! getter for bomb tick time
   int getBombTickTime() const;

protected:
   //! bomb tick time
   int mBombTickTime;
};

BinaryWriter& operator<<(BinaryWriter& out, const ServerConfiguration& config);
BinaryReader& operator>>(BinaryReader& in, ServerConfiguration& config);

#endif  // SERVERCONFIGURATION_H
