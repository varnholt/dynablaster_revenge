#pragma once

#include "gles3.h"

#include <string>

/// \brief compiles, links and binds a GLES3 shader program.
/// mirrors the role client/src/engine/materials/*.cpp play in the legacy renderer, minus the
/// fixed-function state they currently poke.
class Shader
{
public:
   Shader() = default;
   ~Shader();

   Shader(const Shader&) = delete;
   Shader& operator=(const Shader&) = delete;

   /// \brief compiles and links a vertex/fragment pair.
   /// \param vertex_source GLSL ES vertex shader source.
   /// \param fragment_source GLSL ES fragment shader source.
   /// \return true when compilation and linking both succeeded.
   bool load(const std::string& vertex_source, const std::string& fragment_source);

   /// \brief binds this program with glUseProgram.
   void use() const;

   /// \brief looks up a uniform's location.
   /// \param name uniform variable name.
   /// \return uniform location, or -1 when not found.
   GLint uniformLocation(const std::string& name) const;

   /// \brief looks up a vertex attribute's location.
   /// \param name attribute variable name.
   /// \return attribute location, or -1 when not found.
   GLint attribLocation(const std::string& name) const;

   /// \brief returns the raw OpenGL program object handle.
   GLuint programId() const
   {
      return _program;
   }

private:
   static GLuint compile(GLenum type, const std::string& source);

   GLuint _program = 0;
};
