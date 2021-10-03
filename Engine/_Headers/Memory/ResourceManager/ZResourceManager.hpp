/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourceManager.hpp

    Created by Adrian Sanchez on 09/15/2021.
    Copyright © 2021 Pervasive Sense. All rights reserved.

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

/*
    Adapted from "A Generic Handle-Based Resource Manager" by Scott Bilas, Game Programming Gems
*/

#pragma once

#include "ZCommon.hpp"
#include "ZResourcePool.hpp"

template <typename Data, typename Handle>
class ZResourceManager
{
    using ResourcePool = ZResourcePool<Data, Handle>;

public:

    ZResourceManager() : resourcePool_(512)
	{ }

    virtual ~ZResourceManager() = default;

	virtual void Initialize() = 0;
	virtual void CleanUp() = 0;

	inline bool IsLoaded(const std::string& name)
	{
		return loadedResources_.find(name) != loadedResources_.end();
	}

	inline Handle GetFromName(const std::string& name)
	{
		if (loadedResources_.find(name) != loadedResources_.end()) {
			return loadedResources_[name];
		}
		return Handle();
	}

	template <typename Type = Data>
	inline Type* Dereference(const Handle& handle)
	{
		return static_cast<Type*>(resourcePool_.Get(handle));
	}

	template <typename Type = Data>
	inline const Type* Dereference(const Handle& handle) const
	{
		return static_cast<Type*>(resourcePool_.Get(handle));
	}

protected:

	/** Adds a resource to the internal loaded resource map so that we don't accidentally recreate duplicates of the resource */
	void Track(const Handle& handle)
	{
		Data* resource = resourcePool_.Get(handle);
		assert(resource != nullptr && "Cannot track this resource since it doesn't exist!");
		loadedResources_[resource->name] = handle;
	}

protected:

	ResourcePool resourcePool_;
	std::unordered_map<std::string, Handle> loadedResources_;

};