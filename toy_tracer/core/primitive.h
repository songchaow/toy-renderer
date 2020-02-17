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
      Primitive(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes, const Transform& t) : rt_m(rt_ms), Shapeable(meshes, t) {}
      Material* getMaterial() const { return material; }
      std::vector<PBRMaterial>& getPBRMaterial() { return rt_m; }
      //void setPBRMaterial(PBRMaterial m) { rt_m = m; }
      void load();
      void draw(Shader* shader);
      // switch context to the first material
      void drawPrepare(Shader* shader, int meshIndex);
      virtual bool isInstanced() const { return false; }
};

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
Primitive* CreatePrimitiveFromModelFile(std::string path);

class InstancedPrimitive : public Primitive {
      std::vector<Matrix4> obj2worlds;
      GLuint iabo;
public:
      InstancedPrimitive(Shape* shape, PBRMaterial m, const std::vector<Matrix4> obj2worldsIn)
            : obj2worlds(obj2worldsIn), Primitive(shape, m) {
            // transpose matrices
            for (auto& i : obj2worlds)
                  i.transpose();
      }
      InstancedPrimitive(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes, const std::vector<Matrix4> obj2worldsIn)
            : obj2worlds(obj2worldsIn), Primitive(rt_ms, meshes) {
            // transpose matrices
            for (auto& i : obj2worlds)
                  i.transpose();
      }
      void GenInstancedArray();
      void draw(Shader* s);
      bool isInstanced() const override { return true; }
};