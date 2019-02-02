//
//  ZLight.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 01/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGameObject.hpp"
#include <glm/glm.hpp>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
enum ZLightType {
  Directional, Point, Spot, Area, Hemisphere
};

struct ZLBaseProperties {
  glm::vec3 ambient{0.3f};
  glm::vec3 color{0.8f};
};

struct ZLAttenuationProperties {
  float constant;
  float linear;
  float quadratic;
};

struct ZLDirectional {
  ZLBaseProperties components;
  glm::vec3 direction{0.3f, 1.f, 0.3f};
};

struct ZLPoint {
  ZLBaseProperties components;
  ZLAttenuationProperties attenunation;
  glm::vec3 position{0.4f, 1.f, 1.f};
};

struct ZLSpot {
  ZLBaseProperties components;
  ZLAttenuationProperties attenunation;
  glm::vec3 position{0.4f, 1.f, 1.f};
  glm::vec3 coneDirection{0.f, 1.f, 1.f};
  float spotCutoff;
  float spotExponent;
};

struct ZLHemisphere {
  glm::vec3 position{0.8f, 3.f, 1.f};
  glm::vec3 skyColor{0.8f};
  glm::vec3 groundColor{0.1f};
};

struct ZLight : public ZGameObject {
  ZLight() { }
  ZLight(ZLightType lightType) : type(lightType) { }
  ~ZLight() { }

  ZLightType type{ZLightType::Point};
  bool enabled{true};

  ZLDirectional directional;
  ZLPoint point;
  ZLSpot spot;
  ZLHemisphere hemisphere;
};
