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
#include "ZGraphics.hpp"
#include "ZGLGraphicsStrategy.hpp"
#include "ZDomain.hpp"
#include "ZResource.hpp"
#include "ZResourceCache.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZEventAgent.hpp"
#include "ZUIText.hpp"
#include "ZShader.hpp"

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
	if (FT_New_Memory_Face(ft_, (const FT_Byte *)handle->Buffer(), (FT_Long)handle->Size(), 0, &face)) {
		zenith::Log("Could not load font at " + handle->Resource().name, ZSeverity::Error);
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	ZFont font;
	std::string file = handle->Resource().name.substr(handle->Resource().name.find_last_of("/\\") + 1);
	std::string fileName = file.substr(0, file.find_last_of("."));
	font.name = fileName;
	font.size = fontSize;
	font.atlas.width = 0;
	font.atlas.height = 0;
	for (unsigned char c = 0; c < 255; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			zenith::Log("Could not load character " + std::to_string(c), ZSeverity::Error);
			continue;
		}
	
		font.atlas.width += face->glyph->bitmap.width;
		font.atlas.height = std::max(font.atlas.height, face->glyph->bitmap.rows);
	}

	font.atlas.texture.type = "atlas";
	font.atlas.texture.path = fileName;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &font.atlas.texture.id);
	glBindTexture(GL_TEXTURE_2D, font.atlas.texture.id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font.atlas.width, font.atlas.height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int x = 0;
	font.atlas.characterInfo.clear();
	for (unsigned char c = 0; c < 255; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			zenith::Log("Could not load character " + std::to_string(c), ZSeverity::Error);
			continue;
		}

		ZCharacter character;
		character.advance.x = face->glyph->advance.x >> 6;
		character.advance.y = face->glyph->advance.y >> 6;
		character.bitmapSize.x = face->glyph->bitmap.width;
		character.bitmapSize.y = face->glyph->bitmap.rows;
		character.bitmapPos.x = face->glyph->bitmap_left;
		character.bitmapPos.y = face->glyph->bitmap_top;
		character.xOffset = (float) x / font.atlas.width;
		font.atlas.characterInfo[c] = character;

		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		x += face->glyph->bitmap.width;
	}
	loadedFonts_[font.name] = font;
	pendingFonts_.erase(handle->Resource().name);

	FT_Done_Face(face);
}

void ZGLTextStrategy::Draw(std::shared_ptr<ZUIText> textEl)
{
	if (loadedFonts_.find(textEl->font_) == loadedFonts_.end())
	{
		zenith::Log("The font " + textEl->font_ + " has not been loaded.", ZSeverity::Warning);
		return;
	}

	auto font = loadedFonts_[textEl->font_];
	zenith::Graphics()->Strategy()->BindTexture(font.atlas.texture, 0);
	textEl->shader_->SetInt(font.atlas.texture.type + "0", 0);

	auto pos = textEl->Position();
	float x = pos.x, y = pos.y;

	ZVertex2DDataOptions options;
	options.vertices.reserve(textEl->text_.size() * 6);
	for (auto c = textEl->text_.begin(); c != textEl->text_.end(); c++)
	{
		ZCharacter character = font.atlas.characterInfo[*c];

		float one_over_font_size = 1.f / font.size;

		float atlasH = font.atlas.height * 0.5f * textEl->fontScale_ * one_over_font_size;
		float w = character.bitmapSize.x * textEl->fontScale_ * one_over_font_size;
		float h = character.bitmapSize.y * textEl->fontScale_ * one_over_font_size;
		float xpos = x + character.bitmapPos.x * textEl->fontScale_ * one_over_font_size;
		float ypos = (y + atlasH) - character.bitmapPos.y * textEl->fontScale_ * one_over_font_size;

		x += character.advance.x * textEl->fontScale_ * one_over_font_size;
		y += character.advance.y * textEl->fontScale_ * one_over_font_size;

		if (textEl->wrapToBounds_)
		{
			auto maxX = textEl->parent_ ? pos.x + textEl->parent_->Size().x * 2.f - w * 2.f: pos.x + textEl->Size().x - w * 2.f;
			if (x > maxX)
			{
				y += std::floor(x / maxX) * (atlasH + textEl->lineSpacing_);
				x = pos.x;
			}
		}

		if (w == 0 || h == 0) continue;

		options.vertices.push_back(ZVertex2D(glm::vec2(xpos, ypos), glm::vec2(character.xOffset, 0))); 
		options.vertices.push_back(ZVertex2D(glm::vec2(xpos + w, ypos), glm::vec2(character.xOffset + character.bitmapSize.x / font.atlas.width, 0)));
		options.vertices.push_back(ZVertex2D(glm::vec2(xpos, ypos + h), glm::vec2(character.xOffset, character.bitmapSize.y / font.atlas.height)));
		options.vertices.push_back(ZVertex2D(glm::vec2(xpos + w, ypos), glm::vec2(character.xOffset + character.bitmapSize.x / font.atlas.width, 0)));
		options.vertices.push_back(ZVertex2D(glm::vec2(xpos, ypos + h), glm::vec2(character.xOffset, character.bitmapSize.y / font.atlas.height)));
		options.vertices.push_back(ZVertex2D(glm::vec2(xpos + w, ypos + h), glm::vec2(character.xOffset + character.bitmapSize.x / font.atlas.width, character.bitmapSize.y / font.atlas.height)));
	}

	zenith::Graphics()->Strategy()->UpdateBuffer(textEl->bufferData_, options);
	zenith::Graphics()->Strategy()->Draw(textEl->bufferData_, options, ZMeshDrawStyle::Triangle);
}

void ZGLTextStrategy::HandleFontLoaded(const std::shared_ptr<ZEvent>& event) {
	std::shared_ptr<ZResourceLoadedEvent> resourceLoadedEvent = std::static_pointer_cast<ZResourceLoadedEvent>(event);
	std::shared_ptr<ZResourceHandle> handle = resourceLoadedEvent->Handle();

	if (pendingFonts_.find(handle->Resource().name) != pendingFonts_.end()) {
		LoadFont(handle, pendingFonts_[handle->Resource().name]);
		pendingFonts_.erase(handle->Resource().name);
	}
}
