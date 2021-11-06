/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderQueue.cpp

    Created by Adrian Sanchez on 03/03/2021.
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

#include "ZRenderQueue.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZRenderTask.hpp"
#include "ZRenderStateExecutor.hpp"
#include "ZVertexBuffer.hpp"

void ZRenderQueue::Initialize()
{
    executor_ = ZServices::Graphics()->Executor();
}

void ZRenderQueue::Add(const std::shared_ptr<ZRenderTask>& task)
{
    queue_.emplace_back( GenerateKey(task), task );
}

void ZRenderQueue::Submit(bool flush)
{
    ZPR_SESSION_COLLECT_DRAWS(queue_.size());

    std::sort(queue_.begin(), queue_.end());
    for (const auto& [key, task] : queue_)
    {
        Execute(task);
    }
    if (flush)
        queue_.clear();
}

void ZRenderQueue::Execute(const std::shared_ptr<ZRenderTask>& task)
{
    if (executor_)
    {
        (*executor_)(task->pipelineState_);
        (*executor_)(task->resourceState_);
        (*executor_)(task->drawCall_, task->resourceState_.vertexBuffer);
    }
}

uint64_t ZRenderQueue::GenerateKey(const std::shared_ptr<ZRenderTask>& task)
{
    uint64_t key = 0;
    key |= static_cast<uint64_t>(task->fullscreenLayer_ & 0x0f) << 47;
    key |= static_cast<uint64_t>(task->renderLayer_ & 0x0f) << 43;
    key |= static_cast<uint64_t>((static_cast<uint8_t>(task->pipelineState_.blendState) & 0x07)) << 40;
    if (task->pipelineState_.blendState == ZBlendMode::Opaque)
    {
        key |= static_cast<uint64_t>(static_cast<uint16_t>(ZAssets::ShaderManager()->ID(task->resourceState_.shader))) << 24;
        key |= static_cast<uint64_t>((task->renderDepth_ & 0xffffff));
    }
    else
    {

        if (task->fullscreenLayer_ & static_cast<uint8_t>(ZFullScreenLayer::UI))
        {
            // HUD-style UI elements use a zOrder in place of the renderDepth, where higher zOrdere (renderDepth) elements
            // should be rendered in front as opposed to in back, so we order like opaque objects
            key |= static_cast<uint64_t>((task->renderDepth_ & 0xffffff)) << 16;
        }
        else
        {
            // scale and renormalize render depth to make translucent objects monotonically decreasing instead of increasing,
            // in order to render translucency back to front.
            key |= static_cast<uint64_t>((static_cast<uint32_t>(1.f / static_cast<float>(task->renderDepth_ + 1) * 100000.f) & 0xffffff)) << 16;
        }
        key |= static_cast<uint64_t>(static_cast<uint16_t>(ZAssets::ShaderManager()->ID(task->resourceState_.shader)));
    }
    return key;
}

std::shared_ptr<ZRenderQueue> ZRenderQueue::Create()
{
    auto queue = std::make_shared<ZRenderQueue>();
    queue->Initialize();
    return queue;
}
