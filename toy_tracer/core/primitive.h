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
      std::vector<PBRMaterial> rt_m;
      RGBTexture2D* reflection; // currently not used
public:
      Primitive(Shape* shape, Material* m) : Shapeable(shape, ShapeID::Primitive), material(m) {}
      Primitive(Shape* shape, PBRMaterial m, Transform t) : Shapeable(shape, ShapeID::Primitive, t), rt_m(1, m), material(nullptr) {}
      Primitive(Shape* shape, PBRMaterial m) : Shapeable(shape, ShapeID::Primitive), rt_m(1, m), material(nullptr) {}
      Primitive(PBRMaterial m, Transform t) : Primitive(nullptr, m, t) {}
      Primitive(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes) : rt_m(rt_ms), Shapeable(meshes) {}
      Material* getMaterial() const { return material; }
      std::vector<PBRMaterial>& getPBRMaterial() { return rt_m; }
      //void setPBRMaterial(PBRMaterial m) { rt_m = m; }
      void load();
      void draw(Shader* shader);
};

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
Primitive* CreatePrimitiveFromModelFile(std::string path);