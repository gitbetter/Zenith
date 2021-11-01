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

#include "ZSkybox.hpp"
#include "ZServices.hpp"
#include "ZCube.hpp"
#include "ZShader.hpp"
#include "ZMaterial.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZSkyboxReadyEvent.hpp"
#include "ZRenderStateGroup.hpp"

ZSkybox::ZSkybox() : ZGameObject()
{
    properties.renderOrder = ZRenderLayer::Sky;
}

void ZSkybox::OnCreate()
{
    ZFramebuffer::ptr cubemapBuffer;
    ZHTexture cubeMap;
    if (!hdrPath_.empty())
    {
        cubeMap = ZServices::TextureManager()->CreateHDRI(hdrPath_, cubemapBuffer);
    }
    LoadCubemap(cubeMap, cubemapBuffer);
}

void ZSkybox::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (dataNode == nullptr)
    {
        LOG("Could not initalize ZSkybox", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("hdr") != props.end() && props["hdr"]->HasValues())
    {
        std::shared_ptr<ZOFString> hdrProp = props["hdr"]->Value<ZOFString>(0);
        LoadCubemap(hdrProp->value);
    }
}

void ZSkybox::OnCloned(ZGameObject* original)
{
    if (ZSkybox* originalSkybox = dynamic_cast<ZSkybox*>(original))
    {
        hdrPath_ = originalSkybox->hdrPath_;
        iblTexture = originalSkybox->iblTexture;
        if (const ZHComponent& graphicsComp = ZServices::GameObjectManager()->FindComponent<ZGraphicsComponent>(original->handle))
        {
            ZServices::GameObjectManager()->AddComponent(handle, ZServices::ComponentManager()->Clone(graphicsComp));
        }
    }
}

void ZSkybox::LoadCubemap(const std::string& hdr)
{
    hdrPath_ = hdr;
    ZServices::EventAgent()->Subscribe(this, &ZSkybox::HandleCubemapReady);
    ZServices::TextureManager()->CreateHDRIAsync(hdrPath_);
}

void ZSkybox::LoadCubemap(const ZHTexture& cubeMap, const ZFramebuffer::ptr& bufferData)
{
    ZRenderStateGroupWriter writer;
    writer.Begin();

    if (!iblTexture.cubeMap.IsNull())
    {
        writer.ClearTextures();
    }

    iblTexture = ZServices::TextureManager()->CreateIBL(bufferData, cubeMap);

    writer.BindTexture(iblTexture.irradiance);
    writer.BindTexture(iblTexture.prefiltered);
    writer.BindTexture(iblTexture.brdfLUT);
    renderState = writer.End();

    ZHComponent skyboxGraphicsComponent = ZServices::GameObjectManager()->FindComponent<ZGraphicsComponent>(handle);
    if (skyboxGraphicsComponent.IsNull())
    {
        skyboxGraphicsComponent = ZServices::ComponentManager()->CreateIn(ZComponentType::Graphics, handle);
        ZGraphicsComponent* skyboxGraphicsCompObj = ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(skyboxGraphicsComponent);
        skyboxGraphicsCompObj->hasAABB = false;
        skyboxGraphicsCompObj->isShadowCaster = false;
        skyboxGraphicsCompObj->hasDepthInfo = false;
        skyboxGraphicsCompObj->Initialize(ZServices::ModelManager()->Create(ZModelType::Cube));
    }
    else
    {
        ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(skyboxGraphicsComponent)->materials = {};
    }

    ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(skyboxGraphicsComponent)->AddMaterial(
        ZServices::MaterialManager()->Create(
            { iblTexture.cubeMap },
            ZServices::ShaderManager()->Create("/Shaders/Vertex/skybox.vert", "/Shaders/Pixel/skybox.frag")
        )
    );
}

void ZSkybox::HandleCubemapReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    std::string texturePath = ZServices::TextureManager()->Path(event->Texture());
    if (texturePath == hdrPath_)
    {
        ZServices::EventAgent()->Unsubscribe(this, &ZSkybox::HandleCubemapReady);

        ZHTexture texture = event->Texture();
        ZFramebuffer::ptr bufferData = event->BufferData();
        LoadCubemap(texture, bufferData);

        std::shared_ptr<ZSkyboxReadyEvent> skyboxReadyEvent = std::make_shared<ZSkyboxReadyEvent>(handle);
        ZServices::EventAgent()->Queue(skyboxReadyEvent);
    }
}