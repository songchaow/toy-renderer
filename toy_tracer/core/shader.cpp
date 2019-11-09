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
            vertex_file.open(vertex_path);
            fragment_file.open(fragment_path);
            vertex_code << vertex_file.rdbuf();
            fragment_code << fragment_file.rdbuf();
            vertex_shader_code = vertex_code.str();
            fragment_shader_code = fragment_code.str();
      }
      catch (std::ifstream::failure) {
            std::cout << "Read shader files failed." << std::endl;
            return;
      }
      path = vertex_path + fragment_path;
      // compile
      if (f) {
            compile(f);
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
      unsigned int vertex = f->glCreateShader(GL_VERTEX_SHADER);
      err = f->glGetError();
      unsigned int fragment = f->glCreateShader(GL_FRAGMENT_SHADER);
      f->glShaderSource(vertex, 1, &v_shader_str_ptr, NULL);
      f->glShaderSource(fragment, 1, &f_shader_str_ptr, NULL);

      f->glCompileShader(vertex);
      int success = 0;
      int len = 0;
      f->glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
      if (!success) {
            f->glGetShaderInfoLog(vertex, 512, &len, infoLog);
            infoLog[len] = 0;
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
      }
      f->glCompileShader(fragment);
      f->glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
      if (!success) {
            f->glGetShaderInfoLog(fragment, 1024, &len, infoLog);
            infoLog[len] = 0;
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
      }

      // link
      program_id = f->glCreateProgram();
      f->glAttachShader(program_id, vertex);
      f->glAttachShader(program_id, fragment);
      f->glLinkProgram(program_id);
      f->glGetProgramiv(program_id, GL_LINK_STATUS, &success);
      if (!success) {
            f->glGetProgramInfoLog(program_id, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      }

      // delete shader objects
      f->glDeleteShader(vertex);
      f->glDeleteShader(fragment);
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

void Shader::setUniformF(const std::string& name, const Matrix4* data) {
      int loc = f->glGetUniformLocation(program_id, name.c_str());
      f->glUniformMatrix4fv(loc, 1, GL_TRUE, (const GLfloat*)data);
}

void Shader::setUniformF(unsigned int loc, const float val)
{
      f->glUniform1f(loc, val);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2)
{
      f->glUniform2f(loc, val, val2);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2, const float val3)
{
      f->glUniform3f(loc, val, val2, val3);
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
      if (shaderStore.find(id) != shaderStore.end())
            return &shaderStore[id];
      else {
            Shader s(vertex_path, fragment_path, f);
            if (s.loaded()) {
                  shaderStore[id] = s;
                  return &shaderStore[id];
            }
            else return nullptr;
      }
}