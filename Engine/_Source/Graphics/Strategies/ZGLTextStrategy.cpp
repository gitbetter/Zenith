/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZGLTextStrategyStrategy.cpp

	Created by Adrian Sanchez on 11/02/2019.
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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ZGLTextStrategy.hpp"
#include "ZResource.hpp"
#include "ZResourceCache.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZEventAgent.hpp"

void ZGLTextStrategy::Initialize() {
	ZTextStrategy::Initialize();

	ZEventDelegate fontLoadedDelegate = fastdelegate::MakeDelegate(this, &ZGLTextStrategy::HandleFontLoaded);
	zenith::EventAgent()->AddListener(fontLoadedDelegate, ZResourceLoadedEvent::Type);
}

void ZGLTextStrategy::LoadFont(std::string fontPath, unsigned int fontSize) {
	ZResource resource(fontPath, ZResourceType::Font);
	std::shared_ptr<ZResourceHandle> handle = zenith::ResourceCache()->GetHandle(&resource);
	LoadFont(handle, fontSize);
}

void ZGLTextStrategy::LoadFontAsync(std::string fontPath, unsigned int fontSize) {
	ZResource resource(fontPath, ZResourceType::Font);
	zenith::ResourceCache()->RequestHandle(resource);
	pendingFonts_[fontPath] = fontSize;
}

void ZGLTextStrategy::LoadFont(std::shared_ptr<ZResourceHandle> handle, unsigned int fontSize) {
	if (handle == nullptr) return;

	FT_Face face;
	if (FT_New_Memory_Face(ft_, (const FT_Byte*)handle->Buffer(), (FT_Long)handle->Size(), 0, &face)) {
		zenith::Log("Could not load font at " + handle->Resource().name, ZSeverity::Error);
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	ZFont font;
	std::string file = handle->Resource().name.substr(handle->Resource().name.find_last_of("/\\") + 1);
	std::string fileName = file.substr(0, file.find_last_of("."));
	font.name = fileName;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 255; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			zenith::Log("Could not load character " + std::to_string(c), ZSeverity::Error);

		ZCharacter character;
		character.texture.type = "glyph";
		character.texture.path = handle->Resource().name;
		glGenTextures(1, &character.texture.id);
		glBindTexture(GL_TEXTURE_2D, character.texture.id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		character.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		character.advance = face->glyph->advance.x;

		font.characters[c] = character;
	}
	loadedFonts_[fileName] = font;

	FT_Done_Face(face);
}

void ZGLTextStrategy::HandleFontLoaded(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZResourceLoadedEvent> resourceLoadedEvent = std::static_pointer_cast<ZResourceLoadedEvent>(event);
	std::shared_ptr<ZResourceHandle> handle = resourceLoadedEvent->Handle();

	if (pendingFonts_.find(handle->Resource().name) != pendingFonts_.end()) {
		LoadFont(handle, pendingFonts_[handle->Resource().name]);
		pendingFonts_.erase(handle->Resource().name);
	}
}
