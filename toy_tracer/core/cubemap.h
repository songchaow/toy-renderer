#pragma once
#include "core/geometry.h"
#include "core/transform.h"
#include "main/renderworker.h"
#include <QOpenGLFunctions_4_0_Core>

Transform toNDCPerspective(Float n, Float f, Float hwRatio, Float fov);
void LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec, Matrix4& world2cam);
Matrix4 LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec = { 0.f, 0.f, 1.f });

class CubeMap : QOpenGLFunctions_4_0_Core {
      Point3f o;
      GLuint cubeMapObj;
      static const Transform camtoNDC;
      static const Transform o2cam[6];

public:
      void GenCubeMap(Point3f oIn) {
            glGenTextures(1, &cubeMapObj);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
            for (int i = 0; i < 6; i++) {
                  // attach the texture to the current frame buffer
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapObj, 0);
                  // set world2cam, cam2ndc, assuming shader is configured

                  RenderWorker::Instance()->renderScene();
            }
      }
};