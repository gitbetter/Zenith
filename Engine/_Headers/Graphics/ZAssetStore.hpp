/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZAssetStore.hpp

    Created by Adrian Sanchez on 31/01/2021.
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
#include "ZCommon.hpp"
#include "ZOFTree.hpp"

// Forward Declarations
class ZGameObject;
class ZWindow;
class ZShader;
class ZLight;
class ZModel;
class ZMesh2D;
class ZTexture;
class ZResourceHandle;
class ZModelReadyEvent;
class ZFontReadyEvent;
class ZTextureReadyEvent;
class ZShaderReadyEvent;
class ZMaterialReadyEvent;

// Class and Data Structure Definitions
class ZAssetStore
{

public:

    ZAssetStore() = default;
    ~ZAssetStore() = default;

    void Initialize();
    void Load(std::shared_ptr<ZOFTree> root);
    void LoadAsync(std::shared_ptr<ZOFTree> root);
    void CleanUp();

    const std::shared_ptr<ZShader>& PBRShader() const { return pbrShader_; }
    const std::shared_ptr<ZShader>& ShadowShader() const { return shadowShader_; }
    const std::shared_ptr<ZShader>& DepthShader() const { return depthShader_; }
    const std::shared_ptr<ZShader>& PostShader() const { return postShader_; }
    const std::shared_ptr<ZShader>& TextShader() const { return textShader_; }
    const std::shared_ptr<ZShader>& UIShader() const { return uiShader_; }
    const std::shared_ptr<ZShader>& DebugShader() const { return debugShader_; }
    const ZFontMap& Fonts() const { return loadedFonts_; }
    const ZTextureMap& Textures() const { return loadedTextures_; }
    const ZShaderMap& Shaders() const { return loadedShaders_; }
    const ZModelMap& Models() const { return loadedModels_; }
    const ZMaterialMap& Materials() const { return loadedMaterials_; }

    void RegisterFont(const std::string& fontPath, unsigned int fontSize = 64);
    void AddFont(const std::string& id, std::shared_ptr<ZFont> font);
    void AddShader(const std::string& id, std::shared_ptr<ZShader> shader);
    void AddTexture(const std::string& id, const std::shared_ptr<ZTexture>& texture);
    void AddModel(const std::string& id, std::shared_ptr<ZModel> model);
    void AddMaterial(const std::string& id, std::shared_ptr<ZMaterial> material);

    bool HasFont(const std::string& id) const;
    bool HasTexture(const std::string& id) const;
    bool HasModel(const std::string& id) const;
    bool HasShader(const std::string& id) const;
    bool HasMaterial(const std::string& id) const;

    std::shared_ptr<ZFont> GetFont(const std::string& id);
    std::shared_ptr<ZTexture> GetTexture(const std::string& id);
    std::shared_ptr<ZModel> GetModel(const std::string& id);
    std::shared_ptr<ZShader> GetShader(const std::string& id);
    std::shared_ptr<ZMaterial> GetMaterial(const std::string& id);

    void HandleShaderReady(const std::shared_ptr<ZShaderReadyEvent>& event);
    void HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event);
    void HandleModelReady(const std::shared_ptr<ZModelReadyEvent>& event);
    void HandleFontReady(const std::shared_ptr<ZFontReadyEvent>& event);
    void HandleMaterialReady(const std::shared_ptr<ZMaterialReadyEvent>& event);

protected:

    std::shared_ptr<ZShader> pbrShader_ = nullptr;
    std::shared_ptr<ZShader> debugShader_ = nullptr;
    std::shared_ptr<ZShader> shadowShader_ = nullptr;
    std::shared_ptr<ZShader> depthShader_ = nullptr;
    std::shared_ptr<ZShader> postShader_ = nullptr;
    std::shared_ptr<ZShader> uiShader_ = nullptr;
    std::shared_ptr<ZShader> textShader_ = nullptr;
    ZShaderMap loadedShaders_;
    ZTextureMap loadedTextures_;
    ZModelMap loadedModels_;
    ZFontMap loadedFonts_;
    ZMaterialMap loadedMaterials_;

private:

    ZTextureIDMap pendingTextures_;
    ZShaderIDMap pendingShaders_;
    ZModelIDMap pendingModels_;
    ZFontIDMap pendingFonts_;
    ZMaterialIDMap pendingMaterials_;

    void InitializeShaders();
    void InitializeFonts();

};
