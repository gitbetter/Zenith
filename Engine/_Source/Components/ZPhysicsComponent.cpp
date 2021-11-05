/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZPhysicsComponent.cpp

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

#include "ZPhysicsComponent.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZScene.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZOFTree.hpp"
#include "ZBulletRigidBody.hpp"
#include "btBulletDynamicsCommon.h"

#include <rttr/registration>

ZIDSequence ZPhysicsComponent::idGenerator_;

ZPhysicsComponent::ZPhysicsComponent()
    : ZComponent()
{
    name = "PhysicsComponent_" + std::to_string(idGenerator_.Next());
}

void ZPhysicsComponent::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (dataNode == nullptr)
    {
        LOG("Could not initalize ZPhysicsComponent", ZSeverity::Error);
        return;
    }

    btScalar mass = -1.f, damping = 1.f, angularDamping = 1.f, restitution = 0.f;
    ZPhysicsBodyType type = ZPhysicsBodyType::Static;
    bool gravity = false;

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("mass") != props.end() && props["mass"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> massProp = props["mass"]->Value<ZOFNumber>(0);
        if (mass == -1.f)
        {
            mass = massProp->value;
        }
    }

    if (props.find("type") != props.end() && props["type"]->HasValues())
    {
        std::shared_ptr<ZOFString> typeProp = props["type"]->Value<ZOFString>(0);

        if (typeProp->value == "Static")
        {
            mass = 0.f;
        }
        else
        {
            ZAssets::GameObjectManager()->SetRenderOrder(rootObject, ZRenderLayer::Dynamic);
        }

        if (typeProp->value == "Dynamic") type = ZPhysicsBodyType::Dynamic;
        else if (typeProp->value == "Static") type = ZPhysicsBodyType::Static;
        else if (typeProp->value == "Kinematic") type = ZPhysicsBodyType::Kinematic;
        else if (typeProp->value == "Trigger") type = ZPhysicsBodyType::Trigger;
        else if (typeProp->value == "Character") type = ZPhysicsBodyType::Character;
        else if (typeProp->value == "Particle") type = ZPhysicsBodyType::Particle;
    }

    if (props.find("damping") != props.end() && props["damping"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> dampingProp = props["damping"]->Value<ZOFNumber>(0);
        damping = dampingProp->value;
        if (props["damping"]->ValueCount() > 1)
            angularDamping = props["damping"]->Value<ZOFNumber>(1)->value;
    }

    if (props.find("restitution") != props.end() && props["restitution"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> restProp = props["restitution"]->Value<ZOFNumber>(0);
        restitution = restProp->value;
    }

    if (props.find("hasGravity") != props.end() && props["hasGravity"]->HasValues())
    {
        std::shared_ptr<ZOFString> gravProp = props["hasGravity"]->Value<ZOFString>(0);
        gravity = gravProp->value == "Yes";
    }

    if (mass < 0) mass = 0.0;

    Initialize(type, mass, gravity);

    SetLinearDamping(damping);
    SetAngularDamping(angularDamping);
    SetRestitution(restitution);
}

void ZPhysicsComponent::Initialize(ZPhysicsBodyType bodyType, float mass, bool hasGravityInfluence)
{
    body = std::make_shared<ZBulletRigidBody>(bodyType, mass, ZAssets::GameObjectManager()->Position(rootObject), ZAssets::GameObjectManager()->Scale(rootObject), ZAssets::GameObjectManager()->Orientation(rootObject));
    body->Initialize();
    body->SetGameObject(rootObject);
    if (hasGravityInfluence)
    {
        body->SetGravity(glm::vec3(0.f, -9.8f, 0.f));
    }
}

void ZPhysicsComponent::OnUpdate(double deltaTime)
{
    assert(!rootObject.IsNull() && body != nullptr);

    if (!inUniverse_ && ZAssets::GameObjectManager()->Scene(rootObject)) {
        ZAssets::GameObjectManager()->Scene(rootObject)->PhysicsUniverse()->AddRigidBody(body);
        inUniverse_ = true;
    }

    if (body->Type() == ZPhysicsBodyType::Trigger)
        body->SetTransformMatrix(ZAssets::GameObjectManager()->ModelMatrix(rootObject));

    if (ZAssets::GameObjectManager()->Scene(rootObject)->PlayState() != ZPlayState::Playing)
        return;

    // Don't update static rigid bodies with the physic engine, since the physics engine cannot affect them
    if (body->InverseMass() == 0.0) return;

    glm::mat4 M = body->TransformMatrix();
    M = glm::scale(M, ZAssets::GameObjectManager()->Scale(rootObject));
    ZAssets::GameObjectManager()->SetModelMatrix(rootObject, M);
}

void ZPhysicsComponent::OnCloned(const ZHComponent& original)
{
    assert(body != nullptr);
    ZPhysicsComponent* originalComp = ZAssets::ComponentManager()->Dereference<ZPhysicsComponent>(original);
    body = originalComp->body->Clone();
}

void ZPhysicsComponent::OnCleanUp()
{
    if (!rootObject.IsNull() && body != nullptr)
    {
        ZAssets::GameObjectManager()->Scene(rootObject)->PhysicsUniverse()->RemoveRigidBody(body);
    }
}

void ZPhysicsComponent::SetLinearDamping(float damping)
{
    assert(body != nullptr);
    body->SetLinearDamping(damping);
}

void ZPhysicsComponent::SetAngularDamping(float damping)
{
    assert(body != nullptr);
    body->SetAngularDamping(damping);
}

void ZPhysicsComponent::SetRestitution(float restitution)
{
    assert(body != nullptr);
    body->SetRestitution(restitution);
}

void ZPhysicsComponent::SetTransform(const glm::mat4& transform)
{
    assert(body != nullptr);
    body->SetTransformMatrix(transform);
}

void ZPhysicsComponent::AddCollider(const std::shared_ptr<ZCollider>& collider)
{
    assert(body != nullptr);
    body->AddCollider(collider);
}

void ZPhysicsComponent::DisableCollisionResponse()
{
    assert(body != nullptr);
    body->DisableContactResponse();
}

void ZPhysicsComponent::AddForce(const glm::vec3& force)
{
    assert(body != nullptr);
    body->ApplyForce(force);
}

void ZPhysicsComponent::AddForceAtPoint(const glm::vec3& force, const glm::vec3& point)
{
    assert(body != nullptr);
    body->ApplyForceAtPoint(force, point);
}

void ZPhysicsComponent::AddTorque(const glm::vec3& torque)
{
    assert(body != nullptr);
    body->ApplyTorque(torque);
}

bool ZPhysicsComponent::HasFiniteMass()
{
    assert(body != nullptr);
    return body->InverseMass() != 0.0;
}

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<ZPhysicsComponent>("ZPhysicsComponent").constructor<>();
}