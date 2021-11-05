/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZAssets.hpp

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

#pragma once

#include "ZCommon.hpp"
#include "ZScriptManager.hpp"
#include "ZTexture.hpp"
#include "ZShader.hpp"
#include "ZFont.hpp"
#include "ZMaterial.hpp"
#include "ZModel.hpp"
#include "ZUIElement.hpp"
#include "ZComponent.hpp"
#include "ZGameObject.hpp"

class ZAssets
{

public:

    static void Initialize();

    static std::shared_ptr<ZScriptManager> ScriptManager() { return scriptManager_; }
    static std::shared_ptr<ZTextureManager> TextureManager() { return textureManager_; }
    static std::shared_ptr<ZShaderManager> ShaderManager() { return shaderManager_; }
    static std::shared_ptr<ZFontManager> FontManager() { return fontManager_; }
    static std::shared_ptr<ZMaterialManager> MaterialManager() { return materialManager_; }
    static std::shared_ptr<ZModelManager> ModelManager() { return modelManager_; }
    static std::shared_ptr<ZUIElementManager> UIElementManager() { return uiElementManager_; }
    static std::shared_ptr<ZGameObjectManager> GameObjectManager() { return gameObjectManager_; }
    static std::shared_ptr<ZComponentManager> ComponentManager() { return componentManager_; }

    static void Provide(const std::shared_ptr<ZScriptManager>& scriptManager);
    static void Provide(const std::shared_ptr<ZTextureManager>& textureManager);
    static void Provide(const std::shared_ptr<ZShaderManager>& shaderManager);
    static void Provide(const std::shared_ptr<ZFontManager>& fontManager);
    static void Provide(const std::shared_ptr<ZMaterialManager>& materialManager);
    static void Provide(const std::shared_ptr<ZModelManager>& modelManager);
    static void Provide(const std::shared_ptr<ZUIElementManager>& uiElementManager);
    static void Provide(const std::shared_ptr<ZGameObjectManager>& gameObjectManager);
    static void Provide(const std::shared_ptr<ZComponentManager>& componentManager);

private:

    static std::shared_ptr<ZScriptManager> scriptManager_;
    static std::shared_ptr<ZTextureManager> textureManager_;
    static std::shared_ptr<ZShaderManager> shaderManager_;
    static std::shared_ptr<ZFontManager> fontManager_;
    static std::shared_ptr<ZMaterialManager> materialManager_;
    static std::shared_ptr<ZModelManager> modelManager_;
    static std::shared_ptr<ZUIElementManager> uiElementManager_;
    static std::shared_ptr<ZGameObjectManager> gameObjectManager_;
    static std::shared_ptr<ZComponentManager> componentManager_;

    static bool initialized_;

};