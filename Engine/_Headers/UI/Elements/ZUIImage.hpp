/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIImage.hpp

    Created by Adrian Sanchez on 06/02/2019.
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
class ZShader;
class ZTextureReadyEvent;
class ZTexture;

// Class and Data Structure Definitions
class ZUIImage : public ZUIElement
{

private:

    std::string path_;
    std::shared_ptr<ZTexture> pendingTexture_;

public:

    ZUIImage(const std::string& path = "", const glm::vec2& position = glm::vec2(0.5f), const glm::vec2& scale = glm::vec2(0.25f));
    ZUIImage(const ZUIElementOptions& options, const std::string& path = "");
    ~ZUIImage() {}

    void Initialize() override;
    void Initialize(const std::shared_ptr<ZOFNode>& root) override;

    void SetImage(const std::string& path);
    void SetImage(const std::shared_ptr<ZTexture>& texture);

    DECLARE_UI_CREATORS(ZUIImage)

protected:

    void HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event);

};
