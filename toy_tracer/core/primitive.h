#pragma once

#include "core/material.h"
#include "core/shape.h"
#include "core/texture.h"

class Primitive : public Shapeable
{
private:
      Material* material;
      RGBTexture* reflection; // currently not used
public:
      Primitive(Shape* shape) :Shapeable(shape, ShapeID::Primitive) {}
      Material* getMaterial() const { return material; }
};