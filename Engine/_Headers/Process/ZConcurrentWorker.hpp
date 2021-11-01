/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZConcurrentWorker.hpp

    Created by Adrian Sanchez on 05/09/2019.
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
 along with Zenith. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "ZProcess.hpp"

class ZConcurrentWorker : public ZProcess, public std::enable_shared_from_this<ZConcurrentWorker>
{

private:

    static void ThreadStarter(std::shared_ptr<ZConcurrentWorker> process);

public:

    ZConcurrentWorker() { }
    virtual ~ZConcurrentWorker() { if (processThread_.joinable()) processThread_.join(); }

    void Initialize() override;
    void Start();

protected:

    std::thread processThread_;

    virtual void Run() = 0;

};
