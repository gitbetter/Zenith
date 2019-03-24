/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZScriptProcess.hpp

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
#include "ZProcess.hpp"
#include "sol.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZScriptProcess : public ZProcess {

private:

  unsigned long frequency_, time_;
  sol::function scriptInitialize_, scriptUpdate_, scriptPause_, scriptContinue_,
                scriptFinish_, scriptFail_, scriptAbort_;
  sol::table self_;
  static const std::string SCRIPT_PROCESS_NAME;

  static sol::table CreateFromScript(sol::table self, sol::table constructionData, sol::table originalSubclass);
  static void RegisterScriptClassFunctions();
  bool BuildCppDataFromScript(sol::table scriptClass, sol::table constructionData);

  bool ScriptIsAlive() { return IsAlive(); }
  bool ScriptIsDead() { return IsDead(); }
  bool ScriptIsPaused() { return IsPaused(); }

  void ScriptAttachChild(sol::table child);

  explicit ZScriptProcess();

  static ZScriptProcess* Create(const std::string& scriptName = "");
  static void Destroy(ZScriptProcess* process);

public:

  static void RegisterScriptClass();

protected:

  virtual void Initialize() override;
  virtual void Update() override;
  virtual void Render(float frameMix = 1.f, RENDER_OP renderOp = RENDER_OP_COLOR) override;
  virtual void Pause() override;
  virtual void Continue() override;
  virtual void Finish() override;
  virtual void Fail() override;
  virtual void Abort() override;
  virtual void CleanUp() override { }

};
