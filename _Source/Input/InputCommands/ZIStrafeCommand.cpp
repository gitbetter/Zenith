//
//  ZIMoveLeftCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZIStrafeCommand.hpp"
#include "ZObject.hpp"

void ZIStrafeCommand::Execute(std::shared_ptr<ZObject> object) const {
  object->HandleStrafe(controlThrow_);
}
