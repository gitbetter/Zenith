//
//  ZUIElement.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
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
  translationBounds_ = glm::vec4(0.f, (float)ZEngine::Domain()->WindowWidth(), 0.f, (float)ZEngine::Domain()->WindowHeight());
  Scale(scale); Translate(position);
}

void ZUIElement::Render(ZShader* shader) {
  shader->Activate();

  ZEngine::UI()->GraphicsStrategy()->BindTexture(texture_, 0);
  shader->SetInt(texture_.type + "0", 0);

  glm::mat4 ortho = glm::ortho(0.f, (float)ZEngine::Domain()->WindowWidth(), (float)ZEngine::Domain()->WindowHeight(), 0.f);
  shader->SetMat4("M", modelMatrix_);
  shader->SetMat4("P", ortho);
  shader->SetVec4("color", color_);
}

void ZUIElement::RenderChildren(ZShader* shader) {
  for (ZUIElement* child : children_) {
    // TODO: Only render if the child has the dirty flag set
    child->Render((dynamic_cast<ZUIText*>(child)) ? ZEngine::UI()->TextShader() : shader);
  }
}

void ZUIElement::AddChild(ZUIElement* element) {
  // Reset the child translation and move it to the parent's location
  element->ResetTranslation();
  element->Translate(Position());
  element->SetTranslationBounds(translationBounds_.x, translationBounds_.y, translationBounds_.z, translationBounds_.w);
  children_.push_back(element);
}

void ZUIElement::Translate(glm::vec2 translation) {
  glm::vec3 size = glm::vec3(Size().x * (float)ZEngine::Domain()->WindowWidth(),
                             Size().y * (float)ZEngine::Domain()->WindowHeight(),
                             1.f);
  float scaleXFactor = (float)ZEngine::Domain()->WindowWidth() / size.x;
  float scaleYFactor = (float)ZEngine::Domain()->WindowHeight() / size.y;
  modelMatrix_ = glm::translate(modelMatrix_,
                                glm::vec3(scaleXFactor * translation.x + glm::sign(translation.x) * (1.f / scaleXFactor) / 0.5f,
                                          scaleYFactor * translation.y + glm::sign(translation.y) * (1.f / scaleYFactor) / 0.5f,
                                          0.f));

  modelMatrix_[3] = glm::vec4(glm::clamp(modelMatrix_[3][0], translationBounds_.x, translationBounds_.y),
                              glm::clamp(modelMatrix_[3][1], translationBounds_.z, translationBounds_.w),
                              modelMatrix_[3][2],
                              modelMatrix_[3][3]);

  // Recursively update child positions
  for (ZUIElement* child : children_) {
    child->Translate(translation);
  }
}

void ZUIElement::Rotate(float angle) {
  modelMatrix_ = glm::rotate(modelMatrix_, angle, glm::vec3(0.f, 0.f, 1.f));

  for (ZUIElement* child : children_) {
    child->Rotate(angle);
  }
}

void ZUIElement::Scale(glm::vec2 factor) {
  modelMatrix_ = glm::scale(modelMatrix_, glm::vec3((float)ZEngine::Domain()->WindowWidth() * 0.75f * factor.x,
                                                    (float)ZEngine::Domain()->WindowHeight() * 0.75f * factor.y,
                                                    0.f));
}

glm::vec3 ZUIElement::Position() {
  return glm::vec3(modelMatrix_[3].x / (float)ZEngine::Domain()->WindowWidth(),
                   modelMatrix_[3].y / (float)ZEngine::Domain()->WindowHeight(),
                   0.f);
}

glm::vec3 ZUIElement::Size() {
  glm::mat3 scaleMatrix(modelMatrix_);
  return glm::vec3(glm::length(scaleMatrix[0] / (float)ZEngine::Domain()->WindowWidth()),
                   glm::length(scaleMatrix[1] / (float)ZEngine::Domain()->WindowHeight()),
                   glm::length(scaleMatrix[2]));
}

float ZUIElement::Angle() {
  glm::vec3 rotationAxis; float angle;
  glm::axisAngle(modelMatrix_, rotationAxis, angle);
  return angle;
}

void ZUIElement::SetTranslationBounds(float left, float right, float bottom, float top) {
  translationBounds_ = glm::vec4(left, right, bottom, top);
  for (ZUIElement* child : children_) {
    child->SetTranslationBounds(left, right, bottom, top);
  }
}

void ZUIElement::CleanUp() {
  for (ZUIElement* child : children_) {
    child->CleanUp();
    delete child;
  }
}