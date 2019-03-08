//
//  ZIUpDownCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZIYawCommand.hpp"
#include "ZObject.hpp"

void ZIYawCommand::Execute(std::shared_ptr<ZObject> object) const {
  object->HandleYaw(controlThrow_);
}
