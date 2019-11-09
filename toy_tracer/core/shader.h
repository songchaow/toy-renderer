#pragma once
#include <string>
#include <QOpenGLFunctions_4_0_Core>
#include <core/transform.h>

class Shader {
      std::string path;
      std::string vertex_shader_code;
      std::string fragment_shader_code;
      bool _loaded = false;
      unsigned int program_id;
      QOpenGLFunctions_4_0_Core* f = nullptr;
public:
      Shader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLFunctions_4_0_Core* f = nullptr);
      Shader() = default;
      static const size_t maxPointLightNum = 4;
      bool loaded() const { return _loaded; }
      void compile(QOpenGLFunctions_4_0_Core* f);
      void use() const;
      void setUniformF(const std::string& name, const float val);
      void setUniformF(const std::string& name, const float val, const float val2);
      void setUniformF(const std::string& name, const float val, const float val2, const float val3);
      void setUniformF(const std::string& name, const float val, const float val2, const float val3, const float val4);
      void setUniformF(const std::string& name, const Matrix4* data);
      void setUniformF(unsigned int loc, const float val);
      void setUniformF(unsigned int loc, const float val, const float val2);
      void setUniformF(unsigned int loc, const float val, const float val2, const float val3);
      void setUniformF(unsigned int loc, const float val, const float val2, const float val3, const float val4);
      void setUniformI(const std::string& name, const int val);
      void setUniformI(unsigned int loc, const int val);
      unsigned int getUniformLocation(const std::string& name) { return f->glGetUniformLocation(program_id, name.c_str()); }

};

Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLFunctions_4_0_Core* f);