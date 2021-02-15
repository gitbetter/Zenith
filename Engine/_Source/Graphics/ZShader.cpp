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
#include "ZResource.hpp"
#include "ZSkeleton.hpp"
#include "ZTexture.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZOFTree.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>

// TODO: Make ZShader an interface, and rework this specific implementation as ZGLShader
// which derives from the ZShader interface

ZIDSequence ZShader::idGenerator_("ZSH");

ZShader::ZShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath)
    : vertexShaderPath_(vertexShaderPath), pixelShaderPath_(pixelShaderPath), geometryShaderPath_(geomShaderPath), loadedShadersMask_(0)
{
    name_ = "ZSH_" + idGenerator_.Next();
}

void ZShader::Initialize()
{
    vertexShaderCode_ = GetShaderCode(vertexShaderPath_, ZShaderType::Vertex);
    pixelShaderCode_ = GetShaderCode(pixelShaderPath_, ZShaderType::Pixel);
    geometryShaderCode_ = GetShaderCode(geometryShaderPath_, ZShaderType::Geometry);

    PreProcess();
}

void ZShader::InitializeAsync()
{
    GetShaderCode(vertexShaderPath_, ZShaderType::Vertex, true);
    GetShaderCode(pixelShaderPath_, ZShaderType::Pixel, true);
    GetShaderCode(geometryShaderPath_, ZShaderType::Geometry, true);

    ZServices::EventAgent()->Subscribe(this, &ZShader::HandleShaderCodeLoaded);
}

/**
    Compiles the shader using the individual shader sources.

    @param vertexShaderCode the vertex shader code as a string.
    @param pixelShaderCode the pixel shader code as a string.
    @param geometryShaderCode the geometry shader code as a string.
*/
void ZShader::Compile()
{
    int vShader = CompileShader(vertexShaderCode_, ZShaderType::Vertex);
    int pShader = CompileShader(pixelShaderCode_, ZShaderType::Pixel);
    int gShader = CompileShader(geometryShaderCode_, ZShaderType::Geometry);

    id_ = CreateProgram(vShader, pShader, gShader);

    glDeleteShader(vShader);
    glDeleteShader(pShader);
    if (gShader != -1) glDeleteShader(gShader);
}

/**
    Fetches the shader source into a string and returns it.

    @param shaderPath the path to the shader file.
    @return a string containing the shader source code.
*/
std::string ZShader::GetShaderCode(const std::string& shaderPath, ZShaderType shaderType, bool async)
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
        ZResource shaderResource(shaderPath, type);
        if (async)
        {
            ZServices::ResourceCache()->RequestHandle(shaderResource);
        }
        else
        {
            std::shared_ptr<ZResourceHandle> shaderHandle = ZServices::ResourceCache()->GetHandle(&shaderResource);
            if (shaderHandle)
            {               
                shaderCode = std::string((char*) shaderHandle->Buffer());
            }
        }
    }
    return shaderCode;
}

void ZShader::PreProcess()
{
    ProcessIncludes(vertexShaderCode_);
    ProcessIncludes(geometryShaderCode_);
    ProcessIncludes(pixelShaderCode_);
    Compile();
}

void ZShader::ProcessIncludes(std::string& shaderCode)
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
int ZShader::CompileShader(const std::string& shaderCode, ZShaderType shaderType)
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
unsigned int ZShader::CreateProgram(int vShader, int pShader, int gShader)
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
void ZShader::CheckCompileErrors(unsigned int compilationUnit, ZShaderType shaderType, const std::string& shaderSource)
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

void ZShader::Activate()
{
    glUseProgram(id_);
}

void ZShader::Validate()
{
    glValidateProgram(id_);
    GLint status;
    glGetProgramiv(id_, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        int length = 0;
        char log[1000];
        glGetProgramInfoLog(id_, 1000, &length, log);
        LOG(std::string(log), ZSeverity::Info);
    }
}

void ZShader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int) value);
}

void ZShader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int) value);
}

void ZShader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(id_, name.c_str()), (float) value);
}

void ZShader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void ZShader::SetVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(id_, name.c_str()), x, y);
}

void ZShader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void ZShader::SetVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(id_, name.c_str()), x, y, z);
}

void ZShader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void ZShader::SetVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(id_, name.c_str()), x, y, z, w);
}

