//
//  ZGraphicsComponent.cpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#include "ZGraphicsComponent.hpp"
#include "ZModel.hpp"

ZGraphicsComponent::ZGraphicsComponent(ZModel* model, ZShader* shader) : model_(model) {
  if (shader != nullptr) shaders_.push_back(shader);
}

void ZGraphicsComponent::Update(float frameMix) {
  // TODO: Use frameMix to interpolate between frames
  model_->Render(shaders_[currentShaderIndex_]);
}
