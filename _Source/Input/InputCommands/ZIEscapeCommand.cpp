//
//  ZIEscapeCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIEscapeCommand.hpp"
#include "ZObject.hpp"

void ZIEscapeCommand::Execute(std::shared_ptr<ZObject> object) const {
  object->HandleEscape();
}
