/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderStateGroup.cpp

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

 #include "ZServices.hpp"
#include "ZRenderStateGroup.hpp"
#include "ZUniformBuffer.hpp"
#include "ZTexture.hpp"

ZRenderStateGroup::ZRenderStateGroup()
{
    resourceState_.uniformBuffers.fill(nullptr);
}

ZRenderStateGroup::~ZRenderStateGroup()
{
    resourceState_.uniformBuffers.fill(nullptr);
}

void ZRenderStateGroupWriter::Begin()
{
    if (!currentStateGroup_)
        currentStateGroup_ = std::make_shared<ZRenderStateGroup>();
}

void ZRenderStateGroupWriter::SetBlending(ZBlendMode blendMode)
{
    currentStateGroup_->pipelineState_.blendState = blendMode;
}

void ZRenderStateGroupWriter::SetShader(const ZHShader& shader)
{
    currentStateGroup_->resourceState_.shader = shader;
}

void ZRenderStateGroupWriter::BindUniformBuffer(const std::shared_ptr<ZUniformBuffer>& uniformBuffer)
{
    currentStateGroup_->resourceState_.uniformBuffers[uniformBuffer->Index()] = uniformBuffer;
}

void ZRenderStateGroupWriter::BindVertexBuffer(const std::shared_ptr<ZVertexBuffer>& vertexBuffer)
{
    currentStateGroup_->resourceState_.vertexBuffer = vertexBuffer;
}

void ZRenderStateGroupWriter::BindTexture(const ZHTexture& texture)
{
    std::string type = ZServices::TextureManager()->Type(texture);
    if (type == "depth")
        currentStateGroup_->resourceState_.textures[static_cast<uint8_t>(ZTextureBindPoint::Depth)] = texture;
    else if (type == "shadow" || type == "shadowArray")
        currentStateGroup_->resourceState_.textures[static_cast<uint8_t>(ZTextureBindPoint::Shadow)] = texture;
    else if (type == "color")
        currentStateGroup_->resourceState_.textures[static_cast<uint8_t>(ZTextureBindPoint::Color)] = texture;
    else if (type == "irradiance")
        currentStateGroup_->resourceState_.textures[static_cast<uint8_t>(ZTextureBindPoint::Irradiance)] = texture;
    else if (type == "prefilter")
        currentStateGroup_->resourceState_.textures[static_cast<uint8_t>(ZTextureBindPoint::Prefilter)] = texture;
    else if (type == "lut")
        currentStateGroup_->resourceState_.textures[static_cast<uint8_t>(ZTextureBindPoint::BRDFLUT)] = texture;
    else {
        currentStateGroup_->resourceState_.textures[currentTextureBindPoint_] = texture;
        if (++currentTextureBindPoint_ >= MAX_TEXTURE_SLOTS)
            currentTextureBindPoint_ = static_cast<uint16_t>(ZTextureBindPoint::User0);
    }
}

void ZRenderStateGroupWriter::ClearTextures()
{
    currentStateGroup_->resourceState_.textures.fill(ZHTexture());
}

void ZRenderStateGroupWriter::SetRenderLayer(ZRenderLayer layer)
{
    currentStateGroup_->renderLayer_ = static_cast<uint8_t>(layer);
}

void ZRenderStateGroupWriter::SetFullScreenLayer(ZFullScreenLayer layer)
{
    currentStateGroup_->fullscreenLayer_ = static_cast<uint8_t>(layer);
}

void ZRenderStateGroupWriter::SetClearFlags(const std::initializer_list<ZClearFlags>& clearFlags)
{
    currentStateGroup_->pipelineState_.clearState = static_cast<uint8_t>(ZClearFlags::Null);
    for (auto flag : clearFlags) {
        currentStateGroup_->pipelineState_.clearState |= static_cast<uint8_t>(flag);
    }
}

void ZRenderStateGroupWriter::SetDepthStencilState(const std::initializer_list<ZDepthStencilState>& depthStencilState)
{
    currentStateGroup_->pipelineState_.depthStencilState = static_cast<uint8_t>(ZDepthStencilState::Null);
    for (auto state : depthStencilState) {
        currentStateGroup_->pipelineState_.depthStencilState |= static_cast<uint8_t>(state);
    }
}

void ZRenderStateGroupWriter::SetFaceCullState(const std::initializer_list<ZFaceCullState>& faceCullStates)
{
    currentStateGroup_->pipelineState_.faceCullState = static_cast<uint8_t>(ZFaceCullState::Null);
    for (auto state : faceCullStates) {
        currentStateGroup_->pipelineState_.faceCullState |= static_cast<uint8_t>(state);
    }
}

void ZRenderStateGroupWriter::SetRenderDepth(uint32_t depth)
{
    currentStateGroup_->renderDepth_ = depth;
}

std::shared_ptr<ZRenderStateGroup> ZRenderStateGroupWriter::End()
{
    auto stateGroup = currentStateGroup_;
    currentStateGroup_ = nullptr;
    return stateGroup;
}

std::shared_ptr<ZRenderStateGroup> ZRenderStateGroup::Default()
{
     ZRenderStateGroupWriter writer;
     writer.Begin();
     writer.SetBlending(ZBlendMode::Opaque);
     writer.SetRenderLayer(ZRenderLayer::Invisible);
     writer.SetFullScreenLayer(ZFullScreenLayer::Game);
     writer.SetDepthStencilState({ ZDepthStencilState::Depth });
     writer.SetFaceCullState({ ZFaceCullState::Back });
     static std::shared_ptr<ZRenderStateGroup> defaultStateGroup = writer.End();

     return defaultStateGroup;
}
