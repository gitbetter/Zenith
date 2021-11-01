/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZShader.cpp

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

#include "ZServices.hpp"
#include "ZShader.hpp"
#include "ZMaterial.hpp"
#include "ZResourceData.hpp"
#include "ZTexture.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZOFTree.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>

// TODO: Make ZShader an interface, and rework this specific implementation as ZGLShader
// which derives from the ZShader interface

ZIDSequence ZShader::idGenerator_;

ZShader::ZShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath /*= ""*/)
    : vertexShaderPath(vertexShaderPath), pixelShaderPath(pixelShaderPath), geometryShaderPath(geomShaderPath)
{
    name = "Shader_" + std::to_string(idGenerator_.Next());
}

void ZShaderManager::Initialize()
{
	pbrShader_ = Create("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/pbr.frag");
	blinnPhongShader_ = Create("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/blinnphong.frag");
	debugShader_ = Create("/Shaders/Vertex/debug.vert", "/Shaders/Pixel/debug.frag");
	shadowShader_ = Create("/Shaders/Vertex/shadow.vert", "/Shaders/Pixel/null.frag");
	depthShader_ = Create("/Shaders/Vertex/depth.vert", "/Shaders/Pixel/null.frag");
	postShader_ = Create("/Shaders/Vertex/postprocess.vert", "/Shaders/Pixel/postprocess.frag");
	uiShader_ = Create("/Shaders/Vertex/ui.vert", "/Shaders/Pixel/ui.frag");
	textShader_ = Create("/Shaders/Vertex/text.vert", "/Shaders/Pixel/text.frag");
}

/**
    Compiles the shader using the individual shader sources.

    @param vertexShaderCode the vertex shader code as a string.
    @param pixelShaderCode the pixel shader code as a string.
    @param geometryShaderCode the geometry shader code as a string.
*/
void ZShaderManager::Compile(const ZHShader& handle)
{
    assert(!handle.IsNull() && "Cannot compile shader with a null shader handle!");

    ZShader* shader = resourcePool_.Get(handle);

    int vShader = CompileShader(shader->vertexShaderCode, ZShaderType::Vertex);
    int pShader = CompileShader(shader->pixelShaderCode, ZShaderType::Pixel);
    int gShader = CompileShader(shader->geometryShaderCode, ZShaderType::Geometry);

    shader->id = CreateProgram(vShader, pShader, gShader);
    Track(handle);

    glDeleteShader(vShader);
    glDeleteShader(pShader);
    if (gShader != -1) glDeleteShader(gShader);
}

/**
    Fetches the shader source into a string and returns it.

    @param shaderPath the path to the shader file.
    @return a string containing the shader source code.
*/
std::string ZShaderManager::GetShaderCode(const std::string& shaderPath, ZShaderType shaderType, bool async)
{
    std::string shaderCode;
    ZResourceType type = ZResourceType::Other;

    switch (shaderType)
    {
    case ZShaderType::Vertex: type = ZResourceType::VertexShader; break;
    case ZShaderType::Pixel: type = ZResourceType::PixelShader; break;
    case ZShaderType::Tesselation: type = ZResourceType::TesselationShader; break;
    case ZShaderType::Geometry: type = ZResourceType::GeometryShader; break;
    default: break;
    }

    if (!shaderPath.empty())
    {
        ZShaderResourceData::ptr shaderResource = std::make_shared<ZShaderResourceData>(shaderPath, type);
        if (async)
        {
            ZServices::ResourceImporter()->GetDataAsync(shaderResource);
        }
        else
        {
            ZServices::ResourceImporter()->GetData(shaderResource.get());
            if (shaderResource->size > 0)
            {               
                shaderCode = std::string(static_cast<char*>(shaderResource->buffer));
            }
        }
    }
    return shaderCode;
}

void ZShaderManager::ProcessIncludes(std::string& shaderCode)
{
    std::vector<size_t> includePositions;
    size_t pos = shaderCode.find("#include", 0);
    while (pos != std::string::npos) {
        includePositions.push_back(pos);
        pos = shaderCode.find("#include", pos + 1);
    }
    std::regex includeRegex("^#include\\s+\"(\\S+)\".*$");
    std::smatch match;
    for (auto it = includePositions.rbegin(); it != includePositions.rend(); it++) {
        size_t start = *it;
        size_t end = shaderCode.find(NEWLINE, *it);
        std::string includeLine = shaderCode.substr(start, (end - start));
        if (std::regex_search(includeLine, match, includeRegex)) {
            std::string includePath = (match[1].str().front() != '/' ? "/" : "") + match[1].str();
            std::string includeContents = GetShaderCode(includePath, ZShaderType::Other);
            ProcessIncludes(includeContents);
            shaderCode = shaderCode.replace(start, (end - start), includeContents);
        }
    }
}

