#pragma once
#include <string>
#include <QOpenGLFunctions_4_0_Core>
#include <core/transform.h>

class Shader {
      std::string path;
      std::string vertex_shader_code;
      std::string fragment_shader_code;
      bool _complete = false;
      bool _loaded = false;
      unsigned int program_id;
      QOpenGLFunctions_4_0_Core* f = nullptr;
      std::vector<std::string> feedback_vars;
      GLenum feedback_buffmode;
public:
      Shader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLFunctions_4_0_Core* f = nullptr);
      Shader(const std::string & vertex_path, QOpenGLFunctions_4_0_Core* f = nullptr);
      Shader() = default;
      static const size_t maxPointLightNum = 4;
      bool loaded() const { return _loaded; }
      void compileAndLink(QOpenGLFunctions_4_0_Core* f);
      // called before calling link
      void setTransformFeedback(const std::vector<std::string>& fb_vars, GLenum fb_buffmode = GL_INTERLEAVED_ATTRIBS) {
            feedback_vars = fb_vars;
            feedback_buffmode = fb_buffmode;
      }
      void use() const;
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
      unsigned int getUniformLocation(const std::string& name) { return f->glGetUniformLocation(program_id, name.c_str()); }

};

// Convenience functions
Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLFunctions_4_0_Core* f);