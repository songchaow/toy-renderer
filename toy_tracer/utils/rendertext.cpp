#include "core/texture.h"
#include "ft2build.h"
#include "core/geometry.h"
#include "core/rendertext.h"
#include "core/shader.h"
#include "main/renderworker.h"
#include FT_FREETYPE_H 
FT_Library ft;
FT_Face ft_face;
bool ft_init = false;
GLuint vao, vbo;
Character ascii_chars[128];
Shader* text_shader;
constexpr uint32_t pixelHeight = 17;
constexpr uint32_t lineMargin = 2;

void init() {
      if (FT_Init_FreeType(&ft))
            LOG(ERROR) << "Failed to init freetype.";
      else {
            if (FT_New_Face(ft, "fonts/times.ttf", 0, &ft_face)==0) {
                  ft_init = true;

                  int err2 = FT_Set_Pixel_Sizes(ft_face, 0, pixelHeight);
            }
      }
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      for (GLubyte c = 0; c < 128; c++)
      {
            // Load character glyph 
            if (FT_Load_Char(ft_face, c, FT_LOAD_RENDER))
            {
                  LOG(ERROR) << "Failed to load Glyph" << std::endl;
                  continue;
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                  GL_TEXTURE_2D,
                  0,
                  GL_RED,
                  ft_face->glyph->bitmap.width,
                  ft_face->glyph->bitmap.rows,
                  0,
                  GL_RED,
                  GL_UNSIGNED_BYTE,
                  ft_face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                texture,
                Vector2i(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows),
                Vector2i(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top),
                ft_face->glyph->advance.x
            };
            ascii_chars[c] = character;
      }
      FT_Done_Face(ft_face);
      FT_Done_FreeType(ft);
      //glEnable(GL_BLEND);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
      // shader
      text_shader = LoadShader(TEXT, true);
      text_shader->use();
      text_shader->setUniformI("screenSize", RenderWorker::Instance()->resolution().x, RenderWorker::Instance()->resolution().y);
      text_shader->setUniformI("text", 0);
      // vao
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      /*Float vertexBuffer[] = {
                  0,     0 + 60,   0.0, 0.0 ,
                  0,     0,       0.0, 1.0 ,
                  0 + 60, 0,       1.0, 1.0 ,
                  0,     0 + 60,   0.0, 0.0 ,
                  0 + 60, 0,       1.0, 1.0 ,
                  0 + 60, 0 + 60,   1.0, 0.0
      };*/
      glBufferData(GL_ARRAY_BUFFER, 4 * 6 * 4, NULL, GL_DYNAMIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * 4, 0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * 4, (void*)(2 * 4));
      //glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
}

// startPos: down left of the text frame
void renderText(const std::string& txt, Point2f startPos, Float scale) {
      if (!ft_init) {
            init();
      }
      LoadShader(TEXT, true)->use();
      Float currPosx = startPos.x;
      Float currPosy = startPos.y;
      glDisable(GL_DEPTH_TEST);
      for (const char& c : txt) {
            if (c == '\n') {
                  currPosx = 0;
                  currPosy -= (pixelHeight + lineMargin);
                  continue;
            }
            Character& ch = ascii_chars[c];
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            Float&& x = currPosx + ch.Bearing.x * scale;
            Float&& y = startPos.y - (ch.Size.y - ch.Bearing.y) * scale;
            Float&& w = ch.Size.x * scale;
            Float&& h = ch.Size.y * scale;
            Float vertexBuffer[] = {
                  x,     y + h,   0.0, 0.0 ,
                  x,     y,       0.0, 1.0 ,
                  x + w, y,       1.0, 1.0 ,
                  x,     y + h,   0.0, 0.0 ,
                  x + w, y,       1.0, 1.0 ,
                  x + w, y + h,   1.0, 0.0 
            };
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexBuffer), vertexBuffer);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            currPosx += (ch.Advance >> 6) * scale;
      }
      //glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      glEnable(GL_DEPTH_TEST);
}

void renderTextAtTopLeft(const std::string& txt, Float scale) {
      Point2f startPos = { 0, (Float)RenderWorker::Instance()->resolution().y - (Float)pixelHeight };
      renderText(txt, startPos, scale);
}