#pragma once

class String;

class Stream
{
public:
   Stream();

   virtual ~Stream();

   virtual const String& getPath() const;
   
   int getEndian() const;              //! get current endian mode
   void setEndian(int bigEndian);      //! set endian mode

   // abstract functions
   virtual void  getData(void *dst, int size) = 0;    //! get "size" bytes and store in "dst"
   virtual void  writeData(void *dst, int size) = 0;  //! write "size" bytes

   virtual char  getChar();                     //! get single character
   virtual unsigned char getByte();             //! get single byte
   virtual int   getWord();                     //! get word (2 byte)
   virtual short getShort();                    //! get short
   virtual int   getInt();                      //! get integer (4 byte)
   virtual float getFloat();                    //! get float (4 byte)
   virtual char* getString();                   //! get string (0 terminated)

   virtual void  writeChar(char);                 //! write single character
   virtual void  writeByte(unsigned char);        //! write single byte
   virtual void  writeWord(unsigned short);       //! write word (2 byte)
   virtual void  writeShort(short);               //! write short
   virtual void  writeInt(int);                   //! write integer (4 byte)
   virtual void  writeFloat(float);               //! write float (4 byte)
   virtual void  writeString(const char*);        //! write string (0 terminated)

   virtual int   pos() const;
   virtual void  skip(int size);

private:
   void swap32(void *data);
   void swap16(void *data);

   int        mEndian;
   static int mMachineEndian;

protected:
   int        mPosition;
};

Stream& operator << (Stream& stream, float&);
void operator << (float&, Stream& stream);
Stream& operator << (Stream& stream, int&);

void operator >> (float&, Stream& stream);