/**
    Compiles the shader using the provided shader code and outputs the shader ID.

    @param shaderCode the shader code to compile.
    @param shaderType the shader type, itself of type ZShaderType.
    @return the unsigned integer id of the newly created shader, or -1 if the shader could not be compiled.
*/
int ZShaderManager::CompileShader(const std::string& shaderCode, ZShaderType shaderType)
{
    if (!shaderCode.empty())
    {
        int shader;
        switch (shaderType)
        {
        case ZShaderType::Vertex:
            shader = glCreateShader(GL_VERTEX_SHADER);
            break;
        case ZShaderType::Pixel:
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case ZShaderType::Geometry:
            shader = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        default: return -1;
        }
        const char* shaderCode_cstr = shaderCode.c_str();
        glShaderSource(shader, 1, &shaderCode_cstr, NULL);
        glCompileShader(shader);
        CheckCompileErrors(shader, shaderType, shaderCode);
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
unsigned int ZShaderManager::CreateProgram(int vShader, int pShader, int gShader)
{
    unsigned int programId = glCreateProgram();
    if (vShader != -1) glAttachShader(programId, vShader);
    if (pShader != -1) glAttachShader(programId, pShader);
    if (gShader != -1) glAttachShader(programId, gShader);
    glLinkProgram(programId);
    CheckCompileErrors(programId, ZShaderType::Other, "Program");
    return programId;
}

/**
    Helper method that checks for compilation errors, given a "compilation unit" id
    and it's type. The method does not return, but instead prints the error message
    if there is one.

    @param compilationUnit the compilation unit id to check against.
    @param shaderType the type of the compilation unit.
*/
void ZShaderManager::CheckCompileErrors(unsigned int compilationUnit, ZShaderType shaderType, const std::string& shaderSource)
{
    GLint success; GLchar infoLog[1024];
    bool isShader = shaderType < ZShaderType::Other;

    if (isShader) { glGetShaderiv(compilationUnit, GL_COMPILE_STATUS, &success); }
    else { glGetProgramiv(compilationUnit, GL_LINK_STATUS, &success); }

    if (!success)
    {
        if (isShader) { glGetShaderInfoLog(compilationUnit, 1024, NULL, infoLog); }
        else { glGetProgramInfoLog(compilationUnit, 1024, NULL, infoLog); }
        LOG("Shader Compilation Error: (" + std::to_string(compilationUnit) + ") " + std::string(infoLog) + NEWLINE + shaderSource, ZSeverity::Error);
    }
}

unsigned int ZShaderManager::ID(const ZHShader& handle)
{
	assert(!handle.IsNull() && "Cannot fetch shader property with a null shader handle!");
	ZShader* shader = resourcePool_.Get(handle);
    return shader->id;
}

const std::string& ZShaderManager::Name(const ZHShader& handle)
{
    assert(!handle.IsNull() && "Cannot fetch shader property with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    return shader->name;
}

const AttachmentsMap& ZShaderManager::Attachments(const ZHShader& handle)
{
    assert(!handle.IsNull() && "Cannot fetch shader property with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    return shader->attachments;
}

unsigned int ZShaderManager::AttachmentIndex(const ZHShader& handle)
{
    assert(!handle.IsNull() && "Cannot fetch shader property with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    return shader->attachmentIndex;
}

void ZShaderManager::Activate(const ZHShader& handle)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUseProgram(shader->id);
}

void ZShaderManager::Validate(const ZHShader& handle)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);

    glValidateProgram(shader->id);
    GLint status;
    glGetProgramiv(shader->id, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        int length = 0;
        char log[1000];
        glGetProgramInfoLog(shader->id, 1000, &length, log);
        LOG(std::string(log), ZSeverity::Info);
    }
}

void ZShaderManager::SetBool(const ZHShader& handle, const std::string& name, bool value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform1i(glGetUniformLocation(shader->id, name.c_str()), (int)value);
}

void ZShaderManager::SetInt(const ZHShader& handle, const std::string& name, int value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform1i(glGetUniformLocation(shader->id, name.c_str()), (int)value);
}

void ZShaderManager::SetFloat(const ZHShader& handle, const std::string& name, float value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform1f(glGetUniformLocation(shader->id, name.c_str()), (float)value);
}

void ZShaderManager::SetVec2(const ZHShader& handle, const std::string& name, const glm::vec2& value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform2fv(glGetUniformLocation(shader->id, name.c_str()), 1, &value[0]);
}

void ZShaderManager::SetVec2(const ZHShader& handle, const std::string& name, float x, float y)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform2f(glGetUniformLocation(shader->id, name.c_str()), x, y);
}

void ZShaderManager::SetVec3(const ZHShader& handle, const std::string& name, const glm::vec3& value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform3fv(glGetUniformLocation(shader->id, name.c_str()), 1, &value[0]);
}

void ZShaderManager::SetVec3(const ZHShader& handle, const std::string& name, float x, float y, float z)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform3f(glGetUniformLocation(shader->id, name.c_str()), x, y, z);
}

