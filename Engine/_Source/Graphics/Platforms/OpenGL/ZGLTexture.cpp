/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZGLTexture.cpp

	Created by Adrian Sanchez on 27/01/2021.
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

#include "ZGLTexture.hpp"
#include "ZFramebuffer.hpp"
#include "ZServices.hpp"
#include "ZCube.hpp"
#include "ZShader.hpp"
#include "ZVertexBuffer.hpp"
#include "ZResourceExtraData.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZImageImporter.hpp"
#include "ZMaterial.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Store texture format in texture class

void ZGLTexture::Resize(unsigned int width, unsigned int height)
{
    GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    if (type.find("Array") != std::string::npos) {
        target = GL_TEXTURE_2D_ARRAY;
    }

    glBindTexture(target, id);

    if (multisampled)
    {
        glTexImage2DMultisample(target, 4, GL_RGB, width, height, GL_TRUE);
    }
    else if (type.find("shadowArray") != std::string::npos)
    {
        glTexImage3D(target, 0, GL_DEPTH_COMPONENT, width, height, NUM_SHADOW_CASCADES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    else
    {
        glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    glBindTexture(target, 0);
}

void ZGLTexture::Delete()
{
    if (id > 0) {
        glDeleteTextures(1, &id);
    }
}

void ZGLTexture::Bind(unsigned int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    if (type == "cubemap" || type == "irradiance" || type == "prefilter")
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }
    else if (type.find("Array") != std::string::npos)
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
    } 
    else
    {
        glBindTexture(GL_TEXTURE_2D, id);
    }
    index_ = index;
}

void ZGLTexture::Unbind()
{
    glActiveTexture(GL_TEXTURE0 + index_);
    if (type == "cubemap" || type == "irradiance" || type == "prefilter")
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);
    }
    else if (type.find("Array") != std::string::npos)
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, NULL);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, NULL);
    }
    index_ = 0;
}

void ZGLTexture::LoadHDRIAsync(const std::string& hdriPath)
{
    LoadAsync(hdriPath, "", ZTextureWrapping::EdgeClamp, true, false, true);
}

void ZGLTexture::LoadHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData)
{
    ZTexture::ptr hdrTexture = ZTexture::Create(hdriPath, "", ZTextureWrapping::EdgeClamp, true, true);
    LoadCubeMap(hdrTexture, bufferData);
}

void ZGLTexture::LoadCubeMap(const ZTexture::ptr& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData)
{
    bufferData = ZFramebuffer::CreateCubeMap();
    LoadEmptyCubeMap();
    path = hdrTexture->path;

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
    };

    std::shared_ptr<ZCube> cube = ZCube::Create(glm::vec3(1.f, 1.f, 1.f));
    std::shared_ptr<ZShader> equirectToCubemapShader = ZShader::Create("/Shaders/Vertex/basic.vert", "/Shaders/Pixel/equirect_to_cube.frag");
    equirectToCubemapShader->Activate();
    equirectToCubemapShader->SetMat4("P", captureProjection);

    equirectToCubemapShader->BindAttachment("equirectangularMapSampler0", hdrTexture);

    equirectToCubemapShader->Use(ZMaterial::Default());

    bufferData->Bind();
    ZServices::Graphics()->UpdateViewport(glm::vec2(CUBE_MAP_SIZE, CUBE_MAP_SIZE));
    for (unsigned int i = 0; i < 6; i++)
    {
        equirectToCubemapShader->SetMat4("V", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, id, 0);
        ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
        for (const auto& [id, mesh] : cube->Meshes()) {
            ZServices::Graphics()->Draw(mesh->BufferData());   
        }
    }
    bufferData->Unbind();

    // Removes visible dots artifact
    Bind(1);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void ZGLTexture::LoadIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture)
{
    LoadEmptyCubeMap(ZCubemapTextureType::Irradiance);
    type = "irradiance";

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
    };

    std::shared_ptr<ZCube> cube = ZCube::Create(glm::vec3(1.f, 1.f, 1.f));
    std::shared_ptr<ZShader> irradianceShader = ZShader::Create("/Shaders/Vertex/basic.vert", "/Shaders/Pixel/irradiance.frag");
    irradianceShader->Activate();
    irradianceShader->SetMat4("P", captureProjection);

    irradianceShader->BindAttachment("environmentMapSampler0", cubemapTexture);

    irradianceShader->Use(ZMaterial::Default());

    cubemapBufferData->Bind();
    cubemapBufferData->BindRenderbuffer();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE);
    ZServices::Graphics()->UpdateViewport(glm::vec2(IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE));
    for (unsigned int i = 0; i < 6; i++)
    {
        irradianceShader->SetMat4("V", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, id, 0);
        ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
        for (const auto& [id, mesh] : cube->Meshes()) {
            ZServices::Graphics()->Draw(mesh->BufferData());
        }
    }
    cubemapBufferData->UnbindRenderbuffer();
    cubemapBufferData->Unbind();
}

