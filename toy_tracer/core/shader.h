#pragma once
#include <string>
#include <glad/glad.h>
#include <core/transform.h>

constexpr unsigned char POINT_LIGHT_MAX_NUM = 4;

struct ShaderPath {
      std::string vertex;
      std::string geometry;
      std::string fragment;
      bool complete() const { return vertex.size() > 0; }
};

enum ShaderType {
      PBR,
      DEPTH_MAP,
      SKY_BOX,
      HDR_TONE_MAP,
      GAUSSIAN_BLUR_H,
      GAUSSIAN_BLUR_V,
      TANGENE_TEST,
      TEXT,
      PBR_INSTANCED,
      DEPTH_MAP_INSTANCED,
      DIFFERENCE_SHADER,
      NUM_SHADER_TYPE,
};

class Shader {
      ShaderPath path;
      std::string vertex_shader_code;
      std::string fragment_shader_code;
      std::string geo_shader_code;
      bool _complete = false;
      bool _loaded = false;
      unsigned int program_id;
      std::vector<std::string> feedback_vars;
      GLenum feedback_buffmode;
public:
      Shader(const std::string& vertex_path, const std::string& fragment_path);
      Shader(const std::string & vertex_path);
      Shader(const ShaderPath & path);
      Shader& operator=(const Shader& i);
      Shader() = default;
      static const size_t maxPointLightNum = 4;
      bool loaded() const { return _loaded; }
      void compileAndLink();
      // called before calling link
      void setTransformFeedback(const std::vector<std::string>& fb_vars, GLenum fb_buffmode = GL_INTERLEAVED_ATTRIBS) {
            feedback_vars = fb_vars;
            feedback_buffmode = fb_buffmode;
      }
      void use();
      bool complete() { return _complete; }
      void setUniformF(const std::string& name, const float val);
      void setUniformF(const std::string& name, const float val, const float val2);
      void setUniformF(const std::string& name, const float val, const float val2, const float val3);
      void setUniformF(const std::string& name, const float val, const float val2, const float val3, const float val4);
      void setUniformF(const std::string& name, const Vector3f& vec3);
      void setUniformF(const std::string& name, const Matrix4* data);
      void setUniformBool(const std::string& name, bool val);
      void setUniformF(unsigned int loc, const float val);
      void setUniformF(unsigned int loc, const float val, const float val2);
      void setUniformF(unsigned int loc, const float val, const float val2, const float val3);
      void setUniformF(unsigned int loc, const Vector3f& vec3);
      void setUniformF(unsigned int loc, const float val, const float val2, const float val3, const float val4);
      void setUniformBool(unsigned int loc, bool val);
      void setUniformI(const std::string& name, const int val);
      void setUniformI(unsigned int loc, const int val);
      void setUniformI(const std::string& name, const int val1, const int val2);
      unsigned int getUniformLocation(const std::string& name) { return glGetUniformLocation(program_id, name.c_str()); }

};

struct PunctualLightLoc {
      uint16_t pos[POINT_LIGHT_MAX_NUM];
      uint16_t irradiance[POINT_LIGHT_MAX_NUM];
      uint16_t spot[POINT_LIGHT_MAX_NUM];
      uint16_t directional[POINT_LIGHT_MAX_NUM];
      uint16_t direction[POINT_LIGHT_MAX_NUM];
      uint16_t cosAngle[POINT_LIGHT_MAX_NUM];
      uint16_t size[POINT_LIGHT_MAX_NUM];
      void queryLocation(Shader* pbrShader) {
            for (int i = 0; i < POINT_LIGHT_MAX_NUM; i++) {
                  pos[i] = pbrShader->getUniformLocation("pointLights[" + std::to_string(i) + "].pos");
                  irradiance[i] = pbrShader->getUniformLocation("pointLights[" + std::to_string(i) + "].irradiance");
                  spot[i] = pbrShader->getUniformLocation("pointLights[" + std::to_string(i) + "].spot");
                  directional[i] = pbrShader->getUniformLocation("pointLights[" + std::to_string(i) + "].directional");
                  direction[i] = pbrShader->getUniformLocation("pointLights[" + std::to_string(i) + "].direction");
                  cosAngle[i] = pbrShader->getUniformLocation("pointLights[" + std::to_string(i) + "].cosAngle");
                  //size[i] = pbrShader->getUniformLocation("pointLights[" + std::to_string(i) + "].size");
            }
      }
};

extern PunctualLightLoc punctualLightLocations_pbr;

// Convenience functions
Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path, bool compile);
Shader* LoadShader(ShaderType t, bool compile);
Shader* GetDefaultShader();