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
#include "ZAssets.hpp"
#include "ZFramebuffer.hpp"
#include "ZMesh.hpp"
#include "ZScene.hpp"
#include "ZSkybox.hpp"
#include "ZCamera.hpp"
#include "ZRenderQueue.hpp"
#include "ZRenderStateGroup.hpp"
#include "ZUniformBuffer.hpp"

// TODO: Figure out how to move engine passes outside of static scope so the
// we can use ZServices::AssetStore shaders and other renderpasses without issue, otherwise, we face
// static initialization problems
std::shared_ptr<ZRenderPass> ZRenderPass::Depth()
{
    static bool initialized = false;
    static std::shared_ptr<ZRenderPass> depth = std::make_shared<ZDepthPass>(
        ZAssets::ShaderManager()->Create("/Shaders/Vertex/depth.vert", "/Shaders/Pixel/null.frag"),
        ZFramebuffer::CreateDepth(glm::vec2(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE))
        );
    if (!initialized)
    {
        depth->Initialize();
        initialized = true;
    }
    return depth;
}


std::shared_ptr<ZRenderPass> ZRenderPass::Shadow()
{
    static bool initialized = false;
    static std::shared_ptr<ZRenderPass> shadow = std::make_shared<ZShadowPass>(
        ZAssets::ShaderManager()->Create("/Shaders/Vertex/shadow.vert", "/Shaders/Pixel/null.frag"),
        ZFramebuffer::CreateShadow(glm::vec2(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE), NUM_SHADOW_CASCADES)
        );
    if (!initialized) {
        shadow->Initialize();
        initialized = true;
    }
    return shadow;
}

std::shared_ptr<ZRenderPass> ZRenderPass::Color(const glm::vec2& size)
{
    static bool initialized = false;
    static std::shared_ptr<ZRenderPass> color = std::make_shared<ZColorPass>(
        ZAssets::ShaderManager()->Create("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/blinnphong.frag"),
        ZFramebuffer::CreateColor(glm::vec2(1920.f, 1080.f)), true,
        ZRenderPass::Depth(), ZRenderPass::Shadow()
        );
    if (!initialized) {
        color->Initialize();
        initialized = true;
    }
    if (size != glm::vec2(0.f) && size != color->size_) {
        color->SetSize(size);
    }
    return color;
}

std::shared_ptr<ZRenderPass> ZRenderPass::Post(const glm::vec2& size)
{
    static bool initialized = false;
    static std::shared_ptr<ZRenderPass> post = std::make_shared<ZPostPass>(
        ZAssets::ShaderManager()->Create("/Shaders/Vertex/postprocess.vert", "/Shaders/Pixel/postprocess.frag"),
        ZFramebuffer::CreateColor(glm::vec2(1920.f, 1080.f)),
        ZRenderPass::Color(), ZRenderPass::Depth(), ZRenderPass::Shadow()
        );
    if (!initialized)
    {
        post->Initialize();
        initialized = true;
    }
    if (size != glm::vec2(0.f) && size != post->size_)
    {
        post->SetSize(size);
    }
    return post;
}

std::shared_ptr<ZRenderPass> ZRenderPass::UI(const glm::vec2& size)
{
    static bool initialized = false;
    static std::shared_ptr<ZRenderPass> ui = std::make_shared<ZUIPass>(
        ZAssets::ShaderManager()->Create("/Shaders/Vertex/ui.vert", "/Shaders/Pixel/ui.frag"),
        ZFramebuffer::CreateColor(glm::vec2(1920.f, 1080.f)),
        ZRenderPass::Post()
        );
    if (!initialized)
    {
        ui->Initialize();
        initialized = true;
    }
    if (size != glm::vec2(0.f) && size != ui->size_)
    {
        ui->SetSize(size);
    }
    return ui;
}

/****************** Render Pass Base ***********************/

ZRenderPass::ZRenderPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, const std::initializer_list<ZRenderPass::ptr>& dependencies)
    : shader_(shader), framebuffer_(fbo), dependencies_(dependencies), size_(0.f)
{ }

void ZRenderPass::Initialize()
{
    renderQueue_ = ZRenderQueue::Create();
    size_ = framebuffer_->Size();

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.SetShader(shader_);
    std::unordered_map<std::string, unsigned int> attachmentCount;
    for (const auto& dep : dependencies_)
    {
        for (const auto& attachment : dep->framebuffer_->Attachments())
        {
            writer.BindTexture(attachment);
        }
    }
    renderState_ = writer.End();

    clearFlags_ |= (static_cast<uint8_t>(ZClearFlags::Color) | static_cast<uint8_t>(ZClearFlags::Depth) | static_cast<uint8_t>(ZClearFlags::Stencil));
}

void ZRenderPass::Submit(const std::shared_ptr<ZRenderTask>& task)
{
    renderQueue_->Add(task);
}

