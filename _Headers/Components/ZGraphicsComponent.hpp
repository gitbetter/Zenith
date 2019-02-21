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
  bool translatesWithView_ = true;
  ZLightMap gameLights_;
  ZGameObject* gameCamera_ = nullptr ;

  void DrawOutlineIfEnabled();

public:

  ZGraphicsComponent();
  ~ZGraphicsComponent() { }

  void Initialize(ZOFNode* root) override;
  void Initialize(ZModel* model, ZShader* shader);

  virtual void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) override;

  ZShader* GetActiveShader() const { return shaders_[activeShaderIndex_]; }

  void SetOutline(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
  void ClearOutline();

  void SetGameLights(ZLightMap lights) { gameLights_ = lights; }
  void SetGameCamera(ZGameObject* camera) { gameCamera_ = camera; }

  void ShouldTranslateWithView(bool translates) { translatesWithView_ = translates; }

protected:

  ZModel* model_ = nullptr;
  glm::mat4 projectionMatrix_, viewMatrix_, modelMatrix_;
  std::vector<ZShader*> shaders_;
  ZShader* highlightShader_ = nullptr;
  glm::vec4 highlightColor_;
};
