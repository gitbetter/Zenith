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
#include "ZResourceLoadedEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZImageImporter.hpp"
#include "ZMaterial.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Store texture format in texture class

void ZGLTextureManager::Resize(const ZHTexture& handle, unsigned int width, unsigned int height)
{
    ZTexture* texture = texturePool_.Get(handle);
    assert(texture != nullptr && "Trying to resize a null texture!");
	GLenum target = texture->multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	if (texture->type.find("Array") != std::string::npos) {
		target = GL_TEXTURE_2D_ARRAY;
	}

	glBindTexture(target, texture->id);

	if (texture->multisampled)
	{
		glTexImage2DMultisample(target, 4, GL_RGB, width, height, GL_TRUE);
	}
	else if (texture->type.find("shadowArray") != std::string::npos)
	{
		glTexImage3D(target, 0, GL_DEPTH_COMPONENT, width, height, NUM_SHADOW_CASCADES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	else
	{
		glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	glBindTexture(target, 0);
}

void ZGLTextureManager::Bind(const ZHTexture& handle, unsigned int index)
{
    ZTexture* texture = texturePool_.Get(handle);
    assert(texture != nullptr && "Trying to bind a null texture!");
	glActiveTexture(GL_TEXTURE0 + index);
	if (texture->type == "cubemap" || texture->type == "irradiance" || texture->type == "prefilter")
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);
	}
	else if (texture->type.find("Array") != std::string::npos)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture->id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
    texture->index = index;
}

void ZGLTextureManager::Unbind(const ZHTexture& handle)
{
    ZTexture* texture = texturePool_.Get(handle);
    assert(texture != nullptr && "Trying to unbind a null texture!");
	glActiveTexture(GL_TEXTURE0 + texture->index);
	if (texture->type == "cubemap" || texture->type == "irradiance" || texture->type == "prefilter")
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);
	}
	else if (texture->type.find("Array") != std::string::npos)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, NULL);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
}

void ZGLTextureManager::Delete(const ZHTexture& handle)
{
	ZTexture* texture = texturePool_.Get(handle);
	assert(texture != nullptr && "Trying to delete a null texture!");
    if (texture->id > 0)
    {
        glDeleteTextures(1, &(texture->id));
    }
    texturePool_.Delete(handle);
}

ZHTexture ZGLTextureManager::Create(ZTextureResourceData* resource, const std::string& type, ZTextureWrapping wrapping /*= ZTextureWrapping::EdgeClamp*/, bool hdr /*= false*/, bool flip /*= true*/, const ZHTexture& restoreHandle /*= ZHTexture()*/)
{
	ZHTexture handle(restoreHandle);
	ZTexture* texture = nullptr;
	if (!handle.IsNull())
	{
		texture = texturePool_.Restore(handle);
	}
	else
	{
		texture = texturePool_.New(handle);
	}

	if (handle.IsNull())
	{
		LOG("Failed to load texture", ZSeverity::Error);
		return handle;
	}

	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	auto glWrap = wrapping == ZTextureWrapping::Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;

	if (hdr)
	{
		if (resource->buffer != nullptr)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resource->width, resource->height, 0, GL_RGB, GL_FLOAT, resource->buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			LOG("ZGLTexture Error: Failed to load HDR texture at " + resource->path, ZSeverity::Error);
			texturePool_.Delete(handle);
			return ZHTexture();
		}
	}
	else
	{
		if (resource->buffer)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resource->width, resource->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, resource->buffer);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			LOG("ZGLTexture Error: Failed to load texture at " + resource->path, ZSeverity::Error);
			texturePool_.Delete(handle);
			return ZHTexture();
		}
	}

    texture->path = resource->path;
	texture->type = type;

	Track(handle);

    return handle;
}

ZHTexture ZGLTextureManager::CreateDefault()
{
	ZHTexture handle;
	ZTexture* texture = texturePool_.New(handle);

    texture->type = "color";
	GLubyte textureData[] = { 255, 255, 255, 255 };
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

    return handle;
}

ZHTexture ZGLTextureManager::CreateEmptyLUT()
{
	ZHTexture handle;
	ZTexture* texture = texturePool_.New(handle);

    texture->type = "lut";
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, LUT_SIZE, LUT_SIZE, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return handle;
}

ZHTexture ZGLTextureManager::CreateColor(const glm::vec2& size, bool multisample /*= false*/)
{
	ZHTexture handle;
	ZTexture* texture = texturePool_.New(handle);

    texture->type = "color";
    texture->multisampled = multisample;
	GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	glGenTextures(1, &texture->id);
	glBindTexture(target, texture->id);

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

	return handle;
}

