#pragma once

#include "bitmapfont.h"
#include "tools/pool.h"
#include "tools/singleton.h"

class FontPool : public Pool<BitmapFont>, public Singleton<FontPool>
{
};
