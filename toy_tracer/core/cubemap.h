#pragma once
#include "core/geometry.h"
#include "core/transform.h"
#include "core/image.h"
#include "glad/glad.h"

Transform toNDCPerspective(Float n, Float f, Float hwRatio, Float fov);
void LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec, Matrix4& world2cam);
Matrix4 LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec = { 0.f, 1.f, 0.f });

// for internal use, so uses gl in ctors
struct CubeDepthMap {
      Point3f o;
      GLuint cubeMapObj;
      static const unsigned int SHADOW_WIDTH = 1024;
      static const unsigned int SHADOW_HEIGHT = 1024;
      static const Float depthFarPlane;
      static const Float depthNearPlane;
      static const Transform camtoNDC;
      static const Transform o2cam[6];
      //static const Float cam_near = 1.f; 
      CubeDepthMap(const Point3f& o);
      CubeDepthMap(const CubeDepthMap& c) = delete;
      CubeDepthMap() : CubeDepthMap(Point3f(0.f, 0.f, 0.f)) {}
      ~CubeDepthMap() { glDeleteTextures(1, &cubeMapObj); }
      void GenCubeDepthMap();
};

// value-like, no copy
// needs user's configuration, so does not use gl in ctor

// may uses gl before renderloop. but not necessarily
struct CubeMap {
      GLuint cubeMapObj = 0;
      Image* _image[6] = {0};
      Point2i resolution;
      CubeMap() = default;
      CubeMap(const std::vector<std::string>& paths) { loadImage(paths); }
      void loadImage(const std::vector<std::string>& paths);
      bool ready2Load() {
            for (int i = 0; i < 6; i++)
                  if (_image[i] == nullptr)
                        return false;
            return true;
      }
      void glLoad();
      void glUse() { glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj); }
      ~CubeMap() {
            if(cubeMapObj)
                  glDeleteTextures(1, &cubeMapObj);
      }
      CubeMap(const CubeMap& c) = delete;
      CubeMap& operator=(const CubeMap& c) = delete;
};