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
#include "ZDomain.hpp"
#include "ZUI.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

void ZUIElement::Render(ZShader* shader) {
  shader->Activate();

  ZEngine::UI()->Strategy()->BindTexture(texture_, 0);
  shader->SetInt(texture_.type + "0", 0);

  glm::mat4 ortho = glm::ortho(0.f, (float)ZEngine::Domain()->WindowWidth(), (float)ZEngine::Domain()->WindowHeight(), 0.f);
  shader->SetMat4("M", modelMatrix_);
  shader->SetMat4("P", ortho);
  shader->SetVec4("backgroundColor", color_);
}

ZUIElement::ZUIElement(glm::vec2 position, glm::vec2 scale) : modelMatrix_(1.0), color_(0.6) {
   Scale(scale); Translate(position);
   texture_ = ZEngine::UI()->Strategy()->LoadDefaultTexture();
}

void ZUIElement::Translate(glm::vec2 translation) {
  glm::vec3 size = Size();
  float scaleXFactor = (float)ZEngine::Domain()->WindowWidth() / size.x;
  float scaleYFactor = (float)ZEngine::Domain()->WindowHeight() / size.y;
  modelMatrix_ = glm::translate(modelMatrix_,
                                glm::vec3(scaleXFactor * translation.x + (1.f / scaleXFactor) / 0.5f,
                                          scaleYFactor * translation.y + (1.f / scaleYFactor) / 0.5f,
                                           0.f));
}

void ZUIElement::Rotate(float angle) {
  modelMatrix_ = glm::rotate(modelMatrix_, angle, glm::vec3(0.f, 0.f, 1.f));
}

void ZUIElement::Scale(glm::vec2 factor) {
  modelMatrix_ = glm::scale(modelMatrix_, glm::vec3((float)ZEngine::Domain()->WindowWidth() * 0.75f * factor.x,
                                                    (float)ZEngine::Domain()->WindowHeight() * 0.75f * factor.y,
                                                    1.f));
}

glm::vec3 ZUIElement::Position() {
  return glm::vec3(modelMatrix_[3]);
}

glm::vec3 ZUIElement::Size() {
  glm::mat3 scaleMatrix(modelMatrix_);
  return glm::vec3(glm::length(scaleMatrix[0]), glm::length(scaleMatrix[1]), glm::length(scaleMatrix[2]));
}

float ZUIElement::Angle() {
  glm::vec3 rotationAxis; float angle;
  glm::axisAngle(modelMatrix_, rotationAxis, angle);
  return angle;
}
