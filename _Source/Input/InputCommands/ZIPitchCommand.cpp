//
//  ZIUpDownCommand.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIPitchCommand.hpp"
#include "ZObject.hpp"

void ZIPitchCommand::Execute(ZObject* object) const {
  object->HandlePitch(controlThrow_);
}