void ZShaderManager::SetVec4(const ZHShader& handle, const std::string& name, const glm::vec4& value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform4fv(glGetUniformLocation(shader->id, name.c_str()), 1, &value[0]);
}

void ZShaderManager::SetVec4(const ZHShader& handle, const std::string& name, float x, float y, float z, float w)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniform4f(glGetUniformLocation(shader->id, name.c_str()), x, y, z, w);
}

void ZShaderManager::SetMat2(const ZHShader& handle, const std::string& name, const glm::mat2& value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniformMatrix2fv(glGetUniformLocation(shader->id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShaderManager::SetMat3(const ZHShader& handle, const std::string& name, const glm::mat3& value)
{
	assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
	ZShader* shader = resourcePool_.Get(handle);
    glUniformMatrix3fv(glGetUniformLocation(shader->id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShaderManager::SetMat4(const ZHShader& handle, const std::string& name, const glm::mat4& value)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShaderManager::SetUBO(const ZHShader& handle, const std::string& name, int index)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    glUniformBlockBinding(shader->id, glGetUniformBlockIndex(shader->id, name.c_str()), index);
}

void ZShaderManager::SetFloatList(const ZHShader& handle, const std::string& name, const std::vector<float>& value)
{
    for (int i = 0; i < value.size(); i++) {
        SetFloat(handle, name + "[" + std::to_string(i) + "]", value[i]);
    }
}

void ZShaderManager::SetMat4List(const ZHShader& handle, const std::string& name, const std::vector<glm::mat4>& value)
{
    for (int i = 0; i < value.size(); i++) {
        SetMat4(handle, name + "[" + std::to_string(i) + "]", value[i]);
    }
}

void ZShaderManager::Use(const ZHShader& handle, const ZHMaterial& material)
{
    Activate(handle);

    SetBool(handle, "isTextured", ZServices::MaterialManager()->IsTextured(material));
    SetBool(handle, "hasDisplacement", ZServices::MaterialManager()->HasDisplacement(material));
    // TODO: Move this elsewhere for reconfigurability
    SetFloat(handle, "heightScale", 0.01f);

    std::string shaderMaterial = "material";
    SetVec4(handle, shaderMaterial + ".albedo", ZServices::MaterialManager()->Properties(material).albedo);
	SetFloat(handle, shaderMaterial + ".metallic", ZServices::MaterialManager()->Properties(material).metallic);
	SetFloat(handle, shaderMaterial + ".roughness", ZServices::MaterialManager()->Properties(material).roughness);
	SetFloat(handle, shaderMaterial + ".ao", ZServices::MaterialManager()->Properties(material).ao);
	SetFloat(handle, shaderMaterial + ".emission", ZServices::MaterialManager()->Properties(material).emission);
	SetFloat(handle, shaderMaterial + ".diffuse", ZServices::MaterialManager()->Properties(material).diffuse);
	SetFloat(handle, shaderMaterial + ".ambient", ZServices::MaterialManager()->Properties(material).ambient);
	SetFloat(handle, shaderMaterial + ".specular", ZServices::MaterialManager()->Properties(material).specular);
	SetFloat(handle, shaderMaterial + ".shininess", ZServices::MaterialManager()->Properties(material).shininess);

    for (auto const& texture : ZServices::MaterialManager()->Textures(material))
    {
        BindAttachment(handle, ZServices::TextureManager()->Name(texture), texture);
    }
}

void ZShaderManager::Use(const ZHShader& handle, const ZLightMap& lights)
{
    Activate(handle);

    unsigned int i = 0;
    for (auto it = lights.begin(); it != lights.end(); it++)
    {
        ZHGameObject light = it->second;
        ZLight* lightObj = ZServices::GameObjectManager()->Dereference<ZLight>(light);
        std::string shaderLight = "light";
        SetInt(handle, shaderLight + ".lightType", static_cast<unsigned int>(lightObj->lightType));
        SetBool(handle, shaderLight + ".isEnabled", lightObj->lightProperties.isEnabled);
        SetVec3(handle, shaderLight + ".ambient", lightObj->lightProperties.ambient);
        SetVec3(handle, shaderLight + ".color", lightObj->lightProperties.color);
        SetVec3(handle, shaderLight + ".position", ZServices::GameObjectManager()->Position(light));

        switch (lightObj->lightType)
        {
        case ZLightType::Directional:
            SetVec3(handle, shaderLight + ".direction", glm::eulerAngles(ZServices::GameObjectManager()->Orientation(light)));
            break;
        case ZLightType::Point:
            SetFloat(handle, shaderLight + ".constantAttenuation", lightObj->lightProperties.constantAttenuation);
            SetFloat(handle, shaderLight + ".linearAttenuation", lightObj->lightProperties.linearAttenuation);
            SetFloat(handle, shaderLight + ".quadraticAttenuation", lightObj->lightProperties.quadraticAttenuation);
            break;
        case ZLightType::Spot:
            SetFloat(handle, shaderLight + ".constantAttenuation", lightObj->lightProperties.constantAttenuation);
            SetFloat(handle, shaderLight + ".linearAttenuation", lightObj->lightProperties.linearAttenuation);
            SetFloat(handle, shaderLight + ".quadraticAttenuation", lightObj->lightProperties.quadraticAttenuation);
            SetVec3(handle, shaderLight + ".coneDirection", lightObj->lightProperties.coneDirection);
            SetFloat(handle, shaderLight + ".cutoff", lightObj->lightProperties.spotCutoff);
            SetFloat(handle, shaderLight + ".exponent", lightObj->lightProperties.spotExponent);
            break;
        default: break;
        }
        ++i;
    }
}

void ZShaderManager::Use(const ZHShader& handle, const ZBoneList& bones)
{
    Activate(handle);
    ZBone bone;
    for (unsigned int i = 0; i < BONES_PER_MODEL; i++)
    {
        bone = bones[i];
        SetMat4(handle, "Bones[" + std::to_string(i) + "]", bone.transformation);
    }
}

void ZShaderManager::BindAttachments(const ZHShader& handle)
{
	assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
	ZShader* shader = resourcePool_.Get(handle);
    for (const auto& [key, val] : shader->attachments) {
        BindAttachment(handle, key, val);
    }
}

void ZShaderManager::BindAttachment(const ZHShader& handle, const std::string& uniformName, const ZHTexture& attachment)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    shader->attachments[uniformName] = attachment;
    ZServices::TextureManager()->Bind(attachment, shader->attachmentIndex);
    SetInt(handle, uniformName, shader->attachmentIndex);
    ++shader->attachmentIndex;
}

void ZShaderManager::SetAttachments(const ZHShader& handle, const AttachmentsMap& attachments)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    shader->attachments = attachments;
}

void ZShaderManager::SetCode(const ZHShader& handle, const std::string& code, ZShaderType shaderType)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    switch (shaderType)
    {
    case ZShaderType::Vertex:
        shader->vertexShaderCode = code;
        ProcessIncludes(shader->vertexShaderCode);
        break;
    case ZShaderType::Pixel:
        shader->pixelShaderCode = code;
        ProcessIncludes(shader->pixelShaderCode);
        break;
    case ZShaderType::Geometry:
        shader->geometryShaderCode = code;
        ProcessIncludes(shader->geometryShaderCode);
        break;
    default:
        break;
    }
}

void ZShaderManager::AddAttachment(const ZHShader& handle, const std::string& uniformName, const ZHTexture& attachment)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    shader->attachments[uniformName] = attachment;
}

void ZShaderManager::ClearAttachments(const ZHShader& handle)
{
    assert(!handle.IsNull() && "Cannot use shader with a null shader handle!");
    ZShader* shader = resourcePool_.Get(handle);
    for (const auto& [key, val] : shader->attachments) {
        ZServices::TextureManager()->Unbind(val);
    }
    shader->attachments.clear();
    shader->attachmentIndex = 0;
}

ZHShader ZShaderManager::Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
    if (dataNode->type != ZOFObjectType::Shader)
    {
        return ZHShader();
    }

    ZHShader restoreHandle(dataHandle.value);

	std::string vertexPath = "", pixelPath = "", geometryPath = "", name = "";

	if (dataNode->properties.find("name") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["name"]->Value<ZOFString>(0);
		name = prop->value;
	}

	if (dataNode->properties.find("vertex") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["vertex"]->Value<ZOFString>(0);
		vertexPath = prop->value;
	}

	if (dataNode->properties.find("pixel") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["pixel"]->Value<ZOFString>(0);
        pixelPath = prop->value;
	}

	if (dataNode->properties.find("geometry") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["geometry"]->Value<ZOFString>(0);
        geometryPath = prop->value;
	}

	return Create(vertexPath, pixelPath, geometryPath, name, restoreHandle);
}

