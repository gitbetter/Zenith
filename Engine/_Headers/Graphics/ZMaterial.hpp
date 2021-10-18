/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZMaterial.hpp

 Created by Adrian Sanchez on 01/02/2019.
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
#include "ZResourceManager.hpp"
#include "ZOFTree.hpp"

class ZTextureReadyEvent;
class ZShaderReadyEvent;
class ZRenderStateGroup;
class ZUniformBuffer;

struct ZMaterialBase
{
    ZMaterialProperties properties;
	bool hasDisplacement{ false };

	ZHShader shader;
	std::string name;
	ZTextureList textures;

	std::shared_ptr<ZRenderStateGroup> renderState;
	std::shared_ptr<ZUniformBuffer> uniformBuffer;

    ZMaterialBase();

private:

	static ZIDSequence idGenerator_;

};

class ZMaterialManager : public ZResourceManager<ZMaterialBase, ZHMaterial>
{
public:

    virtual ~ZMaterialManager() = default;

	virtual void Initialize() override { }
	virtual void CleanUp() override { }

    ZHMaterial Create(const ZMaterialProperties& materialProperties, const ZHShader& shader = ZHShader());
    ZHMaterial Create(const ZTextureList& textures, const ZHShader& shader = ZHShader());
    ZHMaterial Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
    void DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);

    const std::string& Name(const ZHMaterial& handle) const;
    const ZTextureList& Textures(const ZHMaterial& handle) const;
    const ZHShader& Shader(const ZHMaterial& handle) const;
    float Alpha(const ZHMaterial& handle) const;
    bool IsTextured(const ZHMaterial& handle) const;
    bool HasDisplacement(const ZHMaterial& handle) const;
    const std::shared_ptr<ZRenderStateGroup>& RenderState(const ZHMaterial& handle) const;

    void SetShader(const ZHMaterial& handle, const ZHShader& shader);
    void SetAlpha(const ZHMaterial& handle, float alpha);
    void SetProperty(const ZHMaterial& handle, const std::string& property, float value);
    void SetProperty(const ZHMaterial& handle, const std::string& property, const glm::vec4& value);
    void SetProperty(const ZHMaterial& handle, const std::string& property, bool value);

    void AddTexture(const ZHMaterial& handle, const std::string& slot, const ZHTexture& texture);
    void AddTexture(const ZHMaterial& handle, const std::string& slot, const std::string& textureId) { pendingTextures_[slot] = textureId; }

    ZHMaterial Default();
    ZHMaterial CreateDefault();


protected:

    std::unordered_map<std::string, std::string> pendingTextures_;

protected:

    void UpdateUniformMaterial(const ZHMaterial& handle);

};
