/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderStateExecutor.cpp

    Created by Adrian Sanchez on 03/04/2021.
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

#include "ZRenderStateExecutor.hpp"
#include "ZTexture.hpp"
#include "ZUniformBuffer.hpp"
#include "ZServices.hpp"

void ZRenderStateExecutor::operator()(const ZRenderPipelineState& pipelineState)
{
    if (pipelineState.clearState != cachedState_->pipelineState_.clearState) {
        ZServices::Graphics()->ClearViewport(glm::vec4(0.f), pipelineState.clearState);
        cachedState_->pipelineState_.clearState = pipelineState.clearState;
    }
    if (pipelineState.blendState != cachedState_->pipelineState_.blendState) {
        ZServices::Graphics()->SetBlending(pipelineState.blendState);
        cachedState_->pipelineState_.blendState = pipelineState.blendState;
    }
    if (pipelineState.depthStencilState != cachedState_->pipelineState_.depthStencilState) {
        ZServices::Graphics()->SetDepthStencilState(pipelineState.depthStencilState);
        cachedState_->pipelineState_.depthStencilState = pipelineState.depthStencilState;
    }
    if (pipelineState.faceCullState != cachedState_->pipelineState_.faceCullState) {
        ZServices::Graphics()->CullFaces(pipelineState.faceCullState);
        cachedState_->pipelineState_.faceCullState = pipelineState.faceCullState;
    }
}

void ZRenderStateExecutor::operator()(const ZRenderResourceState& resourceState)
{
    if (resourceState.shader && cachedState_->resourceState_.shader != resourceState.shader) {
        cachedState_->resourceState_.shader = resourceState.shader;
        ZServices::ShaderManager()->Activate(resourceState.shader);
        ZServices::ShaderManager()->ClearAttachments(resourceState.shader);
        // Reset cached list of textures and ubos once the shader changes, to make
        // sure textures/ubos are updated between shader state changes
        cachedState_->resourceState_.textures.fill(ZHTexture());
        cachedState_->resourceState_.uniformBuffers.fill(nullptr);
    }

    std::unordered_map<std::string, unsigned int> attachmentCount;
    for (auto i = 0; i < MAX_TEXTURE_SLOTS; i++) {
        if (resourceState.textures[i] && resourceState.textures[i] != cachedState_->resourceState_.textures[i]) {
            std::string textureType = ZServices::TextureManager()->Type(resourceState.textures[i]);
            std::string uniformName = textureType + "Sampler" + std::to_string(attachmentCount[textureType]++);
            ZServices::ShaderManager()->BindAttachment(cachedState_->resourceState_.shader, uniformName, resourceState.textures[i]);
            cachedState_->resourceState_.textures[i] = resourceState.textures[i];
        }
    }

    for (auto i = 0; i < MAX_UBO_SLOTS; i++) {
        if (resourceState.uniformBuffers[i] && resourceState.uniformBuffers[i] != cachedState_->resourceState_.uniformBuffers[i]) {
            resourceState.uniformBuffers[i]->Bind();
            cachedState_->resourceState_.uniformBuffers[i] = resourceState.uniformBuffers[i];
        }
    }
}

void ZRenderStateExecutor::operator()(ZDrawCall drawCall, const std::shared_ptr<ZVertexBuffer>& vertexBuffer)
{
    ZServices::Graphics()->Draw(vertexBuffer, drawCall.drawStyle_);
}

std::shared_ptr<ZRenderStateExecutor> ZRenderStateExecutor::Create()
{
    auto executor = std::make_shared<ZRenderStateExecutor>();
    executor->cachedState_ = std::make_shared<ZRenderStateGroup>();
    return executor;
}
