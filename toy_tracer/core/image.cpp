#include "core/image.h"
#include "stb_image.h"
#include <map>
#include <utility>

static std::map<std::string, Image> _imageStore;

Image* Image::CreateImageFromFile(std::string path) {
      if (_imageStore.find(path) != _imageStore.end())
            return &_imageStore[path];
      _imageStore.emplace(path, path);
      return &_imageStore[path];
}

// color in sRGB
Image * Image::CreateColorImage(std::string path, ::R8G8B8 color, bool alpha, Float val_alpha)
{
      _imageStore.emplace(path, Image(color, alpha, val_alpha));
      return &_imageStore[path];
}

// color and val_alpha in sRGB
Image::Image(const ::R8G8B8& color, bool alpha, Float val_alpha) {
      // assume we store float internally
      numChannel = alpha ? 4 : 3;
      if (alpha)
            flags = RGBASpectrum;
      else
            flags = RGBSpectrum;
      _resolution.x = _resolution.y = 1;
      _data = new Float[numChannel];
      _elementType = GL_FLOAT;
      for (int i = 0; i < 3; i++)
            static_cast<Float*>(_data)[i] = InverseGammaCorrection(color[i]);
      if (alpha)
            static_cast<Float*>(_data)[3] = GammaInvTransform(val_alpha);
      loaded = true;
}


bool Image::LoadFromFile(std::string path)
{
      int x = 0, y = 0, img_channel = 0;
      if(flags == UNSPECIFIED)
            flags = R8G8B8;
      numChannel = NumChannel(flags);
      void* tmp = stbi_load(path.c_str(), &x, &y, &img_channel, numChannel);
      if(tmp) {
            _resolution.x = x;
            _resolution.y = y;
            if(ByteElement(flags)) {
                  _data = tmp;
                  _elementType = GL_UNSIGNED_BYTE;
                  // apply GammaInvTransform
                  for(int i = 0; i < x*y ; i++) {
                        for(int j = 0; j < numChannel; j++) {
                              // here's WRONG param and return value should be char
                              static_cast<char*>(_data)[numChannel * i + j] = GammaInvTransform(static_cast<char*>(_data)[numChannel * i + j] / 256.0f)*256.0f;
                        }
                  }
            }
            else {
                  // we need to convert char to Float
                  _elementType = GL_FLOAT;
                  if(flags==RGBSpectrum) {
                        _data = new ::RGBSpectrum[x*y];
                        uint16_t sizeRGBSpectrum = sizeof(::RGBSpectrum);
                        // apply InverseGammaCorrection
                        for(int i = 0; i < x*y ; i++) {
                                    static_cast<::RGBSpectrum*>(_data)[i] = ::RGBSpectrum(InverseGammaCorrection(static_cast<unsigned char*>(tmp)[numChannel * i + 0]),
                                                                                       InverseGammaCorrection(static_cast<unsigned char*>(tmp)[numChannel * i + 1]),
                                                                                       InverseGammaCorrection(static_cast<unsigned char*>(tmp)[numChannel * i + 2]));
                        }
                  }
                  stbi_image_free(tmp);
            }
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
      if(_data) {
            if (_elementType == GL_FLOAT) {
                  ::RGBSpectrum* d = (::RGBSpectrum*)_data;
                  delete[] d;
            }
            else
                  stbi_image_free(_data);
      }
}

R8G8B8 Image::R8G8B8Pixel(int i, int j)
{
      if(!_data) return ::R8G8B8(0.f);
      return static_cast<::R8G8B8*>(_data)[i * _resolution.x + _resolution.y];
}

RGBSpectrum Image::SpectrumPixel(int col, int row)
{
      if(!_data) return ::RGBSpectrum(0.f);
      return static_cast<::RGBSpectrum*>(_data)[row * _resolution.x + col];
}