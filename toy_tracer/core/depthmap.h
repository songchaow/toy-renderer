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
      GLuint texArray;
      const View* cameraView;
      Matrix4 view2world;
      // stored in view space
      // order : top-left, top-right, down-left, down-right, and the next layer...
      static const uint32_t numFrustumPoints = (NUM_CASCADED_SHADOW + 1) * 2 * 2;
      Point3f subFrustumPoints[numFrustumPoints];
      View lightViews[NUM_CASCADED_SHADOW];
      Float _zPartition[NUM_CASCADED_SHADOW]; // the last one is redundant
public:
      void initTexture();
      void setCameraView(const View* cameraViewIn);
      CascadedDepthMap() : cameraView(nullptr) {}
      CascadedDepthMap(View* cameraView) : cameraView(cameraView) {
            assert(cameraView);
            setCameraView(cameraView);
      }
      void GenLightViews(const Vector3f& dir);
      ~CascadedDepthMap() {
            glDeleteTextures(1, &texArray);
      }
      View* lightView() { return lightViews; }
      const Float* zPartition() const { return _zPartition; }
      const GLuint tbo() const { return texArray; }
};