void ZShader::SetMat2(const std::string& name, const glm::mat2& value) const
{
    glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShader::SetMat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ZShader::SetFloatList(const std::string& name, const std::vector<float>& value) const
{
    for (int i = 0; i < value.size(); i++) {
        SetFloat(name + "[" + std::to_string(i) + "]", value[i]);
    }
}

void ZShader::SetMat4List(const std::string& name, const std::vector<glm::mat4>& value) const
{
    for (int i = 0; i < value.size(); i++) {
        SetMat4(name + "[" + std::to_string(i) + "]", value[i]);
    }
}

void ZShader::Use(const std::shared_ptr<ZMaterial>& material)
{
    Activate();

    SetInt("materialIndex", material->Index());
    SetBool("isTextured", material->IsTextured());
    SetBool("hasDisplacement", material->HasDisplacement());
    // TODO: Move this elsewhere for configurability
    SetFloat("heightScale", 0.01f);

    std::string shaderMaterial = "materials[" + std::to_string(material->Index()) + "]";
    SetVec4(shaderMaterial + ".albedo", material->Properties().albedo);
    if (material->IsPBR())
    {
        SetFloat(shaderMaterial + ".metallic", material->Properties().metallic);
        SetFloat(shaderMaterial + ".roughness", material->Properties().roughness);
        SetFloat(shaderMaterial + ".ao", material->Properties().ao);
    }
    else
    {
        SetFloat(shaderMaterial + ".emission", material->Properties().emission);
        SetFloat(shaderMaterial + ".diffuse", material->Properties().diffuse);
        SetFloat(shaderMaterial + ".ambient", material->Properties().ambient);
        SetFloat(shaderMaterial + ".specular", material->Properties().specular);
        SetFloat(shaderMaterial + ".shininess", material->Properties().shininess);
    }

    // We start the external texture indices at 6 due to the depth, shadow and PBR irradiance maps, which are set internally
    // and should not be overriden
    unsigned int startIndex = 7;
    for (auto const& [key, val] : material->Textures())
    {
        val->Bind(startIndex);
        SetInt(key, startIndex);
        ++startIndex;
    }
}

void ZShader::Use(const ZLightMap& lights)
{
    Activate();

    SetInt("lightCount", lights.size());

    unsigned int i = 0;
    for (auto it = lights.begin(); it != lights.end(); it++)
    {
        std::shared_ptr<ZLight> light = it->second;
        std::string shaderLight = "lights[" + std::to_string(i) + "]";
        switch (light->type)
        {
        case ZLightType::Directional:
            SetBool(shaderLight + ".isEnabled", light->enabled);
            SetBool(shaderLight + ".isDirectional", true);
            SetVec3(shaderLight + ".ambient", light->ambient);
            SetVec3(shaderLight + ".color", light->color);
            SetVec3(shaderLight + ".direction", glm::eulerAngles(light->Orientation()));
            SetVec3(shaderLight + ".position", light->Position());
            break;
        case ZLightType::Point:
            SetBool(shaderLight + ".isEnabled", light->enabled);
            SetVec3(shaderLight + ".ambient", light->ambient);
            SetVec3(shaderLight + ".color", light->color);
            SetFloat(shaderLight + ".constantAttenuation", light->attenuation.constant);
            SetFloat(shaderLight + ".linearAttenuation", light->attenuation.linear);
            SetFloat(shaderLight + ".quadraticAttenuation", light->attenuation.quadratic);
            SetVec3(shaderLight + ".position", light->Position());
            break;
        case ZLightType::Spot:
            SetBool(shaderLight + ".isEnabled", light->enabled);
            SetBool(shaderLight + ".isSpot", true);
            SetVec3(shaderLight + ".ambient", light->ambient);
            SetVec3(shaderLight + ".color", light->color);
            SetFloat(shaderLight + ".constantAttenuation", light->attenuation.constant);
            SetFloat(shaderLight + ".linearAttenuation", light->attenuation.linear);
            SetFloat(shaderLight + ".quadraticAttenuation", light->attenuation.quadratic);
            SetVec3(shaderLight + ".coneDirection", light->spot.coneDirection);
            SetFloat(shaderLight + ".cutoff", light->spot.cutoff);
            SetFloat(shaderLight + ".exponent", light->spot.exponent);
            SetVec3(shaderLight + ".position", light->Position());
            break;
        default: break;
        }
        ++i;
    }
}

void ZShader::Use(const ZBoneList& bones)
{
    Activate();
    std::shared_ptr<ZBone> bone;
    for (unsigned int i = 0, j = bones.size(); i < j; i++)
    {
        bone = bones[i];
        SetMat4("Bones[" + std::to_string(i) + "]", bone->transformation);
    }
}

void ZShader::HandleShaderCodeLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (!event->Handle()) return;

    std::shared_ptr<ZShaderResourceExtraData> extraData = std::static_pointer_cast<ZShaderResourceExtraData>(event->Handle()->ExtraData());

    switch (event->Handle()->Resource().type)
    {
    case ZResourceType::VertexShader:
        if (event->Handle()->Resource().name == vertexShaderPath_ && (loadedShadersMask_ & 1) == 0)
        {
            loadedShadersMask_ |= 1;
            vertexShaderCode_ = extraData->Code();
        }
        break;
    case ZResourceType::PixelShader:
        if (event->Handle()->Resource().name == pixelShaderPath_ && (loadedShadersMask_ & (1 << 1)) == 0)
        {
            loadedShadersMask_ |= 1 << 1;
            pixelShaderCode_ = extraData->Code();
        }
        break;
    case ZResourceType::GeometryShader:
        if (event->Handle()->Resource().name == geometryShaderPath_ && (loadedShadersMask_ & (1 << 2)) == 0)
        {
            loadedShadersMask_ |= 1 << 2;
            geometryShaderCode_ = extraData->Code();
        }
        break;
    default: break;
    }

    if (loadedShadersMask_ == 3 || loadedShadersMask_ == 7)
    {
        ZServices::EventAgent()->Unsubscribe(this, &ZShader::HandleShaderCodeLoaded);

        PreProcess();

        std::shared_ptr<ZShaderReadyEvent> shaderReadyEvent = std::make_shared<ZShaderReadyEvent>(shared_from_this());
        ZServices::EventAgent()->Queue(shaderReadyEvent);
    }
}

