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
#include "ZLuaScriptManager.hpp"

const std::string ZScriptableProcess::SCRIPT_PROCESS_NAME = "ZScriptableProcess";

ZScriptableProcess::ZScriptableProcess() : ZProcess() { frequency_ = 0; time_ = 0; }

ZScriptableProcess::~ZScriptableProcess() { dynamicFields_.clear(); }

void ZScriptableProcess::RegisterScriptClass()
{
    ZLuaScriptManager* scriptManager = static_cast<ZLuaScriptManager*>(zenith::ScriptManager());
    sol::state& lua = scriptManager->LuaState();
    lua.new_usertype<ZScriptableProcess>(SCRIPT_PROCESS_NAME,
        "Create", sol::factories(ZScriptableProcess::CreateFromScript),
        "Initialize", &ZProcess::Initialize,
        "Update", &ZProcess::Update,
        "Finish", &ZProcess::Finish,
        "Fail", &ZProcess::Fail,
        "Pause", &ZProcess::Pause,
        "Resume", &ZProcess::Resume,
        "Abort", &ZProcess::Abort,
        "IsAlive", &ZScriptableProcess::ScriptIsAlive,
        "IsDead", &ZScriptableProcess::ScriptIsDead,
        "IsPaused", &ZScriptableProcess::ScriptIsPaused,
        "AttachChild", &ZScriptableProcess::ScriptAttachChild,
        sol::meta_function::index, &ZScriptableProcess::GetDynamic,
        sol::meta_function::new_index, &ZScriptableProcess::SetDynamic,
        sol::base_classes, sol::bases<ZProcess>()
        );
}

std::shared_ptr<ZScriptableProcess> ZScriptableProcess::CreateFromScript(const sol::table& self, const sol::table& constructionData)
{
    std::shared_ptr<ZScriptableProcess> process(new ZScriptableProcess);
    if (!process->BuildCppDataFromScript(self, constructionData))
    {
        return nullptr;
    }
    return process;
}

bool ZScriptableProcess::BuildCppDataFromScript(const sol::table& obj, const sol::table& constructionData)
{
    if (obj.valid())
    {
        sol::function temp = obj["OnInitialize"];
        if (temp.valid())
            scriptInitialize_ = temp;

        temp = obj["OnUpdate"];
        if (temp.valid())
        {
            scriptUpdate_ = temp;
        }
        else
        {
            zenith::Log("No update() function found on script process", ZSeverity::Error);
            return false;
        }

        temp = obj["OnPause"];
        if (temp.valid())
            scriptPause_ = temp;

        temp = obj["OnContinue"];
        if (temp.valid())
            scriptResume_ = temp;

        temp = obj["OnFinish"];
        if (temp.valid())
            scriptFinish_ = temp;

        temp = obj["OnFail"];
        if (temp.valid())
            scriptFail_ = temp;

        temp = obj["OnAbort"];
        if (temp.valid())
            scriptAbort_ = temp;
    }
    else
    {
        zenith::Log("Script class is not a valid Lua table", ZSeverity::Error);
        return false;
    }

    if (constructionData.valid())
    {
        constructionData.for_each([this] (const sol::object& key, const sol::object& value) {
            std::string k = key.as<std::string>();
            if (k == "frequency" && value.is<float>())
            {
                frequency_ = value.as<float>();
            }
            else
            {
                SetDynamic(k, value);
            }
        });
    }

    return true;
}

void ZScriptableProcess::ScriptAttachChild(std::shared_ptr<ZScriptableProcess> process)
{
//std::shared_ptr<ZScriptableProcess> process = child.as<std::shared_ptr<ZScriptableProcess>>();
    if (process) AttachChild(process);
    else zenith::Log("Child process being attached is not valid", ZSeverity::Error);
}

sol::object ZScriptableProcess::GetDynamic(const std::string& key)
{
    auto it = dynamicFields_.find(key);
    if (it == dynamicFields_.cend())
        return sol::lua_nil;
    return it->second;
}

void ZScriptableProcess::SetDynamic(const std::string& key, const sol::object& value)
{
    auto it = dynamicFields_.find(key);
    if (it == dynamicFields_.cend())
    {
        dynamicFields_.insert(it, { std::move(key), std::move(value) });
    }
    else
    {
        std::pair<const std::string, sol::object>& pair = *it;
        sol::object& entry = pair.second;
        entry = sol::object(std::move(value));
    }
}

void ZScriptableProcess::OnInitialize()
{
    ZProcess::OnInitialize();
    if (scriptInitialize_.valid())
        scriptInitialize_(*this);
}

void ZScriptableProcess::OnUpdate(double deltaTime)
{
    ZProcess::OnUpdate(deltaTime);

    time_ += (float) deltaTime;
    if (time_ > frequency_ / 1000.f)
    {
        scriptUpdate_(*this);
        time_ = 0;
    }
}

void ZScriptableProcess::OnPause()
{
    ZProcess::OnPause();
    if (scriptPause_.valid())
        scriptPause_(*this);
}

void ZScriptableProcess::OnResume()
{
    ZProcess::OnResume();
    if (scriptResume_.valid())
        scriptResume_(*this);
}

void ZScriptableProcess::OnFinish()
{
    ZProcess::OnFinish();
    if (scriptFinish_.valid())
        scriptFinish_(*this);
}

void ZScriptableProcess::OnFail()
{
    ZProcess::OnFail();
    if (scriptFail_.valid())
        scriptFail_(*this);
}

void ZScriptableProcess::OnAbort()
{
    ZProcess::OnAbort();
    if (scriptAbort_.valid())
        scriptAbort_(*this);
}
