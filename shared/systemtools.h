#pragma once

class QString;

//! check if the version of the installed qt libs matches the compiled/expected version
//! returns true if major version is equal and minor version is greater/equal
bool checkQtVersion( int major = 4, int minor = 8, QString* returnVersion = 0 );

