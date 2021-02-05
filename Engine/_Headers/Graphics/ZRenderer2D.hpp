/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderer2D.hpp

    Created by Adrian Sanchez on 29/01/2021.
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
#include "ZRenderPass.hpp"

// Forward Declarations
class ZScene;

// Class and Data Structure Definitions
class ZRenderer2D
{

public:

    ZRenderer2D(const std::shared_ptr<ZScene>& scene) : scene_(scene) { };
    ~ZRenderer2D() = default;

    const std::vector<ZRenderPass::ptr>& Passes() const { return passes_; }

    void SetTarget(const std::shared_ptr<ZFramebuffer>& target) { target_ = target; }

    void Render(double deltaTime);

    void AddPass(const ZRenderPass::ptr & pass);

private:

    std::vector<ZRenderPass::ptr> passes_;
    std::shared_ptr<ZFramebuffer> target_ = nullptr;
    std::shared_ptr<ZScene> scene_ = nullptr;

};