void ZShader::CreateAsync(std::shared_ptr<ZOFTree> data, ZShaderIDMap& outPendingShaders)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZSH") == 0)
        {
            std::string vertexPath = "", pixelPath = "", geometryPath = "";

            std::shared_ptr<ZOFObjectNode> shaderNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++)
            {
                if (!it->second->HasValues()) continue;

                std::shared_ptr<ZOFString> str = it->second->Value<ZOFString>(0);
                if (it->second->id == "vertex") vertexPath = str->value;
                else if (it->second->id == "pixel") pixelPath = str->value;
                else if (it->second->id == "geometry") geometryPath = str->value;
            }

            std::shared_ptr<ZShader> shader(new ZShader(vertexPath, pixelPath, geometryPath));
            shader->name_ = it->first;
            outPendingShaders[shader] = it->first;
        }
    }

    for (auto it = outPendingShaders.begin(); it != outPendingShaders.end(); it++)
    {
        it->first->InitializeAsync();
    }
}

void ZShader::Create(std::shared_ptr<ZOFTree> data, ZShaderMap& outShaderMap)
{
    ZShaderMap shaders;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZSH") == 0)
        {
            std::string vertexPath = "", pixelPath = "", geometryPath = "";

            std::shared_ptr<ZOFObjectNode> shaderNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++)
            {
                if (!it->second->HasValues()) continue;

                std::shared_ptr<ZOFString> str = it->second->Value<ZOFString>(0);
                if (it->second->id == "vertex") vertexPath = str->value;
                else if (it->second->id == "pixel") pixelPath = str->value;
                else if (it->second->id == "geometry") geometryPath = str->value;
            }

            auto shader = ZShader::Create(vertexPath, pixelPath, geometryPath);
            shader->name_ = it->first;
            shaders[it->first] = shader;
        }
    }
    outShaderMap = shaders;
}

std::shared_ptr<ZShader> ZShader::Create(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath)
{
    std::shared_ptr<ZShader> shader = std::make_shared<ZShader>(vertexShaderPath, pixelShaderPath, geomShaderPath);
    shader->Initialize();
    return shader;
}
