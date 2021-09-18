/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZHandle.hpp

    Created by Adrian Sanchez on 08/09/2021.
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

#include <cassert>

template <typename Tag>
class ZHandle
{
    union
    {
        enum
        {
            MAX_INDEX_BITS = 16,
            MAX_MAGIC_BITS = 16,
            MAX_INDEX = ( 1 << MAX_INDEX_BITS) - 1,
            MAX_MAGIC = ( 1 << MAX_MAGIC_BITS) - 1
        };

        struct
        {
            unsigned index_ : MAX_INDEX_BITS;
            unsigned magic_ : MAX_MAGIC_BITS;
        };

        unsigned int handle_;
    };

public:

    ZHandle(unsigned int handle = 0)
        : handle_(handle)
    { }

    void Initialize(unsigned int index);
    void Restore();

    unsigned int Index() const { return index_; }
    unsigned int Magic() const { return magic_; }
    unsigned int Handle() const { return handle_; }
    unsigned int IsNull() const { return handle_ == 0; }

    operator unsigned int () const { return handle_; }

private:

    static inline unsigned int autoMagic = 0;

};

template <typename Tag>
void ZHandle<Tag>::Initialize(unsigned int index)
{
    assert(IsNull());
    assert(index <= MAX_INDEX);

    if (++autoMagic > MAX_MAGIC)
    {
        autoMagic = 1;
    }

    index_ = index;
    magic_ = autoMagic;
}

template <typename Tag>
void ZHandle<Tag>::Restore()
{
	assert(!IsNull());
	assert(index_ <= MAX_INDEX);

	if (magic_ > autoMagic)
	{
		autoMagic = magic_;
	}
}

template <typename Tag>
inline bool operator != (const ZHandle<Tag>& l, const ZHandle<Tag>& r)
{
    return l.Handle() != r.Handle();
}

template <typename Tag>
inline bool operator == (const ZHandle<Tag>& l, const ZHandle<Tag>& r)
{
    return l.Handle() == r.Handle();
}
