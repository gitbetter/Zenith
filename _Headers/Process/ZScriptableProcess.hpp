/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZScriptableProcess.hpp

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
class ZScriptableProcess : public ZProcess {

private:

  float frequency_, time_;
  sol::function scriptInitialize_, scriptUpdate_, scriptPause_, scriptResume_,
                scriptFinish_, scriptFail_, scriptAbort_;
  std::unordered_map<std::string, sol::object> dynamicFields_;
  static const std::string SCRIPT_PROCESS_NAME;

  static std::shared_ptr<ZScriptableProcess> CreateFromScript(sol::table self, sol::table constructionData);
  bool BuildCppDataFromScript(sol::table scriptClass, sol::table constructionData);

  bool ScriptIsAlive() { return IsAlive(); }
  bool ScriptIsDead() { return IsDead(); }
  bool ScriptIsPaused() { return IsPaused(); }

  void ScriptAttachChild(std::shared_ptr<ZScriptableProcess> process);

  explicit ZScriptableProcess();

  sol::object GetDynamic(std::string key);
  void SetDynamic(std::string key, sol::object value);

public:

  virtual ~ZScriptableProcess();

  static void RegisterScriptClass();

protected:

  virtual void OnInitialize() override;
  virtual void OnUpdate() override;
  virtual void OnPause() override;
  virtual void OnResume() override;
  virtual void OnFinish() override;
  virtual void OnFail() override;
  virtual void OnAbort() override;
  virtual void CleanUp() override { }

};
