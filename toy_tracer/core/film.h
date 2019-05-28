#pragma once
#include <memory>
#include "core/spectrum.h"
class Film {
      int width;
      int height;
      std::shared_ptr<RGBSpectrum[]> matrix;
public:
      Film(int width, int height) : width(width), height(height), matrix(new RGBSpectrum[width*height]) {}
      const int getWidth() const { return width; }
      const int getHeight() const { return height; }


};