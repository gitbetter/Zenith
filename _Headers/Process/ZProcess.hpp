/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZProcess.hpp

    Created by Adrian Sanchez on 23/03/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZProcess {

private:

  std::shared_ptr<ZProcess> child_;

public:

  ZProcess() { }
  virtual ~ZProcess() { }

  virtual void Initialize() { state_ = ZProcessState::Running; };
  virtual void Update() { };
  virtual void Render(float frameMix = 1.f, RENDER_OP renderOp = RENDER_OP_COLOR) { };
  virtual void Pause() { state_ = ZProcessState::Paused; };
  virtual void Continue() { state_ = ZProcessState::Running; };
  virtual void Finish() { state_ = ZProcessState::Finished; };
  virtual void Fail() { state_ = ZProcessState::Failed; };
  virtual void Abort() { state_ = ZProcessState::Aborted; };
  virtual void CleanUp() { };

  std::string ID() { return id_; }

  ZProcessState State() const { return state_; }
  std::shared_ptr<ZProcess> Child() { return child_; }

  bool IsAlive() const { return state_ == ZProcessState::Running || state_ == ZProcessState::Paused; }
  bool IsDead() const { return state_ == ZProcessState::Finished || state_ == ZProcessState::Failed || 
                               state_ == ZProcessState::Aborted; }
  bool IsPaused() const { return state_ == ZProcessState::Paused; }
  
  void SetState(ZProcessState state) { state_ = state; }
  void AttachChild(std::shared_ptr<ZProcess> child);
  std::shared_ptr<ZProcess> RemoveChild();  

protected:

  std::string id_;
  ZProcessState state_;
  
};
