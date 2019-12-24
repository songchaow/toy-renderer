#pragma once
#include <string>
#include <memory>
#include <core/geometry.h>
#include <core/spectrum.h>
#include <QOpenGLFunctions>

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
      Format flags = UNSPECIFIED;
      void* _data = nullptr;
      size_t numChannel;
      GLuint _elementType;

public:
      Image() = default;
      Image(const Image& i) = delete;
      Image(Image&& i) :_resolution(i._resolution), loaded(i.loaded), flags(i.flags), _data(i._data), 
            numChannel(i.numChannel), _elementType(i._elementType) {
            i._data = nullptr;
      }
      Image(std::string path, Format flags = RGBSpectrum) : flags(flags) { LoadFromFile(path); };
      Image(const ::R8G8B8& color, bool alpha = false, Float val_alpha = 0.f);
      Image& operator=(const Image& i) = delete;
      static Image* CreateImageFromFile(std::string path);
      static Image* CreateColorImage(std::string path, ::R8G8B8 color, bool alpha = false, Float val_alpha = 0.f);
      ::R8G8B8 R8G8B8Pixel(int i, int j);
      ::RGBSpectrum SpectrumPixel(int i, int j);
      bool LoadFromFile(std::string path);
      void setFormat(Format f) { flags = f; }
      Format format() const { return flags; }
      const Point2i resolution() const {return _resolution;}
      const void* data() const { return _data; }
      GLuint elementType() const { return _elementType; }
      ~Image();
};