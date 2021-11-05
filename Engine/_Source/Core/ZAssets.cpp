/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZAssets.cpp

	Created by Adrian Sanchez on 11/04/2021.
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

#include "ZAssets.hpp"
#include "ZServices.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZGLTexture.hpp"
#include "ZGLFont.hpp"

std::shared_ptr<ZScriptManager> ZAssets::scriptManager_ = nullptr;
std::shared_ptr<ZTextureManager> ZAssets::textureManager_ = nullptr;
std::shared_ptr<ZShaderManager> ZAssets::shaderManager_ = nullptr;
std::shared_ptr<ZFontManager> ZAssets::fontManager_ = nullptr;
std::shared_ptr<ZMaterialManager> ZAssets::materialManager_ = nullptr;
std::shared_ptr<ZModelManager> ZAssets::modelManager_ = nullptr;
std::shared_ptr<ZUIElementManager> ZAssets::uiElementManager_ = nullptr;
std::shared_ptr<ZGameObjectManager> ZAssets::gameObjectManager_ = nullptr;
std::shared_ptr<ZComponentManager> ZAssets::componentManager_ = nullptr;

bool ZAssets::initialized_ = false;

void ZAssets::Initialize()
{
	if (!initialized_) {
		/* ========= Script Manager ============ */
		Provide(std::make_shared<ZLuaScriptManager>());
		/* ======================================= */

		/* ========= Texture Manager ============ */
		Provide(std::make_shared<ZGLTextureManager>());
		/* =================================== */

		/* ========= Shader Manager ============ */
		Provide(std::make_shared<ZShaderManager>());
		/* =================================== */

		/* ========= Font Manager ============ */
		Provide(std::make_shared<ZGLFontManager>());
		/* =================================== */

		/* ========= Material Manager ============ */
		Provide(std::make_shared<ZMaterialManager>());
		/* =================================== */

		/* ========= Model Manager ============ */
		Provide(std::make_shared<ZModelManager>());
		/* =================================== */

		/* ========= UI Element Manager ============ */
		Provide(std::make_shared<ZUIElementManager>());
		/* =================================== */

		/* ========= Game Object Manager ============ */
		Provide(std::make_shared<ZGameObjectManager>());
		/* =================================== */

		/* ========= Component Manager ============ */
		Provide(std::make_shared<ZComponentManager>());
		/* =================================== */
	}
	initialized_ = true;
}

void ZAssets::Provide(const std::shared_ptr<ZScriptManager>& scriptManager)
{
	if (scriptManager_)
	{
		scriptManager_->CleanUp();
	}

	scriptManager_ = scriptManager;
	scriptManager_->Initialize();

	// We don't need to do anything with this resource. The resource loader
	// will load and execute the script for us.
	ZScriptResourceData::ptr luaSetupScriptResource = std::make_shared<ZScriptResourceData>("/Scripts/init.lua", ZResourceType::Script);
	ZServices::ResourceImporter()->GetData(luaSetupScriptResource.get());
}

void ZAssets::Provide(const std::shared_ptr<ZTextureManager>& textureManager)
{
	if (textureManager_)
	{
		textureManager_->CleanUp();
	}

	textureManager_ = textureManager;
	textureManager_->Initialize();
}

void ZAssets::Provide(const std::shared_ptr<ZShaderManager>& shaderManager)
{
	if (shaderManager_)
	{
		shaderManager_->CleanUp();
	}

	shaderManager_ = shaderManager;
	shaderManager_->Initialize();
}

void ZAssets::Provide(const std::shared_ptr<ZFontManager>& fontManager)
{
	if (fontManager_)
	{
		fontManager_->CleanUp();
	}

	fontManager_ = fontManager;
	fontManager_->Initialize();
}

void ZAssets::Provide(const std::shared_ptr<ZMaterialManager>& materialManager)
{
	if (materialManager_)
	{
		materialManager_->CleanUp();
	}

	materialManager_ = materialManager;
	materialManager_->Initialize();
}

void ZAssets::Provide(const std::shared_ptr<ZModelManager>& modelManager)
{
	if (modelManager_)
	{
		modelManager_->CleanUp();
	}

	modelManager_ = modelManager;
	modelManager_->Initialize();
}

void ZAssets::Provide(const std::shared_ptr<ZUIElementManager>& uiElementManager)
{
	if (uiElementManager_)
	{
		uiElementManager_->CleanUp();
	}

	uiElementManager_ = uiElementManager;
	uiElementManager_->Initialize();
}

void ZAssets::Provide(const std::shared_ptr<ZGameObjectManager>& gameObjectManager)
{
	if (gameObjectManager_)
	{
		gameObjectManager_->CleanUp();
	}

	gameObjectManager_ = gameObjectManager;
	gameObjectManager_->Initialize();
}

void ZAssets::Provide(const std::shared_ptr<ZComponentManager>& componentManager)
{
	if (componentManager_)
	{
        componentManager_->CleanUp();
	}

    componentManager_ = componentManager;
    componentManager_->Initialize();
}