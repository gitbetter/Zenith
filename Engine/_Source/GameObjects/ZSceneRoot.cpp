/*
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
   ZSceneRoot.cpp
 
   Created by Adrian Sanchez on 19/04/19.
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

#include "ZSceneRoot.hpp"

ZSceneRoot::ZSceneRoot() {
	properties_.name = "Root";
	
	std::shared_ptr<ZGameObject> staticGroup = std::make_shared<ZGameObject>("StaticGroup");
	staticGroup->SetRenderPass(ZRenderPass::Static);
	children_.push_back(staticGroup);

	std::shared_ptr<ZGameObject> dynamicGroup = std::make_shared<ZGameObject>("DynamicGroup");
	dynamicGroup->SetRenderPass(ZRenderPass::Dynamic);
	children_.push_back(dynamicGroup);

	std::shared_ptr<ZGameObject> skyGroup = std::make_shared<ZGameObject>("SkyGroup");
	skyGroup->SetRenderPass(ZRenderPass::Sky);
	children_.push_back(skyGroup);

	std::shared_ptr<ZGameObject> invisibleGroup = std::make_shared<ZGameObject>("InvisibleGroup");
	invisibleGroup->SetRenderPass(ZRenderPass::Invisible);
	children_.push_back(invisibleGroup);
    
    std::shared_ptr<ZGameObject> uiGroup = std::make_shared<ZGameObject>("UIGroup");
    skyGroup->SetRenderPass(ZRenderPass::Sky);
    children_.push_back(uiGroup);
}

void ZSceneRoot::AddChild(std::shared_ptr<ZGameObject> gameObject) {
	ZRenderPass pass = gameObject->RenderPass();
	if (pass >= children_.size() || !children_[pass]) {
		_Z("The child being added has a non-extant render pass", ZERROR);
		return;
	}
	children_[pass]->AddChild(gameObject);
}

void ZSceneRoot::RenderChildren(ZRenderOp renderOp) {
	for (int pass = ZRenderPass::First; pass < ZRenderPass::Last; pass++) {
		switch (pass) {
		case ZRenderPass::Static:
		case ZRenderPass::Dynamic:
		case ZRenderPass::Sky:
        case ZRenderPass::UI:
			children_[pass]->RenderChildren(renderOp);
			break;
        default: break;
		}
	}
}
