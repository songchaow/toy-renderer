#include "core/texture.h"
#include "core/image.h"

RGBSpectrum ImageTexture::Evaluate(Float u, Float v)
{
      if(_wrapMode==LOOP) {
            u = u - static_cast<int>(u);
            v = v - static_cast<int>(v);
      }
      Float coordX = u * _image->resolution().x;
      Float coordY = v * _image->resolution().y;
      // use the box filter
      return _image->SpectrumPixel(coordX, coordY);
}