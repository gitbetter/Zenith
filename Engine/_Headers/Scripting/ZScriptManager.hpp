/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZScriptManager.hpp

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

#include "ZCommon.hpp"
#include "ZResourceManager.hpp"
#include "ZOFTree.hpp"
#include "ZHandle.hpp"

struct ZScript
{
	std::string path;
    std::string name;
    std::string code;

    ZScript();

private:

	static ZIDSequence idGenerator_;
};

class ZScriptManager : public ZResourceManager<ZScript, ZHScript>
{
public:

	~ZScriptManager() = default;

    virtual void Initialize() override { }
    virtual void CleanUp() override { }

    virtual void Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
    virtual void DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);

	std::string Name(const ZHScript& handle);
	std::string Code(const ZHScript& handle);

public:

    virtual void Initialize() = 0;
    virtual void ExecuteFile(const std::string& resource) = 0;
    virtual void ExecuteString(const std::string& script) = 0;

private:

	void HandleScriptLoaded(const std::shared_ptr<class ZResourceLoadedEvent>& event);

};