void ZShaderManager::DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
	if (dataNode->type != ZOFObjectType::Shader)
	{
		return;
	}

	ZHShader restoreHandle(dataHandle.value);

	std::string vertexPath = "", pixelPath = "", geometryPath = "", name = "";

	if (dataNode->properties.find("name") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["name"]->Value<ZOFString>(0);
		name = prop->value;
	}

	if (dataNode->properties.find("vertex") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["vertex"]->Value<ZOFString>(0);
		vertexPath = prop->value;
	}

	if (dataNode->properties.find("pixel") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["pixel"]->Value<ZOFString>(0);
		pixelPath = prop->value;
	}

	if (dataNode->properties.find("geometry") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["geometry"]->Value<ZOFString>(0);
		geometryPath = prop->value;
	}

	CreateAsync(vertexPath, pixelPath, geometryPath, name, restoreHandle);
}

ZHShader ZShaderManager::Create(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath, const std::string& name, const ZHShader& restoreHandle)
{
    ZHShader handle(restoreHandle);
    ZShader* shader = nullptr;

    shader = resourcePool_.New(handle, vertexShaderPath, pixelShaderPath, geomShaderPath);

    shader->name = !name.empty() ? name : shader->name;

	SetCode(handle, GetShaderCode(vertexShaderPath, ZShaderType::Vertex), ZShaderType::Vertex);
    SetCode(handle, GetShaderCode(pixelShaderPath, ZShaderType::Pixel), ZShaderType::Pixel);
    SetCode(handle, GetShaderCode(geomShaderPath, ZShaderType::Geometry), ZShaderType::Geometry);

	Compile(handle);

    return handle;
}