ZHTexture ZGLTextureManager::CreateDepth(const glm::vec2& size)
{
	ZHTexture handle;
	ZTexture* texture = texturePool_.New(handle);

    texture->type = "depth";
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, 0);

    return handle;
}

ZHTexture ZGLTextureManager::CreateDepthArray(const glm::vec2& size, int layers)
{
	ZHTexture handle;
	ZTexture* texture = texturePool_.New(handle);

    texture->type = "depthArray";
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture->id);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // GL_LINEAR will enabled PCF depth comparison on hardware if used with sampler2DArrayShadow in fragment shader
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // GL_LINEAR will enabled PCF depth comparison on hardware if used with sampler2DArrayShadow in fragment shader
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, size.x, size.y, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    return handle;
}

ZHTexture ZGLTextureManager::CreateCubeMap(const std::vector<std::string>& faces)
{
	ZHTexture handle;
	ZTexture* texture = texturePool_.New(handle);

    texture->type = "cubemap";
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		ZTextureResourceData::ptr resource = std::make_shared<ZTextureResourceData>(faces[i], ZResourceType::Texture);
		ZServices::ResourceImporter()->GetData(resource.get());

		if (!handle)
		{
			LOG("Failed to load texture at " + faces[i], ZSeverity::Error); continue;
		}

		unsigned char* data = stbi_load_from_memory((const stbi_uc*)resource->buffer, resource->size, &width, &height, &nrChannels, 0);
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

    return handle;
}

ZHTexture ZGLTextureManager::CreateCubeMap(const ZHTexture& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData)
{
	bufferData = ZFramebuffer::CreateCubeMap();
	ZHTexture cubemap = CreateEmptyCubeMap();
	ZTexture* texture = texturePool_.Get(cubemap);
	ZTexture* hdr = texturePool_.Get(hdrTexture);
	texture->path = hdr->path;

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
	ZHShader equirectToCubemapShader = ZServices::ShaderManager()->Create("/Shaders/Vertex/basic.vert", "/Shaders/Pixel/equirect_to_cube.frag");
	ZServices::ShaderManager()->Activate(equirectToCubemapShader);
	ZServices::ShaderManager()->SetMat4(equirectToCubemapShader, "P", captureProjection);

	ZServices::ShaderManager()->BindAttachment(equirectToCubemapShader, "equirectangularMapSampler0", hdrTexture);

	ZServices::ShaderManager()->Use(equirectToCubemapShader, ZMaterial::Default());

	bufferData->Bind();
	ZServices::Graphics()->UpdateViewport(glm::vec2(CUBE_MAP_SIZE, CUBE_MAP_SIZE));
	for (unsigned int i = 0; i < 6; i++)
	{
		ZServices::ShaderManager()->SetMat4(equirectToCubemapShader, "V", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture->id, 0);
		ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
		for (const auto& [id, mesh] : cube->Meshes()) {
			ZServices::Graphics()->Draw(mesh->BufferData());
		}
	}
	bufferData->Unbind();

	// Removes visible dots artifact
	Bind(cubemap, 1);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return cubemap;
}

ZHTexture ZGLTextureManager::CreateEmptyCubeMap(ZCubemapTextureType type /*= ZCubemapTextureType::Normal*/)
{
	ZHTexture handle;
	ZTexture* texture = texturePool_.New(handle);

	texture->type = "cubemap";
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

	unsigned int size = CUBE_MAP_SIZE;
	if (type == ZCubemapTextureType::Irradiance)
	{
		size = IRRADIANCE_MAP_SIZE;
	}
	else if (type == ZCubemapTextureType::Prefilter)
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

	if (type == ZCubemapTextureType::Prefilter)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else if (type == ZCubemapTextureType::Normal)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	return handle;
}

ZHTexture ZGLTextureManager::CreateHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData)
{
	ZHTexture handle = ZTextureManager::Create(hdriPath, "", ZTextureWrapping::EdgeClamp, true, true);
	return CreateCubeMap(handle, bufferData);
}

void ZGLTextureManager::CreateHDRIAsync(const std::string& hdriPath)
{
	CreateAsync(hdriPath, "", ZTextureWrapping::EdgeClamp, true, false, true);
}

