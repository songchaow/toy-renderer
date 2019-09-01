#pragma once
#include <string>
#include <memory>
#include <QOpenGLExtraFunctions>
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
      std::shared_ptr<Image> _image;
      GLuint _tbo = 0;
public:
      enum WrapMode {
            LOOP,
            BLACK
      };
private:
      WrapMode _wrapMode = LOOP;
public:
      ImageTexture(std::string img_path, WrapMode mode = LOOP) : _image(std::make_shared<Image>(img_path)) {}
      ImageTexture(Image* image, WrapMode mode = LOOP) : _image(image) {}
      bool loadable() override { return true; }
      void load(QOpenGLExtraFunctions* f);
      RGBSpectrum Evaluate(Float u, Float v) override;
      GLuint tbo() const { return _tbo; }
};