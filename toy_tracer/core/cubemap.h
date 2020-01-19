#pragma once
#include "core/geometry.h"
#include "core/transform.h"
#include "main/renderworker.h"
#include <QOpenGLFunctions_4_0_Core>

Transform toNDCPerspective(Float n, Float f, Float hwRatio, Float fov);
void LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec, Matrix4& world2cam);
Matrix4 LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec = { 0.f, 0.f, 1.f });

struct CubeMap : QOpenGLFunctions_4_0_Core {
      Point3f o;
      GLuint cubeMapObj;
      static const Transform camtoNDC;
      static const Transform o2cam[6];

      CubeMap(const Point3f& o) : o(o) {
            glGenTextures(1, &cubeMapObj);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
      }
      CubeMap() : CubeMap(Point3f(0.f, 0.f, 0.f)) {}
      ~CubeMap() { glDeleteTextures(1, &cubeMapObj); }
      void GenCubeDepthMap();
};