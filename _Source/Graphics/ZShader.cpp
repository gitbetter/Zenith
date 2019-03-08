//
//  ZShader.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ZShader.hpp"
#include "ZCommon.hpp"
#include "ZMaterial.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <glm/gtx/string_cast.hpp>

// TODO: Make ZShader an interface, and rework this specific implementation as ZGLShader
// which derives from the ZShader interface

void ZShader::Initialize(const std::string& vShaderPath, const std::string& pShaderPath, const std::string& gShaderPath) {
  // Get the shader sources
  std::string vShaderCode = GetShaderCode(vShaderPath);
  std::string pShaderCode = GetShaderCode(pShaderPath);
  std::string gShaderCode = GetShaderCode(gShaderCode);

  // Compile the shaders
  int vShader = CompileShader(vShaderCode, ZShaderTypes::Vertex);
  int pShader = CompileShader(pShaderCode, ZShaderTypes::Pixel);
  int gShader = CompileShader(gShaderCode, ZShaderTypes::Geometry);

  // Create the shader program and link the shaders
  id_ = CreateProgram(vShader, pShader, gShader);

  // Clean up. Easy peazy.
  glDeleteShader(vShader);
  glDeleteShader(pShader);
  if (gShader != -1) glDeleteShader(gShader);
}

/**
    Fetches the shader source into a string and returns it.

    @param shaderPath the path to the shader file.
    @return a string containing the shader source code.
*/
std::string ZShader::GetShaderCode(const std::string& shaderPath) {
  std::string shaderCode;
  if (!shaderPath.empty()) {
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      // Open shader file
      shaderFile.open(shaderPath);
      // Read the contents out into a string stream
      std::stringstream shaderStream, pShaderStream;
      shaderStream << shaderFile.rdbuf();
      // Close the shader file, we have the content
      shaderFile.close();
      // Convert the stream object to a string
      shaderCode = shaderStream.str();
    } catch (std::ifstream::failure e) {
      _Z("There was an error reading a shader file. Consult the code.", ZERROR);
    }
  }
  return shaderCode;
}

/**
    Compiles the shader using the provided shader code and outputs the shader ID.

    @param shaderCode the shader code to compile.
    @param shaderType the shader type, itself of type ZShaderTypes.
    @return the unsigned integer id of the newly created shader, or -1 if the shader could not be compiled.
*/
int ZShader::CompileShader(const std::string& shaderCode, ZShaderTypes shaderType) {
  if (!shaderCode.empty()) {
    int shader;
    switch (shaderType) {
      case ZShaderTypes::Vertex:
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
      case ZShaderTypes::Pixel:
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
      case ZShaderTypes::Geometry:
        shader = glCreateShader(GL_GEOMETRY_SHADER);
        break;
      default: return -1; // The shader is not supported, so return immediately
    }
    const char* shaderCode_cstr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderCode_cstr, NULL);
    glCompileShader(shader);
    CheckCompileErrors(shader, shaderType);
    return shader;
  }
  return -1;
}

/**
    Creates the OpenGL program, links the shaders and outputs the program ID.

    @param vShader the vertex shader handle to link.
    @param pShader the pixel shader handle to link.
    @param gShader the geometry shader handle to link.
    @return the unsigned integer id of the OpenGL created program.
*/
unsigned int ZShader::CreateProgram(int vShader, int pShader, int gShader) {
  unsigned int programId = glCreateProgram();
  if (vShader != -1) glAttachShader(programId, vShader);
  if (pShader != -1) glAttachShader(programId, pShader);
  if (gShader != -1) glAttachShader(programId, gShader);
  glLinkProgram(programId);
  CheckCompileErrors(programId, ZShaderTypes::Other);
  return programId;
}

/**
    Helper method that checks for compilation errors, given a "compilation unit" id
    and it's type. The method does not return, but instead prints the error message
    if there is one.

    @param compilationUnit the compilation unit id to check against.
    @param shaderType the type of the compilation unit.
*/
void ZShader::CheckCompileErrors(unsigned int compilationUnit, ZShaderTypes shaderType) {
    GLint success; GLchar infoLog[1024];
    bool isShader = shaderType < ZShaderTypes::Other;

    if (isShader) { glGetShaderiv(compilationUnit, GL_COMPILE_STATUS, &success); }
    else { glGetProgramiv(compilationUnit, GL_LINK_STATUS, &success); }

    if(!success) {
        if (isShader) { glGetShaderInfoLog(compilationUnit, 1024, NULL, infoLog); }
        else { glGetProgramInfoLog(compilationUnit, 1024, NULL, infoLog); }
        _Z("Shader Compilation Error: " + std::string(infoLog), ZERROR);
    }
}

void ZShader::Activate() {
  glUseProgram(id_);
}

void ZShader::SetBool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}

void ZShader::SetInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}

void ZShader::SetFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), (float)value);
}

