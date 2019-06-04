/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZGLGraphicsStrategy.cpp
 
    Created by Adrian Sanchez on 07/02/2019.
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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ZGLGraphicsStrategy.hpp"
#include "ZDomain.hpp"
#include "ZDomainStrategy.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZResourceCache.hpp"
#include "ZResource.hpp"
#include "ZImageImporter.hpp"
#include "ZResourceExtraData.hpp"
#include "ZEventAgent.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZTextureReadyEvent.hpp"

ZGLGraphicsStrategy::~ZGLGraphicsStrategy() {
    ZEventDelegate modelLoadDelegate = fastdelegate::MakeDelegate(this, &ZGLGraphicsStrategy::HandleTextureLoaded);
    zenith::EventAgent()->RemoveListener(modelLoadDelegate, ZResourceLoadedEvent::Type);
}

void ZGLGraphicsStrategy::Initialize() {
    drawingStylesMap_[ZMeshDrawStyle::Point] = GL_POINT;
    drawingStylesMap_[ZMeshDrawStyle::Line] = GL_LINES;
    drawingStylesMap_[ZMeshDrawStyle::LineStrip] = GL_LINE_STRIP;
    drawingStylesMap_[ZMeshDrawStyle::Triangle] = GL_TRIANGLES;
    drawingStylesMap_[ZMeshDrawStyle::TriangleStrip] = GL_TRIANGLE_STRIP;
    
    glfwSetErrorCallback(GLFWErrorCallback);
    
    EnableDepthTesting();
    EnableStencilTesting();
    EnableSeamlessCubemap();
    EnableMSAA();
    
    ZEventDelegate modelLoadDelegate = fastdelegate::MakeDelegate(this, &ZGLGraphicsStrategy::HandleTextureLoaded);
    zenith::EventAgent()->AddListener(modelLoadDelegate, ZResourceLoadedEvent::Type);
}

void ZGLGraphicsStrategy::ClearViewport() {
    glClearColor(0.3f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);
}

void ZGLGraphicsStrategy::SwapBuffers() {
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSwapBuffers(window);
}

void ZGLGraphicsStrategy::EnableDepthTesting() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void ZGLGraphicsStrategy::DisableDepthTesting() {
    glDisable(GL_DEPTH_TEST);
}

void ZGLGraphicsStrategy::EnableStencilTesting() {
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void ZGLGraphicsStrategy::EnableStencilBuffer() {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void ZGLGraphicsStrategy::DisableStencilBuffer() {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
}

void ZGLGraphicsStrategy::EnableAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ZGLGraphicsStrategy::DisableAlphaBlending() {
    glDisable(GL_BLEND);
}

void ZGLGraphicsStrategy::EnableFaceCulling() {
    glEnable(GL_CULL_FACE);
}

void ZGLGraphicsStrategy::DisableFaceCulling() {
    glDisable(GL_CULL_FACE);
}

void ZGLGraphicsStrategy::EnableSeamlessCubemap() {
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void ZGLGraphicsStrategy::DisableSeamlessCubemap() {
    glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void ZGLGraphicsStrategy::EnableMSAA() {
    glEnable(GL_MULTISAMPLE);
}

void ZGLGraphicsStrategy::DisableMSAA() {
    glDisable(GL_MULTISAMPLE);
}

void ZGLGraphicsStrategy::CullFrontFaces() {
    glCullFace(GL_FRONT);
}

void ZGLGraphicsStrategy::CullBackFaces() {
    glCullFace(GL_BACK);
}

void ZGLGraphicsStrategy::ClearDepth() {
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ZGLGraphicsStrategy::BindFramebuffer(ZBufferData frameBuffer, bool depth) {
	if (depth) {
		glViewport(0, 0, zenith::SHADOW_MAP_SIZE, zenith::SHADOW_MAP_SIZE);
	} else {
		glViewport(0, 0, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY());
	}
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);
}

void ZGLGraphicsStrategy::UnbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ZGLGraphicsStrategy::BlitFramebuffer(ZBufferData source, ZBufferData destination) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, source.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.fbo);
	glBlitFramebuffer(0, 0, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY(), 0, 0, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLGraphicsStrategy::BindTexture(ZTexture texture, unsigned int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    if (texture.type == "cubemap" || texture.type == "irradiance" || texture.type == "prefilter") {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
    } else {
        glBindTexture(GL_TEXTURE_2D, texture.id);
    }
}

ZBufferData ZGLGraphicsStrategy::LoadIndexedVertexData(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) {
    ZBufferData bufferData;
    glGenVertexArrays(1, &bufferData.vao);
    glGenBuffers(1, &bufferData.vbo);
    glGenBuffers(1, &bufferData.ebo);
    
    glBindVertexArray(bufferData.vao);
    
    // Bind the buffer object and set the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ZVertex3D), &vertices[0], GL_STATIC_DRAW);
    
    //  Bind the element buffer object (for indexed drawing) and set the index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferData.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    // Vertex position vector
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Vertex normal vector
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, normal));
    glEnableVertexAttribArray(1);
    
    // Vertex textures coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, uv));
    glEnableVertexAttribArray(2);
    
    // Vertex tangent vector
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, tangent));
    glEnableVertexAttribArray(3);
    
    // Vertex bitangent vector (the tangent to the tangent, but not the normal)
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, bitangent));
    glEnableVertexAttribArray(4);
    
    // Vertex bone id data
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, boneIDs));
    glEnableVertexAttribArray(5);
    
    // Vertex bone weight data
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, boneWeights));
    glEnableVertexAttribArray(6);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return bufferData;
}

