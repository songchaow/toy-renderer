#include "core/cubemap.h"
#include "core/shader.h"
#include "main/renderworker.h"
#include <fstream>

/*static*/ const Transform CubeMap::camtoNDC { toNDCPerspective(0.1f, 1000.f, 1.f, 90.f/ 180.f * Pi) };
/*static*/ const Transform CubeMap::o2cam[6] = { 
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(1.f, 0.f, 0.f)),                            // X+
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(-1.f, 0.f, 0.f)),                           // X-
      LookAt(Point3f(0.f, 0.f ,0.f), Vector3f(0.f, 1.f, 0.f), Vector3f(0.f, 0.f ,-1.f)),  // Y+
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(0.f, -1.f, 0.f), Vector3f(0.f, 0.f, 1.f)),  // Y-
      LookAt(Point3f(0.f, 0.f, 0.f), Vector3f(0.f, 0.f, 1.f), Vector3f(0.f, 1.f, 0.f)),   // Z+
      LookAt(Point3f(0.f, 0.f ,0.f), Vector3f(0.f, 0.f, -1.f), Vector3f(0.f, 1.f, 0.f))    // Z-
};

CubeMap::CubeMap(const Point3f& o) : o(o) {
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

void CubeMap::GenCubeDepthMap() {
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      for (int i = 0; i < 6; i++) {
            // attach the texture to the current frame buffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapObj, 0);
            // set world2cam, cam2ndc, assuming shader is configured
            // fetch the shader from shader store
            Shader* depthMapShader = LoadShader(ShaderType::DEPTH_MAP, true);
            depthMapShader->use();
            Transform world2light = o2cam[i] * Translate(-o.x, -o.y, -o.z);
            depthMapShader->setUniformF("world2cam", &world2light.m);
            depthMapShader->setUniformF("cam2ndc", &camtoNDC.m);
            depthMapShader->setUniformF("far", 1000.f);
            RenderWorker::Instance()->renderPassDepth();
      }
      //// print to file
      //char* block = new char[SHADOW_HEIGHT*SHADOW_WIDTH*4];
      //std::ofstream image_block(std::to_string(4) + ".bin");
      //glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, block);
      //image_block.write(block, SHADOW_HEIGHT*SHADOW_WIDTH * 4);
      ////for (int i = 0; i < 6; i++) {
      ////      std::ofstream image_block(std::to_string(i) + ".bin");
      ////      glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, GL_FLOAT, block);
      ////      image_block.write(block, SHADOW_HEIGHT*SHADOW_WIDTH * 4);
      ////}
      //delete block;
      //glViewport
}