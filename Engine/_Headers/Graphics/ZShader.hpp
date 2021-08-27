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

#include "ZLight.hpp"

class ZMaterial;
class ZResourceLoadedEvent;

using AttachmentsMap = std::unordered_map<std::string, ZHTexture>;

struct ZShader
{
	unsigned int id;
	std::string name;
	std::string vertexShaderPath;
	std::string pixelShaderPath;
	std::string geometryShaderPath;
	std::string vertexShaderCode;
	std::string pixelShaderCode;
	std::string geometryShaderCode;
	unsigned int attachmentIndex{ 0 };
	AttachmentsMap attachments;

    ZShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath = "");

private:

	static ZIDSequence idGenerator_;
};

class ZShaderManager
{

    using ZShaderPool = ZResourcePool<ZShader, ZHShader>;

public:

    ZShaderManager() = default;
    ~ZShaderManager() = default;

    void Initialize() { }
    void CleanUp() { }

    unsigned int ID(const ZHShader& handle);
    const std::string& Name(const ZHShader& handle);
	const AttachmentsMap& Attachments(const ZHShader& handle);
    unsigned int AttachmentIndex(const ZHShader& handle);

    void SetAttachments(const ZHShader& handle, const AttachmentsMap& attachments);
    void SetCode(const ZHShader& handle, const std::string& code, ZShaderType shaderType);

    void Activate(const ZHShader& handle);
    void Validate(const ZHShader& handle);

    void SetBool(const ZHShader& handle, const std::string& name, bool value);
    void SetInt(const ZHShader& handle, const std::string& name, int value);
    void SetFloat(const ZHShader& handle, const std::string& name, float value);
    void SetVec2(const ZHShader& handle, const std::string& name, const glm::vec2& value);
    void SetVec2(const ZHShader& handle, const std::string& name, float x, float y);
    void SetVec3(const ZHShader& handle, const std::string& name, const glm::vec3& value);
    void SetVec3(const ZHShader& handle, const std::string& name, float x, float y, float z);
    void SetVec4(const ZHShader& handle, const std::string& name, const glm::vec4& value);
    void SetVec4(const ZHShader& handle, const std::string& name, float x, float y, float z, float w);
    void SetMat2(const ZHShader& handle, const std::string& name, const glm::mat2& value);
    void SetMat3(const ZHShader& handle, const std::string& name, const glm::mat3& value);
    void SetMat4(const ZHShader& handle, const std::string& name, const glm::mat4& value);
    void SetUBO(const ZHShader& handle, const std::string& name, int index);
    void SetFloatList(const ZHShader& handle, const std::string& name, const std::vector<float>& value);
    void SetMat4List(const ZHShader& handle, const std::string& name, const std::vector<glm::mat4>& value);

    void Use(const ZHShader& handle, const std::shared_ptr<ZMaterial>& material);
    void Use(const ZHShader& handle, const ZLightMap& lights);
    void Use(const ZHShader& handle, const ZBoneList& bones);

    void BindAttachments(const ZHShader& handle);
    void BindAttachment(const ZHShader& handle, const std::string& uniformName, const ZHTexture& attachment);
	void AddAttachment(const ZHShader& handle, const std::string& uniformName, const ZHTexture& attachment);
	void ClearAttachments(const ZHShader& handle);

    void CreateAsync(std::shared_ptr<struct ZOFNode> data);
    void Create(std::shared_ptr<ZOFNode> data, ZShaderMap& outShaderMap);
    ZHShader Create(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath = "", const std::string& name = "");
    void CreateAsync(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath = "", const std::string& name = "");

protected:

	ZShaderPool shaderPool_;
	ZShaderMap loadedShaders_;

    std::unordered_map<std::string, std::vector<ZHShader>> loadedShaderFiles_;
    std::unordered_map<ZHShader, unsigned short> shaderLoadMasks_;

private:

    std::string GetShaderCode(const std::string& shaderPath, ZShaderType shaderType, bool async = false);
    void ProcessIncludes(std::string& shaderCode);

    void Compile(const ZHShader& handle);
    int CompileShader(const std::string& shaderCode, ZShaderType shaderType);
    void CheckCompileErrors(unsigned int compilationUnit, ZShaderType shaderType, const std::string& shaderSource);

    unsigned int CreateProgram(int vShader, int pShader, int gShader);

    void HandleShaderCodeLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);

};