ZBufferData ZGLGraphicsStrategy::LoadVertexData(std::vector<ZVertex3D> vertices) {
    ZBufferData bufferData;
    glGenVertexArrays(1, &bufferData.vao);
    glGenBuffers(1, &bufferData.vbo);
    
    glBindVertexArray(bufferData.vao);
    
    // Bind the buffer object and set the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ZVertex3D), &vertices[0], GL_STATIC_DRAW);
    
    // Vertex position vector
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Vertex normal vector
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, normal));
    glEnableVertexAttribArray(1);
    
    // Vertex textures coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, uv));
    glEnableVertexAttribArray(2);
    
    // Vertex tangent vector
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, tangent));
    glEnableVertexAttribArray(3);
    
    // Vertex bitangent vector (the tangent to the tangent, but not the normal)
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, bitangent));
    glEnableVertexAttribArray(4);
    
    // Vertex bone id data
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, boneIDs));
    glEnableVertexAttribArray(5);
    
    // Vertex bone weight data
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, boneWeights));
    glEnableVertexAttribArray(6);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return bufferData;
}

ZBufferData ZGLGraphicsStrategy::LoadVertexData(std::vector<ZVertex2D> vertices) {
    ZBufferData bufferData;
    glGenVertexArrays(1, &bufferData.vao);
    glGenBuffers(1, &bufferData.vbo);
    
    glBindVertexArray(bufferData.vao);
    
    // Bind the buffer object and set the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ZVertex2D), &vertices[0], GL_STATIC_DRAW);
    
    // Vertex position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Vertex texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)offsetof(ZVertex2D, uv));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return bufferData;
}

ZBufferData ZGLGraphicsStrategy::LoadEmptyVertexData2D(unsigned int size) {
    ZBufferData bufferData;
    glGenVertexArrays(1, &bufferData.vao);
    glGenBuffers(1, &bufferData.vbo);
    
    glBindVertexArray(bufferData.vao);
    
    // Bind the buffer object and set the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ZVertex2D) * size, NULL, GL_STATIC_DRAW);
    
    // Vertex position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Vertex texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)offsetof(ZVertex2D, uv));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return bufferData;
}

void ZGLGraphicsStrategy::DeleteBufferData(ZBufferData bufferData) {
    glDeleteVertexArrays(1, &bufferData.vao);
    glDeleteBuffers(1, &bufferData.vbo);
    glDeleteBuffers(1, &bufferData.ebo);
}

ZTexture ZGLGraphicsStrategy::LoadDefaultTexture() {
    ZTexture texture;
    GLubyte textureData[] = { 255, 255, 255, 255 };
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    return texture;
}

void ZGLGraphicsStrategy::LoadTextureAsync(std::string path, const std::string &directory, bool hdr, bool flip, bool equirect) {
	std::string filename = (!directory.empty() ? directory + '/' : "") + path;
	ZResourceType type = ZResourceType::Other;
	if (hdr && equirect) type = ZResourceType::HDREquirectangularMap;
	else if (hdr) type = ZResourceType::HDRTexture;
	else type = ZResourceType::Texture;

	ZResource textureResource(filename, type);
	zenith::ResourceCache()->RequestHandle(textureResource);
}

