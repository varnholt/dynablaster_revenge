#pragma once

#include "tools/array.h"
#include "tools/stream.h"

class IDList : public Array<int>
{
public:
	IDList();
	IDList(const IDList&);
	virtual ~IDList();

	IDList& operator = (const IDList& other);

	virtual int load(Stream *stream);

private:
};
