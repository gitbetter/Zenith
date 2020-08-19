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

#include "ZShader.hpp"
#include "ZMaterial.hpp"
#include "ZResource.hpp"
#include "ZResourceCache.hpp"
#include "ZSkeleton.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZEventAgent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZShaderReadyEvent.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>

// TODO: Make ZShader an interface, and rework this specific implementation as ZGLShader
// which derives from the ZShader interface

void ZShader::Initialize()
{
    // Get the shader sources
    vertexShaderCode_ = GetShaderCode(vertexShaderPath_, ZShaderType::Vertex);
    pixelShaderCode_ = GetShaderCode(pixelShaderPath_, ZShaderType::Pixel);
    geometryShaderCode_ = GetShaderCode(geometryShaderPath_, ZShaderType::Geometry);

    // Compile the shader from the sources
    Compile();
}

void ZShader::InitializeAsync()
{
    // Get the shader sources
    GetShaderCode(vertexShaderPath_, ZShaderType::Vertex, true);
    GetShaderCode(pixelShaderPath_, ZShaderType::Pixel, true);
    GetShaderCode(geometryShaderPath_, ZShaderType::Geometry, true);

    ZEventDelegate shaderCodeLoadDelegate = fastdelegate::MakeDelegate(this, &ZShader::HandleShaderCodeLoaded);
    zenith::EventAgent()->AddListener(shaderCodeLoadDelegate, ZResourceLoadedEvent::Type);
}

/**
    Compiles the shader using the individual shader sources.

    @param vertexShaderCode the vertex shader code as a string.
    @param pixelShaderCode the pixel shader code as a string.
    @param geometryShaderCode the geometry shader code as a string.
*/
void ZShader::Compile()
{
// Compile the shaders
    int vShader = CompileShader(vertexShaderCode_, ZShaderType::Vertex);
    int pShader = CompileShader(pixelShaderCode_, ZShaderType::Pixel);
    int gShader = CompileShader(geometryShaderCode_, ZShaderType::Geometry);

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
            zenith::ResourceCache()->RequestHandle(shaderResource);
        }
        else
        {
            std::shared_ptr<ZResourceHandle> shaderHandle = zenith::ResourceCache()->GetHandle(&shaderResource);
            if (shaderHandle)
            {               
                shaderCode = std::string((char*) shaderHandle->Buffer());
            }
        }
    }
    return shaderCode;
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
        default: return -1; // The shader is not supported, so return immediately
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
        zenith::Log("Shader Compilation Error: (" + std::to_string(compilationUnit) + ") " + std::string(infoLog) + "\n" + shaderSource, ZSeverity::Error);
    }
}