void ZRenderPass::Render(double deltaTime, const std::shared_ptr<ZScene>& scene, const std::shared_ptr<ZFramebuffer>& target)
{
    if (renderQueue_->Empty()) return;

    Prepare(scene);
    Perform(deltaTime, scene);
    Resolve(target);
}

void ZRenderPass::Prepare(const std::shared_ptr<ZScene>& scene)
{
    ZServices::Graphics()->ClearViewport(scene->GameConfig().graphics.clearColor, clearFlags_);
    ZServices::Graphics()->UpdateViewport(size_);
}

/****************** Depth Pass ***********************/

void ZDepthPass::Initialize()
{
    ZRenderPass::Initialize();

    //clearFlags_ = static_cast<uint8_t>(ZClearFlags::Depth);
}

void ZDepthPass::Prepare(const std::shared_ptr<ZScene>& scene)
{
    framebuffer_->Bind();
    framebuffer_->BindAttachment();

    ZRenderPass::Prepare(scene);
}

void ZDepthPass::Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)
{
    renderQueue_->Submit();
}

/****************** Shadow Pass ***********************/

void ZShadowPass::Initialize()
{
    ZRenderPass::Initialize();

    uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::Light, sizeof(ZLightUniforms));

    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.SetFaceCullState({ ZFaceCullState::Front });
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();
}

void ZShadowPass::Prepare(const std::shared_ptr<ZScene>& scene)
{
    framebuffer_->Bind();
    framebuffer_->BindAttachment();

    ZRenderPass::Prepare(scene);
}

void ZShadowPass::Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)
{
    clearFlags_ = static_cast<uint8_t>(ZClearFlags::Depth);
    auto lights = scene->GameLights();
    for (int j = 0; j < NUM_SHADOW_CASCADES; j++)
    {
        if (!lights.empty())
        {
            auto lightspaceMatrix = ZAssets::GameObjectManager()->Dereference<ZLight>(*lights.begin())->lightspaceMatrices[j];
            uniformBuffer_->Update(offsetof(ZLightUniforms, ViewProjectionLightSpace), sizeof(lightspaceMatrix), glm::value_ptr(lightspaceMatrix));
        }
        framebuffer_->BindAttachmentLayer(j);
        ZServices::Graphics()->ClearViewport(scene->GameConfig().graphics.clearColor, clearFlags_);
        renderQueue_->Submit(j == NUM_SHADOW_CASCADES - 1);
    }
}

/****************** Color Pass ***********************/

void ZColorPass::Initialize()
{
    ZRenderPass::Initialize();

    if (multisample_)
        multisampledFramebuffer_ = ZFramebuffer::CreateColor(size_, true);
}

void ZColorPass::Prepare(const std::shared_ptr<ZScene>& scene)
{
    if (multisample_) {
        multisampledFramebuffer_->Resize(size_.x, size_.y);
        multisampledFramebuffer_->Bind();
        multisampledFramebuffer_->BindAttachment();
    }
    else {
        framebuffer_->Bind();
        framebuffer_->BindAttachment();
    }
    framebuffer_->Resize(size_.x, size_.y);

    ZRenderPass::Prepare(scene);
}

void ZColorPass::Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)
{
    renderQueue_->Submit();
}

void ZColorPass::Resolve(const std::shared_ptr<ZFramebuffer>& target)
{
    if (multisample_) {
        multisampledFramebuffer_->Blit(framebuffer_);
    }
}

/****************** Post Pass ***********************/

void ZPostPass::Initialize()
{
    ZRenderPass::Initialize();

    uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::Post, sizeof(ZPostUniforms));

    bool hasMotionBlur = ZServices::Graphics()->HasMotionBlur();
    uniformBuffer_->Update(offsetof(ZPostUniforms, useMotionBlur), sizeof(hasMotionBlur), &hasMotionBlur);

    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.SetDepthStencilState({ ZDepthStencilState::None });
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();
}

void ZPostPass::Prepare(const std::shared_ptr<ZScene>& scene)
{
    framebuffer_->Bind();
    framebuffer_->BindAttachment();

    framebuffer_->Resize(size_.x, size_.y);

    ZRenderPass::Prepare(scene);
}

void ZPostPass::Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)
{
    renderQueue_->Submit();
}

void ZPostPass::Resolve(const std::shared_ptr<ZFramebuffer>& target)
{
    if (target)
        target->Resize(size_.x, size_.y);
    framebuffer_->Blit(target);
}

/****************** UI Pass ***********************/

void ZUIPass::Prepare(const std::shared_ptr<ZScene>& scene)
{
    framebuffer_->Bind();
    framebuffer_->BindAttachment();

    framebuffer_->Resize(size_.x, size_.y);

    ZRenderPass::Prepare(scene);
}

void ZUIPass::Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)
{
    renderQueue_->Submit();
}

void ZUIPass::Resolve(const std::shared_ptr<ZFramebuffer>& target)
{
    if (target)
        target->Resize(size_.x, size_.y);
    framebuffer_->Blit(target);
}
