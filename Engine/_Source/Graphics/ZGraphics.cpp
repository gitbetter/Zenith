/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZGLGraphics.cpp

	Created by Adrian Sanchez on 27/01/2019.
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

// TODO: Conditional include based on graphics implementation
#include "ZGLGraphicsStrategy.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZGraphicsFactory.hpp"
#include "ZDomainStrategy.hpp"
#include "ZEventAgent.hpp"
#include "ZGameObject.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZShader.hpp"
#include "ZMesh2D.hpp"
#include "ZScene.hpp"
#include "ZLight.hpp"

void ZGraphics::Initialize() {
	// TODO: Switch the strategies here based on implementation details
	if (graphicsStrategy_ == nullptr) {
		graphicsStrategy_ = new ZGLGraphicsStrategy();
		graphicsStrategy_->Initialize();
		shadowBuffer_ = graphicsStrategy_->LoadDepthTexture();
        depthBuffer_ = graphicsStrategy_->LoadDepthTexture();
		colorBufferMultisampled_ = graphicsStrategy_->LoadColorTexture(true);
		colorBuffer_ = graphicsStrategy_->LoadColorTexture();
		shadowFrameBuffer_ = graphicsStrategy_->LoadDepthMapBuffer(shadowBuffer_);
        depthFrameBuffer_ = graphicsStrategy_->LoadDepthMapBuffer(depthBuffer_);
		colorFrameBufferMultisampled_ = graphicsStrategy_->LoadColorBuffer(colorBufferMultisampled_, true);
		colorFrameBuffer_ = graphicsStrategy_->LoadColorBuffer(colorBuffer_);
		shadowShader_ = std::shared_ptr<ZShader>(new ZShader("Assets/Shaders/Vertex/shadow.vert", "Assets/Shaders/Pixel/depth.frag"));
        depthShader_ = std::shared_ptr<ZShader>(new ZShader("Assets/Shaders/Vertex/depth.vert", "Assets/Shaders/Pixel/depth.frag"));
		postShader_ = std::shared_ptr<ZShader>(new ZShader("Assets/Shaders/Vertex/postprocess.vert", "Assets/Shaders/Pixel/postprocess.frag"));
		shadowShader_->Initialize();
        depthShader_->Initialize();
		postShader_->Initialize();
		renderQuad_ = ZMesh2D::NewQuad();
	}
    
    ZEventDelegate shaderReadyDelegate = fastdelegate::MakeDelegate(this, &ZGraphics::HandleShaderReady);
    zenith::EventAgent()->AddListener(shaderReadyDelegate, ZShaderReadyEvent::Type);
    
    ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZGraphics::HandleTextureReady);
    zenith::EventAgent()->AddListener(textureReadyDelegate, ZTextureReadyEvent::Type);
    
    ZEventDelegate modelReadyDelegate = fastdelegate::MakeDelegate(this, &ZGraphics::HandleModelReady);
    zenith::EventAgent()->AddListener(modelReadyDelegate, ZModelReadyEvent::Type);
}

void ZGraphics::Load(std::shared_ptr<ZOFTree> root) {
    ZShaderMap shaders; ZTextureMap textures; ZModelMap models;
    
    zenith::GraphicsFactory()->CreateAssets(root, textures, shaders, models);
    
    for (ZTextureMap::iterator it = textures.begin(); it != textures.end(); it++) {
        AddTexture(it->first, it->second);
    }
    
	for (ZShaderMap::iterator it = shaders.begin(); it != shaders.end(); it++) {
		AddShader(it->first, it->second);
	}
    
    for (ZModelMap::iterator it = models.begin(); it != models.end(); it++) {
        AddModel(it->first, it->second);
    }
}

void ZGraphics::LoadAsync(std::shared_ptr<ZOFTree> root) {
    zenith::GraphicsFactory()->CreateAssetsAsync(root, pendingTextures_, pendingShaders_, pendingModels_);
}

