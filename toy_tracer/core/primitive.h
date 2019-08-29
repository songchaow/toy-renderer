#pragma once

#include "core/material.h"
#include "core/shape.h"
#include "core/texture.h"
#include "core/scene_object.h"
#include "shape/triangle.h"

class Primitive : public Shapeable, public RendererObject
{
private:
      Material* material = nullptr;
      
      RGBTexture* reflection; // currently not used
public:
      Primitive(Shape* shape, Material* m) : Shapeable(shape, ShapeID::Primitive), RendererObject(TypeID::Primitive, "Primitive"), material(m) {}
      Material* getMaterial() const { return material; }
      virtual void addProperties(QWidget* parent) override;
      void load(QOpenGLExtraFunctions* f);
};

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
Primitive* CreatePrimitiveFromModelFile(std::string path);