#include "core/cubemap.h"
#include "core/shader.h"

/*static*/ const Transform CubeMap::camtoNDC { toNDCPerspective(0.1f, 1000.f, 1.f, 90.f/ 180.f * Pi) };
/*static*/ const Transform CubeMap::o2cam[6] = { 
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(1.f, 0.f, 0.f)),                            // X+
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(-1.f, 0.f, 0.f)),                           // X-
      LookAt(Point3f(0.f, 0.f ,0.f), Vector3f(0.f, 1.f, 0.f), Vector3f(0.f, 0.f ,-1.f)),  // Y+
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(0.f, -1.f, 0.f), Vector3f(0.f, 0.f, 1.f)),  // Y-
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(0.f, 0.f, 1.f)),                            // Z+
      LookAt(Point3f(0.f, 0.f ,0.f), Vector3f(0.f, 0.f, -1.f))                            // Z-
};

void CubeMap::GenCubeDepthMap() {
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      for (int i = 0; i < 6; i++) {
            // attach the texture to the current frame buffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapObj, 0);
            // set world2cam, cam2ndc, assuming shader is configured
            // fetch the shader from shader store
            Shader* depthMapShader = LoadShader(ShaderType::DEPTH_MAP, true);
            depthMapShader->use();
            Transform world2cam = Translate(-o.x, -o.y, -o.z)*o2cam[i];
            depthMapShader->setUniformF("world2cam", &world2cam.m);
            depthMapShader->setUniformF("cam2ndc", &camtoNDC.m);
            RenderWorker::Instance()->renderScene();
      }
}