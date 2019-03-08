//
//  ZIEscapeCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZIFireCommand.hpp"
#include "ZObject.hpp"

void ZIFireCommand::Execute(std::shared_ptr<ZObject> object) const {
  object->HandleFire();
}
