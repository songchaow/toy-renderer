#pragma once
#include <cstdint>
#include <cassert>
#include "glad/glad.h"
#include "light/point.h"
#include "core/frustum.h"

constexpr unsigned int SHADOW_WIDTH = 1024;
constexpr unsigned int SHADOW_HEIGHT = 1024;

// for internal use, so uses gl in ctors
struct CubeDepthMap {
      const PointLight* l;
      GLuint cubeMapObj;
      
      static const Float depthFarPlane;
      static const Float depthNearPlane;
      static const Transform camtoNDC;
      static const Transform o2cam[6];
      //static const Float cam_near = 1.f; 
      CubeDepthMap(const PointLight* l);
      CubeDepthMap(const CubeDepthMap& c) = delete;
      CubeDepthMap() : CubeDepthMap(nullptr) {}
      ~CubeDepthMap() { glDeleteTextures(1, &cubeMapObj); }
      void GenCubeDepthMap();
      void clearDepth();
};

// Cascaded Shadow Map
constexpr uint32_t NUM_CASCADED_SHADOW = 4;

struct FrustumTrapezoid {
      Float z_near;
      Float z_far;
      Float x_short;
      Float x_long;
};
class CascadedDepthMap {
      GLuint textures[NUM_CASCADED_SHADOW];
      View* cameraView;
      Matrix4 view2world;
      // stored in view space
      // order : top-left, top-right, down-left, down-right, and the next layer...
      static const uint32_t numFrustumPoints = (NUM_CASCADED_SHADOW + 1) * 2 * 2;
      Point3f subFrustumPoints[numFrustumPoints];
public:
      void initTexture() {
            glGenTextures(NUM_CASCADED_SHADOW, textures);
            for (int i = 0; i < NUM_CASCADED_SHADOW; i++) {
                  glBindTexture(GL_TEXTURE_2D, textures[i]);
                  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)0);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
      }

      CascadedDepthMap() : cameraView(nullptr) {}
      CascadedDepthMap(View* cameraView) : cameraView(cameraView) {
            assert(cameraView);
            view2world = Inverse(cameraView->world2view);
            Float ratio = cameraView->f.far / cameraView->f.near;
            Float ratioPerScale = std::pow(ratio, 1.f / NUM_CASCADED_SHADOW );
            Float currNear = cameraView->f.near;
            Float& far = cameraView->f.far;
            Float oneDivaspectRatio = 1.f / cameraView->f.aspectRatio;
            Float widthSlope = std::tan(cameraView->f.fov_Horizontal / 2);
            for (int i = 0; i < NUM_CASCADED_SHADOW; i++) {
                  subFrustumPoints[4 * i + 0] = { -widthSlope * currNear, widthSlope * currNear * oneDivaspectRatio, -currNear };;
                  subFrustumPoints[4 * i + 1] = { widthSlope * currNear, widthSlope * currNear * oneDivaspectRatio, -currNear };
                  subFrustumPoints[4 * i + 2] = { -widthSlope * currNear, -widthSlope * currNear * oneDivaspectRatio, -currNear };
                  subFrustumPoints[4 * i + 3] = { widthSlope * currNear, -widthSlope * currNear * oneDivaspectRatio, -currNear };
                  currNear *= ratioPerScale;
            }
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 0] = { -widthSlope * far, widthSlope * far * oneDivaspectRatio, -far };
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 1] = { widthSlope * far, widthSlope * far * oneDivaspectRatio, -far };
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 2] = { -widthSlope * far, -widthSlope * far * oneDivaspectRatio, -far };
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 3] = { widthSlope * far, -widthSlope * far * oneDivaspectRatio, -far };

      }
      void GenLightFrustums(const Vector3f& dir);
      ~CascadedDepthMap() {
            glDeleteTextures(NUM_CASCADED_SHADOW, textures);
      }
};