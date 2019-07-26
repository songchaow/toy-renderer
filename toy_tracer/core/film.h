#pragma once
#include <memory>
#include "core/spectrum.h"
#include "core/filter.h"
#include "core/geometry.h"
#include "core/sampler.h"
#include <cassert>
#include <string>
class Film {
      int width;
      int height;
      std::shared_ptr<RGBSpectrum[]> matrix;
      std::shared_ptr<Float[]> weight_sum;
      Filter* filter = nullptr;
      Sampler* sampler;
      Float& WeightSum(const Point2i& index) { assert(index.x < width && index.y < height); return weight_sum[index.y*width + index.x]; }
      RGBSpectrum& ContribSum(const Point2i& index) { assert(index.x < width && index.y < height); return matrix[index.y*width + index.x]; }
public:
      Film(Point2f& size) : width(size.x), height(size.y), matrix(new RGBSpectrum[width*height]()),
            weight_sum(new Float[width*height]()) {}
      const int getWidth() const { return width; }
      const int getHeight() const { return height; }
      void addRay(Spectrum& Li, Point2f& pFilm);
      Point2f SampleFilm(int px, int py);
      void Normalize();
      void writePNG(const std::string& path)
};