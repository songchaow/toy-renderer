#pragma once
#include <string>
#include <memory>
#include <QOpenGLFunctions_4_0_Core>
#include "core/common.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include "core/image.h"

template <typename Element>
class Texture {
public:
      virtual bool loadable() { return false; }
      virtual Element Evaluate(Float u, Float v) = 0;
      Element Evaluate(Point2f pos) {return Evaluate(pos.x, pos.y);};
};

template <typename Element>
class ConstColorTexture : public Texture<Element> {
      Element _color;
public:
      virtual Element Evaluate(Float u, Float v) { return _color; }
      ConstColorTexture(Element color) : _color(color) {}
};

typedef Texture<Float> TextureF;
typedef Texture<R8G8B8> RGBTexture;
typedef Texture<RGBSpectrum> RGBSpectrumTexture;

extern ConstColorTexture<RGBSpectrum> blackConstantTexture;
extern ConstColorTexture<RGBSpectrum> whiteConstantTexture;

class ImageTexture : public RGBSpectrumTexture {
      Image* _image;
      std::string _path;
      GLuint _tbo = 0;
public:
      enum WrapMode {
            LOOP,
            BLACK
      };
private:
      WrapMode _wrapMode = LOOP;
public:
      ImageTexture(std::string img_path, WrapMode mode = LOOP) : _image(new Image(img_path)) {}
      ImageTexture(Image* image, WrapMode mode = LOOP) : _image(image) {}
      ImageTexture() = default;
      void resetImage(Image* i) { _image = i; }
      Float height() { if (!_image) return 0.f; return _image->resolution().x; }
      Float width() { if (!_image) return 0.f; return _image->resolution().y; }
      bool loadable() override { return true; }
      bool isValid() const { return (bool)_image; }
      bool isLoad() const { return _tbo > 0; }
      // create a tbo and load the image
      void load(QOpenGLFunctions_4_0_Core* f);
      // tbo unchanged, but reload the image
      void update(QOpenGLFunctions_4_0_Core* f);
      RGBSpectrum Evaluate(Float u, Float v) override;
      GLuint tbo() const { return _tbo; }
      const std::string& path() const { return _path; }
};