ZTexture ZGLGraphicsStrategy::LoadTexture(std::string path, const std::string &directory, bool hdr, bool flip) {
	std::string filename = (!directory.empty() ? directory + '/' : "") + path;
	std::shared_ptr<ZResourceHandle> handle = ZImageImporter::LoadImage(filename, hdr, flip);
	return LoadTexture(handle, hdr, flip);
}

ZTexture ZGLGraphicsStrategy::LoadTexture(std::shared_ptr<ZResourceHandle> handle, bool hdr, bool flip) {
    ZTexture texture;
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    
    if (!handle) {
        _Z("Failed to load texture", ZERROR);
        return texture;
    }
    
	std::shared_ptr<ZTextureResourceExtraData> textureData = std::dynamic_pointer_cast<ZTextureResourceExtraData>(handle->ExtraData());

    if (hdr) {
        if (textureData->FloatData()) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureData->Width(), textureData->Height(), 0, GL_RGB, GL_FLOAT, textureData->FloatData());
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            _Z("ZGLGraphicsStrategy Error: Failed to load HDR texture at " + handle->Resource().name, ZERROR);
        }
    } else {
        if (textureData->Data()) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData->Width(), textureData->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData->Data());
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            _Z("ZGLGraphicsStrategy Error: Failed to load texture at " + handle->Resource().name, ZERROR);
        }
    }
    
	texture.path = handle->Resource().name;
    return texture;
}

