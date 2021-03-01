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
#include "ZMesh.hpp"
#include "ZScene.hpp"
#include "ZRenderable.hpp"
#include "ZSkybox.hpp"

ZRenderPass::ZRenderPass(const std::shared_ptr<ZRenderable>& root, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp,
    const glm::vec2& size, bool multisample, std::initializer_list<ZRenderPass::ptr> dependencies)
    : root_(root), shader_(shader), renderOp_(renderOp), size_(size), multisample_(multisample), dependencies_(dependencies)
{ }

void ZRenderPass::Initialize()
{
    switch (renderOp_) {
    case ZRenderOp::Post:
        screenTri_ = ZMesh2D::NewScreenTriangle();
    case ZRenderOp::UI:
    case ZRenderOp::Color:
        if (multisample_)
            multisampledFramebuffer_ = ZFramebuffer::CreateColor(size_, true);
        framebuffer_ = ZFramebuffer::CreateColor(size_);
        break;
    case ZRenderOp::Depth:        
        framebuffer_ = ZFramebuffer::CreateDepth(size_);
        break;
    case ZRenderOp::Shadow:
        framebuffer_ = ZFramebuffer::CreateShadow(size_, NUM_SHADOW_CASCADES);
        break;
    default:
        framebuffer_ = ZFramebuffer::CreateColor(size_);
        break;
    }
}

void ZRenderPass::Perform(double deltaTime, const std::shared_ptr<ZScene>& scene, const std::shared_ptr<ZFramebuffer>& target)
{
    if (!root_->Renderable()) return;

    for (auto i = 0; i < framebuffer_->Attachments().size(); i++) {
        if (!fixedSize_) {
            framebuffer_->Resize(size_.x, size_.y);
        }

        if (multisample_) {
            multisampledFramebuffer_->Resize(size_.x, size_.y);
            multisampledFramebuffer_->Bind();
            multisampledFramebuffer_->BindAttachment(i);
        }
        else {
            framebuffer_->Bind();
            framebuffer_->BindAttachment(i);
        }

        if (renderOp_ == ZRenderOp::Post) {
            ZServices::Graphics()->DisableDepthTesting();
        }
        else if (renderOp_ == ZRenderOp::Depth) {
            ZServices::Graphics()->ClearDepth();
        }
        else if (renderOp_ == ZRenderOp::Shadow) {
            ZServices::Graphics()->CullFrontFaces();
        }

        ZServices::Graphics()->ClearViewport(scene->GameConfig().graphics.clearColor);
        ZServices::Graphics()->UpdateViewport(size_);

        shader_->Activate();
        shader_->ClearAttachments();

        BindDependencies();

        if (renderOp_ == ZRenderOp::Post) {
            PreparePostProcessingShader(scene);
        }

        if (renderOp_ == ZRenderOp::Post) {
            shader_->BindAttachments();
            screenTri_->Render(shader_);
        }
        else if (renderOp_ == ZRenderOp::Shadow) {
            auto fb = multisample_ ? multisampledFramebuffer_ : framebuffer_;
            for (int j = 0; j < NUM_SHADOW_CASCADES; j++) {
                PrepareShadowShader(scene, j);
                fb->BindAttachmentLayer(j);
                ZServices::Graphics()->ClearDepth();
                root_->Render(deltaTime, shader_, renderOp_);
            }
        }
        else {
            PrepareIBLAttachments(scene);
            root_->Render(deltaTime, shader_, renderOp_);
        }

        if (renderOp_ == ZRenderOp::Post) {
            ZServices::Graphics()->EnableDepthTesting();
        }

        if (renderOp_ == ZRenderOp::Shadow) {
            ZServices::Graphics()->CullBackFaces();
        }

        if (multisample_) {
            multisampledFramebuffer_->Blit(framebuffer_);
        }
    }

    // TODO: Use a better method to determine when to blit the current renderpass
    // framebuffer onto the default framebuffer
    if (renderOp_ == ZRenderOp::UI || renderOp_ == ZRenderOp::Post) {
        if (target) target->Resize(size_.x, size_.y);
        framebuffer_->Blit(target);
    }
}

void ZRenderPass::BindDependencies()
{
    std::unordered_map<std::string, unsigned int> attachmentCount;
    for (const auto& dep : dependencies_) {
        for (const auto& attachment : dep->framebuffer_->Attachments()) {
            std::string uniformName = attachment->type + "Sampler" + std::to_string(attachmentCount[attachment->type]++);
            shader_->AddAttachment(uniformName, attachment);
        }
    }
}

void ZRenderPass::PrepareIBLAttachments(const std::shared_ptr<ZScene>& scene)
{
    if (scene->Skybox())
    {
        ZIBLTexture iblTexture = scene->Skybox()->IBLTexture();
        if (iblTexture.irradiance) {
            shader_->AddAttachment("irradianceMap", iblTexture.irradiance);
        }
        if (iblTexture.prefiltered) {
            shader_->AddAttachment("prefilterMap", iblTexture.prefiltered);
        }
        if (iblTexture.brdfLUT) {
            shader_->AddAttachment("brdfLUT", iblTexture.brdfLUT);
        }
    }
}

void ZRenderPass::PreparePostProcessingShader(const std::shared_ptr<ZScene>& scene)
{
    shader_->SetMat4("previousViewProjection", scene->PreviousViewProjection());
    shader_->SetMat4("inverseViewProjection", glm::inverse(scene->ViewProjection()));
    shader_->SetBool("useMotionBlur", ZServices::Graphics()->HasMotionBlur());
}

void ZRenderPass::PrepareShadowShader(const std::shared_ptr<ZScene>& scene, int cascade)
{
    auto lights = scene->GameLights();
    if (!lights.empty()) {
        shader_->SetMat4("ViewProjectionLightSpace", lights.begin()->second->LightSpaceMatrices()[cascade]);
    }
}
