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

void ImageTexture::load(QOpenGLExtraFunctions* f) {
      f->glGenTextures(1, &_tbo);
      f->glBindTexture(GL_TEXTURE_2D, _tbo);
      // default tex params
      f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // level: 0
      GLuint image_format;
      if (_image->format() == Image::Format::R8G8B8)
            image_format = GL_RGB;
      else if (_image->format() == Image::Format::R8G8B8A8)
            image_format = GL_RGBA;
      f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _image->resolution().y, _image->resolution().x, 0, image_format, GL_UNSIGNED_BYTE, _image->data());

}