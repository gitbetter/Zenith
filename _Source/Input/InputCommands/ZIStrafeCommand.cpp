//
//  ZIMoveLeftCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIStrafeCommand.hpp"
#include "ZGameObject.hpp"

void ZIStrafeCommand::Execute(ZGameObject* gameObject) const {
  gameObject->HandleStrafe(controlThrow_);
}
