/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZResourceLoadTask.cpp

	Created by Adrian Sanchez on 12/05/2019.
	Copyright � 2019 Pervasive Sense. All rights reserved.

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

#include "ZResourceLoadTask.hpp"
#include "ZProcessRunner.hpp"
#include "ZEventAgent.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZOFParser.hpp"
#include "ZModelImporter.hpp"
#include "ZImageImporter.hpp"

void ZResourceLoadTask::Run() {
	std::shared_ptr<ZResourceHandle> handle = zenith::ResourceCache()->GetHandle(&resource_);

	if (!handle) {
		zenith::Log("Could not find resource at path " + resource_.name, ZSeverity::Error);
	}

	switch (resource_.type) {
	case ZResourceType::VertexShader:
	case ZResourceType::PixelShader:
	case ZResourceType::TesselationShader:
	case ZResourceType::GeometryShader:
	{
		if (handle) {
			std::shared_ptr<ZShaderResourceExtraData> extraData = std::make_shared<ZShaderResourceExtraData>();
			handle->SetExtra(extraData);
			extraData->code_ = std::string((char*)handle->Buffer());

			switch (resource_.type) {
			case ZResourceType::VertexShader: extraData->type_ = ZShaderType::Vertex; break;
			case ZResourceType::PixelShader: extraData->type_ = ZShaderType::Pixel; break;
			case ZResourceType::TesselationShader: extraData->type_ = ZShaderType::Tesselation; break;
			case ZResourceType::GeometryShader: extraData->type_ = ZShaderType::Geometry; break;
			default: break;
			}
		}
		break;
	}
	case ZResourceType::ZOF:
	{
		if (handle) {
			ZOFParser parser;
			std::shared_ptr<ZZOFResourceExtraData> extraData = std::make_shared<ZZOFResourceExtraData>();
			handle->SetExtra(extraData);
			extraData->objectTree_ = parser.Parse(std::string((const char*)handle->Buffer()));
		}
		break;
	}
	case ZResourceType::HDREquirectangularMap:
	case ZResourceType::HDRTexture:
	case ZResourceType::Texture:
	{
		if (handle) {
			ZImageImporter::LoadImage(handle,
				resource_.type == ZResourceType::HDRTexture || resource_.type == ZResourceType::HDREquirectangularMap,
				resource_.type == ZResourceType::HDRTexture || resource_.type == ZResourceType::HDREquirectangularMap);
		}
		break;
	}
	case ZResourceType::Model:
	{
		if (handle) {
			ZModelImporter importer;
			std::string modelDirectory = resource_.name.substr(0, resource_.name.find_last_of("/\\"));
			std::shared_ptr<ZModelResourceExtraData> extraData = std::make_shared<ZModelResourceExtraData>();
			handle->SetExtra(extraData);
			extraData->meshMap_ = importer.LoadModel(handle, extraData->boneMap_, extraData->boneList_, extraData->animationMap_, extraData->skeleton_, modelDirectory);
		}
		break;
	}
	default: break;
	}

	std::shared_ptr<ZResourceLoadedEvent> loadedEvent = std::make_shared<ZResourceLoadedEvent>(handle);
	zenith::EventAgent()->QueueEvent(loadedEvent);
}

void ZResourceLoadTask::Start() {
	std::shared_ptr<ZResourceLoadTask> thisPtr = shared_from_this();
	zenith::ProcessRunner()->AttachProcess(thisPtr);
}
