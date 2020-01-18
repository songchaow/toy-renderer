#pragma once
#include "core/geometry.h"
#include "core/transform.h"
#include <QOpenGLFunctions_4_0_Core>

Transform toNDCPerspective(Float n, Float f, Float hwRatio, Float fov);
void LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec, Matrix4& world2cam);

class CubeMap : QOpenGLFunctions_4_0_Core {
      Point3f o;
      GLuint cubeMapObj;
      static const Transform camtoNDC;
      static const Transform o2cam[6];

public:
      void GenCubeMap(Point3f oIn) {
            glGenTextures(1, &cubeMapObj);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
            // attach the texture to the current frame buffer

      }
};