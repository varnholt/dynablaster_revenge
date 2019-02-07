#include "idlist.h"

IDList::IDList()
: Array<int>()
{
}

IDList::IDList(const IDList& list)
: Array<int>(list)
{
}

IDList::~IDList()
{
}

IDList& IDList::operator = (const IDList& list)
{
	init( list.size() );
	for (int i=0;i<list.size();i++)
		add(list[i]);
	return *this;
}

int IDList::load(Stream *stream)
{
	// get number of items
	int size= stream->getInt();

	init(size);

	// load items
	for (int i=0;i<size;i++)
		add( stream->getInt() );

	return size;
}
