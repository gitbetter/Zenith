//
//  ZObject.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZObject {
private:
public:
  virtual ~ZObject() { }

  virtual void HandleStrafe(float controlThrow) { }
  virtual void HandleUpDown(float controlThrow) { }
  virtual void HandleForwardBack(float controlThrow) { }
  virtual void HandlePitch(float controlThrow) { }
  virtual void HandleYaw(float controlThrow) { }
  virtual void HandleFire() { }
  virtual void HandleEscape() { }

  virtual void On(ZEventType event, ZEventCallback&& callback);
  virtual void Fire(ZEventType event);

  std::sting ID() { return id_; }

protected:
  std::string id_;
  std::map<ZEventType, std::vector<ZEventCallback>> eventCallbacks_;
};
