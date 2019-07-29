#include "core/image.h"
#include "ext/std_image/include/stb_image.h"

bool Image::LoaedFromFile(std::string path)
{
      int x = 0, y = 0, img_channel = 0;
      bool alpha = flags == R8G8B8A8;
      data = stbi_load(path.c_str(), &x, &y, &img_channel, alpha ? 4 : 3);
      if(data) {
            _resolution.x = x;
            _resolution.y = y;
            if(flags == UNSPECIFIED)
                  flags = R8G8B8;
            element_size = (flags==R8G8B8) ? 3 : 4;
            loaded = true;
            return true;
      }
      else {
            loaded = false;
            return false;
      }
}

Image::~Image()
{
      if(data) {
            stbi_image_free(data);
      }
}

R8G8B8 Image::Pixel(int i, int j)
{
      if(!data) return 0;
      return static_cast<::R8G8B8*>(data)[i * _resolution.x + _resolution.y];
}