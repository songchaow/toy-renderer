#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
      // compile
      char infoLog[1024];
      const char* v_shader_str_ptr = vertex_shader_code.c_str();
      const char* f_shader_str_ptr = fragment_shader_code.c_str();
      unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
      unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(vertex, 1, &v_shader_str_ptr, NULL);
      glShaderSource(fragment, 1, &f_shader_str_ptr, NULL);

      glCompileShader(vertex);
      int success = 0;
      glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
      if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
      }
      glCompileShader(fragment);
      glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
      if (!success) {
            glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
      }

      // link
      program_id = glCreateProgram();
      glAttachShader(program_id, vertex);
      glAttachShader(program_id, fragment);
      glLinkProgram(program_id);
      glGetProgramiv(program_id, GL_LINK_STATUS, &success);
      if (!success) {
            glGetProgramInfoLog(program_id, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      }

      // delete shader objects
      glDeleteShader(vertex);
      glDeleteShader(fragment);
}

void Shader::use() const { glUseProgram(program_id); }

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

void Shader::setUniformF(unsigned int loc, const float val)
{
}

void Shader::setUniformI(const std::string& name, const int val)
{
      const char* test = name.c_str();
      int loc = glGetUniformLocation(program_id, name.c_str());
      glUniform1i(loc, val);
}
