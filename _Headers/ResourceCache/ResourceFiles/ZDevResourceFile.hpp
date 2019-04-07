/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZDevResourceFile.hpp

		Created by Adrian Sanchez on 06/04/2019.
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
#include "ZResourceFile.hpp"
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZDevResourceFile : public ZResourceFile {

private:

	std::string fileName_;

public:

	ZDevResourceFile(const std::string& filePath) : fileName_(filePath) {}
	~ZDevResourceFile() {}
	bool Open() override;
	const std::string& Name() const override { return fileName_; }
	unsigned int RawResourceSize(ZResource& resource) override;
	unsigned int RawResource(ZResource& resource, char* buffer) override;
	unsigned int ResourceCount() const override;
	std::string ResourceName(unsigned int num) const override;
	void PrintResources() const override;
	void Close() override;

protected:

	std::map<std::string, unsigned int> assetIndices_;
	std::vector<cppfs::FileHandle> assets_;

	void ReadDirectoryEntries(cppfs::FileHandle& fh);

};
