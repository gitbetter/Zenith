//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

#include "ZComponent.hpp"
#include "ZAABBox.hpp"

class ZModel;
class ZCamera;
class ZMaterial;
class ZFrustum;
class ZRenderStateGroup;
class ZUniformBuffer;
struct ZOFNode;

class ZGraphicsComponent : public ZComponent
{

    RTTR_ENABLE(ZComponent)

public:

    ZGraphicsComponent();
    ~ZGraphicsComponent();

    void Initialize() override { ZComponent::Initialize(); }
    void Initialize(std::shared_ptr<ZOFNode> root) override;
    void Initialize(const ZHModel& model);

    std::shared_ptr<ZComponent> Clone() override;

    void Prepare(double deltaTime, const std::shared_ptr<ZRenderStateGroup>& additionalState = nullptr);

    const ZHModel& Model() const { return model_; }
    const ZMaterialList& Materials() const { return materials_; }
    bool HasAABB() const { return hasAABB_; }
    bool IsShadowCaster() const { return isShadowCaster_; }
    bool HasDepthInfo() const { return hasDepthInfo_; }
    bool HasLightingInfo() const { return hasLightingInfo_; }
    const ZAABBox& AABB() const { return bounds_; }

    void SetOutline(const glm::vec4& color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
    void ClearOutline();

    void SetGameLights(const ZLightList& lights) { gameLights_ = lights; }
    void SetGameCamera(const std::shared_ptr<ZCamera>& camera) { gameCamera_ = camera; }
    void SetModel(const ZHModel& model);
    void SetMaterials(const ZMaterialList& materials) { materials_ = materials; }
    void SetHasAABB(bool hasAABB) { hasAABB_ = hasAABB; }
    void SetIsShadowCaster(bool isShadowCaster) { isShadowCaster_ = isShadowCaster; }
    void SetHasDepthInfo(bool hasDepthInfo) { hasDepthInfo_ = hasDepthInfo; }
    void SetHasLightingInfo(bool hasLightingInfo) { hasLightingInfo_ = hasLightingInfo; }

    void AddMaterial(const ZHMaterial& material);

    bool IsVisible(ZFrustum frustrum);

    void Transform(const glm::mat4& mat);

    DECLARE_COMPONENT_CREATORS(ZGraphicsComponent)

protected:

    ZLightList gameLights_;
    std::shared_ptr<ZCamera> gameCamera_ = nullptr;
    ZHModel model_;
    ZInstancedDataOptions instanceData_;
    ZMaterialList materials_;
    ZHMaterial outlineMaterial_;
    std::shared_ptr<ZRenderStateGroup> overrideState_ = nullptr;
    ZAABBox bounds_;

    // TODO: Implement billboarding
    bool isBillboard_ = false;
    bool hasAABB_ = true;
    bool isShadowCaster_ = true;
    bool hasDepthInfo_ = true;
    bool hasLightingInfo_ = true;


    void SetupAABB();
    void PrepareOutlineDisplay(glm::mat4& modelMatrix, const ZHModel& model, const std::shared_ptr<ZRenderStateGroup>& additionalState, const std::shared_ptr<ZRenderStateGroup>& cameraState);

};
