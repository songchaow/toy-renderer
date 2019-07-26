#include "core/film.h"
#include "ext/std_image/include/lodepng.h"
#include <cassert>
#include <memory>

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

void Film::Normalize()
{
      for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                  assert(WeightSum({ i,j }) != 0.f);
                  ContribSum({ i,j }) /= WeightSum({ i,j });
                  WeightSum({ i,j }) = 1.f;
            }

      }
}

void Film::writePNG(const std::string& path) {
      // always set the alpha channel to 0
      std::unique_ptr<unsigned char[]> img(new unsigned char[height*width * 3]);
      for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                  img[i*width + j*3] = GammaCorrection(ContribSum({ i,j })[0]);
                  img[i*width + j*3 + 1] = GammaCorrection(ContribSum({ i,j })[1]);
                  img[i*width + j*3 + 2] = GammaCorrection(ContribSum({ i,j })[2]);
            }
      }
      lodepng_encode24_file(path.c_str(), img.get(), width, height);
}