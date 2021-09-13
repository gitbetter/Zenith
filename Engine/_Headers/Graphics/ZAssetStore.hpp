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

#include "ZCommon.hpp"
#include "ZOFTree.hpp"

class ZGameObject;
class ZWindow;
class ZLight;
class ZModel;
class ZMesh2D;
class ZModelReadyEvent;
class ZMaterialReadyEvent;

class ZAssetStore
{

public:

    ZAssetStore() = default;
    ~ZAssetStore() = default;

    void Initialize();
    void Load(std::shared_ptr<ZOFNode> root);
    void LoadAsync(std::shared_ptr<ZOFNode> root);
    void CleanUp();

    const ZHShader& PBRShader() const { return pbrShader_; }
    const ZHShader& BlinnPhongShader() const { return blinnPhongShader_; }
    const ZHShader& ShadowShader() const { return shadowShader_; }
    const ZHShader& DepthShader() const { return depthShader_; }
    const ZHShader& PostShader() const { return postShader_; }
    const ZHShader& TextShader() const { return textShader_; }
    const ZHShader& UIShader() const { return uiShader_; }
    const ZHShader& DebugShader() const { return debugShader_; }
    const ZModelMap& Models() const { return loadedModels_; }

    void AddModel(const std::string& id, std::shared_ptr<ZModel> model);

    bool HasModel(const std::string& id) const;

    std::shared_ptr<ZModel> GetModel(const std::string& id);

    void HandleModelReady(const std::shared_ptr<ZModelReadyEvent>& event);

protected:

	ZHShader pbrShader_;
	ZHShader blinnPhongShader_;
	ZHShader debugShader_;
	ZHShader shadowShader_;
	ZHShader depthShader_;
	ZHShader postShader_;
	ZHShader uiShader_;
	ZHShader textShader_;
    ZModelMap loadedModels_;

private:

    ZModelIDMap pendingModels_;

    void InitializeShaders();

};
