//
//  ZUIElement.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZUIElement.hpp"
#include "ZShader.hpp"
#include "ZUIText.hpp"
#include "ZDomain.hpp"
#include "ZUI.hpp"
#include "ZCommon.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

ZUIElement::ZUIElement(glm::vec2 position, glm::vec2 scale) : modelMatrix_(1.0), color_(0.6) {
  translationBounds_ = glm::vec4(0.f, (float)ZEngine::Domain()->ResolutionX(), 0.f, (float)ZEngine::Domain()->ResolutionY());
  Scale(scale); Translate(position);
  id_ = "ZUI_" + ZEngine::IDSequence()->Next();
}

void ZUIElement::Initialize(ZOFNode* root) {
  modelMatrix_ = glm::mat4(1.0); color_ = glm::vec4(0.6);

  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZUIElement", ZERROR);
    return;
  }

  id_ = node->id;

  ZOFPropertyMap props = node->properties;

  if (props.find("scale") != props.end() && props["scale"]->HasValues()) {
    ZOFNumberList* scaleProp = props["scale"]->Value<ZOFNumberList>(0);
    float x = scaleProp->value[0] < 0 ? ZEngine::Domain()->ResolutionX() : scaleProp->value[0];
    float y = scaleProp->value[1] < 0 ? ZEngine::Domain()->ResolutionY() : scaleProp->value[1];
    Scale(glm::vec2(x, y));
  }

  if (props.find("position") != props.end() && props["position"]->HasValues()) {
    ZOFNumberList* posProp = props["position"]->Value<ZOFNumberList>(0);
    Translate(glm::vec2(posProp->value[0], posProp->value[1]));
  }

  if (props.find("color") != props.end() && props["color"]->HasValues()) {
    ZOFNumberList* colorProp = props["color"]->Value<ZOFNumberList>(0);
    color_ = glm::vec4(colorProp->value[0], colorProp->value[1], colorProp->value[2], colorProp->value[3]);
  }

  if (props.find("isHidden") != props.end() && props["isHidden"]->HasValues()) {
    ZOFString* hiddenProp = props["isHidden"]->Value<ZOFString>(0);
    hidden_ = hiddenProp->value == "Yes";
  }

  if (props.find("isEnabled") != props.end() && props["isEnabled"]->HasValues()) {
    ZOFString* enabledProp = props["isEnabled"]->Value<ZOFString>(0);
    enabled_ = enabledProp->value == "Yes";
  }

  if (props.find("texture") != props.end() && props["texture"]->HasValues()) {
    ZOFString* texProp = props["texture"]->Value<ZOFString>(0);
    if (ZEngine::Graphics()->Textures().find(texProp->value) != ZEngine::Graphics()->Textures().end())
      texture_ = ZEngine::Graphics()->Textures()[texProp->value];
  }
}

void ZUIElement::Draw(ZShader* shader) {
  shader->Activate();

  ZEngine::Graphics()->Strategy()->BindTexture(texture_, 0);
  shader->SetInt(texture_.type + "0", 0);

  glm::mat4 ortho = glm::ortho(0.f, (float)ZEngine::Domain()->ResolutionX(), (float)ZEngine::Domain()->ResolutionY(), 0.f);
  shader->SetMat4("M", modelMatrix_);
  shader->SetMat4("P", ortho);
  shader->SetVec4("color", color_);
}

void ZUIElement::RenderChildren(ZShader* shader) {
  for (std::shared_ptr<ZUIElement> child : children_) {
    // TODO: Only render if the child has the dirty flag set
    child->Draw((std::dynamic_pointer_cast<ZUIText>(child)) ? ZEngine::UI()->TextShader().get() : shader);
  }
}

void ZUIElement::AddChild(std::shared_ptr<ZUIElement> element) {
  // Reset the child translation and move it to the parent's location
  glm::vec3 elementPos = element->Position();

  element->ResetTranslation();
  element->Translate(Position() + elementPos);
  element->SetTranslationBounds(translationBounds_.x, translationBounds_.y, translationBounds_.z, translationBounds_.w);
  children_.push_back(element);
}

void ZUIElement::Translate(glm::vec2 translation) {
  modelMatrix_ = glm::translate(modelMatrix_,
                                glm::vec3(translation.x / Size().x,
                                          translation.y / Size().y,
                                          0.f));

  modelMatrix_[3] = glm::vec4(glm::clamp(modelMatrix_[3][0], translationBounds_.x, translationBounds_.y),
                              glm::clamp(modelMatrix_[3][1], translationBounds_.z, translationBounds_.w),
                              modelMatrix_[3][2],
                              modelMatrix_[3][3]);

  // Recursively update child positions
  for (std::shared_ptr<ZUIElement> child : children_) {
    child->Translate(translation);
  }
}

void ZUIElement::Rotate(float angle) {
  modelMatrix_ = glm::rotate(modelMatrix_, angle, glm::vec3(0.f, 0.f, 1.f));

  for (std::shared_ptr<ZUIElement> child : children_) {
    child->Rotate(angle);
  }
}

void ZUIElement::Scale(glm::vec2 factor) {
  modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(factor.x, 
                                                    factor.y, 
                                                    0.f));
}

glm::vec3 ZUIElement::Position() {
  return glm::vec3(modelMatrix_[3].x,
                   modelMatrix_[3].y,
                   0.f);
}

glm::vec3 ZUIElement::Size() {
  glm::mat3 scaleMatrix(modelMatrix_);
  return glm::vec3(glm::length(scaleMatrix[0]),
                   glm::length(scaleMatrix[1]),
                   glm::length(scaleMatrix[2]));
}

float ZUIElement::Angle() {
  glm::vec3 rotationAxis; float angle;
  glm::axisAngle(modelMatrix_, rotationAxis, angle);
  return angle;
}

void ZUIElement::SetTranslationBounds(float left, float right, float bottom, float top) {
  translationBounds_ = glm::vec4(left, right, bottom, top);
  for (std::shared_ptr<ZUIElement> child : children_) {
    child->SetTranslationBounds(left, right, bottom, top);
  }
}

bool ZUIElement::Contains(glm::vec3 point) {
  return point.x >= Position().x - Size().x && point.x <= Position().x + Size().x &&
      point.y >= Position().y - Size().y && point.y <= Position().y + Size().y;
}

ZMeshUI ZUIElement::ElementShape() { 
  static ZMeshUI mesh;
  if (mesh.Vertices().size() == 0) {
    std::vector<ZVertex2D> vertices = {
    ZVertex2D(glm::vec2(-1.f, 1.f)),
    ZVertex2D(glm::vec2(-1.f, -1.f)),
    ZVertex2D(glm::vec2(1.f, 1.f)),
    ZVertex2D(glm::vec2(1.f, -1.f))
    };
    mesh = ZMeshUI(vertices);
  }
  return mesh;
};

void ZUIElement::CleanUp() {
  for (std::shared_ptr<ZUIElement> child : children_) {
    child->CleanUp();
  }
}
