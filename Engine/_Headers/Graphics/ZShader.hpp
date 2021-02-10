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
#include "ZLight.hpp"

// Forward Declarations
class ZMaterial;
class ZResourceLoadedEvent;

// Class and Data Structure Definitions
class ZShader : public std::enable_shared_from_this<ZShader>
{

private:

    unsigned int id_;
    std::string name_;

    std::string vertexShaderPath_;
    std::string pixelShaderPath_;
    std::string geometryShaderPath_;

    std::string vertexShaderCode_;
    std::string pixelShaderCode_;
    std::string geometryShaderCode_;

    unsigned short loadedShadersMask_;

    std::string GetShaderCode(const std::string& shaderPath, ZShaderType shaderType, bool async = false);

    void PreProcess();
    void ProcessIncludes(std::string& shaderCode);

    void Compile();
    int CompileShader(const std::string& shaderCode, ZShaderType shaderType);
    void CheckCompileErrors(unsigned int compilationUnit, ZShaderType shaderType, const std::string& shaderSource);

    unsigned int CreateProgram(int vShader, int pShader, int gShader);

    void HandleShaderCodeLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);

public:

    ZShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath = "");
    ~ZShader() {}

    void Initialize();
    void InitializeAsync();

    unsigned int ID() const { return id_; }
    const std::string& Name() const { return name_; }

    void Activate();

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

    void Use(const std::shared_ptr<ZMaterial>& material);
    void Use(const ZLightMap& lights);
    void Use(const ZBoneList& bones);

    static void CreateAsync(std::shared_ptr<ZOFTree> data, ZShaderIDMap& outPendingShaders);
    static void Create(std::shared_ptr<ZOFTree> data, ZShaderMap& outShaderMap);
    static std::shared_ptr<ZShader> Create(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath = "");

protected:

    static ZIDSequence idGenerator_;
};
