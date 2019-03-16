//
//  ZUIElement.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZMeshUI.hpp"
#include "ZObject.hpp"

// Forward Declarations
class ZMeshUI;
class ZShader;

// Class and Data Structure Definitions

class ZUIElement : public ZObject {

private:

  void ClampToBounds();

public:

  ZUIElement(glm::vec2 position, glm::vec2 scale);
  virtual ~ZUIElement() { }

  virtual void Initialize(ZOFNode* root);

  bool Enabled() { return enabled_; }
  bool Hidden() { return hidden_; }
  bool Selected() { return selected_; }
  glm::vec3 Position();
  glm::vec3 Size();
  float Angle();
  glm::vec4 Color() { return color_; }
  const ZTexture& Texture() { return texture_; }

  void SetSize(glm::vec2 size);
  void SetPosition(glm::vec2 position);
  void SetRotation(float angle);

  void SetTexture(ZTexture texture) { texture_ = texture; }
  virtual void SetColor(glm::vec4 newColor) { color_ = newColor; }
  void SetTranslationBounds(float left, float right, float bottom, float top);
  void ResetModelMatrix() { modelMatrix_ = glm::mat4(1.f); }

  void Translate(glm::vec2 translation);
  void Rotate(float angle);
  void Scale(glm::vec2 factor);

  void Hide() { hidden_ = true; }
  void Show() { hidden_ = false; }
  void Enable() { enabled_ = true; }
  void Disable() { enabled_ = false; }
  void Select() { if (enabled_) selected_ = true; }
  void Deselect() { selected_ = false; }

  void AddChild(std::shared_ptr<ZUIElement> element);

  virtual void Draw(ZShader* shader);
  virtual ZMeshUI ElementShape();

  void RenderChildren(ZShader* shader);

  bool Contains(glm::vec3 point);

  void CleanUp();

  template<class T>
  std::shared_ptr<T> Child() {
    if(!std::is_base_of<ZUIElement, T>::value) return nullptr;

    std::shared_ptr<T> el;
    for (auto it = children_.begin(); it != children_.end(); it++) {
      if (el = std::dynamic_pointer_cast<T>(*it)) return el;
    }

    return el;
  }

protected:

  bool hidden_, enabled_, selected_, dirty_;
  glm::mat4 modelMatrix_;
  glm::vec4 translationBounds_;
  glm::vec4 color_;
  std::vector<std::shared_ptr<ZUIElement>> children_;
  ZTexture texture_;
};
