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

#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsFactory.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZGravityForce.hpp"
#include "ZObjectForceRegistry.hpp"
#include "ZPhysics.hpp"
#include "ZOFTree.hpp"

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
  std::vector<btScalar> size, origin;

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
      std::transform(scaleProp->value.begin(), scaleProp->value.end(), std::back_inserter(size), [](float val) { return btScalar(val); });
    }

    if (props["collider"]->ValueCount() > 2) {
      std::shared_ptr<ZOFNumberList> centerProp = props["collider"]->Value<ZOFNumberList>(2);
      std::transform(centerProp->value.begin(), centerProp->value.end(), std::back_inserter(origin), [](float val) { return btScalar(val); });
    }
  }

  if (props.find("hasGravity") != props.end() && props["hasGravity"]->HasValues()) {
    std::shared_ptr<ZOFString> gravProp = props["hasGravity"]->Value<ZOFString>(0);
    gravity = gravProp->value == "Yes";
  }


  // TODO: Extract rigid body into an interface
  btCollisionShape* collider;
  if (!type.empty()) {
    collider = ZEngine::PhysicsFactory()->CreateCollider(type, size);
  } else {
    _Z("Could not create the given collider for object " + object_->ID() + ". Creating a default collider instead.", ZWARNING);
    collider = ZEngine::PhysicsFactory()->CreateCollider("Box", size);
  }

  btTransform transform;
  transform.setIdentity();
  if (origin.size() >= 3) transform.setOrigin(btVector3(origin[0], origin[1], origin[2]));
  else transform.setOrigin(btVector3(object_->Position().x, object_->Position().y, object_->Position().z));

  if (size.size() < 3)
    collider->setLocalScaling(btVector3(object_->Scale().x, object_->Scale().y, object_->Scale().z));

  if (mass < 0) mass = 0.0;

  btVector3 localInertia(0, 0, 0);
  if (mass > 0.f) collider->calculateLocalInertia(mass, localInertia);

  btDefaultMotionState* motionState = new btDefaultMotionState(transform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collider, localInertia);
  body_ = std::shared_ptr<btRigidBody>(new btRigidBody(rbInfo));

  body_->setUserPointer(object_);

  if (gravity) body_->setGravity(btVector3(0.0, -30.0, 0.0));
  body_->setDamping(damping, angularDamping);
  body_->setRestitution(restitution);

  ZEngine::Physics()->AddRigidBody(body_);
}

void ZPhysicsComponent::Update() {
  assert(object_ != nullptr && body_ != nullptr);

  // Don't update static rigid bodies, since the physics engine
  // cannot affect them
  if (body_->getInvMass() == 0.0) return;

  btTransform transform;
  if (body_->getMotionState()) {
    body_->getMotionState()->getWorldTransform(transform);
  }

  glm::mat4 modelMatrix;
  transform.getOpenGLMatrix(glm::value_ptr(modelMatrix));

  object_->SetModelMatrix(modelMatrix);
}

void ZPhysicsComponent::AddForce(glm::vec3& force) {
  body_->activate();
  body_->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void ZPhysicsComponent::AddForceAtPoint(glm::vec3& force, glm::vec3& point) {
  body_->activate();
  body_->applyForce(btVector3(force.x, force.y, force.z), btVector3(point.x, point.y, point.z));
}

void ZPhysicsComponent::AddTorque(glm::vec3& torque) {
  body_->activate();
  body_->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

void ZPhysicsComponent::SetAwake(const bool awake) {

}

void ZPhysicsComponent::SetCanSleep(const bool canSleep) {

}
