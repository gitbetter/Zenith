//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

#include "ZComponent.hpp"
#include "ZAABBox.hpp"

class ZRenderStateGroup;

struct ZGraphicsComponent : public ZComponent
{

    RTTR_ENABLE(ZComponent)

public:

    ZGraphicsComponent();
    ~ZGraphicsComponent() = default;

	virtual void OnDeserialize(const std::shared_ptr<struct ZOFObjectNode>& dataNode) override;
	virtual void OnCloned(const ZHComponent& original) override;
	virtual void OnUpdate(double deltaTime) override;

public:

    void Initialize(const ZHModel& model);

    void SetOutline(const glm::vec4& color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
    void ClearOutline();
    void AddModel(const ZHModel& model);
    void AddMaterial(const ZHMaterial& material);
    bool IsVisible(class ZFrustum frustrum);
    void Transform(const glm::mat4& mat);

    ZLightList gameLights;
    ZHGameObject gameCamera;
    ZModelList models;
    ZInstancedDataOptions instanceData;
    ZMaterialList materials;
    ZHMaterial outlineMaterial;
    std::shared_ptr<class ZRenderStateGroup> overrideState = nullptr;
    ZAABBox bounds;

    bool isBillboard = false; // TODO: Implement billboarding
    bool hasAABB = true;
    bool isShadowCaster = true;
    bool hasDepthInfo = true;
    bool hasLightingInfo = true;

protected:

    void SetupAABB();
    void UpdateOutlineDisplay(glm::mat4& modelMatrix, const ZHModel& model, const std::shared_ptr<class ZRenderStateGroup>& cameraState);

private:

	static ZIDSequence idGenerator_;

};
