#pragma once
#include <memory>
#include "tracer/spectrum.h"
#include "tracer/filter.h"
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
      Sampler sampler;
      // x: from top to down, 0-height-1 y: from left to right, 0-width-1
      Float& WeightSum(const Point2i& index) { assert(index.y < width && index.x < height); return weight_sum[index.x*width + index.y]; }
      RGBSpectrum& ContribSum(const Point2i& index) { assert(index.y < width && index.x < height); return matrix[index.x*width + index.y]; }
public:
      Film(const Point2i& size) : width(size.x), height(size.y), matrix(new RGBSpectrum[width*height]()),
            weight_sum(new Float[width*height]()) {}
      const RGBSpectrum& ContribSum(const Point2i& index) const { assert(index.y < width && index.x < height); return matrix[index.x*width + index.y]; }
      const int getWidth() const { return width; }
      const int getHeight() const { return height; }
      void addRay(Spectrum& Li, Point2f& pFilm);
      void SetSamplePixelRegion(int px, int py);
      Point2f Sample();
      void Normalize();
      void writePNG(const std::string& path) const;
};

Point2f inline Film::Sample()
{
      // sample uniformly in a rectangular for now
      //assert(px >= 0 && px < width);
      //assert(py >= 0 && py < height);
      //assert(sampler);
      return sampler.Sample2D();
}