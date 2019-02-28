//
//  ZLight.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 01/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGameObject.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
struct ZLBaseProperties {
  glm::vec3 ambient{0.2f};
  glm::vec3 color{0.85f};
};

struct ZLAttenuationProperties {
  float constant{1.0f};
  float linear{0.014f};
  float quadratic{0.0007f};
};

struct ZLDirectional {
  ZLBaseProperties components;
  glm::vec3 direction{10.f, 1.f, 10.f};
};

struct ZLPoint {
  ZLBaseProperties components;
  ZLAttenuationProperties attenunation;
};

struct ZLSpot {
  ZLBaseProperties components;
  ZLAttenuationProperties attenunation;
  glm::vec3 coneDirection{0.f, 1.f, 1.f};
  float spotCutoff;
  float spotExponent;
};

struct ZLHemisphere {
  ZLBaseProperties components;
  glm::vec3 skyColor{0.8f};
  glm::vec3 groundColor{0.1f};
};

struct ZLight : public ZGameObject {
  ZLight() { }
  ZLight(ZLightType lightType) : ZGameObject(glm::vec3(24.f)) { type = lightType; }
  ~ZLight() { }

  ZLightType type{ZLightType::Point};
  bool enabled{true};

  ZLDirectional directional;
  ZLPoint point;
  ZLSpot spot;
  ZLHemisphere hemisphere;
};
