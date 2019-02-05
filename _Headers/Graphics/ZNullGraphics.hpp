//
//  ZNullGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphics.hpp"
#include "ZLogger.hpp"
#include <vector>

// Forward Declarations
class ZGameObject;
class ZWindow;
struct ZLight;

// Class and Data Structure Definitions
class ZNullGraphics : public ZGraphics {
private:

public:
    ZNullGraphics() { }
    ~ZNullGraphics() { }

    void Draw(const std::vector<ZGameObject*>& gameObjects, const std::vector<ZLight*>& gameLights, float frameMix) override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }

    void Delete() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }

    void SwapBuffers() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }

    void EnableStencilBuffer() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }

    void DisableStencilBuffer() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }

    void GenerateDepthMap() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }

    void DrawDepthMap(ZLight* light) override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }

    ZWindow* GetWindow() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
      return nullptr; // Should we?
    }
protected:

};
