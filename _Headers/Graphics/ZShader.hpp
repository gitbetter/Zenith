//
//  ZShader.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZLight.hpp"

// Forward Declarations
class ZMaterial;

// Class and Data Structure Definitions
class ZShader {
private:
  unsigned int id_;

  std::string GetShaderCode(const std::string& shaderPath);
  int CompileShader(const std::string& shaderCode, ZShaderTypes shaderType);
  unsigned int CreateProgram(int vShader, int pShader, int gShader);
  void CheckCompileErrors(unsigned int compilationUnit, ZShaderTypes shaderType);

public:
  ZShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath = "");
  ~ZShader() { }

  void Activate();
  unsigned int GetID() const { return id_; }
  // Helpers for setting uniforms
  void SetBool(const std::string& name, bool value) const;
  void SetInt(const std::string& name, int value) const;
  void SetFloat(const std::string& name, float value) const;
  void SetVec2(const std::string& name, const glm::vec2& value) const;
  void SetVec2(const std::string& name, float x, float y) const;
  void SetVec3(const std::string& name, const glm::vec3& value) const;
  void SetVec3(const std::string& name, float x, float y, float z) const;
  void SetVec4(const std::string& name, const glm::vec4& value) const;
  void SetVec4(const std::string& name, float x, float y, float z, float w) const;
  void SetMat2(const std::string& name, const glm::mat2& value) const;
  void SetMat3(const std::string& name, const glm::mat3& value) const;
  void SetMat4(const std::string& name, const glm::mat4& value) const;

  void Use(const ZMaterial& material);
  void Use(const std::map<std::string, ZLight*>&& lights);
protected:

};
