/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLTextStrategyStrategy.hpp

    Created by Adrian Sanchez on 11/02/2019.
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
#include "ZTextStrategy.hpp"

// Forward Declarations
class ZEvent;

// Class and Data Structure Definitions
class ZGLTextStrategy : public ZTextStrategy
{

private:

    std::map<std::string, unsigned int> pendingFonts_;

    void HandleFontLoaded(const std::shared_ptr<ZEvent>& event);

public:

    ZGLTextStrategy() {}
    ~ZGLTextStrategy() {}

    void Initialize() override;

    void LoadFont(const std::string& fontPath, unsigned int fontSize) override;
    void LoadFont(std::shared_ptr<ZResourceHandle> handle, unsigned int fontSize) override;
    void LoadFontAsync(const std::string& fontPath, unsigned int fontSize) override;
    void Draw(std::shared_ptr<ZUIText> textEl) override;

};
