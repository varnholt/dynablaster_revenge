#pragma once

#include "bitmapfont.h"
#include "tools/singleton.h"
#include "tools/pool.h"

class FontPool : public Pool<BitmapFont>, public Singleton<FontPool>
{
};

