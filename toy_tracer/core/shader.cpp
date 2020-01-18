#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string & vertex_path, const std::string & fragment_path)
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
      _complete = true;
}

Shader::Shader(const std::string & vertex_path) {
      std::ifstream vertex_file;
      vertex_file.exceptions(std::ifstream::failbit);
      try {
            std::stringstream vertex_code;
            vertex_file.open(vertex_path);
            vertex_code << vertex_file.rdbuf();
             vertex_shader_code = vertex_code.str();
      }
      catch (std::ifstream::failure) {
            std::cout << "Read shader files failed." << std::endl;
            return;
      }
      path = vertex_path;
      _complete = true;
}

void Shader::compileAndLink() {
      char infoLog[1024];
      int success = 0;
      int len = 0;
      unsigned int vertex = 0, fragment = 0;
      if(vertex_shader_code.size()>0) {
            const char* v_shader_str_ptr = vertex_shader_code.c_str();
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &v_shader_str_ptr, NULL);
            glCompileShader(vertex);
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success) {
                  glGetShaderInfoLog(vertex, 512, &len, infoLog);
                  infoLog[len] = 0;
                  std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
      }
      else {
            LOG(ERROR) << "Vertex shader is empty.";
            return;
      }
      if(fragment_shader_code.size()>0) {
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            const char* f_shader_str_ptr = fragment_shader_code.c_str();
            glShaderSource(fragment, 1, &f_shader_str_ptr, NULL);
            glCompileShader(fragment);
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if (!success) {
                  glGetShaderInfoLog(fragment, 1024, &len, infoLog);
                  infoLog[len] = 0;
                  std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
      }
      

      // link
      program_id = glCreateProgram();
      glAttachShader(program_id, vertex);
      if(fragment > 0)
            glAttachShader(program_id, fragment);
      // configure transform feedback
      if (!feedback_vars.empty()) {
            std::vector<const GLchar*> ptr_char_array;
            for (auto& v : feedback_vars) {
                  ptr_char_array.push_back(v.c_str());
            }
            glTransformFeedbackVaryings(program_id, feedback_vars.size(), ptr_char_array.data(), feedback_buffmode);
            //GLenum res = f->glGetError();
      }
      glLinkProgram(program_id);
      glGetProgramiv(program_id, GL_LINK_STATUS, &success);
      if (!success) {
            glGetProgramInfoLog(program_id, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      }

      // delete shader objects
      glDeleteShader(vertex);
      glDeleteShader(fragment);
      _loaded = true;
}

void Shader::use() { glUseProgram(program_id); }

void Shader::setUniformF(const std::string & name, const float val)
{
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform1f(loc, val);
}

void Shader::setUniformF(const std::string & name, const float val, const float val2)
{
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform2f(loc, val, val2);
}

void Shader::setUniformF(const std::string & name, const float val, const float val2, const float val3)
{
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform3f(loc, val, val2, val3);
}

void Shader::setUniformF(const std::string & name, const float val, const float val2, const float val3, const float val4)
{
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform4f(loc, val, val2, val3, val4);
}

void Shader::setUniformF(const std::string& name, const Vector3f& vec3) {
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform3f(loc, vec3[0], vec3[1], vec3[2]);
}

void Shader::setUniformF(const std::string& name, const Matrix4* data) {
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniformMatrix4fv(loc, 1, GL_TRUE, (const GLfloat*)data);
}

void Shader::setUniformBool(const std::string & name, bool val)
{
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform1i(loc, val);
}

void Shader::setUniformF(unsigned int loc, const float val)
{
      glUniform1f(loc, val);
}

void Shader::setUniformBool(unsigned int loc, bool val) {
      glUniform1i(loc, val);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2)
{
      glUniform2f(loc, val, val2);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2, const float val3)
{
      glUniform3f(loc, val, val2, val3);
}

void Shader::setUniformF(unsigned int loc, const Vector3f& vec3) {
      glUniform3f(loc, vec3[0], vec3[1], vec3[2]);
}

void Shader::setUniformF(unsigned int loc, const float val, const float val2, const float val3, const float val4)
{
      glUniform4f(loc, val, val2, val3, val4);
}

void Shader::setUniformI(const std::string& name, const int val)
{
      const char* test = name.c_str();
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform1i(loc, val);
}

static std::map<std::string, Shader> shaderStore;

Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path, bool compile) {
      std::string id = vertex_path + fragment_path;
      if (shaderStore.find(id) != shaderStore.end()) {
            DLOG(INFO) << "Shader already exists";
            return &shaderStore[id];
      }
      else {
            Shader s(vertex_path, fragment_path);
            if(!s.complete())
                  return nullptr;
            if(compile)
                  s.compileAndLink();
            if (s.loaded()) {
                  shaderStore[id] = s;
                  return &shaderStore[id];
            }
            else return nullptr;
      }
}