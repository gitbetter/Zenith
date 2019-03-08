//
//  ZParticleSystem.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZParticleSystem.hpp"

ZParticleSystem::ZParticleSystem(std::initializer_list<ZParticleRule> rules) {
  for (ZParticle* particle : particles_) {
    particles_.push_back(particle);
  }
}

void ZParticleSystem::Update() {
  // TODO: Create a particle based on a particle rule and set its age randomly between the min and max ages.
  // Also set the velocity randomly between the min and max velocities. If emitting randomly,
  // pick randmly from the list of provided particles rules to create the particle.
  // Make sure emissionRate time has elapsed before the previous emission to emit a new particle.
}
