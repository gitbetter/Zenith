/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZPhysics.cpp

    Created by Adrian Sanchez on 14/02/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ZPhysics.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZPhysicsDebug.hpp"
#include "ZObjectForceRegistry.hpp"
#include "ZGameObject.hpp"
#include "ZEventAgent.hpp"
#include "ZRaycastEvent.hpp"
#include "ZGame.hpp"
#include "ZCameraComponent.hpp"

void ZPhysics::Initialize() {
  collisionConfig_ = new btDefaultCollisionConfiguration();
  dispatcher_ = new btCollisionDispatcher(collisionConfig_);
  overlappingPairCache_ = new btDbvtBroadphase();
  solver_ = new btSequentialImpulseConstraintSolver;
  dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, overlappingPairCache_, solver_, collisionConfig_);

  debugDrawer_.reset(new ZPhysicsDebug);
  debugDrawer_->Initialize();
  dynamicsWorld_->setDebugDrawer(debugDrawer_.get());
  dynamicsWorld_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

  if (registry_ == nullptr) {
    registry_.reset(new ZObjectForceRegistry);
  }

  ZEventDelegate raycastDelegate = fastdelegate::MakeDelegate(this, &ZPhysics::HandleRaycastEvent);
  ZEngine::EventAgent()->AddListener(raycastDelegate, ZRaycastEvent::Type);
}

void ZPhysics::Update() {
  registry_->UpdateForces();
  dynamicsWorld_->stepSimulation(ZEngine::UPDATE_STEP_SIZE, 1, ZEngine::UPDATE_STEP_SIZE);
}

void ZPhysics::AddRigidBody(std::shared_ptr<btRigidBody> body) {
  dynamicsWorld_->addRigidBody(body.get());
}

void ZPhysics::HandleRaycastEvent(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZRaycastEvent> raycastEvent = std::dynamic_pointer_cast<ZRaycastEvent>(event);

  if (raycastEvent->Origin().z == 0.f) { // Handle screen to world raycasting (i.e. mouse picking)
    std::shared_ptr<ZGameObject> camera = ZEngine::Game()->ActiveCamera();
    if (camera) {
      std::shared_ptr<ZCameraComponent> cameraComp = camera->FindComponent<ZCameraComponent>();
      glm::mat4 InverseProjection = glm::inverse(cameraComp->ProjectionMatrix());
      glm::mat4 InverseView = glm::inverse(cameraComp->ViewMatrix(1.f));

      glm::vec4 rayStart(raycastEvent->Origin().x * 2.f - 1.f, -raycastEvent->Origin().y * 2.f + 1.f, -1.f, 1.f);
      glm::vec4 rayEnd(raycastEvent->Origin().x * 2.f - 1.f, -raycastEvent->Origin().y * 2.f + 1.f, 0.f, 1.f);

      glm::vec4 rayStartCamera = InverseProjection * rayStart; rayStartCamera /= rayStartCamera.w;
      glm::vec4 rayStartWorld = InverseView * rayStartCamera; rayStartWorld /= rayStartWorld.w;
      glm::vec4 rayEndCamera = InverseProjection * rayEnd; rayEndCamera /= rayEndCamera.w;
      glm::vec4 rayEndWorld = InverseView * rayEndCamera; rayEndWorld /= rayEndWorld.w;

      glm::vec3 rayDir = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));

      ZGameObject* objectHit = Raycast(rayStartWorld, rayDir);
      if (objectHit) {
        // TODO: Trigger another event?
        _Z("Object " + objectHit->ID() + " hit", ZINFO);
      }
    }
  } else { // Handle general 3D raycasting

  }
}

ZGameObject* ZPhysics::Raycast(glm::vec3 start, glm::vec3 direction) {
  glm::vec3 end = direction * 1000.f;
  btCollisionWorld::ClosestRayResultCallback rayCallback(
    btVector3(start.x, start.y, start.z),
    btVector3(end.x, end.y, end.z)
  );

  dynamicsWorld_->rayTest(
    btVector3(start.x, start.y, start.z),
    btVector3(end.x, end.y, end.z),
    rayCallback
  );

  if (rayCallback.hasHit()) {
    return static_cast<ZGameObject*>(rayCallback.m_collisionObject->getUserPointer());
  }
  return nullptr;
}

void ZPhysics::DebugDraw() {
  dynamicsWorld_->debugDrawWorld();
}
