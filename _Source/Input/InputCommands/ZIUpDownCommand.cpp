//
//  ZIUpDownCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIUpDownCommand.hpp"
#include "ZGameObject.hpp"

void ZIUpDownCommand::Execute(ZGameObject* gameObject) const {
  gameObject->HandleUpDown(controlThrow_);
}
