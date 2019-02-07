// string class
// holds a single \0-terminated character string

#pragma once

#include "referenced.h"
#include "streamable.h"
#include <string.h>

class String : public Referenced, public Streamable
{
public:
   //! construct empty string
   String();

   //! construct string from given "text"
   String(const char* text);

   //! construct reference of given string "other"
   String(const String& other);

   //! delete the string
   virtual ~String();

   //! assignment operator: reference given array
   String& operator = (const String& other);

   //! cast to char*
   operator const char* () const;

   //! get char at index
   char operator[](int index) const;

   //! concat two strings, return new string
   String operator + (const String& other) const;

   //! append string
   void operator += (const String& other);

   //! comparism
   bool operator == (const String& other) const;
   bool operator == (const char* other) const;
   bool operator != (const String& other) const;
   bool operator < (const String& other) const;
   bool operator > (const String& other) const;

   String& operator << (Stream& stream);

   //! is empty string?
   bool isEmpty() const;

   //! clear the string
   void clear();

   //! apend string
   void append(const String& other);

   //! get index of substring "other"
   int indexOf(const String& other) const;

   //! get middle part of string
   String mid(int start, int end) const;

   //! get char at index
   char get(int index) const;

   //! get char pointer
   const char* data() const;

   //! return size of string
   int size() const;

   //! load string from stream
   void load(Stream* stream);

   //! write string to stream
   void write(Stream* stream);


private:
   void  alloc(int size);
   void  dealloc();
   char* dataIntern() const;
   void  setSize(int size);

   char* mData;  //!< string data (null pointer for empty strings)
};

