#pragma once
#include <string>

#include "core/common.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include "core/image.h"

template <typename Element>
class Texture {
public:
      virtual Element Evaluate(Float u, Float v) = 0;
      Element Evaluate(Point2f pos) {return Evaluate(pos.x, pos.y);};
};

typedef Texture<Float> TextureF;
typedef Texture<R8G8B8> RGBTexture;

class ImageTexture : public RGBTexture {
      Image* _image;
public:
      enum WrapMode {
            LOOP,
            BLACK
      };
private:
      WrapMode _wrapMode;
public:
      ImageTexture(std::string img_path);
      ImageTexture(Image* image);


      R8G8B8 Evaluate(Float u, Float v) override;
};