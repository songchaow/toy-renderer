#pragma once
#include "core/geometry.h"
#include "glad/glad.h"
struct Character {
      GLuint     TextureID;  // ID handle of the glyph texture
      Vector2i Size;       // Size of glyph
      Vector2i Bearing;    // Offset from baseline to left/top of glyph
      GLuint     Advance;    // Offset to advance to next glyph
};

void renderTextAtTopLeft(const std::string& txt, Float scale);
void renderTextAtTopRight(const std::string& txt, Float scale);
void renderText(const std::string& txt, Point2f startPos, Float scale);