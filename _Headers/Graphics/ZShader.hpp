/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZShader.hpp

    Created by Adrian Sanchez on 26/01/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

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
  void CheckCompileErrors(unsigned int compilationUnit, ZShaderTypes shaderType, const std::string& shaderSource);

public:

  ZShader() { }
  ~ZShader() { }

  void Initialize(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath = "");

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

  void Use(ZMaterial* material);
  void Use(const ZLightMap& lights);
  void Use(const ZBoneMap& bones);
protected:


};
