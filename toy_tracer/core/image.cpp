#include "core/image.h"
#include "ext/std_image/include/stb_image.h"

bool Image::LoadFromFile(std::string path)
{
      int x = 0, y = 0, img_channel = 0;
      if(flags == UNSPECIFIED)
            flags = R8G8B8;
      element_size = ElementSize(flags);
      void* tmp = stbi_load(path.c_str(), &x, &y, &img_channel, element_size);
      if(tmp) {
            _resolution.x = x;
            _resolution.y = y;
            if(isCompatible(flags)) {
                  _data = tmp;
                  // apply GammaInvTransform
                  for(int i = 0; i < x*y ; i++) {
                        for(int j = 0; j < element_size; j++) {
                              static_cast<char*>(_data)[element_size * i + j] = GammaInvTransform(static_cast<char*>(_data)[element_size * i + j]);
                        }
                  }
            }
            else {
                  // we need to convert char to Float
                  if(flags==RGBSpectrum) {
                        _data = new ::RGBSpectrum[x*y];
                        uint16_t sizeRGBSpectrum = sizeof(::RGBSpectrum);
                        // apply InverseGammaCorrection
                        for(int i = 0; i < x*y ; i++) {
                                    static_cast<::RGBSpectrum*>(_data)[i] = ::RGBSpectrum(InverseGammaCorrection(static_cast<unsigned char*>(tmp)[element_size * i + 0]),
                                                                                       InverseGammaCorrection(static_cast<unsigned char*>(tmp)[element_size * i + 1]),
                                                                                       InverseGammaCorrection(static_cast<unsigned char*>(tmp)[element_size * i + 2]));
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

RGBSpectrum Image::SpectrumPixel(int i, int j)
{
      if(!_data) return ::RGBSpectrum(0.f);
      return static_cast<::RGBSpectrum*>(_data)[j * _resolution.x + i];
}