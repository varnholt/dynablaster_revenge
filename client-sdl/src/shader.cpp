#include "shader.h"

#include <SDL3/SDL.h>
#include <vector>

GLuint Shader::compile(GLenum type, const std::string& source)
{
   const GLuint shader = glCreateShader(type);
   const char* src = source.c_str();
   const GLint length = static_cast<GLint>(source.size());
   glShaderSource(shader, 1, &src, &length);
   glCompileShader(shader);

   GLint status = GL_FALSE;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
   if (status == GL_FALSE)
   {
      GLint log_length = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
      std::vector<char> log(static_cast<size_t>(log_length) + 1, '\0');
      glGetShaderInfoLog(shader, log_length, nullptr, log.data());
      SDL_Log("shader compile error: %s", log.data());
      glDeleteShader(shader);
      return 0;
   }

   return shader;
}

bool Shader::load(const std::string& vertex_source, const std::string& fragment_source)
{
   const GLuint vertex_shader = compile(GL_VERTEX_SHADER, vertex_source);
   if (vertex_shader == 0)
      return false;

   const GLuint fragment_shader = compile(GL_FRAGMENT_SHADER, fragment_source);
   if (fragment_shader == 0)
   {
      glDeleteShader(vertex_shader);
      return false;
   }

   _program = glCreateProgram();
   glAttachShader(_program, vertex_shader);
   glAttachShader(_program, fragment_shader);
   glLinkProgram(_program);

   glDeleteShader(vertex_shader);
   glDeleteShader(fragment_shader);

   GLint status = GL_FALSE;
   glGetProgramiv(_program, GL_LINK_STATUS, &status);
   if (status == GL_FALSE)
   {
      GLint log_length = 0;
      glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &log_length);
      std::vector<char> log(static_cast<size_t>(log_length) + 1, '\0');
      glGetProgramInfoLog(_program, log_length, nullptr, log.data());
      SDL_Log("program link error: %s", log.data());
      glDeleteProgram(_program);
      _program = 0;
      return false;
   }

   return true;
}

Shader::~Shader()
{
   if (_program != 0)
      glDeleteProgram(_program);
}

void Shader::use() const
{
   glUseProgram(_program);
}

GLint Shader::uniformLocation(const std::string& name) const
{
   return glGetUniformLocation(_program, name.c_str());
}

GLint Shader::attribLocation(const std::string& name) const
{
   return glGetAttribLocation(_program, name.c_str());
}
