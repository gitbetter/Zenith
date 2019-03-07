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
  typedef std::map<std::string, std::shared_ptr<ZLight>> ZLightMap;
private:

  unsigned int activeShaderIndex_ = -1;
  ZLightMap gameLights_;
<<<<<<< HEAD
  ZGameObject* gameCamera_ = nullptr;
=======
  std::shared_ptr<ZGameObject> gameCamera_;
>>>>>>> c0a4930e1082db03d65015326e8f04dbeb1b3417

  void DrawOutlineIfEnabled(glm::mat4& model, glm::mat4& view, glm::mat4& projection);

public:

  ZGraphicsComponent();
  ~ZGraphicsComponent() { }

  void Initialize(ZOFNode* root) override;
  void Initialize(std::shared_ptr<ZModel> model, std::shared_ptr<ZShader> shader);

  void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) override;

  std::shared_ptr<ZShader> ActiveShader() const { return shaders_[activeShaderIndex_]; }
  std::shared_ptr<ZModel> Model() const { return model_; }

  void SetOutline(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
  void ClearOutline();

  void SetGameLights(ZLightMap lights) { gameLights_ = lights; }
  void SetGameCamera(std::shared_ptr<ZGameObject> camera) { gameCamera_ = camera; }

protected:

  std::shared_ptr<ZModel> model_ = nullptr;
  std::vector<std::shared_ptr<ZShader>> shaders_;
  std::shared_ptr<ZShader> highlightShader_ = nullptr;
  glm::vec4 highlightColor_;
};
