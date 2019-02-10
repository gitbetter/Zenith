//
//  ZIForwardBackCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIForwardBackCommand.hpp"
#include "ZObject.hpp"

void ZIForwardBackCommand::Execute(ZObject* object) const {
  object->HandleForwardBack(controlThrow_);
}
