#pragma once
#include <string>
#include <memory>
#include <glad/glad.h>
#include "core/common.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include "core/image.h"

template <typename Element>
class Texture2D {
public:
      virtual Element Evaluate(Float u, Float v) = 0;
      Element Evaluate(Point2f pos) {return Evaluate(pos.x, pos.y);};
};

template <typename Element>
class ConstColorTexture : public Texture2D<Element> {
      Element _color;
public:
      virtual Element Evaluate(Float u, Float v) { return _color; }
      ConstColorTexture(Element color) : _color(color) {}
};

typedef Texture2D<Float> TextureF;
typedef Texture2D<R8G8B8> RGBTexture2D;
typedef Texture2D<RGBSpectrum> RGBSpectrumTexture2D;

extern ConstColorTexture<RGBSpectrum> blackConstantTexture;
extern ConstColorTexture<RGBSpectrum> whiteConstantTexture;

// Point-like class (not necessarily)
class ImageTexture : public RGBSpectrumTexture2D {
      Image* _image; // temporarily owned
      // shared attributes
      GLuint* _tbo;
      Point2f* resolution;
      unsigned int* _ref;
public:
      enum WrapMode {
            LOOP,
            BLACK
      };
private:
      WrapMode _wrapMode = LOOP;
      void destruct() {
            if (*_tbo > 0) {
                  // free the texture
                  // free tbo int
            }
            // free the image(if)
            // free the ref int
            // free the resolution
      }
public:
      ImageTexture(std::string img_path, WrapMode mode = LOOP) : _image(new Image(img_path)) {
            _tbo = new GLuint();
            _ref = new unsigned int(1);
            resolution = new Point2f(_image->resolution());
      }
      ImageTexture(Image* image, WrapMode mode = LOOP) : _image(image), _ref(0) {
            _tbo = new GLuint();
            _ref = new unsigned int(1);
            resolution = new Point2f(_image->resolution());
      }
      ImageTexture() : _ref(0), _image(nullptr) {
            _tbo = new GLuint();
            _ref = new unsigned int(1);
            resolution = new Point2f();
      }
      ImageTexture(const ImageTexture& i) :_image(i._image), _tbo(i._tbo), _ref(i._ref), resolution(resolution) {
            ++*_ref;
      }
      ImageTexture& operator=(const ImageTexture& i) {
            --*_ref;
            ++*(i._ref);
            if (*_ref == 0) {
                  destruct();
            }
            _image = i._image;
            _tbo = i._tbo;
            _ref = i._ref;
            resolution = i.resolution;
            return *this;
      }
      
      ~ImageTexture() {
            --*_ref;
            if (*_ref == 0) {
                  destruct();
            }
      }
      void resetImage(Image* i) { _image = i; }
      void resetImage(std::string path) {
            if (_image)
                  delete _image;
            _image = new Image(path);
      }
      Float height() { return resolution->x; }
      Float width() { return resolution->y; }
      bool ready2Load() const { return (bool)_image; }
      bool isLoad() const { return *_tbo > 0; }
      // create a tbo and load the image
      void load();
      // tbo unchanged, but reload the image
      void update();
      RGBSpectrum Evaluate(Float u, Float v) override;
      GLuint tbo() const { return *_tbo; }

};

template <typename Element>
struct Texture1D {
      virtual Element Evaluate(Float u) = 0;

};
typedef Texture1D<RGBSpectrum> RGBSpectrumTexture1D;

