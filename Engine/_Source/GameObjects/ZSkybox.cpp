/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZSkybox.cpp

    Created by Adrian Sanchez on 02/03/2019.
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

#include "ZServices.hpp"
#include "ZSkybox.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZSkyboxReadyEvent.hpp"

ZSkybox::ZSkybox(const std::string& hdr) : ZGameObject(glm::vec3(0.f)), hdrPath_(hdr)
{
    properties_.renderOrder = ZRenderOrder::Sky;
}

void ZSkybox::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);

    if (!node)
    {
        LOG("Could not initalize ZSkybox", ZSeverity::Error);
        return;
    }

    id_ = node->id;

    ZOFPropertyMap props = node->properties;

    if (props.find("hdr") != props.end() && props["hdr"]->HasValues())
    {
        std::shared_ptr<ZOFString> hdrProp = props["hdr"]->Value<ZOFString>(0);
        hdrPath_ = hdrProp->value;
        InitializeAsync();
    }
}

void ZSkybox::Initialize()
{
    ZFramebuffer::ptr cubemapBuffer;
    ZTexture::ptr cubeMap = ZTexture::CreateHDRI(hdrPath_, cubemapBuffer);
    Initialize(cubeMap, cubemapBuffer);
}

void ZSkybox::InitializeAsync()
{
    ZServices::EventAgent()->Subscribe(this, &ZSkybox::HandleCubemapReady);

    pendingHDRTexture_ = ZTexture::CreateHDRIAsync(hdrPath_);
}

void ZSkybox::Initialize(const ZTexture::ptr& cubeMap, const ZFramebuffer::ptr& bufferData)
{
    std::shared_ptr<ZModel> skybox = ZModel::NewSkybox(cubeMap, bufferData, iblTexture_);

    std::shared_ptr<ZShader> skyboxShader(new ZShader("/Shaders/Vertex/skybox.vert", "/Shaders/Pixel/skybox.frag"));
    skyboxShader->Initialize();

    std::shared_ptr<ZGraphicsComponent> skyboxGraphicsComponent(new ZGraphicsComponent);
    skyboxGraphicsComponent->Initialize(skybox, skyboxShader);

    std::vector<ZTexture::ptr> textures = { iblTexture_.cubeMap };
    skyboxGraphicsComponent->AddMaterial(std::make_shared<ZMaterial>(textures));

    AddComponent(skyboxGraphicsComponent);
}

std::shared_ptr<ZGameObject> ZSkybox::Clone()
{
    std::shared_ptr<ZSkybox> clone = std::make_shared<ZSkybox>();
    clone->id_ = id_;
    clone->hdrPath_ = hdrPath_;
    clone->iblTexture_ = iblTexture_;
    if (std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>())
        clone->AddComponent(graphicsComp->Clone());
    return clone;
}

void ZSkybox::Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp)
{
    if (renderOp != ZRenderOp::Depth && renderOp != ZRenderOp::Shadow)
    {
        ZGameObject::Render(deltaTime, shader, renderOp);
    }
}

void ZSkybox::HandleCubemapReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    if (event->Texture()->path == hdrPath_)
    {
        ZTexture::ptr texture = event->Texture();
        ZFramebuffer::ptr bufferData = event->BufferData();
        Initialize(texture, bufferData);

        ZServices::EventAgent()->Unsubscribe(this, &ZSkybox::HandleCubemapReady);

        std::shared_ptr<ZSkyboxReadyEvent> skyboxReadyEvent = std::make_shared<ZSkyboxReadyEvent>(std::static_pointer_cast<ZSkybox>(shared_from_this()));
        ZServices::EventAgent()->Queue(skyboxReadyEvent);
    }
}
