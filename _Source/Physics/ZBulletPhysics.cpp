/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
   ZBulletPhysics.cpp
 
   Created by Adrian Sanchez on 16/04/2019.
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

#include "ZBulletPhysics.hpp"
#include "ZGame.hpp"
#include "ZCameraComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZPhysicsDebug.hpp"
#include "ZGameObject.hpp"
#include "ZEventAgent.hpp"
#include "ZRaycastEvent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZBulletRigidBody.hpp"
#include "ZCollisionEvent.hpp"
#include "ZCollisionEndEvent.hpp"

void ZBulletPhysics::Initialize() {
    ZPhysics::Initialize();
    
    collisionConfig_ = new btDefaultCollisionConfiguration();
    dispatcher_ = new btCollisionDispatcher(collisionConfig_);
    overlappingPairCache_ = new btDbvtBroadphase();
    solver_ = new btSequentialImpulseConstraintSolver;
    dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, overlappingPairCache_, solver_, collisionConfig_);
    
    debugDrawer_.reset(new ZPhysicsDebug);
    debugDrawer_->Initialize();
    dynamicsWorld_->setDebugDrawer(debugDrawer_.get());
    dynamicsWorld_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    dynamicsWorld_->setInternalTickCallback(TickCallback);
    dynamicsWorld_->setWorldUserInfo(this);
    
    ZEventDelegate raycastDelegate = fastdelegate::MakeDelegate(this, &ZBulletPhysics::HandleRaycastEvent);
    ZEngine::EventAgent()->AddListener(raycastDelegate, ZRaycastEvent::Type);
}

void ZBulletPhysics::Update() {
    ZPhysics::Update();
    dynamicsWorld_->stepSimulation(ZEngine::DeltaTime(), ZEngine::MAX_FIXED_UPDATE_ITERATIONS, ZEngine::UPDATE_STEP_SIZE);
}

void ZBulletPhysics::AddRigidBody(std::shared_ptr<ZRigidBody> body) {
    btRigidBody* ptr = static_cast<btRigidBody*>(body->Get());
    if (!ptr) {
        _Z("The given rigid body is not a btRigidBody", ZERROR);
        return;
    }
    
    dynamicsWorld_->addRigidBody(ptr, (int)body->Type(), (int)ZPhysicsBodyType::All);
}

void ZBulletPhysics::HandleRaycastEvent(std::shared_ptr<ZEvent> event) {
    std::shared_ptr<ZRaycastEvent> raycastEvent = std::dynamic_pointer_cast<ZRaycastEvent>(event);
    
    if (raycastEvent->Origin().z == 0.f) { // Handle screen to world raycasting (i.e. mouse picking)
        std::shared_ptr<ZGameObject> camera = ZEngine::Game()->ActiveScene()->ActiveCamera();
        if (camera) {
            std::shared_ptr<ZCameraComponent> cameraComp = camera->FindComponent<ZCameraComponent>();
            glm::mat4 InverseProjection = glm::inverse(cameraComp->ProjectionMatrix());
            glm::mat4 InverseView = glm::inverse(cameraComp->ViewMatrix());
            
            glm::vec4 rayStart(raycastEvent->Origin().x * 2.f - 1.f, -raycastEvent->Origin().y * 2.f + 1.f, -1.f, 1.f);
            glm::vec4 rayEnd(raycastEvent->Origin().x * 2.f - 1.f, -raycastEvent->Origin().y * 2.f + 1.f, 0.f, 1.f);
            
            glm::vec4 rayStartCamera = InverseProjection * rayStart; rayStartCamera /= rayStartCamera.w;
            glm::vec4 rayStartWorld = InverseView * rayStartCamera; rayStartWorld /= rayStartWorld.w;
            glm::vec4 rayEndCamera = InverseProjection * rayEnd; rayEndCamera /= rayEndCamera.w;
            glm::vec4 rayEndWorld = InverseView * rayEndCamera; rayEndWorld /= rayEndWorld.w;
            
            glm::vec3 rayDir = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));
            
            ZRaycastHitResult hitResult = Raycast(rayStartWorld, rayDir);
            if (hitResult.hasHit) {
                std::shared_ptr<ZObjectSelectedEvent> objectSelectEvent(new ZObjectSelectedEvent(hitResult.objectHit->ID(), hitResult.hitPosition));
                ZEngine::EventAgent()->TriggerEvent(objectSelectEvent);
            }
        }
    } else { // Handle general 3D raycasting
        
    }
}

