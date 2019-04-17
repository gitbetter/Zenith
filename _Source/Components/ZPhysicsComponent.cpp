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
#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsFactory.hpp"
#include "ZPhysics.hpp"
#include "ZOFTree.hpp"
#include "ZBulletRigidBody.hpp"

ZPhysicsComponent::ZPhysicsComponent() : ZComponent() {
    id_ = "ZCPhysics_" + ZEngine::IDSequence()->Next();
}

// TODO: These initalize functions can get pretty hectic. Maybe there's a better way...
void ZPhysicsComponent::Initialize(std::shared_ptr<ZOFNode> root) {
    ZComponent::Initialize();
    
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if(node == nullptr) {
        _Z("Could not initalize ZPhysicsComponent", ZERROR);
        return;
    }
    
    btScalar mass = -1., damping = 1., angularDamping = 1., restitution = 0.;
    std::string type = "";
    bool gravity = false;
    std::vector<float> size, origin;
    
    ZOFPropertyMap props = node->properties;
    
    if (props.find("type") != props.end() && props["type"]->HasValues()) {
        std::shared_ptr<ZOFString> typeProp = props["type"]->Value<ZOFString>(0);
        if (typeProp->value == "Static") mass = 0.f;
    }
    
    if (props.find("mass") != props.end() && props["mass"]->HasValues()) {
        std::shared_ptr<ZOFNumber> massProp = props["mass"]->Value<ZOFNumber>(0);
        if (mass == -1.f) mass = massProp->value;
    }
    
    if (props.find("damping") != props.end() && props["damping"]->HasValues()) {
        std::shared_ptr<ZOFNumber> dampingProp = props["damping"]->Value<ZOFNumber>(0);
        damping = dampingProp->value;
        if (props["damping"]->ValueCount() > 1)
            angularDamping = props["damping"]->Value<ZOFNumber>(1)->value;
    }
    
    if (props.find("restitution") != props.end() && props["restitution"]->HasValues()) {
        std::shared_ptr<ZOFNumber> restProp = props["restitution"]->Value<ZOFNumber>(0);
        restitution = restProp->value;
    }
    
    if (props.find("collider") != props.end() && props["collider"]->HasValues()) {
        std::shared_ptr<ZOFString> colliderProp = props["collider"]->Value<ZOFString>(0);
        type = colliderProp->value;
        
        if (props["collider"]->ValueCount() > 1) {
            std::shared_ptr<ZOFNumberList> scaleProp = props["collider"]->Value<ZOFNumberList>(1);
            std::transform(scaleProp->value.begin(), scaleProp->value.end(), std::back_inserter(size), [](float val) { return val; });
        }
        
        if (props["collider"]->ValueCount() > 2) {
            std::shared_ptr<ZOFNumberList> centerProp = props["collider"]->Value<ZOFNumberList>(2);
            std::transform(centerProp->value.begin(), centerProp->value.end(), std::back_inserter(origin), [](float val) { return val; });
        }
    }
    
    if (props.find("hasGravity") != props.end() && props["hasGravity"]->HasValues()) {
        std::shared_ptr<ZOFString> gravProp = props["hasGravity"]->Value<ZOFString>(0);
        gravity = gravProp->value == "Yes";
    }
    
    glm::vec3 pos, scale;
    
    std::shared_ptr<ZCollider> collider;
    if (!type.empty()) {
        collider = ZEngine::PhysicsFactory()->CreateCollider(type, size);
    } else {
        _Z("Could not create the given collider for object " + object_->ID() + ". Creating a default collider instead.", ZWARNING);
        collider = ZEngine::PhysicsFactory()->CreateCollider("Box", size);
    }
    
    if (mass < 0) mass = 0.0;
    
    if (origin.size() < 3) pos = object_->Position();
    else pos = glm::vec3(origin[0], origin[1], origin[2]);
    
    if (size.size() < 3) scale = object_->Scale();
    else scale = glm::vec3(size[0], size[1], size[2]);
    
    body_ = std::make_shared<ZBulletRigidBody>(collider, mass, pos, scale);
    
    if (gravity) {
        glm::vec3 gravityForce(0.f, -30.f, 0.f);
        if (gravity) body_->SetGravity(gravityForce);
    }
    body_->SetGameObject(object_);
    body_->SetLinearDamping(damping);
    body_->SetAngularDamping(angularDamping);
    body_->SetRestitution(restitution);
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    
    ZEngine::Physics()->AddRigidBody(body_);
}

void ZPhysicsComponent::Update() {
    assert(object_ != nullptr && body_ != nullptr);
    
    // Don't update static rigid bodies, since the physics engine cannot affect them
    if (body_->InverseMass() == 0.0) return;
    
    object_->SetModelMatrix(body_->TransformMatrix());
}

void ZPhysicsComponent::AddForce(glm::vec3& force) {
    body_->ApplyForce(force);
}

void ZPhysicsComponent::AddForceAtPoint(glm::vec3& force, glm::vec3& point) {
    body_->ApplyForceAtPoint(force, point);
}

void ZPhysicsComponent::AddTorque(glm::vec3& torque) {
    body_->ApplyTorque(torque);
}

bool ZPhysicsComponent::HasFiniteMass() {
    return body_->InverseMass() != 0.0;
}
