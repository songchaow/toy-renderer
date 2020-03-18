#include "core/image.h"
#include "stb_image.h"
#include <map>
#include <utility>
#include <memory>

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
Image::Image(const ::R8G8B8& color, bool alpha, Float val_alpha, bool fromsRGB) {
      // assume we store float internally
      numChannel = alpha ? 4 : 3;
      if (alpha)
            flags = RGBASpectrum;
      else
            flags = RGBSpectrum;
      _resolution.x = _resolution.y = 1;
      _data = new Float[numChannel];
      _elementType = GL_FLOAT;
      if (fromsRGB)
            for (int i = 0; i < 3; i++)
                  static_cast<Float*>(_data)[i] = sRGB2RadianceFloat(color[i]);
      else
            for (int i = 0; i < 3; i++)
                  static_cast<Float*>(_data)[i] = Float(color[i]) / 256.f;
      if (alpha)
            static_cast<Float*>(_data)[3] = GammaInvTransform(val_alpha);
      loaded = true;
}


bool Image::LoadFromFile(std::string path, bool flip_y/*=true*/)
{
      int width = 0, height = 0, srcNumChannel = 0;
      numChannel = NumChannel(flags);
      stbi_set_flip_vertically_on_load(flip_y);
      void* tmp = stbi_load(path.c_str(), &width, &height, &srcNumChannel, numChannel);
      if(tmp) {
            _resolution.x = width;
            _resolution.y = height;
            if(ByteElement(flags)) {
                  _data = tmp;
                  _elementType = GL_UNSIGNED_BYTE;
                  // apply GammaInvTransform
                  for(int i = 0; i < width*height ; i++) {
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
                        _data = new ::RGBSpectrum[width*height];
                        constexpr uint16_t sizeRGBSpectrum = sizeof(::RGBSpectrum);
                        // apply sRGB2RadianceFloat
                        if(convertfromsRGB)
                              for(int i = 0; i < width*height ; i++) {
                                          static_cast<::RGBSpectrum*>(_data)[i] = ::RGBSpectrum(sRGB2RadianceFloat(static_cast<unsigned char*>(tmp)[numChannel * i + 0]),
                                                                                             sRGB2RadianceFloat(static_cast<unsigned char*>(tmp)[numChannel * i + 1]),
                                                                                             sRGB2RadianceFloat(static_cast<unsigned char*>(tmp)[numChannel * i + 2]));
                              }
                        else {
                              Float* p = (Float*)_data;
                              for (int i = 0; i < width*height; i++) {
                                    for (int k = 0; k < 3; k++)
                                          p[i * 3 + k] = Float(static_cast<unsigned char*>(tmp)[numChannel * i + k]) / 256;
                              }
                        }
                  }
                  else if(flags==RGBASpectrum) {
                        _data = new ::RGBASpectrum[width*height];
                        constexpr uint16_t sizeRGBASpectrum = sizeof(::RGBSpectrum);
                        if(convertfromsRGB)
                              for (int i = 0; i < width*height; i++) {
                                    static_cast<::RGBASpectrum*>(_data)[i] = ::RGBASpectrum(sRGB2RadianceFloat(static_cast<unsigned char*>(tmp)[numChannel * i + 0]),
                                          sRGB2RadianceFloat(static_cast<unsigned char*>(tmp)[numChannel * i + 1]),
                                          sRGB2RadianceFloat(static_cast<unsigned char*>(tmp)[numChannel * i + 2]),
                                          sRGB2RadianceFloat(static_cast<unsigned char*>(tmp)[numChannel * i + 3]));
                              }
                        else {
                              Float* p = (Float*)_data;
                              for (int i = 0; i < width*height; i++) {
                                    for (int k = 0; k < 4; k++)
                                          p[i * 4 + k] = Float(static_cast<unsigned char*>(tmp)[numChannel * i + k]) / 256;
                              }
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

bool Image::LoadHDR(std::string path, bool flip_y)
{
      if (!stbi_is_hdr(path.c_str()))
            return false;
      stbi_set_flip_vertically_on_load(flip_y);
      int originalChannel;
      numChannel = 3;
      Float* data = stbi_loadf(path.c_str(), &_resolution.x, &_resolution.y, &originalChannel, numChannel);
      if (!data)
            return false;
      _data = data;
      _elementType = GL_FLOAT;
      isHDR = true;
      flags = Image::Format::RGBSpectrum;
      return false;
}

void Image::RotateCW()
{
      if (flags == RGBSpectrum) {
            //std::allocator<::RGBSpectrum> a;
            ::RGBSpectrum* old_data = (::RGBSpectrum*)_data;
            ::RGBSpectrum* new_data = new ::RGBSpectrum[_resolution.x*_resolution.y];
            for (int y = 0; y < _resolution.x; y++) {
                  for (int x = 0; x < _resolution.y; x++) {
                        int oldx = _resolution.x - y - 1;
                        int oldy = x;
                        new_data[y*_resolution.y + x] = old_data[oldy*_resolution.x + oldx];
                  }
            }
            delete[] old_data;
            _data = new_data;
            std::swap(_resolution.x, _resolution.y);
      }
      else if (flags == RGBASpectrum) {
            ::RGBASpectrum* old_data = (::RGBASpectrum*)_data;
            ::RGBASpectrum* new_data = new ::RGBASpectrum[_resolution.x*_resolution.y];
            for (int y = 0; y < _resolution.x; y++) {
                  for (int x = 0; x < _resolution.y; x++) {
                        int oldx = _resolution.x - y - 1;
                        int oldy = x;
                        new_data[y*_resolution.y + x] = old_data[oldy*_resolution.x + oldx];
                  }
            }
            delete[] old_data;
            _data = new_data;
            std::swap(_resolution.x, _resolution.y);
      }

}

void Image::RotateCCW()
{
      if (flags == RGBSpectrum) {
            //std::allocator<::RGBSpectrum> a;
            ::RGBSpectrum* old_data = (::RGBSpectrum*)_data;
            ::RGBSpectrum* new_data = new ::RGBSpectrum[_resolution.x*_resolution.y];
            for (int y = 0; y < _resolution.x; y++) {
                  for (int x = 0; x < _resolution.y; x++) {
                        int oldx = y;
                        int oldy = _resolution.y - x - 1;
                        new_data[y*_resolution.y + x] = old_data[oldy*_resolution.x + oldx];
                  }
            }
            delete[] old_data;
            _data = new_data;
            std::swap(_resolution.x, _resolution.y);
      }
      else if (flags == RGBASpectrum) {
            ::RGBASpectrum* old_data = (::RGBASpectrum*)_data;
            ::RGBASpectrum* new_data = new ::RGBASpectrum[_resolution.x*_resolution.y];
            for (int y = 0; y < _resolution.x; y++) {
                  for (int x = 0; x < _resolution.y; x++) {
                        int oldx = y;
                        int oldy = _resolution.y - x - 1;
                        new_data[y*_resolution.y + x] = old_data[oldy*_resolution.x + oldx];
                  }
            }
            delete[] old_data;
            _data = new_data;
            std::swap(_resolution.x, _resolution.y);
      }
}

Image::~Image()
{
      if(_data) {
            if (_elementType == GL_FLOAT && !isHDR) {
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