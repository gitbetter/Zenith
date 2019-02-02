//
//  TModel.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh.hpp"
#include <glm/vec4.hpp>
#include <vector>
#include <string>

// Forward Declarations
class ZShader;
class ZCamera;

// Class Definitions
enum ZPrimitiveType {
  Plane, Cube, Sphere, Cylinder, Cone
};

class ZModel {
private:
  std::vector<ZMesh> meshes_;

  void CreatePlane(glm::vec3 scale);
  void CreateCube(glm::vec3 scale);
  void CreateSphere(glm::vec3 scale);
  void CreateCylinder(glm::vec3 scale);
  void CreateCone(glm::vec3 scale);

public:
  ZModel(ZPrimitiveType primitiveType, glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f));
  ZModel(std::string path);
  ~ZModel() { }

  virtual void Render(ZShader* shader);

  static ZModel NewPlanePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f));
  static ZModel NewCubePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f));
  static ZModel NewSpherePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f));
  static ZModel NewCylinderPrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f));
  static ZModel NewConePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f));
};