ZRaycastHitResult ZBulletPhysics::Raycast(glm::vec3 start, glm::vec3 direction) {
    glm::vec3 end = direction * 1000.f;
    btCollisionWorld::ClosestRayResultCallback rayCallback(
                                                           btVector3(start.x, start.y, start.z),
                                                           btVector3(end.x, end.y, end.z)
                                                           );
    rayCallback.m_collisionFilterMask &= ~(int)ZPhysicsBodyType::Trigger;
    
    dynamicsWorld_->rayTest(
                            btVector3(start.x, start.y, start.z),
                            btVector3(end.x, end.y, end.z),
                            rayCallback
                            );
    
    ZRaycastHitResult hitResult;
    hitResult.hasHit = false;
    if (rayCallback.hasHit()) {
        hitResult.objectHit = static_cast<ZGameObject*>(rayCallback.m_collisionObject->getUserPointer());
        hitResult.hitPosition = glm::vec3(rayCallback.m_hitPointWorld.x(), rayCallback.m_hitPointWorld.y(), rayCallback.m_hitPointWorld.z());
        hitResult.hasHit = true;
    }
    return hitResult;
}

void ZBulletPhysics::DebugDraw() {
    dynamicsWorld_->debugDrawWorld();
}

void ZBulletPhysics::TickCallback(btDynamicsWorld* world, btScalar timeStep) {
    ZBulletPhysics* physics = static_cast<ZBulletPhysics*>(world->getWorldUserInfo());
    assert(physics != nullptr);
    if (physics == nullptr) return;
    
    ZCollisionPairs collisionPairs;
    
    btDispatcher* dispatcher = world->getDispatcher();
    for (int idx = 0; idx < dispatcher->getNumManifolds(); idx++) {
        btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(idx);
        if (!manifold) continue;
        
        btRigidBody const* body1 = static_cast<btRigidBody const*>(manifold->getBody0());
        btRigidBody const* body2 = static_cast<btRigidBody const*>(manifold->getBody1());
        
        bool swapped = body1 > body2;
        
        btRigidBody const* sortedBody1 = swapped ? body2 : body1;
        btRigidBody const* sortedBody2 = swapped ? body1 : body2;
        ZGameObject* body1GO = static_cast<ZGameObject*>(sortedBody1->getUserPointer());
        ZGameObject* body2GO = static_cast<ZGameObject*>(sortedBody2->getUserPointer());
        
        ZCollisionPair pair = std::make_pair(body1GO, body2GO);
        collisionPairs.insert(pair);
        
        if (physics->previousTickCollisionPairs_.find(pair) != physics->previousTickCollisionPairs_.end()) {
            std::shared_ptr<ZCollisionEvent> collisionEvent(new ZCollisionEvent(pair));
            ZEngine::EventAgent()->QueueEvent(collisionEvent);
        }
    }
    
    ZCollisionPairs removedCollisionPairs;
    
    std::set_difference(physics->previousTickCollisionPairs_.begin(), physics->previousTickCollisionPairs_.end(),
                        collisionPairs.begin(), collisionPairs.end(),
                        std::inserter(removedCollisionPairs, removedCollisionPairs.begin()));
    
    for (ZCollisionPairs::const_iterator it = removedCollisionPairs.begin(), end = removedCollisionPairs.end(); it != end; it++) {
        std::shared_ptr<ZCollisionEndEvent> collisionEndEvent(new ZCollisionEndEvent(*it));
        ZEngine::EventAgent()->QueueEvent(collisionEndEvent);
    }
    
    physics->previousTickCollisionPairs_ = collisionPairs;
}

void ZBulletPhysics::CleanUp() { 
    delete dynamicsWorld_;
    delete solver_;
    delete overlappingPairCache_;
    delete dispatcher_;
    delete collisionConfig_;
    ZPhysics::CleanUp();
}
