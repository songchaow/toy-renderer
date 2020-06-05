#pragma once

#include "core/material.h"
#include "core/shape.h"
#include "core/texture.h"
#include "main/scene_object.h"
#include "shape/triangle.h"
#include "core/shader.h"

class Primitive3D : public Shapeable
{
private:
      Material* material = nullptr;
      bool loaded = false;
      // TODO: don't use pointer, or modify the dctor
      std::vector<PBRMaterial> rt_m;
      AnimatedTransform _obj2world;
public:
      Primitive3D(Shape* shape, Material* m) : Shapeable(shape, ShapeID::Primitive3D), material(m) {}
      Primitive3D(Shape* shape, PBRMaterial m, Transform t) : Shapeable(shape, ShapeID::Primitive3D), rt_m(1, m), material(nullptr), _obj2world(t) {}
      Primitive3D(Shape* shape, PBRMaterial m) : Shapeable(shape, ShapeID::Primitive3D), rt_m(1, m), material(nullptr) {}
      Primitive3D(PBRMaterial m, Transform t) : Primitive3D(nullptr, m, t) {}
      Primitive3D(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes) : rt_m(rt_ms), Shapeable(meshes) {}
      Primitive3D(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes, const Transform& t) : rt_m(rt_ms), Shapeable(meshes), _obj2world(t) {}
      Material* getMaterial() const { return material; }
      std::vector<PBRMaterial>& getPBRMaterial() { return rt_m; }
      //void setPBRMaterial(PBRMaterial m) { rt_m = m; }
      bool isLoad() const { return loaded; }
      void load();
      void draw(Shader* shader);
      // Do not bind textures and material properties
      void drawSimple(Shader* shader);
      // switch context to the first material
      void drawPrepare(Shader* shader, int meshIndex);
      virtual bool isInstanced() const { return false; }
      AnimatedTransform& obj2world() { return _obj2world; }
};

Primitive3D* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
//Primitive* CreatePrimitiveFromModelFile(std::string path);

class InstancedPrimitive : public Primitive3D {
      std::vector<Matrix4> obj2worlds;
      GLuint iabo;
public:
      InstancedPrimitive(Shape* shape, PBRMaterial m, const std::vector<Matrix4> obj2worldsIn)
            : obj2worlds(obj2worldsIn), Primitive3D(shape, m) {
            // transpose matrices
            for (auto& i : obj2worlds)
                  i.transpose();
      }
      InstancedPrimitive(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes, const std::vector<Matrix4> obj2worldsIn)
            : obj2worlds(obj2worldsIn), Primitive3D(rt_ms, meshes) {
            // transpose matrices
            for (auto& i : obj2worlds)
                  i.transpose();
      }
      void GenInstancedArray();
      void draw(Shader* s);
      bool isInstanced() const override { return true; }
};

// no vertex data is needed currently
//
class Primitive2D {
      Point3f posWorld;
      Matrix4 obj2world; // calculated from posWorld
      Point2f size;
      ImageTexture image;
      void draw();
      void load();
public:
      Primitive2D(Point3f pos, Point2f size, ImageTexture i) : posWorld(pos), size(size), image(i) {}
};