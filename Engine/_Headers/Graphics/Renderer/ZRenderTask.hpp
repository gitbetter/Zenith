/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderTask.hpp

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

#pragma once

// Includes
#include "ZRenderStateGroup.hpp"
#include "ZDrawCall.hpp"

class ZRenderQueue;
class ZRenderPass;

class ZRenderTask : public std::enable_shared_from_this<ZRenderTask>
{

    friend class ZRenderQueue;

public:

    ZRenderTask();
    ~ZRenderTask();

    void Submit(const std::initializer_list<std::shared_ptr<ZRenderPass>>& passes);

    static std::shared_ptr<ZRenderTask> Compile(ZDrawCall drawCall, const std::initializer_list<std::shared_ptr<ZRenderStateGroup>>& stateStack, const std::shared_ptr<ZRenderPass>& pass = nullptr);

protected:

    ZRenderResourceState resourceState_;
    ZRenderPipelineState pipelineState_;

    uint8_t fullscreenLayer_ = (uint8_t)ZFullScreenLayer::Null;
    uint8_t renderLayer_ = (uint8_t)ZRenderLayer::Null;
    uint32_t renderDepth_ = 0;

    ZDrawCall drawCall_;

    void ApplyState(const std::shared_ptr<ZRenderStateGroup>& state);

};