#pragma once
#include "core/geometry.h"
#include "core/transform.h"
#include "glad/glad.h"

Transform toNDCPerspective(Float n, Float f, Float hwRatio, Float fov);
void LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec, Matrix4& world2cam);
Matrix4 LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec = { 0.f, 1.f, 0.f });

struct CubeMap {
      Point3f o;
      GLuint cubeMapObj;
      static const unsigned int SHADOW_WIDTH = 1024;
      static const unsigned int SHADOW_HEIGHT = 1024;
      static const Transform camtoNDC;
      static const Transform o2cam[6];
      //static const Float cam_near = 1.f; 
      CubeMap(const Point3f& o);
      CubeMap() : CubeMap(Point3f(0.f, 0.f, 0.f)) {}
      ~CubeMap() { glDeleteTextures(1, &cubeMapObj); }
      void GenCubeDepthMap();
};