#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string & vertex_path, const std::string & fragment_path, QOpenGLFunctions_4_0_Core* f)
{
      std::ifstream vertex_file, fragment_file;
      vertex_file.exceptions(std::ifstream::failbit);
      fragment_file.exceptions(std::ifstream::failbit);
      try {
            std::stringstream vertex_code, fragment_code;
            if (vertex_path.size() > 0) {
                  vertex_file.open(vertex_path);
                  vertex_code << vertex_file.rdbuf();
                  vertex_shader_code = vertex_code.str();
            }
            if (fragment_path.size() > 0) {
                  fragment_file.open(fragment_path);
                  fragment_code << fragment_file.rdbuf();
                  fragment_shader_code = fragment_code.str();
            }
            
      }
      catch (std::ifstream::failure) {
            std::cout << "Read shader files failed." << std::endl;
            return;
      }
      path = vertex_path + fragment_path;
      // compile
      if (f) {
            compileAndLink(f);
      }
      this->f = f;
}

void Shader::compile(QOpenGLFunctions_4_0_Core* f) {
      this->f = f;
      char infoLog[1024];
      const char* v_shader_str_ptr = vertex_shader_code.c_str();
      const char* f_shader_str_ptr = fragment_shader_code.c_str();
      GLenum err;
      err = f->glGetError();
      vertex_shader_id = f->glCreateShader(GL_VERTEX_SHADER);
      err = f->glGetError();
      fragment_shader_id = f->glCreateShader(GL_FRAGMENT_SHADER);
      f->glShaderSource(vertex_shader_id, 1, &v_shader_str_ptr, NULL);
      f->glShaderSource(fragment_shader_id, 1, &f_shader_str_ptr, NULL);

      f->glCompileShader(vertex_shader_id);
      int success = 0;
      int len = 0;
      f->glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
      if (!success) {
            f->glGetShaderInfoLog(vertex_shader_id, 512, &len, infoLog);
            infoLog[len] = 0;
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
      }
      if (fragment_shader_code.size() > 0) {
            f->glCompileShader(fragment_shader_id);
            f->glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
            if (!success) {
                  f->glGetShaderInfoLog(fragment_shader_id, 1024, &len, infoLog);
                  infoLog[len] = 0;
                  std::cout << "ERROR::SHADER::fragment_shader_id::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
      }
            
}

void Shader::link(QOpenGLFunctions_4_0_Core* f) {
      int success = 0;
      char infoLog[1024];
      program_id = f->glCreateProgram();
      f->glAttachShader(program_id, vertex_shader_id);
      if (fragment_shader_code.size() > 0)
            f->glAttachShader(program_id, fragment_shader_id);
      f->glLinkProgram(program_id);
      f->glGetProgramiv(program_id, GL_LINK_STATUS, &success);
      if (!success) {
            f->glGetProgramInfoLog(program_id, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      }
}

void Shader::compileAndLink(QOpenGLFunctions_4_0_Core* f) {
      this->f = f;
      char infoLog[1024];
      const char* v_shader_str_ptr = vertex_shader_code.c_str();
      const char* f_shader_str_ptr = fragment_shader_code.c_str();
      GLenum err;
      err = f->glGetError();
      vertex_shader_id = f->glCreateShader(GL_VERTEX_SHADER);
      err = f->glGetError();
      fragment_shader_id = f->glCreateShader(GL_FRAGMENT_SHADER);
      f->glShaderSource(vertex_shader_id, 1, &v_shader_str_ptr, NULL);
      f->glShaderSource(fragment_shader_id, 1, &f_shader_str_ptr, NULL);

      f->glCompileShader(vertex_shader_id);
      int success = 0;
      int len = 0;
      f->glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
      if (!success) {
            f->glGetShaderInfoLog(vertex_shader_id, 512, &len, infoLog);
            infoLog[len] = 0;
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
      }
      if (fragment_shader_code.size() > 0) {
            f->glCompileShader(fragment_shader_id);
            f->glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
            if (!success) {
                  f->glGetShaderInfoLog(fragment_shader_id, 1024, &len, infoLog);
                  infoLog[len] = 0;
                  std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
      }
      
      // link
      program_id = f->glCreateProgram();
      f->glAttachShader(program_id, vertex_shader_id);
      if (fragment_shader_code.size() > 0)
            f->glAttachShader(program_id, fragment_shader_id);
      if (!feedback_vars.empty()) {
            std::vector<const GLchar*> ptr_char_array;
            for (auto& v : feedback_vars) {
                  ptr_char_array.push_back(v.c_str());
            }
            f->glTransformFeedbackVaryings(program_id, feedback_vars.size(), ptr_char_array.data(), feedback_buffmode);
            GLenum res = f->glGetError();
      }
      f->glLinkProgram(program_id);
      f->glGetProgramiv(program_id, GL_LINK_STATUS, &success);
      if (!success) {
            f->glGetProgramInfoLog(program_id, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      }

      // delete shader objects
      f->glDeleteShader(vertex_shader_id);
      f->glDeleteShader(fragment_shader_id);
      _loaded = true;
}

void Shader::use() const { f->glUseProgram(program_id); }

void Shader::setUniformF(const std::string & name, const float val)
{
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniform1f(loc, val);
}

void Shader::setUniformF(const std::string & name, const float val, const float val2)
{
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniform2f(loc, val, val2);
}

void Shader::setUniformF(const std::string & name, const float val, const float val2, const float val3)
{
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniform3f(loc, val, val2, val3);
}

void Shader::setUniformF(const std::string & name, const float val, const float val2, const float val3, const float val4)
{
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniform4f(loc, val, val2, val3, val4);
}

void Shader::setUniformF(const std::string& name, const Vector3f& vec3) {
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniform3f(loc, vec3[0], vec3[1], vec3[2]);
}

void Shader::setUniformF(const std::string& name, const Matrix4* data) {
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniformMatrix4fv(loc, 1, GL_TRUE, (const GLfloat*)data);
}

void Shader::setUniformBool(const std::string & name, bool val)
{
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniform1i(loc, val);
}

void Shader::setUniformF(unsigned int loc, const float val)
{
      f->glUniform1f(loc, val);
}

void Shader::setUniformBool(unsigned int loc, bool val) {
      f->glUniform1i(loc, val);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2)
{
      f->glUniform2f(loc, val, val2);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2, const float val3)
{
      f->glUniform3f(loc, val, val2, val3);
}

void Shader::setUniformF(unsigned int loc, const Vector3f& vec3) {
      f->glUniform3f(loc, vec3[0], vec3[1], vec3[2]);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2, const float val3, const float val4)
{
      f->glUniform4f(loc, val, val2, val3, val4);
}

void Shader::setUniformI(const std::string& name, const int val)
{
      const char* test = name.c_str();
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniform1i(loc, val);
}

static std::map<std::string, Shader> shaderStore;

Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLFunctions_4_0_Core* f) {
      std::string id = vertex_path + fragment_path;
      if (shaderStore.find(id) != shaderStore.end()) {
            DLOG(INFO) << "Shader already exists";
            return &shaderStore[id];
      }
      else {
            Shader s(vertex_path, fragment_path, f);
            if (!f || s.loaded()) {
                  shaderStore[id] = s;
                  return &shaderStore[id];
            }
            else {
                  LOG(ERROR) << "Shader load failed.";
                  return nullptr;
            }
      }
}