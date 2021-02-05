/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZRenderPass.cpp

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

#include "ZRenderPass.hpp"
#include "ZServices.hpp"
#include "ZFramebuffer.hpp"
#include "ZShader.hpp"
#include "ZMesh2D.hpp"
#include "ZScene.hpp"
#include "ZRenderable.hpp"

ZRenderPass::ZRenderPass(const std::shared_ptr<ZRenderable>& root, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp,
    const glm::vec2& size, bool multisample, std::initializer_list<ZRenderPass::ptr> dependencies)
    : root_(root), shader_(shader), renderOp_(renderOp), size_(size), multisample_(multisample), dependencies_(dependencies)
{ }

void ZRenderPass::Initialize()
{
    switch (renderOp_) {
    case ZRenderOp::Post:
        screenQuad_ = ZMesh2D::NewQuad();
    case ZRenderOp::UI:
    case ZRenderOp::Color:
        if (multisample_)
            multisampledFramebuffer_ = ZFramebuffer::CreateColor(size_, true);
        framebuffer_ = ZFramebuffer::CreateColor(size_);
        break;
    case ZRenderOp::Depth:        
    case ZRenderOp::Shadow:
        framebuffer_ = ZFramebuffer::CreateDepth(size_);
        break;
    default:
        framebuffer_ = ZFramebuffer::CreateColor(size_);
        break;
    }
}

void ZRenderPass::Perform(double deltaTime, const std::shared_ptr<ZScene>& scene, const std::shared_ptr<ZFramebuffer>& target)
{
    if (!root_->Renderable()) return;

    framebuffer_->Resize(size_.x, size_.y);

    if (multisample_) {
        multisampledFramebuffer_->Resize(size_.x, size_.y, true);
        multisampledFramebuffer_->Bind();
    }
    else {
        framebuffer_->Bind();
    }

    ZServices::Graphics()->ClearViewport();
    ZServices::Graphics()->UpdateViewport(size_);

    if (renderOp_ == ZRenderOp::Depth || renderOp_ == ZRenderOp::Shadow) {
        ZServices::Graphics()->ClearDepth();
    }
    else if (renderOp_ == ZRenderOp::Post) {
        ZServices::Graphics()->DisableDepthTesting();
    }

    shader_->Activate();
    BindDependencies();

    if (renderOp_ == ZRenderOp::Post) {
        PreparePostProcessing(scene);
        screenQuad_->Render(shader_);
    }
    else {
        root_->Render(deltaTime, shader_, renderOp_);
    }

    if (renderOp_ == ZRenderOp::Post) {
        ZServices::Graphics()->EnableDepthTesting();
    }

    if (multisample_) {
        multisampledFramebuffer_->Blit(framebuffer_);
    }

    // TODO: Use a better method to determine when to blit the current renderpass
    // framebuffer onto the default framebuffer
    if (renderOp_ == ZRenderOp::UI) {
        framebuffer_->Blit(target);
    }
}

void ZRenderPass::BindDependencies()
{
    unsigned int depthAttachments = 0;
    unsigned int colorAttachments = 0;
    unsigned int shadowAttachments = 0;
    unsigned int index = 0;
    for (auto dep : dependencies_) {
        dep->framebuffer_->Attachment()->Bind(index);
        switch (dep->renderOp_) {
        case ZRenderOp::Post:
        case ZRenderOp::Color:
            shader_->SetInt(std::string("colorSampler") + std::to_string(colorAttachments++), index);
            break;
        case ZRenderOp::Shadow:
            shader_->SetInt(std::string("shadowSampler") + std::to_string(shadowAttachments++), index);
            break;
        case ZRenderOp::Depth:
            shader_->SetInt(std::string("depthSampler") + std::to_string(depthAttachments++), index);
            break;
        }
        ++index;
    }
}

void ZRenderPass::PreparePostProcessing(const std::shared_ptr<ZScene>& scene)
{
    shader_->SetMat4("previousViewProjection", scene->PreviousViewProjection());
    shader_->SetMat4("inverseViewProjection", glm::inverse(scene->ViewProjection()));
    shader_->SetBool("useMotionBlur", ZServices::Graphics()->HasMotionBlur());
}
