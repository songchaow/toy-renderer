#pragma once
#include <string>
#include <core/geometry.h>
#include <core/spectrum.h>

class Image {
      Point2i _resolution;
public:
      enum Format {
            UNSPECIFIED,
            R8G8B8,
            R8G8B8A8
      };
private:
      bool loaded = false;
      Format flags = UNSPECIFIED;
      void* data = nullptr;
      size_t element_size;

public:
      Image() = default;
      Image(std::string path);
      ::R8G8B8 Pixel(int i, int j);
      bool LoaedFromFile(std::string path);
      void setFormat(Format f) { flags = f; }
      const Point2i resolution() const {return _resolution;}
      ~Image();
};