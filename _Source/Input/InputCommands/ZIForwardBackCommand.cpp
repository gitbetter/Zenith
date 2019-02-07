//
//  ZIForwardBackCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIForwardBackCommand.hpp"
#include "ZGameObject.hpp"

void ZIForwardBackCommand::Execute(ZGameObject* gameObject) const {
  gameObject->HandleForwardBack(controlThrow_);
}
