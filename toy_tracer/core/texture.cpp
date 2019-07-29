#include "core/texture.h"
#include "core/image.h"

ImageTexture::ImageTexture(std::string img_path)
{
      _image = new Image(img_path);
}

R8G8B8 ImageTexture::Evaluate(Float u, Float v)
{
      if(_wrapMode==LOOP) {
            u = u - static_cast<int>(u);
            v = v - static_cast<int>(v);
      }
      Float coordX = u * _image->resolution().x;
      Float coordY = v * _image->resolution().y;
      // use the box filter
      return _image->Pixel(coordX, coordY);
}