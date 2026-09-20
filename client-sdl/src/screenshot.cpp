#include "screenshot.h"

#include "gles3.h"

#include <SDL3/SDL.h>
#include <cstring>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool saveScreenshot(const std::string& path, int width, int height)
{
   std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 4);

   glPixelStorei(GL_PACK_ALIGNMENT, 1);
   glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

   // glReadPixels' origin is bottom-left, PNG expects top-left rows first.
   std::vector<uint8_t> flipped(pixels.size());
   const size_t row_bytes = static_cast<size_t>(width) * 4;
   for (int row = 0; row < height; ++row)
   {
      const uint8_t* src = pixels.data() + row_bytes * row;
      uint8_t* dst = flipped.data() + row_bytes * (height - 1 - row);
      std::memcpy(dst, src, row_bytes);
   }

   const int ok = stbi_write_png(path.c_str(), width, height, 4, flipped.data(), static_cast<int>(row_bytes));
   if (ok == 0)
   {
      SDL_Log("failed to write screenshot: %s", path.c_str());
      return false;
   }

   SDL_Log("wrote screenshot: %s", path.c_str());
   return true;
}
