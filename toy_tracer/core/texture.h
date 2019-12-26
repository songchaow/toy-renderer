#pragma once
#include <string>
#include <memory>
#include <QOpenGLFunctions_4_0_Core>
#include "core/common.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include "core/image.h"

template <typename Element>
class Texture2D {
public:
      virtual bool loadable() { return false; }
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

// Point-like class
class ImageTexture : public RGBSpectrumTexture2D {
      Image* _image;
      std::string _path;
      GLuint* _tbo;
      unsigned int* _ref;
public:
      enum WrapMode {
            LOOP,
            BLACK
      };
private:
      WrapMode _wrapMode = LOOP;
public:
      ImageTexture(std::string img_path, WrapMode mode = LOOP) : _image(new Image(img_path)) {
            _tbo = new GLuint();
            _ref = new unsigned int(1);
      }
      ImageTexture(Image* image, WrapMode mode = LOOP) : _image(image), _ref(0) {
            _tbo = new GLuint();
            _ref = new unsigned int(1);
      }
      ImageTexture() : _ref(0), _image(nullptr) {
            _tbo = new GLuint();
            _ref = new unsigned int(1);
      }
      ImageTexture(const ImageTexture& i) :_image(i._image), _path(i._path), _tbo(i._tbo), _ref(i._ref) {
            ++*_ref;
      }
      ImageTexture& operator=(const ImageTexture& i) {
            --*_ref;
            ++*(i._ref);
            if (*_ref == 0) {
                  // TODO: destruct
            }
            _image = i._image;
            _path = i._path;
            _tbo = i._tbo;
            _ref = i._ref;
            return *this;
      }
      ~ImageTexture() {
            --*_ref;
            if (*_ref == 0) {
                  if (*_tbo > 0) {
                        // free the texture
                        // free tbo int
                  }
                  // free the image
                  // free the ref int
            }
      }
      void resetImage(Image* i) { _image = i; }
      Float height() { if (!_image) return 0.f; return _image->resolution().x; }
      Float width() { if (!_image) return 0.f; return _image->resolution().y; }
      bool loadable() override { return true; }
      bool isValid() const { return (bool)_image; }
      bool isLoad() const { return *_tbo > 0; }
      // create a tbo and load the image
      void load(QOpenGLFunctions_4_0_Core* f);
      // tbo unchanged, but reload the image
      void update(QOpenGLFunctions_4_0_Core* f);
      RGBSpectrum Evaluate(Float u, Float v) override;
      GLuint tbo() const { return *_tbo; }
      const std::string& path() const { return _path; }
};

template <typename Element>
struct Texture1D {
      virtual Element Evaluate(Float u) = 0;

};
typedef Texture1D<RGBSpectrum> RGBSpectrumTexture1D;

// Designed for offline
// Pointer-like
template <typename Element>
struct ArrayTexture1D : public Texture1D<Element> {
      std::shared_ptr<std::vector<Element>> _data;
      virtual Element Evaluate(Float u) override {
            assert(0 >= 0.f && u <= 1.f);
            return (*_data)[u*(_data->size() - 0.001f)];
      }
      ArrayTexture1D(const std::vector<Element>& data) { _data = std::make_shared<std::vector<Element>>(data); }
      size_t size() const { return _data->size(); }
};

typedef ArrayTexture1D<RGBSpectrum> RGBArrayTexture1D;

// Loadable, designed for realtime
struct RTRGBArrayTexture1D : public ArrayTexture1D<RGBSpectrum> {
      GLuint* _tbo;
      unsigned int* _ref;
      RTRGBArrayTexture1D(const std::vector<RGBSpectrum>& data): _tbo(new GLuint()), _ref(new unsigned int(1)), ArrayTexture1D<RGBSpectrum>(data) {}
      bool isValid() const { return static_cast<bool>(_data); }
      bool isLoad() const { return *_tbo > 0; }
      RTRGBArrayTexture1D(const RTRGBArrayTexture1D& t) : ArrayTexture1D<RGBSpectrum>(t), _tbo(t._tbo), _ref(t._ref) {
            ++*t._ref;
      }
      RTRGBArrayTexture1D& operator=(const RTRGBArrayTexture1D& t) {
            ArrayTexture1D<RGBSpectrum>::operator=(t);
            --*_ref;
            ++*(t._ref);
            if (*_ref == 0) {
                  // destruct texture AND tbo integer. data is handled by shared_ptr
            }
            _ref = t._ref;
            _tbo = t._tbo;
      }
      ~RTRGBArrayTexture1D() {
            --*_ref;
            if (*_ref == 0) {
                  // destruct texture AND tbo integer

            }
      }
      // create a tbo and load the array
      void load(QOpenGLFunctions_4_0_Core* f);
      // tbo unchanged, but reload the image
      void update(QOpenGLFunctions_4_0_Core* f);
};