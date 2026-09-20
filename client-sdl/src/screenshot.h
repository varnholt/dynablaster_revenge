#pragma once

#include <string>

/// \brief dumps the current GL framebuffer to a PNG on disk.
/// this is the "golden image" side of the verification loop: a headless run drives the game via
/// InputInjector, then calls this once to produce a screenshot that gets diffed against a
/// reference image (or eyeballed the first time).
/// \param path output PNG path.
/// \param width framebuffer width in pixels.
/// \param height framebuffer height in pixels.
/// \return true when the PNG was written successfully.
bool saveScreenshot(const std::string& path, int width, int height);