ZTexture ZGLGraphicsStrategy::LoadEmptyLUT() {
    ZTexture lut;
    lut.type = "lut";
    glGenTextures(1, &lut.id);
    glBindTexture(GL_TEXTURE_2D, lut.id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, zenith::LUT_SIZE, zenith::LUT_SIZE, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return lut;
}

ZTexture ZGLGraphicsStrategy::LoadCubeMap(std::vector<std::string> faces) {
    ZTexture cubemap;
    cubemap.type = "cubemap";
    glGenTextures(1, &cubemap.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        ZResource resource(faces[i], ZResourceType::Texture);
        std::shared_ptr<ZResourceHandle> handle = zenith::ResourceCache()->GetHandle(&resource);
        
        if (!handle) {
            _Z("Failed to load texture at " + faces[i], ZERROR); continue;
        }
        
        unsigned char* data = stbi_load_from_memory((const stbi_uc*)handle->Buffer(), handle->Size(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else {
            _Z("Could not load texture at path " + faces[i], ZERROR);
        }
        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return cubemap;
}

ZTexture ZGLGraphicsStrategy::LoadEmptyCubeMap(ZCubemapTextureType type) {
    ZTexture cubemap;
    cubemap.type = "cubemap";
    glGenTextures(1, &cubemap.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    
    unsigned int size = zenith::CUBE_MAP_SIZE;
    if (type == ZCubemapTextureType::Irradiance) {
        size = zenith::IRRADIANCE_MAP_SIZE;
    } else if (type == ZCubemapTextureType::Prefilter) {
        size = zenith::PREFILTER_MAP_SIZE;
    }
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if (type == ZCubemapTextureType::Prefilter) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    } else if (type == ZCubemapTextureType::Normal) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    
    return cubemap;
}

ZBufferData ZGLGraphicsStrategy::LoadColorBuffer(ZTexture colorTexture, bool multisample) {
	ZBufferData color;
	color.type = ZBufferDataType::FrameBuffer;
	glGenFramebuffers(1, &color.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, color.fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, colorTexture.id, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	glGenRenderbuffers(1, &color.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, color.rbo);
	
	if (multisample) {
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY());
	} else {
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY());
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, color.rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
		_Z("Framebuffer operation incomplete dimensions", ZERROR);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return color;
}

ZBufferData ZGLGraphicsStrategy::LoadDepthMapBuffer(ZTexture depthTexture) {
    ZBufferData depth;
    depth.type = ZBufferDataType::FrameBuffer;
    glGenFramebuffers(1, &depth.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth.fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
        _Z("Framebuffer operation incomplete dimensions", ZERROR);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return depth;
}

ZBufferData ZGLGraphicsStrategy::LoadCubeMapBuffer() {
    ZBufferData capture;
    glGenFramebuffers(1, &capture.fbo);
    glGenRenderbuffers(1, &capture.rbo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, capture.fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, capture.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, zenith::CUBE_MAP_SIZE, zenith::CUBE_MAP_SIZE);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture.rbo);
    return capture;
}

ZTexture ZGLGraphicsStrategy::LoadColorTexture(bool multisample) {
	ZTexture texture;
	texture.type = "color";
	GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	glGenTextures(1, &texture.id);
	glBindTexture(target, texture.id);

	if (multisample) {
		glTexImage2DMultisample(target, 4, GL_RGB, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY(), GL_TRUE);
	} else {
		glTexImage2D(target, 0, GL_RGB, zenith::Domain()->ResolutionX(), zenith::Domain()->ResolutionY(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(target, 0);

	return texture;
}

ZTexture ZGLGraphicsStrategy::LoadDepthTexture() {
    ZTexture depthTexture;
    depthTexture.type = "depth";
    glGenTextures(1, &depthTexture.id);
    glBindTexture(GL_TEXTURE_2D, depthTexture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, zenith::SHADOW_MAP_SIZE, zenith::SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return depthTexture;
}

void ZGLGraphicsStrategy::UpdateBuffer(ZBufferData buffer, std::vector<ZVertex2D> data) {
    glBindVertexArray(buffer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(ZVertex2D), &data[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZGLGraphicsStrategy::EquirectToCubemapAsync(std::string equirectHDRPath) {
	LoadTextureAsync(equirectHDRPath, "", true, false, true);
}

ZTexture ZGLGraphicsStrategy::EquirectToCubemap(std::string equirectHDRPath, ZBufferData& bufferData) {
	ZTexture hdrTexture = LoadTexture(equirectHDRPath, "", true, true);
	return EquirectToCubemap(hdrTexture, bufferData);
}

ZTexture ZGLGraphicsStrategy::EquirectToCubemap(ZTexture& hdrTexture, ZBufferData& bufferData) {
	bufferData = LoadCubeMapBuffer();
	ZTexture cubeMap = LoadEmptyCubeMap();
    cubeMap.path = hdrTexture.path;
    
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
    };
    
    std::unique_ptr<ZModel> cube = ZModel::NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f));
    ZShader equirectToCubemapShader("Engine/_Assets/Shaders/Vertex/basic.vert", "Engine/_Assets/Shaders/Pixel/equirect_to_cube.frag");
    equirectToCubemapShader.Initialize();
    equirectToCubemapShader.Activate();
    equirectToCubemapShader.SetInt("equirectangularMap", 1);
    equirectToCubemapShader.SetMat4("P", captureProjection);
    
    BindTexture(hdrTexture, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferData.fbo);
    glViewport(0, 0, zenith::CUBE_MAP_SIZE, zenith::CUBE_MAP_SIZE);
    for (unsigned int i = 0; i < 6; i++) {
        equirectToCubemapShader.SetMat4("V", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap.id, 0);
        ClearViewport();
        
        cube->Render(&equirectToCubemapShader);
    }
    UnbindFramebuffer();
    
    // Removes visible dots artifact
    BindTexture(cubeMap, 1);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    return cubeMap;
}

ZTexture ZGLGraphicsStrategy::IrradianceMapFromCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) {
    ZTexture irradianceMap = LoadEmptyCubeMap(ZCubemapTextureType::Irradiance);
    irradianceMap.type = "irradiance";
    
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
    };
    
    std::unique_ptr<ZModel> cube = ZModel::NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f));
    ZShader irradianceShader("Engine/_Assets/Shaders/Vertex/basic.vert", "Engine/_Assets/Shaders/Pixel/irradiance.frag");
    irradianceShader.Initialize();
    irradianceShader.Activate();
    irradianceShader.SetInt("environmentMap", 1);
    irradianceShader.SetMat4("P", captureProjection);
    
    BindTexture(cubemapTexture, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, cubemapBufferData.fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, cubemapBufferData.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, zenith::IRRADIANCE_MAP_SIZE, zenith::IRRADIANCE_MAP_SIZE);
    glViewport(0, 0, zenith::IRRADIANCE_MAP_SIZE, zenith::IRRADIANCE_MAP_SIZE);
    for (unsigned int i = 0; i < 6; i++) {
        irradianceShader.SetMat4("V", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap.id, 0);
        ClearViewport();
        
        cube->Render(&irradianceShader);
    }
    UnbindFramebuffer();
    
    return irradianceMap;
}

ZTexture ZGLGraphicsStrategy::PrefilterCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) {
    ZTexture prefilterMap = LoadEmptyCubeMap(ZCubemapTextureType::Prefilter);
    prefilterMap.type = "prefilter";
    
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
    };
    
    std::unique_ptr<ZModel> cube = ZModel::NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f));
    ZShader prefilterShader("Engine/_Assets/Shaders/Vertex/basic.vert", "Engine/_Assets/Shaders/Pixel/prefilter_convolution.frag");
    prefilterShader.Initialize();
    prefilterShader.Activate();
    prefilterShader.SetInt("environmentMap", 1);
    prefilterShader.SetMat4("P", captureProjection);
    prefilterShader.SetFloat("resolution", zenith::PREFILTER_MAP_SIZE);
    
    BindTexture(cubemapTexture, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, cubemapBufferData.fbo);
    
    unsigned int maxMipLevels = 6;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        unsigned int mipSize = zenith::PREFILTER_MAP_SIZE * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, cubemapBufferData.rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipSize, mipSize);
        
        glViewport(0, 0, mipSize, mipSize);
        
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.SetFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i) {
            prefilterShader.SetMat4("V", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap.id, mip);
            ClearViewport();
            cube->Render(&prefilterShader);
        }
    }
    UnbindFramebuffer();
    
    return prefilterMap;
}