void ZGLTexture::LoadPrefilterCubeMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture)
{
    LoadEmptyCubeMap(ZCubemapTextureType::Prefilter);
    type = "prefilter";

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
    };

    std::shared_ptr<ZCube> cube = ZCube::Create(glm::vec3(1.f, 1.f, 1.f));
    std::shared_ptr<ZShader> prefilterShader = ZShader::Create("/Shaders/Vertex/basic.vert", "/Shaders/Pixel/prefilter_convolution.frag");
    prefilterShader->Activate();
    prefilterShader->SetMat4("P", captureProjection);
    prefilterShader->SetFloat("resolution", PREFILTER_MAP_SIZE);

    prefilterShader->BindAttachment("environmentMapSampler0", cubemapTexture);

    prefilterShader->Use(ZMaterial::Default());

    cubemapBufferData->Bind();

    unsigned int maxMipLevels = 6;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipSize = PREFILTER_MAP_SIZE * std::pow(0.5, mip);
        cubemapBufferData->BindRenderbuffer();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipSize, mipSize);

        ZServices::Graphics()->UpdateViewport(glm::vec2(mipSize, mipSize));

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader->SetFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader->SetMat4("V", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, id, mip);
            ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
            for (const auto& [id, mesh] : cube->Meshes()) {
                ZServices::Graphics()->Draw(mesh->BufferData());
            }
        }
    }
    cubemapBufferData->UnbindRenderbuffer();
    cubemapBufferData->Unbind();
}

void ZGLTexture::LoadBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData)
{
    LoadEmptyLUT();

    ZVertex2DDataOptions options;
    options.vertices = ZVertex2DList{
        ZVertex2D(-1.f, 1.f, 0.f, 1.f),
        ZVertex2D(-1.f, -1.f, 0.f, 0.f),
        ZVertex2D(1.f, 1.f, 1.f, 1.f),
        ZVertex2D(1.f, -1.f, 1.f, 0.f),
    };
    ZVertexBuffer::ptr quadBufferData = ZVertexBuffer::Create(options);
    auto brdfLUTShader = ZShader::Create("/Shaders/Vertex/brdf_lut.vert", "/Shaders/Pixel/brdf_lut.frag");
    brdfLUTShader->Activate();

    cubemapBufferData->Bind();
    cubemapBufferData->BindRenderbuffer();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, LUT_SIZE, LUT_SIZE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

    ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
    ZServices::Graphics()->UpdateViewport(glm::vec2(LUT_SIZE, LUT_SIZE));

    ZServices::Graphics()->Draw(quadBufferData);

    cubemapBufferData->UnbindRenderbuffer();
    cubemapBufferData->Unbind();
}

void ZGLTexture::LoadDefault()
{
    if (id > 0) {
        Delete();
    }

    type = "color";
    GLubyte textureData[] = { 255, 255, 255, 255 };
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
}

void ZGLTexture::LoadCubeMap(const std::vector<std::string>& faces)
{
    if (id > 0) {
        Delete();
    }

    type = "cubemap";
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        ZResource resource(faces[i], ZResourceType::Texture);
        std::shared_ptr<ZResourceHandle> handle = ZServices::ResourceCache()->GetHandle(&resource);

        if (!handle)
        {
            LOG("Failed to load texture at " + faces[i], ZSeverity::Error); continue;
        }

        unsigned char* data = stbi_load_from_memory((const stbi_uc*)handle->Buffer(), handle->Size(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            LOG("Could not load texture at path " + faces[i], ZSeverity::Error);
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void ZGLTexture::LoadEmptyCubeMap(ZCubemapTextureType textureType)
{
    if (id > 0) {
        Delete();
    }

    type = "cubemap";
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    unsigned int size = CUBE_MAP_SIZE;
    if (textureType == ZCubemapTextureType::Irradiance)
    {
        size = IRRADIANCE_MAP_SIZE;
    }
    else if (textureType == ZCubemapTextureType::Prefilter)
    {
        size = PREFILTER_MAP_SIZE;
    }
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (textureType == ZCubemapTextureType::Prefilter)
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    else if (textureType == ZCubemapTextureType::Normal)
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}

void ZGLTexture::Load(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping, bool hdr, bool flip)
{
    if (id > 0) {
        Delete();
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    auto glWrap = wrapping == ZTextureWrapping::Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;

    if (!handle)
    {
        LOG("Failed to load texture", ZSeverity::Error);
        return;
    }

    std::shared_ptr<ZTextureResourceExtraData> textureData = std::static_pointer_cast<ZTextureResourceExtraData>(handle->ExtraData());

    if (hdr)
    {
        if (textureData->FloatData())
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureData->Width(), textureData->Height(), 0, GL_RGB, GL_FLOAT, textureData->FloatData());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            LOG("ZGLTexture Error: Failed to load HDR texture at " + handle->Resource().name, ZSeverity::Error);
        }
    }
    else
    {
        if (textureData->Data())
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData->Width(), textureData->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData->Data());
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            LOG("ZGLTexture Error: Failed to load texture at " + handle->Resource().name, ZSeverity::Error);
        }
    }

    path = handle->Resource().name;
}

void ZGLTexture::LoadEmptyLUT()
{
    if (id > 0) {
        Delete();
    }

    type = "lut";
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, LUT_SIZE, LUT_SIZE, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ZGLTexture::LoadColor(const glm::vec2& size, bool multisample)
{
    if (id > 0) {
        Delete();
    }

    type = "color";
    multisampled = multisample;
    GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    glGenTextures(1, &id);
    glBindTexture(target, id);

    if (multisample)
    {
        glTexImage2DMultisample(target, 4, GL_RGB, size.x, size.y, GL_TRUE);
    }
    else
    {
        glTexImage2D(target, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(target, 0);
}

void ZGLTexture::LoadDepth(const glm::vec2& size)
{
    if (id > 0) {
        Delete();
    }

    type = "depth";
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void ZGLTexture::LoadDepthArray(const glm::vec2& size, int layers)
{
    if (id > 0) {
        Delete();
    }

    type = "depthArray";
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // GL_LINEAR will enabled PCF depth comparison on hardware if used with sampler2DArrayShadow in fragment shader
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // GL_LINEAR will enabled PCF depth comparison on hardware if used with sampler2DArrayShadow in fragment shader
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, size.x, size.y, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
