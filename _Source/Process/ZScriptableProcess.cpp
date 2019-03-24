/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZScriptableProcess.cpp

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

#include "ZScriptableProcess.hpp"
#include "ZEngine.hpp"
#include "ZLuaScriptManager.hpp"

const std::string ZScriptableProcess::SCRIPT_PROCESS_NAME = "ZScriptableProcess";

ZScriptableProcess::ZScriptableProcess() : frequency_(0), time_(0) { }

void ZScriptableProcess::RegisterScriptClass() {
  sol::table metaTable = ZEngine::ScriptManager()->LuaState().create_named_table(SCRIPT_PROCESS_NAME);
  metaTable[sol::metatable_key] = metaTable;
  metaTable["base"] = metaTable;
  metaTable["cpp"] = true;
  RegisterScriptClassFunctions();
  metaTable["new"] = ZScriptableProcess::CreateFromScript;
}

void ZScriptableProcess::RegisterScriptClassFunctions() {
  sol::table metaTable = ZEngine::ScriptManager()->LuaState()[SCRIPT_PROCESS_NAME];
  metaTable["initialize"] = &ZProcess::Initialize;
  metaTable["finish"] = &ZProcess::Finish;
  metaTable["fail"] = &ZProcess::Fail;
  metaTable["pause"] = &ZProcess::Pause;
  metaTable["continue"] = &ZProcess::Continue;
  metaTable["abort"] = &ZProcess::Abort;
  metaTable["isAlive"] = &ZScriptableProcess::IsAlive;
  metaTable["isDead"] = &ZScriptableProcess::IsDead;
  metaTable["isPaused"] = &ZScriptableProcess::IsPaused;
  metaTable["attachChild"] = &ZScriptableProcess::ScriptAttachChild;
}

void ZScriptableProcess::ScriptAttachChild(sol::table child) {
  std::shared_ptr<ZProcess> process = child["_object"];
  if (process) AttachChild(process);
  else _Z("Child process being attached is not valid", ZERROR);
}

sol::table ZScriptableProcess::CreateFromScript(sol::table self, sol::table constructionData, sol::table originalSubclass) {
  std::shared_ptr<ZScriptableProcess> process(new ZScriptableProcess);
  process->self_ = ZEngine::ScriptManager()->LuaState().create_table();
  if (process->BuildCppDataFromScript(originalSubclass, constructionData)) {
    sol::table metaTable = ZEngine::ScriptManager()->LuaState()[SCRIPT_PROCESS_NAME];
    assert(metaTable.valid());
    process->self_["_object"] = process;
    process->self_[sol::metatable_key] = metaTable;
  } else {
    process->self_ = sol::lua_nil;
  }
  return process->self_;
}

bool ZScriptableProcess::BuildCppDataFromScript(sol::table scriptClass, sol::table constructionData) {
  if (scriptClass.valid()) {
    sol::table temp = scriptClass["initialize"];
    if (temp.valid())
      scriptInitialize_ = temp;

    temp = scriptClass["update"];
    if (temp.valid()) {
      scriptUpdate_ = temp;
    } else {
      _Z("No update() function found on script process", ZERROR);
      return false;
    }

    temp = scriptClass["pause"];
    if (temp.valid())
      scriptPause_ = temp;

    temp = scriptClass["continue"];
    if (temp.valid())
      scriptContinue_ = temp;

    temp = scriptClass["finish"];
    if (temp.valid())
      scriptFinish_ = temp;

    temp = scriptClass["fail"];
    if (temp.valid())
      scriptFail_ = temp;

    temp = scriptClass["abort"];
    if (temp.valid())
      scriptAbort_ = temp;
  } else {
    _Z("Script class is not a valid Lua table", ZERROR);
    return false;
  }

  if (constructionData.valid()) {
    for (auto it = constructionData.begin(); it != constructionData.end(); it++) {
      std::string key = (*it).first.as<std::string>();
      sol::object val = (*it).second;
      if (key == "frequency" && val.is<int>()) {
        frequency_ = val.as<int>();
      } else {
        self_[key] = val;
      }
    }
  }

  return true;
}

void ZScriptableProcess::Initialize() {
  ZProcess::Initialize();
  if (scriptInitialize_.valid())
    scriptInitialize_(self_);
}

void ZScriptableProcess::Update() {
  ZProcess::Update();
  time_ += ZEngine::UPDATE_STEP_SIZE;
  if (time_ > frequency_) {
    scriptUpdate_(self_);
    time_ = 0;
  }
}

void ZScriptableProcess::Render(float frameMix, RENDER_OP renderOp) {
  ZProcess::Render();
}

void ZScriptableProcess::Pause() {
  ZProcess::Pause();
  if (scriptPause_.valid())
    scriptPause_(self_);
}

void ZScriptableProcess::Continue() {
  ZProcess::Continue();
  if (scriptContinue_.valid())
    scriptContinue_(self_);
}

void ZScriptableProcess::Finish() {
  ZProcess::Finish();
  if (scriptFinish_.valid())
    scriptFinish_(self_);
}

void ZScriptableProcess::Fail() {
  ZProcess::Fail();
  if (scriptFail_.valid())
    scriptFail_(self_);
}

void ZScriptableProcess::Abort() {
  ZProcess::Abort();
  if (scriptAbort_.valid())
    scriptAbort_(self_);
}