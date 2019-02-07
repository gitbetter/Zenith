//
//  ZUIElement.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>
#include <glm/glm.hpp>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZUIElement {
private:
public:
  ZUIElement(glm::vec2 position, glm::vec2 scale) : position_(position), scale_(scale) { }
  virtual ~ZUIElement() { }

  void Hide() { hidden_ = true; }
  void Show() { hidden_ = false; }
  void Enable() { enabled_ = true; }
  void Disable() { enabled_ = false; }
  void Select() { if (selectable_) selected_ = true; }
  void Deselect() { selected_ = false; }
  void SetSelectable(bool selectable) { selectable_ = selectable; }

  void AddChild(ZUIElement* element) { children_.push_back(element); }

  // TODO: Translate, Rotate and Scale methods
  void Translate(glm::vec2 translation);
  void Rotate(float angle);
  void Scale(glm::vec2 factor);

  virtual void Render() = 0;

protected:
  bool hidden_, enabled_, selected_, selectable_;
  glm::vec2 position_, scale_;
  float rotationAngle_;
  std::vector<ZUIElement*> children_;
};
