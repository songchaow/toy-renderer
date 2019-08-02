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
      Primitive(Shape* shape, Material* m) :Shapeable(shape, ShapeID::Primitive), material(m) {}
      Material* getMaterial() const { return material; }
};