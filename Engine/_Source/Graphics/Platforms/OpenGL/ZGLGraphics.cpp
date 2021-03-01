/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLGraphics.cpp

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

// TODO: Conditional include based on graphics implementation
#include "ZGLGraphics.hpp"
#include "ZServices.hpp"
#include "ZVertexBuffer.hpp"
#include "ZFrustum.hpp"
#include "ZShader.hpp"
#include "ZFont.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void ZGLGraphics::Initialize()
{
    glfwSetErrorCallback(GLFWErrorCallback);
    glfwInit();

    EnableDepthTesting();
    EnableStencilTesting();
    EnableSeamlessCubemap();
    EnableFaceCulling();
    EnableMSAA();

    drawingStylesMap_[ZMeshDrawStyle::Point] = GL_POINT;
    drawingStylesMap_[ZMeshDrawStyle::Line] = GL_LINES;
    drawingStylesMap_[ZMeshDrawStyle::LineStrip] = GL_LINE_STRIP;
    drawingStylesMap_[ZMeshDrawStyle::Triangle] = GL_TRIANGLES;
    drawingStylesMap_[ZMeshDrawStyle::TriangleStrip] = GL_TRIANGLE_STRIP;
    drawingStylesMap_[ZMeshDrawStyle::TriangleFan] = GL_TRIANGLE_FAN;
    drawingStylesMap_[ZMeshDrawStyle::Quads] = GL_QUADS;
}

void ZGLGraphics::UpdateViewport(const glm::vec2& size, const glm::vec2& position)
{
    glViewport(position.x, position.y, size.x, size.y);
}

void ZGLGraphics::EnableDepthTesting()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
}

void ZGLGraphics::DisableDepthTesting()
{
    glDisable(GL_DEPTH_TEST);
}

