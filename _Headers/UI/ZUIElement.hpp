//
//  ZUIElement.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
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
public:
  ZUIElement(glm::vec2 position, glm::vec2 scale);
  virtual ~ZUIElement() { }

  void Hide() { hidden_ = true; }
  void Show() { hidden_ = false; }
  void Enable() { enabled_ = true; }
  void Disable() { enabled_ = false; }
  void Select() { if (selectable_) selected_ = true; }
  void Deselect() { selected_ = false; }
  void SetSelectable(bool selectable) { selectable_ = selectable; }

  void AddChild(ZUIElement* element) { children_.push_back(element); }

  void Translate(glm::vec2 translation);
  void Rotate(float angle);
  void Scale(glm::vec2 factor);
  virtual void SetColor(glm::vec4 newColor) { color_ = newColor; }

  glm::vec3 Position();
  glm::vec3 Size();
  float Angle();

  const ZTexture& Texture() { return texture_; }
  void SetTexture(ZTexture texture) { texture_ = texture; }

  virtual void Render(ZShader* shader);
  virtual ZMeshUI ElementShape() { };

  void CleanUp();

protected:
  bool hidden_, enabled_, selected_, selectable_, dirty_;
  glm::mat4 modelMatrix_;
  glm::vec4 color_;
  std::vector<ZUIElement*> children_;
  ZTexture texture_;
};