ZTexture ZGLGraphicsStrategy::BRDFLUT(ZBufferData cubemapBufferData) {
    ZTexture brdfLUT = LoadEmptyLUT();
    
    std::vector<ZVertex2D> quadVertices = {
        ZVertex2D(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f)),
        ZVertex2D(glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f)),
        ZVertex2D(glm::vec2(1.f, 1.f), glm::vec2(1.f, 1.f)),
        ZVertex2D(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f)),
    };
    ZBufferData quadBufferData = LoadVertexData(quadVertices);
    ZShader brdfLUTShader("Engine/_Assets/Shaders/Vertex/brdf_lut.vert", "Engine/_Assets/Shaders/Pixel/brdf_lut.frag");
    brdfLUTShader.Initialize();
    brdfLUTShader.Activate();
    
    glBindFramebuffer(GL_FRAMEBUFFER, cubemapBufferData.fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, cubemapBufferData.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, zenith::LUT_SIZE, zenith::LUT_SIZE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT.id, 0);
    
    glViewport(0, 0, zenith::LUT_SIZE, zenith::LUT_SIZE);
    ClearViewport();
    
    Draw(quadBufferData, quadVertices);
    
    UnbindFramebuffer();

    return brdfLUT;
}

void ZGLGraphicsStrategy::Draw(ZBufferData bufferData, std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMeshDrawStyle drawStyle) {
    glBindVertexArray(bufferData.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferData.ebo);
    glDrawElements(drawingStylesMap_[drawStyle], indices.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::Draw(ZBufferData bufferData, std::vector<ZVertex2D> vertices, ZMeshDrawStyle drawStyle) {
    glBindVertexArray(bufferData.vao);
    glDrawArrays(drawingStylesMap_[drawStyle], 0, vertices.size());
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::DrawLines(ZBufferData bufferData, std::vector<ZVertex3D> vertices) {
    glBindVertexArray(bufferData.vao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::HandleTextureLoaded(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZResourceLoadedEvent> loaded = std::dynamic_pointer_cast<ZResourceLoadedEvent>(event);
	if (!loaded->Handle()) return;

	ZResource resource = loaded->Handle()->Resource();
	if (resource.type != ZResourceType::Texture &&
		resource.type != ZResourceType::HDRTexture &&
		resource.type != ZResourceType::HDREquirectangularMap)
		return;

	std::shared_ptr<ZTextureResourceExtraData> textureData = std::dynamic_pointer_cast<ZTextureResourceExtraData>(loaded->Handle()->ExtraData());

	ZTexture texture = LoadTexture(loaded->Handle(), textureData->IsHDR(), textureData->IsFlipped());

	std::shared_ptr<ZTextureReadyEvent> textureReadyEvent;
	if (resource.type == ZResourceType::HDREquirectangularMap) {
		ZBufferData bufferData;
		texture = EquirectToCubemap(texture, bufferData);
		textureReadyEvent = std::make_shared<ZTextureReadyEvent>(texture, bufferData);
	} else {
		textureReadyEvent = std::make_shared<ZTextureReadyEvent>(texture);
	}
	zenith::EventAgent()->QueueEvent(textureReadyEvent);
}

void ZGLGraphicsStrategy::GLFWErrorCallback(int id, const char* description) {
    _Z(description, ZERROR);
}