ZHTexture ZGLTextureManager::CreateIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZHTexture& cubemapTexture)
{
	ZHTexture handle = CreateEmptyCubeMap(ZCubemapTextureType::Irradiance);
	ZTexture* texture = texturePool_.Get(handle);
	ZTexture* cubemap = texturePool_.Get(cubemapTexture);
	texture->type = "irradiance";

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
	ZHShader irradianceShader = ZServices::ShaderManager()->Create("/Shaders/Vertex/basic.vert", "/Shaders/Pixel/irradiance.frag");
	ZServices::ShaderManager()->Activate(irradianceShader);
	ZServices::ShaderManager()->SetMat4(irradianceShader, "P", captureProjection);

	ZServices::ShaderManager()->BindAttachment(irradianceShader, "environmentMapSampler0", cubemapTexture);

	ZServices::ShaderManager()->Use(irradianceShader, ZMaterial::Default());

	cubemapBufferData->Bind();
	cubemapBufferData->BindRenderbuffer();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE);
	ZServices::Graphics()->UpdateViewport(glm::vec2(IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE));
	for (unsigned int i = 0; i < 6; i++)
	{
		ZServices::ShaderManager()->SetMat4(irradianceShader, "V", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture->id, 0);
		ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
		for (const auto& [id, mesh] : cube->Meshes()) {
			ZServices::Graphics()->Draw(mesh->BufferData());
		}
	}
	cubemapBufferData->UnbindRenderbuffer();
	cubemapBufferData->Unbind();

	return handle;
}

ZHTexture ZGLTextureManager::CreatePrefilterMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZHTexture& cubemapTexture)
{
	ZHTexture handle = CreateEmptyCubeMap(ZCubemapTextureType::Prefilter);
	ZTexture* texture = texturePool_.Get(handle);
	ZTexture* cubemap = texturePool_.Get(cubemapTexture);
	texture->type = "prefilter";

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
	ZHShader prefilterShader = ZServices::ShaderManager()->Create("/Shaders/Vertex/basic.vert", "/Shaders/Pixel/prefilter_convolution.frag");
	ZServices::ShaderManager()->Activate(prefilterShader);
	ZServices::ShaderManager()->SetMat4(prefilterShader, "P", captureProjection);
	ZServices::ShaderManager()->SetFloat(prefilterShader, "resolution", PREFILTER_MAP_SIZE);

	ZServices::ShaderManager()->BindAttachment(prefilterShader, "environmentMapSampler0", cubemapTexture);

	ZServices::ShaderManager()->Use(prefilterShader, ZMaterial::Default());

	cubemapBufferData->Bind();

	unsigned int maxMipLevels = 6;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipSize = PREFILTER_MAP_SIZE * std::pow(0.5, mip);
		cubemapBufferData->BindRenderbuffer();
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipSize, mipSize);

		ZServices::Graphics()->UpdateViewport(glm::vec2(mipSize, mipSize));

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		ZServices::ShaderManager()->SetFloat(prefilterShader, "roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			ZServices::ShaderManager()->SetMat4(prefilterShader, "V", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture->id, mip);
			ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
			for (const auto& [id, mesh] : cube->Meshes()) {
				ZServices::Graphics()->Draw(mesh->BufferData());
			}
		}
	}
	cubemapBufferData->UnbindRenderbuffer();
	cubemapBufferData->Unbind();

	return handle;
}

ZHTexture ZGLTextureManager::CreateBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData)
{
	ZHTexture handle = CreateEmptyLUT();
	ZTexture* texture = texturePool_.Get(handle);

	ZVertex2DDataOptions options;
	options.vertices = ZVertex2DList{
		ZVertex2D(-1.f, 1.f, 0.f, 1.f),
		ZVertex2D(-1.f, -1.f, 0.f, 0.f),
		ZVertex2D(1.f, 1.f, 1.f, 1.f),
		ZVertex2D(1.f, -1.f, 1.f, 0.f),
	};
	ZVertexBuffer::ptr quadBufferData = ZVertexBuffer::Create(options);
	ZHShader brdfLUTShader = ZServices::ShaderManager()->Create("/Shaders/Vertex/brdf_lut.vert", "/Shaders/Pixel/brdf_lut.frag");

	cubemapBufferData->Bind();
	cubemapBufferData->BindRenderbuffer();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, LUT_SIZE, LUT_SIZE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);

	ZServices::Graphics()->ClearViewport(glm::vec4(0.f), 0);
	ZServices::Graphics()->UpdateViewport(glm::vec2(LUT_SIZE, LUT_SIZE));

	ZServices::Graphics()->Draw(quadBufferData);

	cubemapBufferData->UnbindRenderbuffer();
	cubemapBufferData->Unbind();
	
	return handle;
}
