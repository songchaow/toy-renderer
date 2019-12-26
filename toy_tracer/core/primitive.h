#pragma once

#include "core/material.h"
#include "core/shape.h"
#include "core/texture.h"
#include "main/scene_object.h"
#include "shape/triangle.h"
#include "core/shader.h"

class Primitive : public Shapeable
{
private:
      Material* material = nullptr;
      // TODO: don't use pointer, or modify the dctor
      PBRMaterial rt_m;
      Shader* shader = nullptr; // acquired from material
      RGBTexture2D* reflection; // currently not used
public:
      Primitive(Shape* shape, Material* m) : Shapeable(shape, ShapeID::Primitive), material(m) {}
      Primitive(Shape* shape, PBRMaterial m) : Shapeable(shape, ShapeID::Primitive), rt_m(m), material(nullptr) {}
      Material* getMaterial() const { return material; }
      PBRMaterial* getPBRMaterial() { return &rt_m; }
      void setPBRMaterial(PBRMaterial m) { rt_m = m; }
      void load(QOpenGLFunctions_4_0_Core* f);
      void draw(QOpenGLFunctions_4_0_Core* f);
};

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
Primitive* CreatePrimitiveFromModelFile(std::string path);