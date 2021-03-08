/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderStateGroup.hpp

    Created by Adrian Sanchez on 03/01/2021.
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
#include "ZUniformBuffer.hpp"

class ZVertexBuffer;
class ZUniformBuffer;

// Class and Data Structure Definitions
struct ZRenderPipelineState
{
    ZBlendMode blendState = ZBlendMode::Null;
    uint16_t depthStencilState = (uint8_t)ZDepthStencilState::Null;
    uint8_t faceCullState = (uint8_t)ZFaceCullState::Null;
    uint8_t clearState = (uint8_t)ZClearFlags::Null;
};

struct ZRenderResourceState
{
    ZTextureList textures;
    ZUBOList uniformBuffers;
    std::shared_ptr<ZVertexBuffer> vertexBuffer = nullptr;
    std::shared_ptr<ZShader> shader = nullptr;
};

class ZRenderStateGroup
{

    friend class ZRenderPass;
    friend class ZRenderStateGroupWriter;
    friend class ZRenderTask;
    friend class ZRenderStateExecutor;

public:

    ZRenderStateGroup();
    ~ZRenderStateGroup();

    static std::shared_ptr<ZRenderStateGroup> Default();

protected:

    ZRenderResourceState resourceState_;
    ZRenderPipelineState pipelineState_;

    uint8_t fullscreenLayer_ = (uint8_t)ZFullScreenLayer::Null;
    uint8_t renderLayer_ = (uint8_t)ZRenderLayer::Null;
    uint32_t renderDepth_ = 0;

};

class ZRenderStateGroupWriter
{

public:

    ZRenderStateGroupWriter(const std::shared_ptr<ZRenderStateGroup>& stateGroup = nullptr)
        : currentStateGroup_(stateGroup)
    { }
    ~ZRenderStateGroupWriter() { }

    void Begin();
    void SetBlending(ZBlendMode blendMode);
    void SetShader(const std::shared_ptr<ZShader>& shader);
    void BindUniformBuffer(const std::shared_ptr<ZUniformBuffer>& uniformBuffer);
    void BindVertexBuffer(const std::shared_ptr<ZVertexBuffer>& vertexBuffer);
    void BindTexture(const std::shared_ptr<ZTexture>& texture);
    void ClearTextures();
    void SetRenderLayer(ZRenderLayer layer);
    void SetFullScreenLayer(ZFullScreenLayer layer);
    void SetClearFlags(const std::initializer_list<ZClearFlags>& clearFlags);
    void SetDepthStencilState(const std::initializer_list<ZDepthStencilState>& depthStencilStates);
    void SetFaceCullState(const std::initializer_list<ZFaceCullState>& faceCullStates);
    void SetRenderDepth(uint32_t depth);
    std::shared_ptr<ZRenderStateGroup> End();

protected:

    std::shared_ptr<ZRenderStateGroup> currentStateGroup_;
    uint16_t currentTextureBindPoint_ = static_cast<uint16_t>(ZTextureBindPoint::User0);
    uint16_t currentUBOBindPoint_ = static_cast<uint16_t>(ZUniformBufferType::UserDefined);

};