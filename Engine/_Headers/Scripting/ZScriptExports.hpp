/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZScriptExports.hpp

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
#include "sol.hpp"

// Forward Declarations
class ZScriptableEvent;
class ZScriptableProcess;

// Class and Data Structure Definitions
class ZInternalScriptExports
{

public:

    static bool Initialize() { return true; }
    static bool Destroy() { return true; }

    static bool LoadAndExecuteScriptResource(const std::string& scriptResource);

    static void AttachScriptProcess(const std::shared_ptr<ZScriptableProcess>& scriptProcess, const std::shared_ptr<ZProcessRunner>& processRunner);

    static bool Queue(ZTypeIdentifier eventType, const sol::table& eventData);
    static bool Trigger(ZTypeIdentifier eventType, const sol::table& eventData);
    static std::shared_ptr<ZScriptableEvent> BuildEvent(ZTypeIdentifier eventType, const sol::table& eventData);
    static unsigned long RegisterEventListener(ZTypeIdentifier eventType, const sol::function& callback);

    static void Log(const sol::object& string);

};

namespace ZScriptExports
{
    void Register();
    void UnRegister();
}
