//
//  ZPhysicsDebug.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZPhysicsDebug.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZCameraComponent.hpp"
#include "ZGame.hpp"

void ZPhysicsDebug::Initialize() {
  shader_ = std::unique_ptr<ZShader>(new ZShader);
  shader_->Initialize("Assets/Shaders/Vertex/debug.vert", "Assets/Shaders/Pixel/debug.frag");
}

void ZPhysicsDebug::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
  std::shared_ptr<ZCameraComponent> cameraComp = ZEngine::Game()->ActiveCamera()->FindComponent<ZCameraComponent>();
  glm::mat4 projectionMatrix = cameraComp->ProjectionMatrix();
  glm::mat4 viewMatrix = cameraComp->ViewMatrix(0.5f);

  shader_->Activate();

  shader_->SetMat4("P", projectionMatrix);
  shader_->SetMat4("V", viewMatrix);
  shader_->SetVec4("color", glm::vec4(color.x(), color.y(), color.z(), 1.f));

  std::vector<ZVertex3D> vertices({
    ZVertex3D(glm::vec3(from.x(), from.y(), from.z())),
    ZVertex3D(glm::vec3(to.x(), to.y(), to.z()))
  });

  ZBufferData bufferData = ZEngine::Graphics()->Strategy()->LoadVertexData(vertices);
  ZEngine::Graphics()->Strategy()->DrawLines(bufferData, vertices);
  ZEngine::Graphics()->Strategy()->DeleteBufferData(bufferData);
}
