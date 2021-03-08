/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUICanvas.hpp

    Created by Adrian Sanchez on 05/02/2021.
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
#include "ZUIElement.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZUICanvas : public ZUIElement
{

public:

    ZUICanvas(const glm::vec2& position = glm::vec2(0.1f), const glm::vec2& scale = glm::vec2(0.07f, 0.03f));
    ZUICanvas(const ZUIElementOptions& options);
    ~ZUICanvas() {}

    void                                Initialize() override;
    void                                Initialize(const std::shared_ptr<ZOFNode>& root) override;

    void                                Prepare(double deltaTime, unsigned int zOrder = 0) override;

    DECLARE_UI_CREATORS(ZUICanvas)

};
