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
#include "ZEventAgent.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZTextureReadyEvent.hpp"

void ZSkybox::Initialize(const std::string& hdrMap) {
	hdrPath_ = hdrMap;
	ZBufferData cubemapBuffer;
	ZTexture cubeMap = ZEngine::Graphics()->Strategy()->EquirectToCubemap(hdrMap, cubemapBuffer);
	Initialize(cubeMap, cubemapBuffer);
}

void ZSkybox::Initialize(ZTexture& cubeMap, ZBufferData& bufferData) {
	std::shared_ptr<ZModel> skybox = ZModel::NewSkybox(cubeMap, bufferData, iblTexture_);

	std::shared_ptr<ZShader> skyboxShader(new ZShader("Assets/Shaders/Vertex/skybox.vert", "Assets/Shaders/Pixel/skybox.frag"));
	skyboxShader->Initialize();

	std::shared_ptr<ZGraphicsComponent> skyboxGraphicsComponent(new ZGraphicsComponent);
	skyboxGraphicsComponent->Initialize(skybox, skyboxShader);

    std::vector<ZTexture> textures = { iblTexture_.cubeMap };
	skyboxGraphicsComponent->AddMaterial(std::make_shared<ZMaterial>(textures));

	AddComponent(skyboxGraphicsComponent);
	ZEngine::ProcessRunner()->AttachProcess(skyboxGraphicsComponent);
	properties_.renderPass = ZRenderPass::Sky;
}

void ZSkybox::InitializeAsync(const std::string& hdrMap) {
	ZEngine::Graphics()->Strategy()->EquirectToCubemapAsync(hdrMap);

	ZEventDelegate cubemapReadyDelegate = fastdelegate::MakeDelegate(this, &ZSkybox::HandleCubemapReady);
	ZEngine::EventAgent()->AddListener(cubemapReadyDelegate, ZTextureReadyEvent::Type);
}

void ZSkybox::HandleCubemapReady(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZTextureReadyEvent> textureReadyEvent = std::dynamic_pointer_cast<ZTextureReadyEvent>(event);
	if (textureReadyEvent->Texture().path == hdrPath_) {
        ZTexture texture = textureReadyEvent->Texture();
        ZBufferData bufferData = textureReadyEvent->BufferData();
		Initialize(texture, bufferData);
        
		ZEventDelegate cubemapReadyDelegate = fastdelegate::MakeDelegate(this, &ZSkybox::HandleCubemapReady);
		ZEngine::EventAgent()->RemoveListener(cubemapReadyDelegate, ZTextureReadyEvent::Type);
	}
}
