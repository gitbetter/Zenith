/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRenderPass.hpp

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
#include "ZCommon.hpp"

// Forward Declarations
class ZRenderable;
class ZFramebuffer;
class ZShader;
class ZMesh2D;

// Class and Data Structure Definitions
class ZRenderPass
{

public:
    
    using ptr = std::shared_ptr<ZRenderPass>;

    template<class ...Args>
    ZRenderPass(const std::shared_ptr<ZRenderable>& root, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp,
        const glm::vec2& size, bool multisample = false, Args... args)
        : ZRenderPass(root, shader, renderOp, size, multisample, { args... }) { }
    ZRenderPass(const std::shared_ptr<ZRenderable>& root, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp,
        const glm::vec2& size, bool multisample = false, std::initializer_list<ZRenderPass::ptr> dependencies = {});
    virtual ~ZRenderPass() { }

    void Initialize();
    void Perform(double deltaTime, const std::shared_ptr<ZFramebuffer>& target = nullptr);

    std::shared_ptr<ZFramebuffer> Framebuffer() { return framebuffer_; }

    void SetSize(const glm::vec2& size) { if (!fixedSize_) size_ = size; }
    void SetIsSizeFixed(bool fixed) { fixedSize_ = fixed; }

protected:

    std::shared_ptr<ZRenderable> root_;
    std::shared_ptr<ZShader> shader_;
    glm::vec2 size_;
    std::shared_ptr<ZFramebuffer> framebuffer_, multisampledFramebuffer_;
    std::shared_ptr<ZMesh2D> screenQuad_;
    std::vector<ZRenderPass::ptr> dependencies_;
    ZRenderOp renderOp_;
    bool multisample_ = false;
    bool fixedSize_ = false;

    void BindDependencies();

};
