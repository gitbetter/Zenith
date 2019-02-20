//
//  ZIEscapeCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIFireCommand.hpp"
#include "ZObject.hpp"

void ZIFireCommand::Execute(ZObject* object) const {
  object->HandleFire();
}