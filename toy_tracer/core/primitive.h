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
      PBRMaterial* rt_m = nullptr;
      Shader* shader = nullptr; // acquired from material
      RGBTexture* reflection; // currently not used
public:
      Primitive(Shape* shape, Material* m) : Shapeable(shape, ShapeID::Primitive), material(m) {}
      Material* getMaterial() const { return material; }
      PBRMaterial* getPBRMaterial() const { return rt_m; }
      void load(QOpenGLFunctions_4_0_Core* f);
      void draw(QOpenGLFunctions_4_0_Core* f);
};

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
Primitive* CreatePrimitiveFromModelFile(std::string path);