//
//  ZIUpDownCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIYawCommand.hpp"
#include "ZObject.hpp"

void ZIYawCommand::Execute(ZObject* object) const {
  object->HandleYaw(controlThrow_);
}
