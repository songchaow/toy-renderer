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
      void initTexture() {
            glGenTextures(1, &texArray);
            glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
            // use floating point!
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, NUM_CASCADED_SHADOW, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)0);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // set border color to black!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      }
      void setCameraView(const View* cameraViewIn) {
            cameraView = cameraViewIn;
            view2world = Inverse(cameraViewIn->world2view);
            Float ratio = cameraViewIn->f.far / cameraViewIn->f.near;
            Float ratioPerScale = std::pow(ratio, 1.f / NUM_CASCADED_SHADOW);
            Float currNear = cameraViewIn->f.near;
            const Float& far = cameraViewIn->f.far;
            Float oneDivaspectRatio = 1.f / cameraViewIn->f.aspectRatio;
            Float widthSlope = std::tan(cameraViewIn->f.fov_Horizontal / 2);
            for (int i = 0; i < NUM_CASCADED_SHADOW; i++) {
                  subFrustumPoints[4 * i + 0] = { -widthSlope * currNear, widthSlope * currNear * oneDivaspectRatio, -currNear };;
                  subFrustumPoints[4 * i + 1] = { widthSlope * currNear, widthSlope * currNear * oneDivaspectRatio, -currNear };
                  subFrustumPoints[4 * i + 2] = { -widthSlope * currNear, -widthSlope * currNear * oneDivaspectRatio, -currNear };
                  subFrustumPoints[4 * i + 3] = { widthSlope * currNear, -widthSlope * currNear * oneDivaspectRatio, -currNear };
                  currNear *= ratioPerScale;
                  _zPartition[i] = currNear;
            }
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 0] = { -widthSlope * far, widthSlope * far * oneDivaspectRatio, -far };
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 1] = { widthSlope * far, widthSlope * far * oneDivaspectRatio, -far };
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 2] = { -widthSlope * far, -widthSlope * far * oneDivaspectRatio, -far };
            subFrustumPoints[4 * NUM_CASCADED_SHADOW + 3] = { widthSlope * far, -widthSlope * far * oneDivaspectRatio, -far };
      }
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