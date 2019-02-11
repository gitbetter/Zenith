//
//  ZObject.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>
#include <functional>
#include <map>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
enum ZEventType {
  FirePress
};

typedef std::function<void()> ZEventCallback;

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
  
protected:
  std::map<ZEventType, std::vector<ZEventCallback>> eventCallbacks_;
};
