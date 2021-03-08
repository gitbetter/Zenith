/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderQueue.hpp

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

#pragma once

// Includes
#include "ZCommon.hpp"

class ZRenderTask;
class ZRenderStateExecutor;

class ZRenderQueue
{

    using ZRenderTaskEntry = std::pair<uint64_t, std::shared_ptr<ZRenderTask>>;

public:

    ZRenderQueue() { }
    ~ZRenderQueue() { }

    void Initialize();

    bool Empty() { return queue_.empty(); }

    void Add(const std::shared_ptr<ZRenderTask>& task);
    void Submit(bool flush = true);

    static std::shared_ptr<ZRenderQueue> Create();

protected:

    std::vector<ZRenderTaskEntry> queue_;
    std::shared_ptr<ZRenderStateExecutor> executor_ = nullptr;

    void Execute(const std::shared_ptr<ZRenderTask>& task);
    uint64_t GenerateKey(const std::shared_ptr<ZRenderTask>& task);

};