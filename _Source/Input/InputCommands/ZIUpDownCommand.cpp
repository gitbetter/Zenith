//
//  ZIUpDownCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZIUpDownCommand.hpp"
#include "ZObject.hpp"

void ZIUpDownCommand::Execute(std::shared_ptr<ZObject> object) const {
  object->HandleUpDown(controlThrow_);
}
