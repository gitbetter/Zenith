/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZResourceExtraData.hpp

	Created by Adrian Sanchez on 11/04/2019.
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
#include "ZEngine.hpp"

// Forward Declarations
class ZWavResourceLoader;
class ZOggResourceLoader;
class ZResourceLoadTask;
class ZImageImporter;
struct ZSkeleton;

// Class and Data Structure Definitions
class ZResourceExtraData {

public:

	virtual ~ZResourceExtraData() {}

	virtual std::string ToString() = 0;

};

class ZSoundResourceExtraData : public ZResourceExtraData {

	friend class ZWavResourceLoader;
	friend class ZOggResourceLoader;
	friend class ZResourceLoadTask;

public:

	ZSoundResourceExtraData();
	~ZSoundResourceExtraData() {}

	std::string ToString() override { return "ZSoundResourceExtraData"; }
	ZSoundType SoundType() { return soundType_; }
	unsigned int LengthMilli() const { return lengthMilli_; }
	ZWavFormatDesc* WavFormatDesc() { return &wavFormatDesc_; }

protected:

	ZSoundType soundType_;
	unsigned int lengthMilli_;
	ZWavFormatDesc wavFormatDesc_;

};

class ZShaderResourceExtraData : public ZResourceExtraData {

	friend class ZResourceLoadTask;

public:

	ZShaderResourceExtraData();
	~ZShaderResourceExtraData() {}

	std::string ToString() override { return "ZShaderResourceExtraData"; }
	const std::string& Code() { return code_; }
	ZShaderType Type() { return type_; }

protected:

	ZShaderType type_;
	std::string code_;

};

class ZZOFResourceExtraData : public ZResourceExtraData {

	friend class ZResourceLoadTask;

public:

	ZZOFResourceExtraData() { }
	~ZZOFResourceExtraData() { }

	std::string ToString() override { return "ZZOFResourceExtraData"; }
	std::shared_ptr<ZOFTree> ObjectTree() { return objectTree_; }

protected:

	std::shared_ptr<ZOFTree> objectTree_;

};

class ZTextureResourceExtraData : public ZResourceExtraData {

	friend class ZResourceLoadTask;
	friend class ZImageImporter;

public:

	ZTextureResourceExtraData();
	~ZTextureResourceExtraData();

	std::string ToString() override { return "ZTextureResourceExtraData"; }
	bool IsHDR() { return hdr_; }
	bool IsFlipped() { return flipped_; }
	int Width() { return width_; }
	int Height() { return height_; }
	int NumChannels() { return channels_; }
	float* FloatData() { return fData_; }
	unsigned char* Data() { return ucData_; }

protected:

	bool hdr_;
	bool flipped_;
	int width_, height_, channels_;
	union {
		float* fData_;
		unsigned char* ucData_ = nullptr;
	};

};

class ZModelResourceExtraData : public ZResourceExtraData {

	friend class ZResourceLoadTask;

public:

	ZModelResourceExtraData() { }
	~ZModelResourceExtraData() { }

	std::string ToString() override { return "ZModelResourceExtraData"; }
	ZMesh3DMap& Meshes() { return meshMap_; }
	ZBoneMap& BoneMap() { return boneMap_; }
	ZBoneList& Bones() { return boneList_; }
	ZAnimationMap& Animations() { return animationMap_; }
	std::shared_ptr<ZSkeleton> Skeleton() { return skeleton_; }

protected:

	ZMesh3DMap meshMap_;
	ZBoneMap boneMap_;
	ZBoneList boneList_;
	ZAnimationMap animationMap_;
	std::shared_ptr<ZSkeleton> skeleton_;

};
