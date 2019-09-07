#pragma once

#include "core/material.h"
#include "core/shape.h"
#include "core/texture.h"
#include "core/scene_object.h"
#include "shape/triangle.h"
#include "core/shader.h"

class Primitive : public Shapeable, public RendererObject
{
private:
      Material* material = nullptr;
      PBRMaterial* rt_m = nullptr;
      Shader* shader = nullptr; // acquired from material
      RGBTexture* reflection; // currently not used
public:
      Primitive(Shape* shape, Material* m) : Shapeable(shape, ShapeID::Primitive), RendererObject(TypeID::Primitive, "Primitive"), material(m) {}
      Material* getMaterial() const { return material; }
      virtual void addProperties(QWidget* parent) override;
      void load(QOpenGLExtraFunctions* f);
      void draw(QOpenGLExtraFunctions* f);
};

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
Primitive* CreatePrimitiveFromModelFile(std::string path);