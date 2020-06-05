#pragma once

#include "core/material.h"
#include "core/shape.h"
#include "core/texture.h"
#include "main/scene_object.h"
#include "shape/triangle.h"
#include "core/shader.h"

class PrimitiveBase {
protected:
      // loaded and ready to render
      bool loaded = false;
      AnimatedTransform _obj2world;
public:
      PrimitiveBase(const Transform& obj2world) : _obj2world(obj2world) {}
      PrimitiveBase() = default;
      bool isLoad() const { return loaded; }
      virtual bool is3D() const = 0;
      virtual void load() = 0;
};

class Primitive3D : public PrimitiveBase
{
protected:
      Material* material = nullptr; // old
      Shape* _shape;
      std::vector<TriangleMesh*> _meshes;
      // TODO: don't use pointer, or modify the dctor
      std::vector<PBRMaterial> rt_m;
      
public:
      Primitive3D(Shape* shape, Material* m) : _shape(shape), material(m) {}
      Primitive3D(Shape* shape, PBRMaterial m, Transform t) : _shape(shape), rt_m(1, m), material(nullptr), PrimitiveBase(t) {}
      Primitive3D(Shape* shape, PBRMaterial m) : _shape(shape), rt_m(1, m), material(nullptr) {}
      Primitive3D(PBRMaterial m, Transform t) : Primitive3D(nullptr, m, t) {}
      Primitive3D(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes) : rt_m(rt_ms), _meshes(meshes) {}
      Primitive3D(const std::vector<PBRMaterial> rt_ms, const std::vector<TriangleMesh*>& meshes, const Transform& t) : rt_m(rt_ms), _meshes(meshes), PrimitiveBase(t) {}
      Material* getMaterial() const { return material; }
      std::vector<PBRMaterial>& getPBRMaterial() { return rt_m; }
      //void setPBRMaterial(PBRMaterial m) { rt_m = m; }
      void load() override;
      void draw(Shader* shader);
      // Do not bind textures and material properties
      void drawSimple(Shader* shader);
      // switch context to the first material
      void drawPrepare(Shader* shader, int meshIndex);
      virtual bool isInstanced() const { return false; }
      bool is3D() const { return true; }
      Shape* shape() const { return _shape; }
      AnimatedTransform& obj2world() { return _obj2world; }
      void GenMeshes() {
            assert(_shape);
            _meshes = _shape->GenMesh();
      }
      const std::vector<TriangleMesh*>& meshes() const { return _meshes; }
};

Primitive3D* CreatePrimitiveFromMeshes(TriangleMesh* mesh);
//Primitive* CreatePrimitiveFromModelFile(std::string path);

class InstancedPrimitive : public Primitive3D {
      std::vector<Matrix4> obj2worlds;
      // instanced array buffer object
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
class Primitive2D : public PrimitiveBase {
      Point3f posWorld;
      //Matrix4 _obj2world; // calculated from posWorld
      Point2f size;
      ImageTexture image;
public:
      void load() override;
      void draw();
      Primitive2D(Point3f pos, Point2f size, ImageTexture i) : posWorld(pos), size(size), image(i) {}
      bool is3D() const { return false; }
};