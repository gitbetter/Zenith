/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourcePool.hpp

    Created by Adrian Sanchez on 08/10/2021.
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

// TODO: Look into extending this to use a more cache friendly object pool as opposed to the unique_ptr paradigm, which will allocate
// from the heap arbitrarily and possibly cause a bunch of cache misses

#pragma once

#include "ZCommon.hpp"

template <typename Data, typename Handle>
class ZResourcePool
{
public:

    explicit ZResourcePool(unsigned int initialAllocationSize)
    {
        userData_.reserve(initialAllocationSize);
        magicNumbers_.reserve(initialAllocationSize);
        freeSlots_.reserve(initialAllocationSize);
    }
    ~ZResourcePool() { }

    template <typename Type, typename ...Args>
    Type* New(Handle& handle, Args && ...args)
	{
        static_assert(std::is_base_of<Data, Type>::value, "Type is not derived from Data");

        if (!handle.IsNull())
        {
            return Restore<Type>(handle, std::forward<Args>(args)...);
        }
        else
        {
			unsigned int index;
			if (freeSlots_.empty())
			{
				index = magicNumbers_.size();
				handle.Initialize(index);
				userData_.push_back(std::make_unique<Type>(std::forward<Args>(args)...));
				magicNumbers_.push_back(handle.Magic());
			}
			else
			{
				index = freeSlots_.back();
				freeSlots_.pop_back();
				handle.Initialize(index);
				magicNumbers_[index] = handle.Magic();
			}
			return dynamic_cast<Type*>((*(userData_.begin() + index)).get());
        }
	}

    void Delete(const Handle& handle)
    {
        unsigned int index = handle.Index();

        assert(index < userData_.size());
        assert(magicNumbers_[index] == handle.Magic());

        magicNumbers_[index] = 0;
        freeSlots_.push_back(index);
    }

    Data* Get(const Handle& handle)
    {
        if (handle.IsNull())
        {
            return nullptr;
        }

        unsigned int index = handle.Index();
        if (index >= userData_.size())
        {
            assert(false);
            return nullptr;
        }

        return (*(userData_.begin() + index)).get();
    }

    const Data* Get(const Handle& handle) const
    {
        using ThisType = ZResourcePool<Data, Handle>;
        return const_cast<ThisType*>(this)->Get(handle);
    }

    unsigned int GetUsedHandleCount() const
    {
       magicNumbers_.size() - freeSlots_.size();
    }

    bool HasUsedHandles() const
    {
        return GetUsedHandleCount() > 0;
    }

protected:

	template <typename Type, typename ...Args>
	Type* Restore(Handle& handle, Args && ...args)
	{
		static_assert(std::is_base_of<Data, Type>::value, "Type is not derived from Data");

		handle.Restore();
		userData_[handle.Index()] = std::make_unique<Type>(std::forward<Args>(args)...);
		magicNumbers_[handle.Index()] = handle.Magic();

		return dynamic_cast<Type*>((*(userData_.begin() + handle.Index())).get());
	}

private:

    using DataVector = std::vector<std::unique_ptr<Data>>;
    using MagicVector = std::vector<unsigned int>;
    using FreeVector = std::vector<unsigned int>;

    DataVector userData_;
    MagicVector magicNumbers_;
    FreeVector freeSlots_;

};
