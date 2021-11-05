/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZScriptManager.cpp

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

#include "ZServices.hpp"
#include "ZScriptManager.hpp"
#include "ZResourceData.hpp"
#include "ZResourceLoadedEvent.hpp"

ZIDSequence ZScript::idGenerator_;

ZScript::ZScript()
{
	name = "Script_" + idGenerator_.Next();
}

std::string ZScriptManager::Name(const ZHScript& handle)
{
	assert(!handle.IsNull() && "Cannot get property with a null script handle!");
	ZScript* script = resourcePool_.Get(handle);
	return script->name;
}

std::string ZScriptManager::Code(const ZHScript& handle)
{
	assert(!handle.IsNull() && "Cannot get property with a null script handle!");
	ZScript* script = resourcePool_.Get(handle);
	return script->code;
}

void ZScriptManager::Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
	if (dataNode->type != ZOFObjectType::Script)
	{
		return;
	}

	ZHScript restoreHandle(dataHandle.value);

	ZScript* script = resourcePool_.New<ZScript>(restoreHandle);

	if (dataNode->properties.find("path") != dataNode->properties.end() && dataNode->properties["path"]->HasValues())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["path"]->Value<ZOFString>(0);
		ZScriptResourceData::ptr scriptResource = std::make_shared<ZScriptResourceData>(prop->value);
		ZServices::ResourceImporter()->GetData(scriptResource.get());
		script->code = scriptResource->code;
		script->path = scriptResource->path;
	}

	Track(restoreHandle);
}

void ZScriptManager::DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
	if (dataNode->type != ZOFObjectType::Script)
	{
		return;
	}

	ZHScript restoreHandle(dataHandle.value);

    if (dataNode->properties.find("path") != dataNode->properties.end() && dataNode->properties["path"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = dataNode->properties["path"]->Value<ZOFString>(0);
		ZScriptResourceData::ptr scriptResource = std::make_shared<ZScriptResourceData>(prop->value);
		scriptResource->restoreHandle = restoreHandle;
        ZServices::ResourceImporter()->GetDataAsync(scriptResource);
    }

	Track(restoreHandle);
}

void ZScriptManager::HandleScriptLoaded(const std::shared_ptr<class ZResourceLoadedEvent>& event)
{
	if (event->Resource() == nullptr)
	{
		return;
	}

	if (event->Resource()->type != ZResourceType::Script)
	{
		return;
	}

	ZScriptResourceData::ptr scriptData = std::static_pointer_cast<ZScriptResourceData>(event->Resource());

	ZScript* script = resourcePool_.New<ZScript>(scriptData->restoreHandle);

	script->code = scriptData->code;
	script->path = scriptData->path;

	Track(scriptData->restoreHandle);
}

