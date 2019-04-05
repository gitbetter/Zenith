/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZLight.hpp

    Created by Adrian Sanchez on 01/02/2019.
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

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGameObject.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
struct ZLBaseProperties {
  glm::vec3 ambient{0.2f};
  glm::vec3 color{0.25f};
};

struct ZLAttenuationProperties {
  float constant{1.0f};
  float linear{0.7f};
  float quadratic{1.8f};
};

struct ZLDirectional {
  ZLBaseProperties components;
  glm::vec3 direction{20.f, 5.f, 10.f};
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
  ZLight(ZLightType lightType) : ZGameObject(glm::vec3(1.f)) { type = lightType; }
  ~ZLight() { }

  ZLightType type{ZLightType::Point};
  bool enabled{true};

  ZLDirectional directional;
  ZLPoint point;
  ZLSpot spot;
  ZLHemisphere hemisphere;
};
