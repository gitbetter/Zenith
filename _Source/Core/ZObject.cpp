//
//  ZObject.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZObject.hpp"

void ZObject::On(ZEventType event, ZEventCallback&& callback) {
  if (eventCallbacks_.find(event) == eventCallbacks_.end()) {
    eventCallbacks_.emplace(std::make_pair(event, std::vector<ZEventCallback>{std::move(callback)}));
  } else {
    eventCallbacks_[event].push_back(std::move(callback));
  }
}

void ZObject::Fire(ZEventType event) {
  if (eventCallbacks_.find(event) == eventCallbacks_.end()) return;
  for (ZEventCallback& callback : eventCallbacks_[event]) {
    callback();
  }
}
