#pragma once
#include "core/shape.h"
#include <string>

class Triangle;
class TriangleMesh {
      // TODO: use a raw data field to store various fields
      struct LayoutItem {
            uint16_t strip;

      };

      std::vector<Point3f> points;        // size:   v*Point3f
      std::vector<Point3f> point_normals; // size:   v*Point3f
      std::vector<Point2f> uv_coords;     // size:   v*Point2f
      std::vector<Triangle> face_indices;
};

class Triangle : public Shape {
      TriangleMesh* mesh;
      //
      uint32_t index0, index1, index2;
public:
      virtual Float Area() const override;
      // const GLenum GLIndexType() const { return GL_UNSINGED_INT; }
      // ctor: create a triangle in one new TriangleMesh, with Point3f as input
      // ctor: create a triangle using existing TriangleMesh, with Point3f as input
      // ctor: create a triangle using existing TriangleMesh

};

TriangleMesh* LoadTriangleMesh(const std::string& path);