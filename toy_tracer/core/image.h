#pragma once
#include <string>
#include <memory>
#include <core/geometry.h>
#include <core/spectrum.h>

class Image {
      Point2i _resolution;
public:
      enum Format {
            UNSPECIFIED,
            R8G8B8,
            R8G8B8A8,
            RGBSpectrum
      };
#define ElementSize(format) (format==R8G8B8||format==RGBSpectrum)?3:4
#define isCompatible(format) (format==R8G8B8||format==R8G8B8A8)

private:
      bool loaded = false;
      Format flags = UNSPECIFIED;
      void* data = nullptr;
      size_t element_size;

public:
      Image() = default;

      Image(std::string path, Format flags = RGBSpectrum) : flags(flags) {};
      ::R8G8B8 R8G8B8Pixel(int i, int j);
      ::RGBSpectrum SpectrumPixel(int i, int j);
      bool LoadFromFile(std::string path);
      void setFormat(Format f) { flags = f; }
      const Point2i resolution() const {return _resolution;}
      ~Image();
};