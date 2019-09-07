#pragma once
#include <string>
#include <QOpenGLExtraFunctions>
#include <core/transform.h>

class Shader {
      std::string path;
      std::string vertex_shader_code;
      std::string fragment_shader_code;
      bool _loaded = false;
      unsigned int program_id;
      QOpenGLExtraFunctions* f = nullptr;
public:
      Shader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLExtraFunctions* f = nullptr);
      Shader() = default;
      bool loaded() const { return _loaded; }
      void compile(QOpenGLExtraFunctions* f);
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
      unsigned int getUniformLocation(const std::string& name);

};

Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLExtraFunctions* f);