#include "core/film.h"
#include <cassert>


void Film::addRay(Spectrum & Li, Point2f & pFilm)
{
      if (!filter) {
            // use minimal box filtering
            Point2i pixelIndex(pFilm);
            WeightSum(pixelIndex) += 1.f;
            ContribSum(pixelIndex) += Li;
      }
}

Point2f inline Film::SampleFilm(int px, int py)
{
      assert(px>=0 && px < width);
      assert(py>=0 && py < height);
      assert(sampler);
      return sampler->SamplePixel(px, py);
}
