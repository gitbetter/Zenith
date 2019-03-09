//
//  ZResource.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 08/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZResource.hpp"

ZResource::ZResource(const std::string& name) {
  this->name = name;
  std::transform(this->name.begin(), this->name.end(), this->name.begin(), (int(*)(int))std::tolower);
}
