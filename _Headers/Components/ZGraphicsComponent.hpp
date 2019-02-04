//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

// Includes
#include "ZComponent.hpp"
#include <vector>
#include <glm/glm.hpp>

// Forward Declarations
class ZModel;
class ZShader;
class ZCamera;

// Class and Data Structure Definitions
class ZGraphicsComponent : public ZComponent {
private:
  unsigned int activeShaderIndex_ = -1;
  bool translatesWithView_ = true;

  void DrawOutlineIfEnabled();

public:
  ZGraphicsComponent(ZModel* model, ZShader* shader);
  ~ZGraphicsComponent() { }

  virtual void Update(ZCamera* camera, float frameMix);
  ZShader* GetActiveShader() const { return shaders_[activeShaderIndex_]; }

  void SetOutline(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.1f, 1.f));
  void ClearOutline();

  void ShouldTranslateWithView(bool translates) { translatesWithView_ = translates; }

  void Scale(glm::vec3 scale);
  void Translate(glm::vec3 translate);
  void Rotate(float angle, glm::vec3 rotationAxis);

protected:
  ZModel* model_ = nullptr;
  glm::mat4 projectionMatrix_, viewMatrix_, modelMatrix_;
  std::vector<ZShader*> shaders_;
  ZShader* highlightShader_ = nullptr;
  glm::vec4 highlightColor_;
};
