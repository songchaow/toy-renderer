#pragma once
#include "core/geometry.h"
#include "core/transform.h"
#include <QOpenGLFunctions_4_0_Core>

class CubeMap : QOpenGLFunctions_4_0_Core {
      Point3f o;
      GLuint cubeMapObj;
      static Transform toNDC[6];

public:
      void GenCubeMap(Point3f oIn) {
            glGenTextures(1, &cubeMapObj);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
            // attach the texture to the current frame buffer

      }
};