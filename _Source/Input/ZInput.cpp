//
//  ZInput.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZInput.hpp"
#include "ZObject.hpp"
#include "ZInputCommand.hpp"

void ZInput::Register(std::shared_ptr<ZObject> object) {
  registeredObjects_.push_back(object);
}

void ZInput::Broadcast(const ZInputCommand& command) {
  for (std::shared_ptr<ZObject> object : registeredObjects_) {
    command.Execute(object);
  }
}
