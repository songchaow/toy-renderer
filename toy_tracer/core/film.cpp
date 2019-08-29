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

void Film::writePNG(const std::string& path) const {
      // always set the alpha channel to 0
      std::unique_ptr<unsigned char[]> img(new unsigned char[height*width * 3]);
      // lodepng uses a pixel order where it goes in rows from top to bottom
      for (int i = height-1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                  int image_row = height - 1 - i;
                  img[(image_row*width + j) * 3] = GammaCorrection(ContribSum({ i,j })[0]);
                  img[(image_row*width + j) * 3 + 1] = GammaCorrection(ContribSum({ i,j })[1]);
                  img[(image_row*width + j) * 3 + 2] = GammaCorrection(ContribSum({ i,j })[2]);
                  /*if (img[(i*width + j) * 3] == 0)
                        LOG(WARNING) << "R:" << img[(i*width + j) * 3] << " G:" << img[(i*width + j) * 3 + 1] << " B:" << img[(i*width + j) * 3 + 2];*/
            }
      }
      lodepng_encode24_file(path.c_str(), img.get(), width, height);
}

void Film::SetSamplePixelRegion(int px, int py) {
      // reconstruct sampler
      sampler.SetUniformDist(static_cast<Float>(px), static_cast<Float>(px + 1), static_cast<Float>(py), static_cast<Float>(py + 1));
}