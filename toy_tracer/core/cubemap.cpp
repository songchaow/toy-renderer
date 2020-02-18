#include "core/cubemap.h"
#include "core/shader.h"
#include "main/renderworker.h"
#include <fstream>
#include <cassert>



void CubeMap::loadImage(const std::vector<std::string>& paths)
{
      assert(paths.size() >= 6);
      for (int i = 0; i < 6; i++)
            if(_image[i])
                  delete _image[i];
      for (int i = 0; i < 6; i++)
            _image[i] = new Image(paths[i], Image::Format::RGBASpectrum, false);
      resolution = _image[0]->resolution();
}

void CubeMap::loadImage(const std::vector<Image*> images) {
      for (int i = 0; i < 6; i++)
            if (_image[i])
                  delete _image[i];
      for (int i = 0; i < 6; i++)
            _image[i] = images[i];
      resolution = _image[0]->resolution();
}

void CubeMap::glLoad()
{
      if (cubeMapObj > 0)
            return;
      glGenTextures(1, &cubeMapObj);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
      for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, resolution.x, resolution.y, 0, _image[i]->glPixelFormat(), _image[i]->elementFormat(), _image[i]->data());
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      for (int i = 0; i < 6; i++) {
            delete _image[i];
            _image[i] = nullptr;
      }
}
