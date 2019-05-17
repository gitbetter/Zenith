//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZComponent.hpp"
#include "ZGraphics.hpp"

// Forward Declarations
class ZModel;
class ZGameObject;
class ZShader;
struct ZOFNode;

// Class and Data Structure Definitions
class ZGraphicsComponent : public ZComponent {
    
    typedef ZLightMap ZLightMap;
    
private:
    
    unsigned int activeShaderIndex_ = -1;
    ZLightMap gameLights_;
    std::shared_ptr<ZGameObject> gameCamera_;
	std::vector<std::string> shaders_;
	std::string model_;
    
    void DrawOutlineIfEnabled(glm::mat4& model, glm::mat4& viewProjection);
    
public:
    
    ZGraphicsComponent();
    ~ZGraphicsComponent();
    
    void Initialize() override { }
    void Initialize(std::shared_ptr<ZOFNode> root) override;
    void Initialize(std::shared_ptr<ZModel> model, std::shared_ptr<ZShader> shader);
    
    void Render(ZRenderOp renderOp = ZRenderOp::Color);
    
    std::shared_ptr<ZShader> ActiveShader();
	std::shared_ptr<ZModel> Model();
    
    void SetOutline(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
    void ClearOutline();
    
    void SetGameLights(ZLightMap lights) { gameLights_ = lights; }
    void SetGameCamera(std::shared_ptr<ZGameObject> camera) { gameCamera_ = camera; }
    
    void AddMaterial(std::shared_ptr<ZMaterial> material);
    
protected:
    
    std::shared_ptr<ZModel> modelObject_ = nullptr;
    std::shared_ptr<ZShader> currentShaderObject_;
    std::vector<std::shared_ptr<ZMaterial>> materials_;
    std::shared_ptr<ZShader> highlightShader_ = nullptr;
    glm::vec4 highlightColor_{0.f};
};