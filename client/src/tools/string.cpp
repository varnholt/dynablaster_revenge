#include "string.h"
#include "stream.h"
#include <memory.h>
#include <string.h>

//! construct empty string
String::String()
: Referenced()
, mData(0)
{
}

//! construct string from char*
String::String(const char* text)
: Referenced()
{
   if (text)
   {
      int size= strlen(text);
      alloc( size );
      memcpy(mData, text, size+1);
   }
   else
   {
      mData= 0;
   }
}

//! construct reference of given String "other"
String::String(const String& other)
: Referenced(other)
{
   mData= (char*)other.data();
}


//! destructor: delete string if no more references exist
String::~String()
{
   if (getRefCount()==1)
      dealloc();
}

//! get middle part of string
String String::mid(int start, int end) const
{
   if (start<0) start= 0;
   if (start > size()) start=size();

   if (end < 0) end= 0;
   if (end > size()) end=size();

   String s;
   s.alloc(end-start);
   char* dst= (char*)s.data();
   memcpy(dst, data()+start, end-start);
   dst[end-start]= 0;

   return s;
}

bool String::isEmpty() const
{
   return (size()==0);
}

//! find substring
int String::indexOf(const String& other) const
{
   int index= 0;
   int s1= size();
   int s2= other.size();
   if (s2<=s1)
   {
      while (index < s1-s2)
      {
         int i;
         for (i=0; i<s2; i++)
         {
            if (mData[index+i] != other[i])
               break;
         }
         if (i == s2) // match
            return index;
         index++;
      }
   }
   return -1;
}

//! allocate data for "size" characters + \0
void String::alloc(int size)
{
   // allocate single data chunk to store size, text and \0
   mData= new char[size+sizeof(int)+1];
   // store size
   *((int*)mData)= size;
   // "data" always points to the text
   mData += sizeof(int);
}

//! deallocate data
void String::dealloc()
{
   char *data= dataIntern();
   if (data)
      delete[] data;
   mData= 0;
}

//! set size field
void String::setSize(int size)
{
   int* data= (int*)dataIntern();
   if (data)
      *data= size;
}

//! get originally allocated data
char* String::dataIntern() const
{
   if (mData)
      return mData - sizeof(int);
   else
      return 0;
}

String& String::operator << (Stream& stream)
{
   load(&stream);
   return *this;
}

//! cast to char*
String::operator const char* () const 
{ 
   return mData; 
}

//! get char at given index
char String::operator[](int index) const
{
   return mData[index];
}

//! assignment operator: create reference of given string
String& String::operator = (const String& other)
{
   if (this != &other)
   {
      // string is not referenced: delete data and reference counter
      if (getRefCount() == 1)
      {
         dealloc();
         delete mReferences;
      }

      mReferences= other.getRef();
      addRef();
      mData= (char*)other.data();
   }
   return *this;
}

//! clear string
void String::clear()
{
   // if string was referenced we have to leave the existing data untouched
   if (!copyRef())
      dealloc();
   else
      mData= 0;
}

//! get char from string at given "index"
char String::get(int index) const
{
   return mData[index];
}

//! append "other" at end of string
void String::append(const String& other)
{
   char* oldData= dataIntern();
   char* data= mData;

   bool needDel= !copyRef();

   int size1= size();
   int size2= other.size();
   alloc(size1+size2);
   if (size1>0)
      memcpy(mData, data, size1);
   if (other.data())
      memcpy(mData+size1, other.data(), size2+1);
   else
      mData[size1]= 0;

   if (needDel)
      delete[] oldData;
}

// append
void String::operator += (const String& other)
{
   append(other);
}

// concat strings
String String::operator + (const String& other) const
{
   String n= *this;
   n.append(other);
   return n;
}

// equal?
bool String::operator == (const String& other) const
{
   if (size() != other.size())
      return false;
   int len= size();
   for (int i=0;i<len;i++)
   {
      if (mData[i] != other[i])
         return false;
   }
   return true;
}

// equal?
bool String::operator == (const char* other) const
{
   if (other == 0)
      return size() == 0;

   int len= size();
   for (int i=0;i<len;i++)
   {
      if (mData[i] != other[i])
         return false;
   }
   return true;
}

// not equal?
bool String::operator != (const String& other) const
{
   return !(*this == other);
}

// smaller?
bool String::operator < (const String& other) const
{
   const char *data= other.data();

   // special case: mindestens einer von beiden ist 0
   if (!mData || !data)
   {
      if (!mData && !data) // both empty string
         return false;
      else
         return (!mData);
   }

   // minimum length of both strings
   int len;
   if (size() < other.size())
      len= size();
   else
      len= other.size();

   for (int i=0;i<=len;i++)
   {
      if (mData[i] < data[i])
         return true;
      else if (mData[i] > data[i])
         return false;
   }

   return false;
}

//! bigger?
bool String::operator > (const String& other) const
{
   return *this < other;
}

//! return pointer to string-data
const char* String::data() const
{
   return mData;
}

// return length of string
int String::size() const
{
   int *size= (int*)dataIntern();
   if (size)
      return *size;
   else
      return 0;
}

// load string from stream
void String::load(Stream* stream)
{
   if (!copyRef())
      dealloc();

   int size= stream->getByte();
   alloc( size );

   stream->getData(mData, size);
   mData[size]= 0;

   // was \0 already present?
   while (size>0 && mData[size-1]==0)
      size--;
   setSize(size);
}

// write string to stream
void String::write(Stream* stream)
{
   int len= size();
   if (len > 255) len= 255;

   stream->writeByte( len );
   stream->writeData(mData, len);
}

