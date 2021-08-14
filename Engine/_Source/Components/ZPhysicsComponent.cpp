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
#include "ZScene.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZOFTree.hpp"
#include "ZBulletRigidBody.hpp"
#include "ZServices.hpp"
#include "btBulletDynamicsCommon.h"

#include <rttr/registration>

ZPhysicsComponent::ZPhysicsComponent() : ZComponent()
{
    id_ = "ZCOMP_PHYSICS_" + std::to_string(idGenerator_.Next());
}

void ZPhysicsComponent::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZPhysicsComponent", ZSeverity::Error);
        return;
    }

    btScalar mass = -1.f, damping = 1.f, angularDamping = 1.f, restitution = 0.f;
    ZPhysicsBodyType type = ZPhysicsBodyType::Static;
    bool gravity = false;

    ZOFPropertyMap props = node->properties;

    if (props.find("mass") != props.end() && props["mass"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> massProp = props["mass"]->Value<ZOFNumber>(0);
        if (mass == -1.f) mass = massProp->value;
    }

    if (props.find("type") != props.end() && props["type"]->HasValues())
    {
        std::shared_ptr<ZOFString> typeProp = props["type"]->Value<ZOFString>(0);

        if (typeProp->value == "Static") mass = 0.f;
        else object_->SetRenderOrder(ZRenderLayer::Dynamic);

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
    body_ = std::make_shared<ZBulletRigidBody>(bodyType, mass, object_->Position(), object_->Scale(), object_->Orientation());
    body_->Initialize();
    body_->SetGameObject(object_);
    if (hasGravityInfluence)
    {
        body_->SetGravity(glm::vec3(0.f, -9.8f, 0.f));
    }
}

void ZPhysicsComponent::Update(double deltaTime)
{
    assert(object_ != nullptr && body_ != nullptr);

    if (!inUniverse_ && object_->Scene()) {
        object_->Scene()->PhysicsUniverse()->AddRigidBody(body_);
        inUniverse_ = true;
    }

    if (body_->Type() == ZPhysicsBodyType::Trigger)
        body_->SetTransformMatrix(object_->ModelMatrix());

    if (object_->Scene()->PlayState() != ZPlayState::Playing)
        return;

    // Don't update static rigid bodies with the physic engine, since the physics engine cannot affect them
    if (body_->InverseMass() == 0.0) return;

    glm::mat4 M = body_->TransformMatrix();
    M = glm::scale(M, object_->Scale());
    object_->SetModelMatrix(M);
}

std::shared_ptr<ZComponent> ZPhysicsComponent::Clone()
{
    assert(body_ != nullptr);
    std::shared_ptr<ZPhysicsComponent> clone = std::make_shared<ZPhysicsComponent>();
    clone->body_ = body_->Clone();
    return clone;
}

void ZPhysicsComponent::CleanUp()
{
    ZComponent::CleanUp();
    if (object_ && body_) {
        object_->Scene()->PhysicsUniverse()->RemoveRigidBody(body_);
    }
}

void ZPhysicsComponent::SetLinearDamping(float damping)
{
    assert(body_ != nullptr);
    body_->SetLinearDamping(damping);
}

void ZPhysicsComponent::SetAngularDamping(float damping)
{
    assert(body_ != nullptr);
    body_->SetAngularDamping(damping);
}

void ZPhysicsComponent::SetRestitution(float restitution)
{
    assert(body_ != nullptr);
    body_->SetRestitution(restitution);
}

void ZPhysicsComponent::SetTransform(const glm::mat4& transform)
{
    assert(body_ != nullptr);
    body_->SetTransformMatrix(transform);
}

void ZPhysicsComponent::AddCollider(const std::shared_ptr<ZCollider>& collider)
{
    assert(body_ != nullptr);
    body_->AddCollider(collider);
}

void ZPhysicsComponent::DisableCollisionResponse()
{
    assert(body_ != nullptr);
    body_->DisableContactResponse();
}

void ZPhysicsComponent::AddForce(const glm::vec3& force)
{
    assert(body_ != nullptr);
    body_->ApplyForce(force);
}

void ZPhysicsComponent::AddForceAtPoint(const glm::vec3& force, const glm::vec3& point)
{
    assert(body_ != nullptr);
    body_->ApplyForceAtPoint(force, point);
}

void ZPhysicsComponent::AddTorque(const glm::vec3& torque)
{
    assert(body_ != nullptr);
    body_->ApplyTorque(torque);
}

bool ZPhysicsComponent::HasFiniteMass()
{
    assert(body_ != nullptr);
    return body_->InverseMass() != 0.0;
}

DEFINE_COMPONENT_CREATORS(ZPhysicsComponent)

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<ZPhysicsComponent>("ZPhysicsComponent").constructor<>();
}