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

#include "ZCommon.hpp"

class ZFramebuffer;
class ZMesh2D;
class ZRenderQueue;
class ZRenderStateGroup;
class ZRenderTask;

class ZRenderPass
{

public:
    
    using ptr = std::shared_ptr<ZRenderPass>;

    template<class ...Args>
    ZRenderPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, Args... args)
        : ZRenderPass(shader, fbo, { args... }) { }
    ZRenderPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, const std::initializer_list<ZRenderPass::ptr>& dependencies = {});
    virtual ~ZRenderPass() { }

    virtual void Initialize();

    std::shared_ptr<ZFramebuffer> Framebuffer() { return framebuffer_; }
    std::shared_ptr<ZRenderStateGroup> RenderState() const { return renderState_; }

    virtual void SetSize(const glm::vec2& size) { size_ = size; }

    void Submit(const std::shared_ptr<ZRenderTask>& task);
    void Render(double deltaTime, const std::shared_ptr<ZScene>& scene, const std::shared_ptr<ZFramebuffer>& target = nullptr);

    static std::shared_ptr<ZRenderPass> Depth();
    static std::shared_ptr<ZRenderPass> Shadow();
    static std::shared_ptr<ZRenderPass> Color(const glm::vec2& size = glm::vec2(0.f));
    static std::shared_ptr<ZRenderPass> Post(const glm::vec2& size = glm::vec2(0.f));
    static std::shared_ptr<ZRenderPass> UI(const glm::vec2& size = glm::vec2(0.f));

protected:

    ZHShader shader_;
    glm::vec2 size_;
    std::shared_ptr<ZFramebuffer> framebuffer_ = nullptr;
    std::vector<ZRenderPass::ptr> dependencies_;

    std::shared_ptr<ZRenderQueue> renderQueue_ = nullptr;
    std::shared_ptr<ZRenderStateGroup> renderState_ = nullptr;

    uint8_t clearFlags_ = 0;

    virtual void Prepare(const std::shared_ptr<ZScene>& scene);
    virtual void Perform(double deltaTime, const std::shared_ptr<ZScene>& scene) { }
    virtual void Resolve(const std::shared_ptr<ZFramebuffer>& target = nullptr) { }

    void BindDependencies() { }

};

class ZDepthPass : public ZRenderPass
{

public:

    template<class ...Args>
    ZDepthPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, Args... args)
        : ZDepthPass(shader, fbo, { args... }) { }
    ZDepthPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, const std::initializer_list<ZRenderPass::ptr>& dependencies = {})
        : ZRenderPass(shader, fbo, dependencies) { }
    ~ZDepthPass() { }

    void Initialize() override;

    void SetSize(const glm::vec2& size) override { }

protected:

    void Prepare(const std::shared_ptr<ZScene>& scene);
    void Perform(double deltaTime, const std::shared_ptr<ZScene>& scene);
    void Resolve(const std::shared_ptr<ZFramebuffer>& target = nullptr) { }

};

class ZShadowPass : public ZRenderPass
{

public:

    template<class ...Args>
    ZShadowPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, Args... args)
        : ZShadowPass(shader, fbo, { args... }) { }
    ZShadowPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, const std::initializer_list<ZRenderPass::ptr>& dependencies = {})
        : ZRenderPass(shader, fbo, dependencies) { }
    ~ZShadowPass() { }

    void Initialize() override;

    void SetSize(const glm::vec2& size) override { }

protected:

    std::shared_ptr<ZUniformBuffer> uniformBuffer_ = nullptr;

    void Prepare(const std::shared_ptr<ZScene>& scene) override;
    void Perform(double deltaTime, const std::shared_ptr<ZScene>& scene )override;
    void Resolve(const std::shared_ptr<ZFramebuffer>& target = nullptr) override { }

};

class ZColorPass : public ZRenderPass
{

public:

    template<class ...Args>
    ZColorPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, bool multisample = true, Args... args)
        : ZColorPass(shader, fbo, multisample, { args... }) { }
    ZColorPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, bool multisample = true, const std::initializer_list<ZRenderPass::ptr>& dependencies = {})
        : ZRenderPass(shader, fbo, dependencies), multisample_(multisample) { }
    ~ZColorPass() { }

    void Initialize() override;

protected:

    std::shared_ptr<ZFramebuffer> multisampledFramebuffer_;
    bool multisample_ = false;

    void Prepare(const std::shared_ptr<ZScene>& scene) override;
    void Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)override;
    void Resolve(const std::shared_ptr<ZFramebuffer>& target = nullptr) override;

};

class ZPostPass : public ZRenderPass
{

public:

    template<class ...Args>
    ZPostPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, Args... args)
        : ZPostPass(shader, fbo, { args... }) { }
    ZPostPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, const std::initializer_list<ZRenderPass::ptr>& dependencies = {})
        : ZRenderPass(shader, fbo, dependencies) { }
    ~ZPostPass() { }

    void Initialize() override;

protected:

    std::shared_ptr<ZUniformBuffer> uniformBuffer_ = nullptr;

    void Prepare(const std::shared_ptr<ZScene>& scene) override;
    void Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)override;
    void Resolve(const std::shared_ptr<ZFramebuffer>& target = nullptr) override;

};

class ZUIPass : public ZRenderPass
{

public:

    template<class ...Args>
    ZUIPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, Args... args)
        : ZUIPass(shader, fbo, { args... }) { }
    ZUIPass(const ZHShader& shader, const std::shared_ptr<ZFramebuffer>& fbo, const std::initializer_list<ZRenderPass::ptr>& dependencies = {})
        : ZRenderPass(shader, fbo, dependencies) { }
    ~ZUIPass() { }

protected:

    void Prepare(const std::shared_ptr<ZScene>& scene) override;
    void Perform(double deltaTime, const std::shared_ptr<ZScene>& scene)override;
    void Resolve(const std::shared_ptr<ZFramebuffer>& target = nullptr) override;

};