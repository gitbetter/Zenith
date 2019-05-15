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
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZSkyboxReadyEvent.hpp"

ZSkybox::ZSkybox(std::string hdr) : ZGameObject(glm::vec3(0.f)), hdrPath_(hdr) {
    properties_.renderPass = ZRenderPass::Sky;
}

void ZSkybox::Initialize(std::shared_ptr<ZOFNode> root) {
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);

    if(node == nullptr) {
        _Z("Could not initalize ZSkybox", ZERROR);
        return;
    }

    id_ = node->id;

    ZOFPropertyMap props = node->properties;

    if (props.find("hdr") != props.end() && props["hdr"]->HasValues()) {
        std::shared_ptr<ZOFString> hdrProp = props["hdr"]->Value<ZOFString>(0);
        hdrPath_ = hdrProp->value;
        InitializeAsync();
    }
}

void ZSkybox::Initialize() {
	ZBufferData cubemapBuffer;
	ZTexture cubeMap = ZEngine::Graphics()->Strategy()->EquirectToCubemap(hdrPath_, cubemapBuffer);
	Initialize(cubeMap, cubemapBuffer);
}

void ZSkybox::InitializeAsync() {
    ZEventDelegate cubemapReadyDelegate = fastdelegate::MakeDelegate(this, &ZSkybox::HandleCubemapReady);
    ZEngine::EventAgent()->AddListener(cubemapReadyDelegate, ZTextureReadyEvent::Type);

    ZEngine::Graphics()->Strategy()->EquirectToCubemapAsync(hdrPath_);
}

void ZSkybox::Initialize(ZTexture& cubeMap, ZBufferData& bufferData) {
	std::shared_ptr<ZModel> skybox = ZModel::NewSkybox(cubeMap, bufferData, iblTexture_);

  std::shared_ptr<ZShader> skyboxShader(new ZShader);
  skyboxShader->Initialize("Assets/Shaders/Vertex/skybox.vert", "Assets/Shaders/Pixel/skybox.frag");

  std::shared_ptr<ZGraphicsComponent> skyboxGraphicsComponent(new ZGraphicsComponent);
  skyboxGraphicsComponent->Initialize(skybox, skyboxShader);

    std::vector<ZTexture> textures = { iblTexture_.cubeMap };
	skyboxGraphicsComponent->AddMaterial(std::make_shared<ZMaterial>(textures));

	AddComponent(skyboxGraphicsComponent);
}

void ZSkybox::HandleCubemapReady(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZTextureReadyEvent> textureReadyEvent = std::dynamic_pointer_cast<ZTextureReadyEvent>(event);
	if (textureReadyEvent->Texture().path == hdrPath_) {
        ZTexture texture = textureReadyEvent->Texture();
        ZBufferData bufferData = textureReadyEvent->BufferData();
		Initialize(texture, bufferData);

		ZEventDelegate cubemapReadyDelegate = fastdelegate::MakeDelegate(this, &ZSkybox::HandleCubemapReady);
		ZEngine::EventAgent()->RemoveListener(cubemapReadyDelegate, ZTextureReadyEvent::Type);

        std::shared_ptr<ZSkyboxReadyEvent> skyboxReadyEvent = std::make_shared<ZSkyboxReadyEvent>(shared_from_this());
        ZEngine::EventAgent()->QueueEvent(skyboxReadyEvent);
	}
}
