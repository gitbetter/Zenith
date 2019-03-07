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
struct ZOFNode;

// Class and Data Structure Definitions
class ZGraphicsComponent : public ZComponent {
  typedef std::map<std::string, ZLight*> ZLightMap;
private:

  unsigned int activeShaderIndex_ = -1;
  ZLightMap gameLights_;
  ZGameObject* gameCamera_ = nullptr;

  void DrawOutlineIfEnabled(glm::mat4& model, glm::mat4& view, glm::mat4& projection);

public:

  ZGraphicsComponent();
  ~ZGraphicsComponent() { }

  void Initialize(ZOFNode* root) override;
  void Initialize(ZModel* model, ZShader* shader);

  void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) override;

  ZShader* ActiveShader() const { return shaders_[activeShaderIndex_]; }
  ZModel* Model() const { return model_; }

  void SetOutline(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
  void ClearOutline();

  void SetGameLights(ZLightMap lights) { gameLights_ = lights; }
  void SetGameCamera(ZGameObject* camera) { gameCamera_ = camera; }

protected:

  ZModel* model_ = nullptr;
  std::vector<ZShader*> shaders_;
  ZShader* highlightShader_ = nullptr;
  glm::vec4 highlightColor_;
};
