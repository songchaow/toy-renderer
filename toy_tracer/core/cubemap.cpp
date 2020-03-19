#include "core/cubemap.h"
#include "core/shader.h"
#include "main/renderworker.h"
#include <fstream>
#include <cassert>

void CubeMap::deleteImages() {
      for (int i = 0; i < _mipImages.size(); i++) {
            for (int j = 0; j < _mipImages[i].size(); j++) {
                  if (_mipImages[i][j])
                        delete _mipImages[i][j];
                  _mipImages[i][j] = nullptr;
            }
      }
      _mipImages.clear();
}

void CubeMap::loadImage(const std::vector<std::string>& paths)
{
      assert(paths.size() >= 6);
      deleteImages();
      _mipImages.resize(1);
      _mipImages[0].resize(6);
      for (int i = 0; i < 6; i++)
            _mipImages[0][i] = new Image(paths[i], Image::Format::RGBASpectrum, false);
      resolution = _mipImages[0][0]->resolution();
}

void CubeMap::loadImage(const std::vector<Image*> images) {
      deleteImages();
      _mipImages.resize(1);
      _mipImages[0].resize(6);
      for (int i = 0; i < 6; i++)
            _mipImages[0][i] = images[i];
      resolution = _mipImages[0][0]->resolution();
}

void CubeMap::loadMipmapImage(const std::vector<std::vector<Image*>>& images)
{
      deleteImages();
      _mipImages = images;
}

void CubeMap::glLoad()
{
      if (cubeMapObj > 0)
            return;
      glGenTextures(1, &cubeMapObj);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
      for (int mipLevel = 0; mipLevel < _mipImages.size(); mipLevel++) {
            //int currResolutionX = resolution.x >> mipLevel;
            //int currResolutionY = resolution.y >> mipLevel;
            for (int i = 0; i < 6; i++) {
                  Image* m = _mipImages[mipLevel][i];
                  if (!m)
                        continue;
                  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevel, GL_RGBA32F, m->resolution().x, m->resolution().y, 0, m->glPixelFormat(), m->elementFormat(), m->data());
            }
      }
      
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, _mipImages.size() - 1);
      deleteImages();
}
