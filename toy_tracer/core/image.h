#pragma once
#include <string>
#include <memory>
#include <core/geometry.h>
#include <tracer/spectrum.h>
#include <glad/glad.h>

class Image {
public:
      enum Format {
            UNSPECIFIED,
            R8G8B8,
            R8G8B8A8,
            RGBSpectrum,
            RGBASpectrum, // TODO: implement in LoaedfromFile
      };
#define NumChannel(format) (format==R8G8B8||format==RGBSpectrum)?3:4
#define ByteElement(format) (format==R8G8B8||format==R8G8B8A8)

private:
      Point2i _resolution;
      bool loaded = false;
      bool convertfromsRGB = true;
      bool isHDR = false;
      Format flags = UNSPECIFIED;
      void* _data = nullptr;
      size_t numChannel;
      GLuint _elementType;
      std::string _path;
      bool _flipY;

public:
      Image() = default;
      Image(const Image& i) = delete;
      Image& operator=(const Image& i) = delete;
      Image(Image&& i) :_resolution(i._resolution), loaded(i.loaded), flags(i.flags), _data(i._data), 
            numChannel(i.numChannel), _elementType(i._elementType) {
            i._data = nullptr;
      }
      Image(std::string path, Format flags = RGBASpectrum, bool flip_y = true, bool load = true) : flags(flags),
            _flipY(flip_y), _path(path)
      {
            if(load)
                  LoadFromFile(path, flip_y); 
      }
      Image(const ::R8G8B8& color, bool alpha = false, Float val_alpha = 0.f, bool fromsRGB = true);
      static Image* CreateImageFromFile(std::string path);
      static Image* CreateColorImage(std::string path, ::R8G8B8 color, bool alpha = false, Float val_alpha = 0.f);
      ::R8G8B8 R8G8B8Pixel(int i, int j);
      ::RGBSpectrum SpectrumPixel(int i, int j);
      bool LoadFromFile(std::string path, bool flip_y = true);
      bool LoadHDR(std::string path, bool flip_y = true);
      void setFormat(Format f) { flags = f; }
      void setRequestNumChannel(unsigned int c) { numChannel = c; }
      void Load() { LoadFromFile(_path, _flipY); }
      Format format() const { return flags; }
      GLenum glPixelFormat() const {
            if (format() == Image::Format::R8G8B8 || format() == Image::Format::RGBSpectrum)
                  return GL_RGB;
            else if (format() == Image::Format::R8G8B8A8 || format() == Image::Format::RGBASpectrum)
                  return GL_RGBA;
      }
      const Point2i resolution() const {return _resolution;}
      const void* data() const { return _data; }
      GLuint elementFormat() const { return _elementType; }
      void RotateCW();
      void RotateCCW();
      void setConvertFromsRGB(bool b) { convertfromsRGB = b;}
      ~Image();
};