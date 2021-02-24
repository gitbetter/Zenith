/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZBulletPhysicsUniverse.cpp

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

#include "ZServices.hpp"
#include "ZBulletPhysicsUniverse.hpp"
#include "ZGame.hpp"
#include "ZScene.hpp"
#include "ZCamera.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZRaycastEvent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZBulletRigidBody.hpp"
#include "ZCollisionEvent.hpp"
#include "ZCollisionEndEvent.hpp"

void ZBulletPhysicsUniverse::Initialize()
{
    ZPhysicsUniverse::Initialize();

    collisionConfig_ = new btDefaultCollisionConfiguration();
    dispatcher_ = new btCollisionDispatcher(collisionConfig_);
    overlappingPairCache_ = new btDbvtBroadphase();
    solver_ = new btSequentialImpulseConstraintSolver;
    dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, overlappingPairCache_, solver_, collisionConfig_);

    dynamicsWorld_->setDebugDrawer(this);
    dynamicsWorld_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    dynamicsWorld_->setInternalTickCallback(TickCallback);
    dynamicsWorld_->setWorldUserInfo(this);
}

void ZBulletPhysicsUniverse::Update(double deltaTime)
{
    ZPhysicsUniverse::Update(deltaTime);
    dynamicsWorld_->stepSimulation(deltaTime, MAX_FIXED_UPDATE_ITERATIONS, UPDATE_STEP_SIZE);
}

void ZBulletPhysicsUniverse::AddRigidBody(std::shared_ptr<ZRigidBody> body)
{
    btRigidBody* ptr = static_cast<btRigidBody*>(body->Get());
    if (!ptr)
    {
        LOG("The given rigid body is not a btRigidBody", ZSeverity::Error);
        return;
    }

    dynamicsWorld_->addRigidBody(ptr, (int) body->Type(), (int) ZPhysicsBodyType::All);
}

void ZBulletPhysicsUniverse::RemoveRigidBody(std::shared_ptr<ZRigidBody> body)
{
    btRigidBody* ptr = static_cast<btRigidBody*>(body->Get());
    if (!ptr)
    {
        LOG("The given rigid body is not a btRigidBody", ZSeverity::Error);
        return;
    }

    dynamicsWorld_->removeRigidBody(ptr);
}

ZRaycastHitResult ZBulletPhysicsUniverse::Raycast(const glm::vec3& start, const glm::vec3& direction, float t)
{
    glm::vec3 end = direction * t;
    btCollisionWorld::ClosestRayResultCallback rayCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z)
    );
    rayCallback.m_collisionFilterMask &= ~(int) ZPhysicsBodyType::Trigger;

    dynamicsWorld_->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        rayCallback
    );

    ZRaycastHitResult hitResult;
    hitResult.hasHit = false;
    if (rayCallback.hasHit())
    {
        hitResult.objectHit = static_cast<ZGameObject*>(rayCallback.m_collisionObject->getUserPointer());
        hitResult.hitPosition = glm::vec3(rayCallback.m_hitPointWorld.x(), rayCallback.m_hitPointWorld.y(), rayCallback.m_hitPointWorld.z());
        hitResult.hasHit = true;
    }
    return hitResult;
}

void ZBulletPhysicsUniverse::DebugDraw(const std::shared_ptr<ZScene>& scene)
{
    if (!scene) return;
    scene_ = scene;
    dynamicsWorld_->debugDrawWorld();
    scene_.reset();
}

void ZBulletPhysicsUniverse::TickCallback(btDynamicsWorld* world, btScalar timeStep)
{
    ZBulletPhysicsUniverse* physics = static_cast<ZBulletPhysicsUniverse*>(world->getWorldUserInfo());
    assert(physics != nullptr);
    if (physics == nullptr) return;

    ZCollisionPairs collisionPairs;

    btDispatcher* dispatcher = world->getDispatcher();
    for (int idx = 0; idx < dispatcher->getNumManifolds(); idx++)
    {
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

        if (physics->previousTickCollisionPairs_.find(pair) != physics->previousTickCollisionPairs_.end())
        {
            std::shared_ptr<ZCollisionEvent> collisionEvent(new ZCollisionEvent(pair));
            ZServices::EventAgent()->Queue(collisionEvent);
        }
    }

    ZCollisionPairs removedCollisionPairs;

    std::set_difference(physics->previousTickCollisionPairs_.begin(), physics->previousTickCollisionPairs_.end(),
        collisionPairs.begin(), collisionPairs.end(),
        std::inserter(removedCollisionPairs, removedCollisionPairs.begin()));

    for (ZCollisionPairs::const_iterator it = removedCollisionPairs.begin(), end = removedCollisionPairs.end(); it != end; it++)
    {
        std::shared_ptr<ZCollisionEndEvent> collisionEndEvent(new ZCollisionEndEvent(*it));
        ZServices::EventAgent()->Queue(collisionEndEvent);
    }

    physics->previousTickCollisionPairs_ = collisionPairs;
}

void ZBulletPhysicsUniverse::CleanUp()
{
    delete dynamicsWorld_;
    delete solver_;
    delete overlappingPairCache_;
    delete dispatcher_;
    delete collisionConfig_;
    ZPhysicsUniverse::CleanUp();
}

void ZBulletPhysicsUniverse::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    auto scene = scene_.lock();
    if (!scene) return;

    ZServices::Graphics()->DebugDrawLine(
        scene,
        glm::vec3(from.x(), from.y(), from.z()),
        glm::vec3(to.x(), to.y(), to.z()),
        glm::vec4(color.x(), color.y(), color.z(), 1.f)
    );
}
