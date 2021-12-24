/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderTask.cpp

    Created by Adrian Sanchez on 03/02/2021.
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

#include "ZRenderTask.hpp"
#include "ZDrawCall.hpp"
#include "ZRenderStateGroup.hpp"
#include "ZRenderPass.hpp"

ZRenderTask::ZRenderTask()
{
    resourceState_.uniformBuffers.fill(nullptr);
}

ZRenderTask::~ZRenderTask()
{
    resourceState_.uniformBuffers.fill(nullptr);
}

void ZRenderTask::Submit(const std::initializer_list<std::shared_ptr<ZRenderPass>>& passes)
{
    for (auto pass : passes)
    {
        pass->Submit(shared_from_this());
    }
}

std::shared_ptr<ZRenderTask> ZRenderTask::Compile(ZDrawCall drawCall, const std::initializer_list<std::shared_ptr<ZRenderStateGroup>>& stateStack, const std::shared_ptr<ZRenderPass>& pass)
{
    auto renderTask = std::make_shared<ZRenderTask>();

    renderTask->ApplyState(ZRenderStateGroup::Default());

    if (pass)
    {
        renderTask->ApplyState(pass->RenderState());
    }

    for (auto it = std::rbegin(stateStack); it != std::rend(stateStack); it++)
    {
        if (!(*it)) continue;
        renderTask->ApplyState(*it);
    }

    renderTask->drawCall_ = drawCall;

    return renderTask;
}

void ZRenderTask::ApplyState(const std::shared_ptr<ZRenderStateGroup>& state)
{
    if (state->renderLayer_ != static_cast<uint8_t>(ZRenderLayer::Null))
        renderLayer_ = state->renderLayer_;
    if (state->fullscreenLayer_ != static_cast<uint8_t>(ZFullScreenLayer::Null))
        fullscreenLayer_ = state->fullscreenLayer_;
    if (state->pipelineState_.blendState != ZBlendMode::Null)
        pipelineState_.blendState = state->pipelineState_.blendState;
    if (state->pipelineState_.depthStencilState != static_cast<uint8_t>(ZDepthStencilState::Null))
        pipelineState_.depthStencilState = state->pipelineState_.depthStencilState;
    if (state->pipelineState_.faceCullState != static_cast<uint8_t>(ZFaceCullState::Null))
        pipelineState_.faceCullState = state->pipelineState_.faceCullState;
    if (state->pipelineState_.clearState != static_cast<uint8_t>(ZClearFlags::Null))
        pipelineState_.clearState = state->pipelineState_.clearState;
    if (state->renderDepth_ > 0)
        renderDepth_ = state->renderDepth_;

    if (state->resourceState_.shader)
        resourceState_.shader = state->resourceState_.shader;
    if (state->resourceState_.vertexBuffer)
        resourceState_.vertexBuffer = state->resourceState_.vertexBuffer;

    for (auto i = 0; i < MAX_TEXTURE_SLOTS; i++) {
        if (state->resourceState_.textures[i]) {
            resourceState_.textures[i] = state->resourceState_.textures[i];
        }
    }

    for (auto i = 0; i < MAX_UBO_SLOTS; i++) {
        if (state->resourceState_.uniformBuffers[i]) {
            resourceState_.uniformBuffers[i] = state->resourceState_.uniformBuffers[i];
        }
    }
}