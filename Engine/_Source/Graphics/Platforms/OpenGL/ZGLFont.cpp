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

void ZGLFont::Load(const std::shared_ptr<ZResourceHandle>& handle, unsigned int fontSize)
{
	if (handle == nullptr) return;

	FT_Face face;
	if (FT_New_Memory_Face(ft_, (const FT_Byte*)handle->Buffer(), (FT_Long)handle->Size(), 0, &face)) {
		LOG("Could not load font at " + handle->Resource().name, ZSeverity::Error);
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	std::string file = handle->Resource().name.substr(handle->Resource().name.find_last_of("/\\") + 1);
	std::string fileName = file.substr(0, file.find_last_of("."));
	name_ = fileName;
	size_ = fontSize;
	atlas_.width = 0;
	atlas_.height = 0;
	for (unsigned char c = 0; c < 255; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			LOG("Could not load character " + std::to_string(c), ZSeverity::Error);
			continue;
		}

		atlas_.width += face->glyph->bitmap.width + 3; // Add an extra few pixels to avoid sampling pixels from previous glyph
		atlas_.height = std::max(atlas_.height, face->glyph->bitmap.rows);
	}

	atlas_.texture = ZTexture::Create();
	atlas_.texture->type = "atlas";
	atlas_.texture->path = fileName;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &atlas_.texture->id);
	glBindTexture(GL_TEXTURE_2D, atlas_.texture->id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_.width, atlas_.height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int x = 0;
	atlas_.characterInfo.clear();
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
		character.xOffset = (double)x / atlas_.width;
		atlas_.characterInfo[c] = character;

		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		x += face->glyph->bitmap.width + 3; // Add an extra few pixels to avoid sampling pixels from previous glyph
	}

	FT_Done_Face(face);
}

void ZGLFont::SetSize(unsigned int size)
{
	ZFont::Load(name_, size);
}
