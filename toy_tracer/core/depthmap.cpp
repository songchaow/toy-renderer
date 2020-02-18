#include "core/depthmap.h"
#include "core/geometry.h"
#include "main/renderworker.h"

Transform toNDCPerspective(Float n, Float f, Float hwRatio, Float fov);
void LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec, Matrix4& world2cam);
Matrix4 LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec);

const Float CubeDepthMap::depthFarPlane = 1500.f;
const Float CubeDepthMap::depthNearPlane = 1.f;
/*static*/ const Transform CubeDepthMap::camtoNDC{ toNDCPerspective(depthNearPlane, CubeDepthMap::depthFarPlane, 1.f, 90.f / 180.f * Pi) };
/*static*/ const Transform CubeDepthMap::o2cam[6] = {
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(1.f, 0.f, 0.f), Vector3f(0.f, -1.f, 0.f)),                            // X+
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(-1.f, 0.f, 0.f), Vector3f(0.f, -1.f, 0.f)),                           // X-
      LookAt(Point3f(0.f, 0.f ,0.f), Vector3f(0.f, 1.f, 0.f), Vector3f(0.f, 0.f ,1.f)),                             // Y+
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(0.f, -1.f, 0.f), Vector3f(0.f, 0.f, -1.f)),                           // Y-
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(0.f, 0.f, 1.f), Vector3f(0.f, -1.f, 0.f)),                            // Z+
      LookAt(Point3f(0.f, 0.f ,0.f), Vector3f(0.f, 0.f, -1.f), Vector3f(0.f, -1.f, 0.f)),                           // Z-
};

CubeDepthMap::CubeDepthMap(const PointLight* l) : l(l) {
      glGenTextures(1, &cubeMapObj);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
      for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void CubeDepthMap::GenCubeDepthMap() {
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      Shader* depthMapShader = LoadShader(ShaderType::DEPTH_CUBE_MAP, true);
      depthMapShader->use();
      depthMapShader->setUniformF("far", depthFarPlane);
      depthMapShader->setUniformF("cam2ndc", &camtoNDC.m);
      depthMapShader->setUniformF("camPos", l->pos().x, l->pos().y, l->pos().z);
      depthMapShader->setUniformBool("directional", l->isDirectionalLight());
      depthMapShader->setUniformF("direction", l->direction());
      uint32_t world2cam_pos = depthMapShader->getUniformLocation("world2cams[0]");
      Matrix4 world2light[6];
      for (int i = 0; i < 6; i++) {
            world2light[i] = o2cam[i].m * TranslateM(-l->pos().x, -l->pos().y, -l->pos().z);
            depthMapShader->setUniformF(world2cam_pos + i, &world2light[i]);
      }
      Shader* depthInstanceShader = LoadShader(ShaderType::DEPTH_CUBE_MAP_INSTANCED, true);
      depthInstanceShader->use();
      depthInstanceShader->setUniformF("far", depthFarPlane);
      depthInstanceShader->setUniformF("cam2ndc", &camtoNDC.m);
      depthInstanceShader->setUniformF("camPos", l->pos().x, l->pos().y, l->pos().z);
      depthInstanceShader->setUniformBool("directional", l->isDirectionalLight());
      depthInstanceShader->setUniformF("direction", l->direction());
      world2cam_pos = depthInstanceShader->getUniformLocation("world2cams[0]");
      for (int i = 0; i < 6; i++) {
            depthInstanceShader->setUniformF(world2cam_pos + i, &world2light[i]);
      }
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapObj, 0);
      RenderWorker::Instance()->renderPassCubeMapDepth();
#if 0
      for (int i = 0; i < 6; i++) {
            // attach the texture to the current frame buffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapObj, 0);
            // set world2cam, cam2ndc, assuming shader is configured
            // fetch the shader from shader store
            depthMapShader->use();
            Transform world2light = o2cam[i] * Translate(-l->pos().x, -l->pos().y, -l->pos().z);
            depthMapShader->setUniformF("world2cam", &world2light.m);


            depthInstanceShader->use();
            depthInstanceShader->setUniformF("world2cam", &world2light.m);

            RenderWorker::Instance()->renderPassCubeMapDepth();
      }
#endif
      //// print to file
#if 0
      static int ii = 0;
      char* block = new char[SHADOW_HEIGHT*SHADOW_WIDTH * 4];
      std::ofstream image_block(std::to_string(5) + ".bin");
      glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5, 0, GL_DEPTH_COMPONENT, GL_FLOAT, block);
      image_block.write(block, SHADOW_HEIGHT*SHADOW_WIDTH * 4);
      /*for (int i = 0; i < 6; i++) {
            std::ofstream image_block(std::to_string(i) + ".bin");
            glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, GL_FLOAT, block);
            image_block.write(block, SHADOW_HEIGHT*SHADOW_WIDTH * 4);
      }*/
      delete block;
      ii++;
      if (ii == 1)
            exit(0);
#endif
      //glViewport
}

void CubeDepthMap::clearDepth() {
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      for (int i = 0; i < 6; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapObj, 0);
            glClearDepth(1.0);
            glClear(GL_DEPTH_BUFFER_BIT);
      }

}

AABB FindAABB(const Point3f* pBegin, const Point3f* pEnd) {
      if (pBegin == pEnd)
            return AABB();
      AABB ret(*pBegin);
      for (const Point3f* i = pBegin + 1; i < pEnd; i++) {
            ret.Add(*i);
      }
      return ret;
}

void CascadedDepthMap::GenLightFrustums(const Vector3f& dir)
{
      // transform to light space, with the origin set to (0,0,0) (arbitrarily)
      Matrix4 world2light = LookAt({ 0,0,0 }, dir);
      Matrix4 light2world = Inverse(world2light);
      Matrix4 view2light = Inverse(cameraView->world2view) * world2light;
      Point3f frustumPoints[numFrustumPoints];
      for (int i = 0; i < numFrustumPoints; i++) {
            frustumPoints[i] = view2light(subFrustumPoints[i]);
      }
      std::vector<View> lightViews;
      for (int i = 0; i < NUM_CASCADED_SHADOW; i++) {
            const Point3f* pbegin = frustumPoints + 4 * i;
            const Point3f* pEnd = frustumPoints + 4 * i + 8;
            AABB bbox = FindAABB(pbegin, pEnd);
            Float centerX = (bbox.pMax.x + bbox.pMin.x) / 2;
            Float centerY = (bbox.pMax.y + bbox.pMin.y) / 2;
            // light dir is to the -Z direction, so the pMax.z is the starting
            Point3f posInLightSpace(centerX, centerY, bbox.pMax.z);
            Point3f pos = light2world(posInLightSpace);
            Frustum lightFrustum(bbox.pMax.x - bbox.pMin.x, bbox.pMax.y - bbox.pMin.y, bbox.pMax.z - bbox.pMin.z);
            lightViews.emplace_back(lightFrustum, TranslateM(pos.x, pos.y, pos.z));
      }
      // determine length(far-near), x, y, and position in view space first
      // iterate through 4 points: 
      Point2f trazepoid[4];
}
