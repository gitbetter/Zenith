/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourceData.hpp

    Created by Adrian Sanchez on 08/03/2019.
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
#include "ZMesh.hpp"

/**
	ZResourceData is a lifetime load object that is only meant to exist
	temporarily once resources get imported. The recycling of the resource data
	buffer is left up the STL shared pointer reference counter, so you must make sure
	to copy all the relevant data from the resource data onto the appropriate
	engine data structures, as it will get recycled once the shared pointer is no longer held.
*/
class ZResourceData
{

public:

	using ptr = std::shared_ptr<ZResourceData>;

    ZResourceType type;
    std::string path;
	void* buffer{ nullptr };
	unsigned int size{ 0 };

public:

    ZResourceData(const std::string& path, ZResourceType type, void* buffer = nullptr, unsigned int size = 0)
        : path(path), type(type), buffer(buffer), size(size)
    { }
	virtual ~ZResourceData()
	{
		if (buffer != nullptr)
		{
			delete buffer;
			buffer = nullptr;
		}
	}

};

class ZModelResourceData : public ZResourceData
{

public:

	using ptr = std::shared_ptr<ZModelResourceData>;

	ZHModel restoreHandle;
	ZMesh3DList meshMap;
	ZBoneMap boneMap;
	ZBoneList boneList;
	ZAnimationMap animationMap;
	ZSkeleton skeleton;

public:

	ZModelResourceData(const std::string& path, ZResourceType type = ZResourceType::Model, void* buffer = nullptr, unsigned int size = 0)
		: ZResourceData(path, type, buffer, size)
	{ }

};

class ZTextureResourceData : public ZResourceData
{

public:

	using ptr = std::shared_ptr<ZTextureResourceData>;
    
	ZTextureWrapping wrapping{ ZTextureWrapping::Repeat };
	std::string textureType;
	ZHTexture restoreHandle;
	bool hdr{ false };
	bool flipped{ false };
	int width{ 0 };
    int height{ 0 };
    int channels{ 0 };
	void* data{ nullptr };

public:

	ZTextureResourceData(const std::string& path, ZTextureWrapping wrapping, const std::string& textureType, const ZHTexture& restoreHandle = ZHTexture(), ZResourceType type = ZResourceType::Texture, void* buffer = nullptr, unsigned int size = 0)
		: ZResourceData(path, type, buffer, size), wrapping(wrapping), textureType(textureType), restoreHandle(restoreHandle)
	{ }
	~ZTextureResourceData();

};

class ZZofResourceData : public ZResourceData
{

public:

	using ptr = std::shared_ptr<ZZofResourceData>;

    std::shared_ptr<struct ZOFNode> objectTree;

public:

	ZZofResourceData(const std::string& path, ZResourceType type = ZResourceType::ZOF, void* buffer = nullptr, unsigned int size = 0)
		: ZResourceData(path, type, buffer, size)
	{ }

};

class ZShaderResourceData : public ZResourceData
{

public:

	using ptr = std::shared_ptr<ZShaderResourceData>;

	ZShaderType type;
	std::string code;

public:

	ZShaderResourceData(const std::string& path, ZResourceType type = ZResourceType::VertexShader, void* buffer = nullptr, unsigned int size = 0)
		: ZResourceData(path, type, buffer, size)
	{ }

};

class ZAudioResourceData : public ZResourceData
{

public:

	using ptr = std::shared_ptr<ZAudioResourceData>;

	ZSoundType soundType;
	unsigned int lengthMilli;
	ZWavFormatDesc wavFormatDesc;

public:

	ZAudioResourceData(const std::string& path, ZResourceType type = ZResourceType::Sound, void* buffer = nullptr, unsigned int size = 0)
		: ZResourceData(path, type, buffer, size)
	{ }

};

class ZScriptResourceData : public ZResourceData
{

public:

	using ptr = std::shared_ptr<ZScriptResourceData>;

	ZHScript restoreHandle;
	std::string code;

	ZScriptResourceData(const std::string& path, ZResourceType type = ZResourceType::Script, void* buffer = nullptr, unsigned int size = 0)
		: ZResourceData(path, type, buffer, size)
	{ }

};

class ZFontResourceData : public ZResourceData
{

public:

	using ptr = std::shared_ptr<ZFontResourceData>;

	ZHFont restoreHandle;

	ZFontResourceData(const std::string& path, ZResourceType type = ZResourceType::Font, void* buffer = nullptr, unsigned int size = 0)
		: ZResourceData(path, type, buffer, size)
	{ }

};