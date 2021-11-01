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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

ZHFont ZGLFontManager::Create(ZResourceData* resource, unsigned int fontSize)
{
	ZHFont handle;
	ZFont* font = resourcePool_.New(handle);

	if (resource == nullptr)
	{
		return ZHFont();
	}

	FT_Face face;
	if (FT_New_Memory_Face(ft_, (const FT_Byte*)resource->buffer, (FT_Long)resource->size, 0, &face)) {
		LOG("Could not load font at " + resource->path, ZSeverity::Error);
		return ZHFont();
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	std::string file = resource->path.substr(resource->path.find_last_of("/\\") + 1);
	std::string fileName = file.substr(0, file.find_last_of("."));
	font->name = fileName;
	font->size = fontSize;
	font->atlas.width = 0;
	font->atlas.height = 0;
	for (unsigned char c = 0; c < 255; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			LOG("Could not load character " + std::to_string(c), ZSeverity::Error);
			continue;
		}

		font->atlas.width += face->glyph->bitmap.width + 3; // Add an extra few pixels to avoid sampling pixels from previous glyph
		font->atlas.height = std::max(font->atlas.height, face->glyph->bitmap.rows);
	}

	font->atlas.texture = ZServices::TextureManager()->Create();
	ZServices::TextureManager()->SetType(font->atlas.texture, "atlas");
	ZServices::TextureManager()->SetPath(font->atlas.texture, fileName);

	ZServices::TextureManager()->Bind(font->atlas.texture, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->atlas.width, font->atlas.height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int x = 0;
	font->atlas.characterInfo.clear();
	for (unsigned char c = 0; c < 255; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			LOG("Could not load character " + std::to_string(c), ZSeverity::Error);
			continue;
		}

		ZCharacter character;
		character.advance.x = face->glyph->advance.x >> 6;
		character.advance.y = face->glyph->advance.y >> 6;
		character.bitmapSize.x = face->glyph->bitmap.width;
		character.bitmapSize.y = face->glyph->bitmap.rows;
		character.bitmapPos.x = face->glyph->bitmap_left;
		character.bitmapPos.y = face->glyph->bitmap_top;
		character.xOffset = (double)x / font->atlas.width;
		font->atlas.characterInfo[c] = character;

		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		x += face->glyph->bitmap.width + 3; // Add an extra few pixels to avoid sampling pixels from previous glyph
	}

	FT_Done_Face(face);

	Track(handle);

	return handle;
}
