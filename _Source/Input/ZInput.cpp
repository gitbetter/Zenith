//
//  ZInput.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZInput.hpp"
#include "ZGameObject.hpp"
#include "ZInputCommand.hpp"

void ZInput::Register(ZGameObject& gameObject) {
  registeredObjects_.push_back(&gameObject);
}

void ZInput::Broadcast(const ZInputCommand& command) {
  for (ZGameObject* object : registeredObjects_) {
    command.Execute(object);
  }
}
