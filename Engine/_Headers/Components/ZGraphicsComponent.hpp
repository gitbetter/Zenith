//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

// Includes
#include "ZComponent.hpp"
#include "ZRenderable.hpp"

// Forward Declarations
class ZModel;
class ZShader;
class ZCamera;
struct ZOFNode;

// Class and Data Structure Definitions
class ZGraphicsComponent : public ZComponent, public ZRenderable
{

public:

    ZGraphicsComponent();
    ~ZGraphicsComponent();

    void Initialize() override { ZComponent::Initialize(); }
    void Initialize(std::shared_ptr<ZOFNode> root) override;
    void Initialize(std::shared_ptr<ZModel> model, std::shared_ptr<ZShader> shader);

    std::shared_ptr<ZComponent> Clone() override;

    void Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp = ZRenderOp::Color) override;
    bool Renderable() override { return true; }

    std::shared_ptr<ZShader> ActiveShader();
    std::shared_ptr<ZModel> Model();
    const std::vector<std::shared_ptr<ZMaterial>>& Materials();

    void SetOutline(const glm::vec4& color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
    void ClearOutline();

    void SetGameLights(const ZLightMap& lights) { gameLights_ = lights; }
    void SetGameCamera(std::shared_ptr<ZCamera> camera) { gameCamera_ = camera; }
    void SetModel(std::shared_ptr<ZModel> model);

    void AddMaterial(std::shared_ptr<ZMaterial> material);

protected:

    std::shared_ptr<ZModel> modelObject_ = nullptr;
    std::shared_ptr<ZShader> currentShaderObject_ = nullptr;
    std::vector<std::shared_ptr<ZMaterial>> materials_;
    std::string highlightShaderId_;
    std::shared_ptr<ZShader> highlightShader_ = nullptr;
    glm::vec4 highlightColor_{ 0.f };
    ZInstancedDataOptions instanceData_;
    ZLightMap gameLights_;
    std::shared_ptr<ZCamera> gameCamera_ = nullptr;
    std::vector<std::string> shadersIds_;
    std::vector<std::string> materialIds_;
    std::string model_;
    int activeShaderIndex_ = -1;
    bool isBillboard_;

    void DrawOutlineIfEnabled(const glm::mat4& model, const glm::mat4& viewProjection);

};
