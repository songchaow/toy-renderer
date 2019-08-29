#include "core/texture.h"
#include "core/image.h"

ConstColorTexture<RGBSpectrum> blackConstantTexture = ConstColorTexture<RGBSpectrum>(RGBSpectrum(0.f, 0.f, 0.f));
ConstColorTexture<RGBSpectrum> whiteConstantTexture = ConstColorTexture<RGBSpectrum>(RGBSpectrum(1.f, 1.f, 1.f));

RGBSpectrum ImageTexture::Evaluate(Float u, Float v)
{
      if(_wrapMode==LOOP) {
            u = u - static_cast<int>(u);
            v = v - static_cast<int>(v);
      }
      else if (_wrapMode == BLACK) {
            if (u<0.f || u>1.f || v<0.f || v>1.f)
                  return 0;
      }
      unsigned int coordX = u * (_image->resolution().x - 0.001);
      unsigned int coordY = v * (_image->resolution().y - 0.001);
      // use the box filter
      return _image->SpectrumPixel(coordX, coordY);
}