void ZShader::Activate()
{
    glUseProgram(id_);
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

void ZShader::Use(ZMaterial* material)
{
    Activate();

    SetInt("materialIndex", material->Index());
    SetBool("isTextured", material->IsTextured());
    SetBool("hasDisplacement", material->HasDisplacement());
    // TODO: Move this elsewhere for configurability
    SetFloat("heightScale", 0.05f);
    SetVec4("materials[" + std::to_string(material->Index()) + "].albedo", material->Properties().albedo);
    if (material->IsPBR())
    {
        SetFloat("materials[" + std::to_string(material->Index()) + "].metallic", material->Properties().metallic);
        SetFloat("materials[" + std::to_string(material->Index()) + "].roughness", material->Properties().roughness);
        SetFloat("materials[" + std::to_string(material->Index()) + "].ao", material->Properties().ao);
    }
    else
    {
        SetFloat("materials[" + std::to_string(material->Index()) + "].emission", material->Properties().emission);
        SetFloat("materials[" + std::to_string(material->Index()) + "].diffuse", material->Properties().diffuse);
        SetFloat("materials[" + std::to_string(material->Index()) + "].ambient", material->Properties().ambient);
        SetFloat("materials[" + std::to_string(material->Index()) + "].specular", material->Properties().specular);
        SetFloat("materials[" + std::to_string(material->Index()) + "].shininess", material->Properties().shininess);
    }

    // We start the external texture indices at 4 due to the depth, shadow and PBR irradiance maps, which are set internally
    // and should not be overriden
    for (unsigned int i = 0, j = material->Textures().size(); i < j; i++)
    {
        ZTexture texture = material->Textures()[i];
        SetInt(texture.type, i + 6);
        zenith::Graphics()->Strategy()->BindTexture(texture, i + 6);
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
        switch (light->type)
        {
        case ZLightType::Directional:
            SetBool("lights[" + std::to_string(i) + "].isEnabled", light->enabled);
            SetBool("lights[" + std::to_string(i) + "].isDirectional", true);
            SetVec3("lights[" + std::to_string(i) + "].ambient", light->ambient);
            SetVec3("lights[" + std::to_string(i) + "].color", light->color);
            SetVec3("lights[" + std::to_string(i) + "].direction", glm::eulerAngles(light->Orientation()));
            SetVec3("lights[" + std::to_string(i) + "].position", light->Position());
            break;
        case ZLightType::Point:
            SetBool("lights[" + std::to_string(i) + "].isEnabled", light->enabled);
            SetVec3("lights[" + std::to_string(i) + "].ambient", light->ambient);
            SetVec3("lights[" + std::to_string(i) + "].color", light->color);
            SetFloat("lights[" + std::to_string(i) + "].constantAttenuation", light->attenuation.constant);
            SetFloat("lights[" + std::to_string(i) + "].linearAttenuation", light->attenuation.linear);
            SetFloat("lights[" + std::to_string(i) + "].quadraticAttenuation", light->attenuation.quadratic);
            SetVec3("lights[" + std::to_string(i) + "].position", light->Position());
            break;
        case ZLightType::Spot:
            SetBool("lights[" + std::to_string(i) + "].isEnabled", light->enabled);
            SetBool("lights[" + std::to_string(i) + "].isSpot", true);
            SetVec3("lights[" + std::to_string(i) + "].ambient", light->ambient);
            SetVec3("lights[" + std::to_string(i) + "].color", light->color);
            SetFloat("lights[" + std::to_string(i) + "].constantAttenuation", light->attenuation.constant);
            SetFloat("lights[" + std::to_string(i) + "].linearAttenuation", light->attenuation.linear);
            SetFloat("lights[" + std::to_string(i) + "].quadraticAttenuation", light->attenuation.quadratic);
            SetVec3("lights[" + std::to_string(i) + "].coneDirection", light->spot.coneDirection);
            SetFloat("lights[" + std::to_string(i) + "].cutoff", light->spot.cutoff);
            SetFloat("lights[" + std::to_string(i) + "].exponent", light->spot.exponent);
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

void ZShader::HandleShaderCodeLoaded(std::shared_ptr<ZEvent> event)
{
    std::shared_ptr<ZResourceLoadedEvent> loaded = std::static_pointer_cast<ZResourceLoadedEvent>(event);
    if (!loaded->Handle()) return;

    std::shared_ptr<ZShaderResourceExtraData> extraData = std::static_pointer_cast<ZShaderResourceExtraData>(loaded->Handle()->ExtraData());

    switch (loaded->Handle()->Resource().type)
    {
    case ZResourceType::VertexShader:
        if (loaded->Handle()->Resource().name == vertexShaderPath_ && (loadedShadersMask_ & 1) == 0)
        {
            loadedShadersMask_ |= 1;
            vertexShaderCode_ = extraData->Code();
        }
        break;
    case ZResourceType::PixelShader:
        if (loaded->Handle()->Resource().name == pixelShaderPath_ && (loadedShadersMask_ & (1 << 1)) == 0)
        {
            loadedShadersMask_ |= 1 << 1;
            pixelShaderCode_ = extraData->Code();
        }
        break;
    case ZResourceType::GeometryShader:
        if (loaded->Handle()->Resource().name == geometryShaderPath_ && (loadedShadersMask_ & (1 << 2)) == 0)
        {
            loadedShadersMask_ |= 1 << 2;
            geometryShaderCode_ = extraData->Code();
        }
        break;
    default: break;
    }

    if (loadedShadersMask_ == 3 || loadedShadersMask_ == 7)
    {
        ZEventDelegate shaderCodeLoadDelegate = fastdelegate::MakeDelegate(this, &ZShader::HandleShaderCodeLoaded);
        zenith::EventAgent()->RemoveListener(shaderCodeLoadDelegate, ZResourceLoadedEvent::Type);

        Compile();

        std::shared_ptr<ZShaderReadyEvent> shaderReadyEvent = std::make_shared<ZShaderReadyEvent>(shared_from_this());
        zenith::EventAgent()->QueueEvent(shaderReadyEvent);
    }
}
