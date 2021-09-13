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

#include "ZGLFont.hpp"
#include "ZServices.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZFontReadyEvent.hpp"

FT_Library ZFontManager::ft_ = nullptr;
bool ZFontManager::initialized_ = false;

ZFontManager::ZFontManager()
    : fontPool_(512)
{
}

void ZFontManager::Initialize()
{
	ZServices::EventAgent()->Subscribe(this, &ZFontManager::HandleFontLoaded);
	ZHFont defaultFont = Create("/Fonts/arial/arial.ttf", 64.f);
}

void ZFontManager::CleanUp()
{
	ZServices::EventAgent()->Unsubscribe(this, &ZFontManager::HandleFontLoaded);
}

ZHFont ZFontManager::Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> data)
{
}

void ZFontManager::DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> data)
{
}

const std::string& ZFontManager::Name(const ZHFont& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null font handle!");
	ZFont* font = fontPool_.Get(handle);
	return font->name;
}

float ZFontManager::Size(const ZHFont& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null font handle!");
	ZFont* font = fontPool_.Get(handle);
	return font->size;
}

ZAtlas ZFontManager::Atlas(const ZHFont& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null font handle!");
	ZFont* font = fontPool_.Get(handle);
	return font->atlas;
}

ZCharacter ZFontManager::Character(const ZHFont& handle, unsigned char c)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null font handle!");
	ZFont* font = fontPool_.Get(handle);
	return font->atlas.characterInfo[c];
}

ZHFont ZFontManager::Create(const std::string& fontPath, unsigned int fontSize)
{
    // TODO: Switch on constant, variable or define to choose implementation
	InitializeFreeTypeIfNecessary();
	ZResourceData::ptr resource = std::make_shared<ZResourceData>(fontPath, ZResourceType::Font);
	ZServices::ResourceImporter()->GetData(resource.get());
	return Create(resource.get(), fontSize);
}

void ZFontManager::CreateAsync(const std::string& fontPath, unsigned int fontSize)
{
    // TODO: Switch on constant, variable or define to choose implementation
	InitializeFreeTypeIfNecessary();
	ZResourceData::ptr resource = std::make_shared<ZResourceData>(fontPath, ZResourceType::Font);
	pendingFontSizes_[fontPath] = fontSize;
	ZServices::ResourceImporter()->GetDataAsync(resource);
}

bool ZFontManager::IsLoaded(const std::string& name)
{
	return loadedFonts_.find(name) != loadedFonts_.end();
}

ZHFont ZFontManager::GetFromName(const std::string& name)
{
	if (loadedFonts_.find(name) != loadedFonts_.end()) {
		return loadedFonts_[name];
	}
	return ZHFont();
}

void ZFontManager::Track(const ZHFont& handle)
{
	ZFont* font = fontPool_.Get(handle);
	assert(font != nullptr && "Cannot track this font since it doesn't exist!");
	loadedFonts_[font->name] = handle;
}

void ZFontManager::InitializeFreeTypeIfNecessary()
{
    if (!initialized_) {
        if (FT_Init_FreeType(&ft_)) {
            LOG("Could not Initialize the FreeType library", ZSeverity::Error);
            return;
        }
        initialized_ = true;
    }
}

void ZFontManager::HandleFontLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
	if (event->Resource() == nullptr)
	{
		return;
	}

	ZResourceData::ptr resource = event->Resource();
	if (resource->type != ZResourceType::Font)
		return;

	ZHFont handle = Create(resource.get(), pendingFontSizes_[resource->path]);
	pendingFontSizes_.erase(resource->path);

	std::shared_ptr<ZFontReadyEvent> fontReadyEvent = std::make_shared<ZFontReadyEvent>(handle);
	ZServices::EventAgent()->Queue(fontReadyEvent);
}
