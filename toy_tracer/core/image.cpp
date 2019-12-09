#include "core/image.h"
#include "ext/std_image/include/stb_image.h"
#include <map>

static std::map<std::string, Image> _imageStore;

Image* Image::CreateImageFromFile(std::string path) {
      if (_imageStore.find(path) != _imageStore.end())
            return &_imageStore[path];
      _imageStore.emplace(path, path);
      return &_imageStore[path];
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
                              static_cast<char*>(_data)[numChannel * i + j] = GammaInvTransform(static_cast<char*>(_data)[numChannel * i + j]);
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
                  delete tmp;
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