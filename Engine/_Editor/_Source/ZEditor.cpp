/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditor.cpp
 
    Created by Adrian Sanchez on 18/01/21.
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

#include "ZEditor.hpp"
#include "ZEditorScene.hpp"
#include "ZResourceCache.hpp"
#include "ZUI.hpp"

#include "ZDevResourceFile.hpp"

void ZEditor::Initialize() {
	ZDomainOptions windowOptions;
	windowOptions.width = 1920;
	windowOptions.height = 1080;
	windowOptions.maximized = false;
	zenith::Initialize(shared_from_this(), windowOptions);

	zenith::ResourceCache()->RegisterResourceFile(std::shared_ptr<ZDevResourceFile>(new ZDevResourceFile(std::string(EDITOR_ROOT) + "/_Assets")));

	zenith::UI()->RegisterFont("/Fonts/earth_orbiter/earthorbiter.ttf");

	zenith::LoadScene<ZEditorScene>();
}

void ZEditor::CleanUp() {
	zenith::CleanUp();
}