#pragma once

#include "core/material.h"
#include "core/shape.h"
#include "core/texture.h"

class Primitive : public Shapeable
{
private:
      // TODO: general material type should be replaced for Surface later.
      //Surface* m_material;
      Surface* material;
      RGBTexture* reflection;
public:
      Primitive(Shape* shape) :Shapeable(shape, ShapeID::Primitive) {}
      const Surface* getMaterial() const { return material; }
};