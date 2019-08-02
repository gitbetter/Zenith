/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZUIImage.cpp

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

#include "ZUI.hpp"
#include "ZUIImage.hpp"
#include "ZShader.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZEventAgent.hpp"

ZUIImage::ZUIImage(std::string path, glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
	SetImage(path);
	enabled_ = false;
}

void ZUIImage::Initialize(std::shared_ptr<ZOFNode> root) {
	ZUIElement::Initialize(root);
}

void ZUIImage::Render(ZRenderOp renderOp) {
	zenith::Graphics()->Strategy()->EnableAlphaBlending();
	ZUIElement::Render();
	zenith::Graphics()->Strategy()->DisableAlphaBlending();
	RenderChildren();
}

void ZUIImage::SetImage(std::string path) {
	if (!path.empty()) {
		path_ = path;
        
        ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZUIImage::HandleTextureReady);
        zenith::EventAgent()->AddListener(textureReadyDelegate, ZTextureReadyEvent::Type);
        
		zenith::Graphics()->Strategy()->LoadTextureAsync(path, "");
	}
}

void ZUIImage::HandleTextureReady(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZTextureReadyEvent> textureReadyEvent = std::static_pointer_cast<ZTextureReadyEvent>(event);
	if (textureReadyEvent->Texture().path == path_) {
		texture_ = textureReadyEvent->Texture();
		texture_.type = "image";
        
        ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZUIImage::HandleTextureReady);
        zenith::EventAgent()->RemoveListener(textureReadyDelegate, ZTextureReadyEvent::Type);
	}
}