void ZShaderManager::CreateAsync(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath, const std::string& name, const ZHShader& restoreHandle)
{
	ZHShader handle(restoreHandle);
	ZShader* shader = nullptr;

    shader = resourcePool_.New(handle, vertexShaderPath, pixelShaderPath, geomShaderPath);

    shader->name = !name.empty() ? name : shader->name;

    loadedShaderFiles_[vertexShaderPath].push_back(handle);
    loadedShaderFiles_[pixelShaderPath].push_back(handle);
    loadedShaderFiles_[geomShaderPath].push_back(handle);
    shaderLoadMasks_[handle.Handle()] = 0;

	GetShaderCode(vertexShaderPath, ZShaderType::Vertex, true);
	GetShaderCode(pixelShaderPath, ZShaderType::Pixel, true);
	GetShaderCode(geomShaderPath, ZShaderType::Geometry, true);

	ZServices::EventAgent()->Subscribe(this, &ZShaderManager::HandleShaderCodeLoaded);
}

void ZShaderManager::HandleShaderCodeLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (event->Resource() == nullptr)
    {
        return;
    }

    std::shared_ptr<ZShaderResourceData> shaderResource = std::static_pointer_cast<ZShaderResourceData>(event->Resource());

	if (loadedShaderFiles_.find(shaderResource->path) != loadedShaderFiles_.end())
	{
		for (auto it = loadedShaderFiles_[shaderResource->path].begin(); it != loadedShaderFiles_[shaderResource->path].end(); ++it)
		{
            ZHShader handle = *it;
            unsigned int mask = 1 << static_cast<unsigned short>(shaderResource->type);
			if ((shaderLoadMasks_[handle.Handle()] & mask) != 0)
			{
				shaderLoadMasks_[handle.Handle()] |= mask;
				SetCode(handle, shaderResource->code, shaderResource->type);
			}

			if (shaderLoadMasks_[handle.Handle()] == 3 || shaderLoadMasks_[handle.Handle()] == 7)
			{
				Compile(handle);

                shaderLoadMasks_.erase(handle.Handle());
                it = loadedShaderFiles_[shaderResource->path].erase(it);

				std::shared_ptr<ZShaderReadyEvent> shaderReadyEvent = std::make_shared<ZShaderReadyEvent>(handle);
				ZServices::EventAgent()->Queue(shaderReadyEvent);
			}
		}

        if (loadedShaderFiles_[shaderResource->path].empty())
        {
            loadedShaderFiles_.erase(shaderResource->path);
        }
	}
}