void ZShader::SetVec2(const std::string& name, const glm::vec2& value) const {
  glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void ZShader::SetVec2(const std::string& name, float x, float y) const {
  glUniform2f(glGetUniformLocation(id_, name.c_str()), x, y);
}

void ZShader::SetVec3(const std::string& name, const glm::vec3& value) const {
  glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void ZShader::SetVec3(const std::string& name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(id_, name.c_str()), x, y, z);
}

void ZShader::SetVec4(const std::string& name, const glm::vec4& value) const {
  glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void ZShader::SetVec4(const std::string& name, float x, float y, float z, float w) const {
  glUniform4f(glGetUniformLocation(id_, name.c_str()), x, y, z, w);
}

void ZShader::SetMat2(const std::string& name, const glm::mat2& value) const {
  glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShader::SetMat3(const std::string& name, const glm::mat3& value) const {
  glUniformMatrix3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShader::SetMat4(const std::string& name, const glm::mat4& value) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShader::Use(ZMaterial* material) {
  Activate();

  SetInt("materialIndex", material->Index());
  SetBool("isTextured", material->IsTextured());
  SetVec4("materials[" + std::to_string(material->Index()) + "].albedo", material->Properties().albedo);
  if (material->IsPBR()) {
    SetFloat("materials[" + std::to_string(material->Index()) + "].metallic", material->Properties().metallic);
    SetFloat("materials[" + std::to_string(material->Index()) + "].roughness", material->Properties().roughness);
    SetFloat("materials[" + std::to_string(material->Index()) + "].ao", material->Properties().ao);
  } else {
    SetFloat("materials[" + std::to_string(material->Index()) + "].emission", material->Properties().emission);
    SetFloat("materials[" + std::to_string(material->Index()) + "].diffuse", material->Properties().diffuse);
    SetFloat("materials[" + std::to_string(material->Index()) + "].ambient", material->Properties().ambient);
    SetFloat("materials[" + std::to_string(material->Index()) + "].specular", material->Properties().specular);
    SetFloat("materials[" + std::to_string(material->Index()) + "].shininess", material->Properties().shininess);
  }

  // We start the external texture indices at 2 due to the depth and PBR irradiance maps, which are set internally
  // and should not be overriden
  for (unsigned int i = 0; i < material->Textures().size(); i++) {
    SetInt(material->Textures()[i].type, i+3);
    ZEngine::Graphics()->Strategy()->BindTexture(material->Textures()[i], i+3);
  }
}

void ZShader::Use(const std::map<std::string, std::shared_ptr<ZLight>>& lights) {
  Activate();
  unsigned int i = 0;
  for (auto it = lights.begin(); it != lights.end(); it++) {
    std::shared_ptr<ZLight> light = it->second;
    switch(light->type) {
      case ZLightType::Directional:
        SetBool("lights[" + std::to_string(i) + "].isEnabled", light->enabled);
        SetBool("lights[" + std::to_string(i) + "].isDirectional", true);
        SetVec3("lights[" + std::to_string(i) + "].ambient", light->directional.components.ambient);
        SetVec3("lights[" + std::to_string(i) + "].color", light->directional.components.color);
        SetVec3("lights[" + std::to_string(i) + "].direction", light->directional.direction);
        SetVec3("lights[" + std::to_string(i) + "].position", light->Position());
        break;
      case ZLightType::Point:
        SetBool("lights[" + std::to_string(i) + "].isEnabled", light->enabled);
        SetVec3("lights[" + std::to_string(i) + "].ambient", light->point.components.ambient);
        SetVec3("lights[" + std::to_string(i) + "].color", light->point.components.color);
        SetFloat("lights[" + std::to_string(i) + "].constantAttenuation", light->point.attenunation.constant);
        SetFloat("lights[" + std::to_string(i) + "].linearAttenuation", light->point.attenunation.linear);
        SetFloat("lights[" + std::to_string(i) + "].quadraticAttenuation", light->point.attenunation.quadratic);
        SetVec3("lights[" + std::to_string(i) + "].position", light->Position());
        break;
      case ZLightType::Spot:
        SetBool("lights[" + std::to_string(i) + "].isEnabled", light->enabled);
        SetBool("lights[" + std::to_string(i) + "].isSpot", true);
        SetVec3("lights[" + std::to_string(i) + "].ambient", light->spot.components.ambient);
        SetVec3("lights[" + std::to_string(i) + "].color", light->spot.components.color);
        SetFloat("lights[" + std::to_string(i) + "].constantAttenuation", light->spot.attenunation.constant);
        SetFloat("lights[" + std::to_string(i) + "].linearAttenuation", light->spot.attenunation.linear);
        SetFloat("lights[" + std::to_string(i) + "].quadraticAttenuation", light->spot.attenunation.quadratic);
        SetVec3("lights[" + std::to_string(i) + "].coneDirection", light->spot.coneDirection);
        SetFloat("lights[" + std::to_string(i) + "].spotCutoff", light->spot.spotCutoff);
        SetFloat("lights[" + std::to_string(i) + "].spotExponent", light->spot.spotExponent);
        SetVec3("lights[" + std::to_string(i) + "].position", light->Position());
        break;
      case ZLightType::Hemisphere:
        SetVec3("hemisphereLight.position", light->Position());
        SetVec3("hemisphereLight.skyColor", light->hemisphere.skyColor);
        SetVec3("hemisphereLight.groundColor", light->hemisphere.groundColor);
        break;
      default: break;
    }
    ++i;
  }
}