void ZGLGraphics::EnableStencilTesting()
{
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void ZGLGraphics::DisableStencilTesting()
{
    glDisable(GL_STENCIL_TEST);
}

void ZGLGraphics::EnableStencilBuffer()
{
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void ZGLGraphics::DisableStencilBuffer()
{
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
}

void ZGLGraphics::EnableAlphaBlending()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ZGLGraphics::EnableAdditiveBlending()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void ZGLGraphics::DisableBlending()
{
    glDisable(GL_BLEND);
}

void ZGLGraphics::EnableFaceCulling()
{
    glEnable(GL_CULL_FACE);
}

void ZGLGraphics::DisableFaceCulling()
{
    glDisable(GL_CULL_FACE);
}

void ZGLGraphics::EnableSeamlessCubemap()
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void ZGLGraphics::DisableSeamlessCubemap()
{
    glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void ZGLGraphics::EnableMSAA()
{
    glEnable(GL_MULTISAMPLE);
}

void ZGLGraphics::DisableMSAA()
{
    glDisable(GL_MULTISAMPLE);
}

void ZGLGraphics::CullFrontFaces()
{
    glCullFace(GL_FRONT);
}

void ZGLGraphics::CullBackFaces()
{
    glCullFace(GL_BACK);
}

void ZGLGraphics::ClearViewport(const glm::vec4& clearColor, uint8_t clearFlags)
{
    GLbitfield clearBits = clearFlags == 0 ? GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT : 0;

    if (clearFlags > 0) {
        clearBits |= (clearFlags & static_cast<uint8_t>(ZClearFlags::Color)) ? GL_COLOR_BUFFER_BIT : 0;
        clearBits |= (clearFlags & static_cast<uint8_t>(ZClearFlags::Depth)) ? GL_DEPTH_BUFFER_BIT : 0;
        clearBits |= (clearFlags & static_cast<uint8_t>(ZClearFlags::Stencil)) ? GL_STENCIL_BUFFER_BIT : 0;
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearBits);
}

void ZGLGraphics::ClearViewport(const glm::vec4& clearColor, const std::initializer_list<ZClearFlags>& clearFlags)
{
    GLbitfield clearBits = clearFlags.size() == 0 ? GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT : 0;

    for (auto flag : clearFlags) {
        if (flag == ZClearFlags::Color) clearBits |= GL_COLOR_BUFFER_BIT;
        else if (flag == ZClearFlags::Depth) clearBits |= GL_DEPTH_BUFFER_BIT;
        else if (flag == ZClearFlags::Stencil) clearBits |= GL_STENCIL_BUFFER_BIT;
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearBits);
}

void ZGLGraphics::CullFaces(uint8_t faceCullState)
{
    bool frontFace = faceCullState & static_cast<uint8_t>(ZFaceCullState::Front);
    bool backFace = faceCullState & static_cast<uint8_t>(ZFaceCullState::Back);
    if (frontFace && backFace)
        glCullFace(GL_FRONT_AND_BACK);
    else if (frontFace)
        glCullFace(GL_FRONT);
    else if (backFace)
        glCullFace(GL_BACK);
}

void ZGLGraphics::CullFaces(const std::initializer_list<ZFaceCullState>& faceCullState)
{
    bool frontFace = false, backFace = false;
    for (auto faceCull : faceCullState) {
        if (faceCull == ZFaceCullState::Front) frontFace = true;
        else if (faceCull == ZFaceCullState::Back) backFace = true;
    }
    if (frontFace && backFace)
        glCullFace(GL_FRONT_AND_BACK);
    else if (frontFace)
        glCullFace(GL_FRONT);
    else if (backFace)
        glCullFace(GL_BACK);
}

void ZGLGraphics::SetDepthStencilState(uint8_t depthStencilState)
{
    bool depthEnabled = depthStencilState & static_cast<uint8_t>(ZDepthStencilState::Depth);
    bool stencilEnabled = depthStencilState & static_cast<uint8_t>(ZDepthStencilState::Stencil);

    if (depthEnabled) {
        EnableDepthTesting();
    }
    else {
        DisableDepthTesting();
    }

    if (stencilEnabled) {
        EnableStencilTesting();
        EnableStencilBuffer();
    }
    else {
        DisableStencilBuffer();
        DisableStencilTesting();
    }
}

void ZGLGraphics::SetDepthStencilState(const std::initializer_list<ZDepthStencilState>& depthStencilState)
{
    bool depthEnabled = false, stencilEnabled = false;
    for (auto state : depthStencilState) {
        if (state == ZDepthStencilState::Depth) depthEnabled = true;
        if (state == ZDepthStencilState::Stencil) stencilEnabled = true;

    }

    if (depthEnabled) {
        EnableDepthTesting();
    }
    else {
        DisableDepthTesting();
    }

    if (stencilEnabled) {
        EnableStencilTesting();
        EnableStencilBuffer();
    }
    else {
        DisableStencilBuffer();
        DisableStencilTesting();
    }
}

void ZGLGraphics::SetBlending(ZBlendMode blendMode)
{
    if (blendMode == ZBlendMode::Transluscent)
        EnableAlphaBlending();
    else if (blendMode == ZBlendMode::Additive)
        EnableAdditiveBlending();
    else
        DisableBlending();
}

void ZGLGraphics::Draw(const std::shared_ptr<ZVertexBuffer>& bufferData, ZMeshDrawStyle drawStyle)
{
    bufferData->Bind();
    if (bufferData->indexCount > 0)
    {
        if (bufferData->instanceCount > 1)
        {
            glDrawElementsInstanced(drawingStylesMap_[drawStyle], bufferData->indexCount, GL_UNSIGNED_INT, 0, bufferData->instanceCount);
        }
        else
        {
            glDrawElements(drawingStylesMap_[drawStyle], bufferData->indexCount, GL_UNSIGNED_INT, 0);
        }
    }
    else
    {
        if (bufferData->instanceCount > 1)
        {
            glDrawArraysInstanced(drawingStylesMap_[drawStyle], 0, bufferData->vertexCount, bufferData->instanceCount);
        }
        else
        {
            glDrawArrays(drawingStylesMap_[drawStyle], 0, bufferData->vertexCount);
        }
    }
    bufferData->Unbind();
    glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphics::GLFWErrorCallback(int id, const char* description)
{
    LOG(description, ZSeverity::Error);
}
