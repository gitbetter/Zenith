//
//  ZCollisionPrimitive.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 21/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZCollisionPrimitive.hpp"
#include "ZGameObject.hpp"

void ZCollisionPrimitive::CalculateInternals() {
  transform = body->ModelMatrix() * offset;
}

ZCollisionPrimitive* ZCollisionPrimitive::Create(std::string type, glm::vec3 min, glm::vec3 max) {
  ZCollisionPrimitive* primitive = nullptr;
  if (type == "Sphere") {
    ZCollisionSphere* sphere = new ZCollisionSphere;
    sphere->radius = glm::length(max - min) / 2.f;
    primitive = sphere;
  } else if (type == "Plane") {
    ZCollisionPlane* plane = new ZCollisionPlane;
    plane->direction = glm::vec3(0.f, 1.f, 0.f);
    plane->offset = 0.f;
    primitive = plane;
  } else if (type == "Box") {
    ZCollisionBox* box = new ZCollisionBox;
    box->halfSize = glm::vec3((max.x - min.x) / 2.f, (max.y - min.y) / 2.f, (max.z - min.z) / 2.f);
    primitive = box;
  }
  return primitive;
}
