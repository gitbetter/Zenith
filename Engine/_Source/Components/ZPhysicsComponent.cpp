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
#include "ZPhysicsFactory.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZOFTree.hpp"
#include "ZBulletRigidBody.hpp"
#include "ZServices.hpp"

ZPhysicsComponent::ZPhysicsComponent() : ZComponent()
{
    id_ = "ZCPhysics_" + idGenerator_.Next();
}

void ZPhysicsComponent::Initialize()
{
    ZComponent::Initialize();
    if (body_) object_->Scene()->PhysicsUniverse()->AddRigidBody(body_);
}

void ZPhysicsComponent::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZPhysicsComponent", ZSeverity::Error);
        return;
    }

    btScalar mass = -1., damping = 1., angularDamping = 1., restitution = 0.;
    ZPhysicsBodyType type = ZPhysicsBodyType::Static;
    std::string shape = "";
    bool gravity = false;
    std::vector<float> size, origin, offset, rotation;

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
        else object_->SetRenderOrder(ZRenderOrder::Dynamic);

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

    if (props.find("collider") != props.end() && props["collider"]->HasValues())
    {
        std::shared_ptr<ZOFString> colliderProp = props["collider"]->Value<ZOFString>(0);
        shape = colliderProp->value;

        if (props["collider"]->ValueCount() > 1)
        {
            std::shared_ptr<ZOFNumberList> scaleProp = props["collider"]->Value<ZOFNumberList>(1);
            std::transform(scaleProp->value.begin(), scaleProp->value.end(), std::back_inserter(size), [] (float val) { return val; });
        }

        if (props["collider"]->ValueCount() > 2)
        {
            std::shared_ptr<ZOFNumberList> centerProp = props["collider"]->Value<ZOFNumberList>(2);
            std::transform(centerProp->value.begin(), centerProp->value.end(), std::back_inserter(origin), [] (float val) { return val; });
        }

        if (props["collider"]->ValueCount() > 3)
        {
            std::shared_ptr<ZOFNumberList> rotProp = props["collider"]->Value<ZOFNumberList>(2);
            std::transform(rotProp->value.begin(), rotProp->value.end(), std::back_inserter(rotation), [] (float val) { return val; });
        }
    }

    if (props.find("colliderOffset") != props.end() && props["colliderOffset"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> offsetProp = props["colliderOffset"]->Value<ZOFNumberList>(0);
        std::transform(offsetProp->value.begin(), offsetProp->value.end(), std::back_inserter(offset), [] (float val) { return val; });
    }

    if (props.find("hasGravity") != props.end() && props["hasGravity"]->HasValues())
    {
        std::shared_ptr<ZOFString> gravProp = props["hasGravity"]->Value<ZOFString>(0);
        gravity = gravProp->value == "Yes";
    }

    glm::vec3 pos, scale, offs;
    glm::quat rot;

    if (mass < 0) mass = 0.0;

    if (size.size() < 3) scale = object_->Scale();
    else scale = glm::vec3(size[0], size[1], size[2]);

    if (origin.size() < 3) pos = object_->Position();
    else pos = glm::vec3(origin[0], origin[1], origin[2]);

    if (rotation.size() < 3) rot = object_->Orientation();
    else rot = glm::quat(glm::vec3(rotation[0], rotation[1], rotation[2]));

    if (offset.size() < 3) offs = glm::vec3(0.f);
    else offs = glm::vec3(offset[0], offset[1], offset[2]);

    Initialize(type, shape, mass, pos, scale, rot);

    if (gravity)
    {
        glm::vec3 gravityForce(0.f, -30.f, 0.f);
        if (gravity) body_->SetGravity(gravityForce);
    }
    body_->SetLinearDamping(damping);
    body_->SetAngularDamping(angularDamping);
    body_->SetRestitution(restitution);
    body_->SetColliderOffset(offs);
}

void ZPhysicsComponent::Initialize(ZPhysicsBodyType bodyType, const std::string& colliderType, float mass, const glm::vec3& position, const glm::vec3& size, const glm::quat& rotation)
{
    std::shared_ptr<ZCollider> collider;
    if (!colliderType.empty())
    {
        collider = ZCollider::Create(colliderType, size);
    }
    else
    {
        LOG("Could not create the given collider for object " + object_->ID() + ". Creating a default collider instead.", ZSeverity::Warning);
        collider = ZCollider::Create("Box", size);
    }

    body_ = std::make_shared<ZBulletRigidBody>(bodyType, collider, mass, position, size, rotation);
    body_->Initialize();
    body_->SetGameObject(object_);
}

void ZPhysicsComponent::Update(double deltaTime)
{
    assert(object_ != nullptr && body_ != nullptr);

    // Don't update static rigid bodies, since the physics engine cannot affect them
    if (body_->InverseMass() == 0.0) return;

    glm::mat4 M = body_->TransformMatrix();
    M = glm::scale(M, object_->Scale());
    object_->SetModelMatrix(M);
}

std::shared_ptr<ZComponent> ZPhysicsComponent::Clone()
{
    std::shared_ptr<ZPhysicsComponent> clone = std::make_shared<ZPhysicsComponent>();
    clone->body_ = body_->Clone();
    return clone;
}

void ZPhysicsComponent::CleanUp()
{
    ZComponent::CleanUp();
    object_->Scene()->PhysicsUniverse()->RemoveRigidBody(body_);
}

void ZPhysicsComponent::DisableCollisionResponse()
{
    if (body_) body_->DisableContactResponse();
}

void ZPhysicsComponent::AddForce(const glm::vec3& force)
{
    body_->ApplyForce(force);
}

void ZPhysicsComponent::AddForceAtPoint(const glm::vec3& force, const glm::vec3& point)
{
    body_->ApplyForceAtPoint(force, point);
}

void ZPhysicsComponent::AddTorque(const glm::vec3& torque)
{
    body_->ApplyTorque(torque);
}

bool ZPhysicsComponent::HasFiniteMass()
{
    return body_->InverseMass() != 0.0;
}