void ZGraphics::SetupShadowDepthPass(std::shared_ptr<ZLight> light) {
	zenith::Graphics()->Strategy()->ClearViewport();
	graphicsStrategy_->BindFramebuffer(shadowFrameBuffer_, true);
	graphicsStrategy_->ClearDepth();

	shadowShader_->Activate();
	// TODO: For now we support one light source for shadows, but this should change
	// so that multiple light space matrices are supported for multiple light sources
	// that can cast shadows, possibly using deferred rendering
	// TODO: Do something about these magic numbers!
	glm::mat4 lightP = glm::ortho(-25.f, 25.f, -25.f, 25.f, 0.01f, 100.f);
	glm::mat4 lightV = glm::lookAt(light->type == ZLightType::Directional ?
		light->direction :
		light->Position(), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 lightSpaceMatrix = lightP * lightV;
	currentLightSpaceMatrix_ = lightSpaceMatrix;
}

void ZGraphics::SetupDepthPass() {
    graphicsStrategy_->BindFramebuffer(depthFrameBuffer_, true);
	zenith::Graphics()->Strategy()->ClearViewport();
	graphicsStrategy_->ClearDepth();
}

void ZGraphics::SetupColorPass() {
	graphicsStrategy_->BindFramebuffer(colorFrameBufferMultisampled_);
	zenith::Graphics()->Strategy()->ClearViewport();
}

void ZGraphics::PostProcessingPass(ZScene* scene) {
	graphicsStrategy_->BlitFramebuffer(colorFrameBufferMultisampled_, colorFrameBuffer_);

	zenith::Graphics()->Strategy()->ClearViewport();
	graphicsStrategy_->DisableDepthTesting();

	postShader_->Activate();
	postShader_->SetMat4("previousViewProjection", scene->PreviousViewProjection());
	postShader_->SetMat4("inverseViewProjection", glm::inverse(scene->ViewProjection()));

	graphicsStrategy_->BindTexture(colorBuffer_, 0);
	postShader_->SetInt("colorSampler", 0);

	graphicsStrategy_->BindTexture(depthBuffer_, 1);
	postShader_->SetInt("depthTexture", 1);

	renderQuad_->Render(postShader_.get());

	graphicsStrategy_->EnableDepthTesting();
}

void ZGraphics::FinishRenderPass() {
	graphicsStrategy_->UnbindFramebuffer();
}

void ZGraphics::AddShader(std::string id, std::shared_ptr<ZShader> shader) {
	if (shader != nullptr) loadedShaders_[id] = shader;
}

void ZGraphics::AddTexture(std::string id, ZTexture texture) {
	loadedTextures_[id] = texture;
}

void ZGraphics::AddModel(std::string id, std::shared_ptr<ZModel> model) {
    loadedModels_[id] = model;
}

void ZGraphics::ComputeTangentBitangent(ZVertex3D& v1, ZVertex3D& v2, ZVertex3D& v3) {
	glm::vec3 deltaPos1 = v2.position - v1.position;
	glm::vec3 deltaPos2 = v3.position - v1.position;
	glm::vec2 deltaUV1 = v2.uv - v1.uv;
	glm::vec2 deltaUV2 = v3.uv - v1.uv;

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

	v1.tangent = tangent; v2.tangent = tangent; v3.tangent = tangent;
	v1.bitangent = bitangent; v2.bitangent = bitangent; v3.bitangent = bitangent;
}

void ZGraphics::CleanUp() {
	if (graphicsStrategy_ != nullptr) {
		graphicsStrategy_ = nullptr;
	}

	if (shadowShader_ != nullptr) {
		shadowShader_ = nullptr;
	}
    
    ZEventDelegate shaderReadyDelegate = fastdelegate::MakeDelegate(this, &ZGraphics::HandleShaderReady);
    zenith::EventAgent()->RemoveListener(shaderReadyDelegate, ZShaderReadyEvent::Type);
    
    ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZGraphics::HandleTextureReady);
    zenith::EventAgent()->RemoveListener(textureReadyDelegate, ZTextureReadyEvent::Type);
    
    ZEventDelegate modelReadyDelegate = fastdelegate::MakeDelegate(this, &ZGraphics::HandleModelReady);
    zenith::EventAgent()->RemoveListener(modelReadyDelegate, ZModelReadyEvent::Type);
}

void ZGraphics::HandleShaderReady(std::shared_ptr<ZEvent> event) {
    std::shared_ptr<ZShaderReadyEvent> shaderReadyEvent = std::dynamic_pointer_cast<ZShaderReadyEvent>(event);
    if (pendingShaders_.find(shaderReadyEvent->Shader()) != pendingShaders_.end()) {
        std::shared_ptr<ZShader> shader = shaderReadyEvent->Shader();
        AddShader(pendingShaders_[shader], shader);
        pendingShaders_.erase(shader);
    }
}

void ZGraphics::HandleTextureReady(std::shared_ptr<ZEvent> event) {
    std::shared_ptr<ZTextureReadyEvent> textureReadyEvent = std::dynamic_pointer_cast<ZTextureReadyEvent>(event);
    if (pendingTextures_.find(textureReadyEvent->Texture().path) != pendingTextures_.end()) {
        ZTexture texture = textureReadyEvent->Texture();
        AddTexture(pendingTextures_[texture.path], texture);
        pendingTextures_.erase(texture.path);
    }
}

void ZGraphics::HandleModelReady(std::shared_ptr<ZEvent> event) {
    std::shared_ptr<ZModelReadyEvent> modelReadyEvent = std::dynamic_pointer_cast<ZModelReadyEvent>(event);
    if (pendingModels_.find(modelReadyEvent->Model()) != pendingModels_.end()) {
        std::shared_ptr<ZModel> model = modelReadyEvent->Model();
        AddModel(pendingModels_[model], model);
        pendingModels_.erase(model);
    }
}