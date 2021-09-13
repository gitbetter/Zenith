/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZFont.hpp

    Created by Adrian Sanchez on 29/01/2021.
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

#include "ZTexture.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

class ZResourceLoadedEvent;

struct ZAtlas
{
    unsigned int width;
    unsigned int height;
    ZHTexture texture;
    std::map<unsigned char, ZCharacter> characterInfo;
};

struct ZFont
{
    std::string name;
    float size;
    ZAtlas atlas;
};

class ZFontManager
{
	using ZFontPool = ZResourcePool<ZFont, ZHFont>;

public:

    ZFontManager();
	virtual ~ZFontManager() = default;

	virtual void Initialize();
	virtual void CleanUp();

	ZHFont Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
	void DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
	virtual ZHFont Create(const std::string& fontPath, unsigned int fontSize);
	virtual void CreateAsync(const std::string& fontPath, unsigned int fontSize);

	bool IsLoaded(const std::string& name);
	ZHFont GetFromName(const std::string& name);

	const std::string& Name(const ZHFont& handle);
	float Size(const ZHFont& handle);
	ZAtlas Atlas(const ZHFont& handle);
	ZCharacter Character(const ZHFont& handle, unsigned char c);

	virtual ZHFont Create(class ZResourceData* resource, unsigned int fontSize) = 0;

protected:

	ZFontPool fontPool_;
	ZFontMap loadedFonts_;

	std::unordered_map<std::string, unsigned int> pendingFontSizes_;

	static FT_Library ft_;
	static bool initialized_;

protected:

	/** Adds a font to the internal loaded font map so that we don't accidentally recreate duplicates of the font */
	void Track(const ZHFont& handle);

	static void InitializeFreeTypeIfNecessary();

private:

	void HandleFontLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);
};
