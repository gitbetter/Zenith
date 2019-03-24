/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZScriptProcess.cpp

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

#include "ZScriptProcess.hpp"
#include "ZEngine.hpp"
#include "ZLuaScriptManager.hpp"

const std::string ZScriptProcess::SCRIPT_PROCESS_NAME = "ZScriptProcess";

void ZScriptProcess::RegisterScriptClass() {
  sol::table metaTable = ZEngine::ScriptManager()->LuaState().create_named_table(SCRIPT_PROCESS_NAME);
  metaTable[sol::metatable_key] = metaTable;
  metaTable["base"] = metaTable;
  metaTable["cpp"] = true;
  RegisterScriptClassFunctions();
  metaTable["new"] = ZScriptProcess::CreateFromScript;
}

void ZScriptProcess::RegisterScriptClassFunctions() {
  sol::table metaTable = ZEngine::ScriptManager()->LuaState()[SCRIPT_PROCESS_NAME];
  metaTable["initialize"] = ZProcess::Initialize;
  metaTable["finish"] = ZProcess::Finish;
  metaTable["fail"] = ZProcess::Fail;
  metaTable["pause"] = ZProcess::Pause;
  metaTable["continue"] = ZProcess::Continue;
  metaTable["abort"] = ZProcess::Abort;
  metaTable["isAlive"] = ZScriptProcess::IsAlive;
  metaTable["isDead"] = ZScriptProcess::IsDead;
  metaTable["isPaused"] = ZScriptProcess::IsPaused;
  metaTable["attachChild"] = ZScriptProcess::ScriptAttachChild;
}

void ZScriptProcess::ScriptAttachChild(sol::table child) {
  std::shared_ptr<ZProcess> process = child["process"];
  if (process) AttachChild(process);
  else _Z("Child process being attached is not valid", ZERROR);
}

sol::table ZScriptProcess::CreateFromScript(sol::table self, sol::table constructionData, sol::table originalSubclass) {
  std::shared_ptr<ZScriptProcess> process(new ZScriptProcess);
  process->self_ = ZEngine::ScriptManager()->LuaState().create_table();
  if (process->BuildCppDataFromScript(originalSubclass, constructionData)) {
    sol::table metaTable = ZEngine::ScriptManager()->LuaState()[SCRIPT_PROCESS_NAME];
    assert(metaTable.valid());
    process->self_["process"] = process;
    process->self_[sol::metatable_key] = metaTable;
  } else {
    process->self_ = sol::lua_nil;
  }
  return process->self_;
}

bool ZScriptProcess::BuildCppDataFromScript(sol::table scriptClass, sol::table constructionData) {
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
    for (sol::basic_table_iterator it = constructionData.begin(); it != constructionData.end(); it++) {
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

void ZScriptProcess::Initialize() {
  ZProcess::Initialize();
  if (scriptInitialize_.valid())
    scriptInitialize_(self_);
}

void ZScriptProcess::Update() {
  ZProcess::Update();
  time_ += ZEngine::UPDATE_STEP_SIZE;
  if (time_ > frequency_) {
    scriptUpdate_(self_);
    time_ = 0;
  }
}

void ZScriptProcess::Render(float frameMix = 1.f, RENDER_OP renderOp = RENDER_OP_COLOR) {
  ZProcess::Render();
}

void ZScriptProcess::Pause() {
  ZProcess::Pause();
  if (scriptPause_.valid())
    scriptPause_(self_);
}

void ZScriptProcess::Continue() {
  ZProcess::Continue();
  if (scriptContinue_.valid())
    scriptContinue_(self_);
}

void ZScriptProcess::Finish() {
  ZProcess::Finish();
  if (scriptFinish_.valid())
    scriptFinish_(self_);
}

void ZScriptProcess::Fail() {
  ZProcess::Fail();
  if (scriptFail_.valid())
    scriptFail_(self_);
}

void ZScriptProcess::Abort() {
  ZProcess::Abort();
  if (scriptAbort_.valid())
    scriptAbort_(self_);
}