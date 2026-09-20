#pragma once

#include <SDL3/SDL.h>

#include <cstdint>
#include <string>

/// \brief owns the SDL window and GLES3 context. replaces the legacy client's
/// framework/gldevice.cpp + QGLWidget pairing with a plain SDL3 window and an explicitly
/// requested ES profile context.
class GlesContext
{
public:
   GlesContext() = default;
   ~GlesContext();

   GlesContext(const GlesContext&) = delete;
   GlesContext& operator=(const GlesContext&) = delete;

   /// \brief initializes SDL video, creates the window and a GLES 3.0 context.
   /// \param title window title.
   /// \param width window width in pixels.
   /// \param height window height in pixels.
   /// \return true on success.
   bool init(const std::string& title, int width, int height);

   /// \brief presents the back buffer.
   void swap() const;

   int width() const
   {
      return _width;
   }

   int height() const
   {
      return _height;
   }

   SDL_Window* window() const
   {
      return _window;
   }

private:
   SDL_Window* _window = nullptr;
   SDL_GLContext _context = nullptr;
   int _width = 0;
   int _height = 0;
};
