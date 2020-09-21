/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZOFParser.hpp

    Created by Adrian Sanchez on 17/02/2019.
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
#include "ZOFTree.hpp"
#include "ZEngine.hpp"
#include "ZStringHelpers.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZOFParser
{
private:

    std::string currentToken_;
    std::stringstream zof_;
    std::regex id_ = std::regex("\"[^\"]+\"|[^\\-\\d\"]\\S+");
    std::regex number_ = std::regex("-?\\d+\\.?\\d*");

    std::string Scan();
    void Match(const std::string& token);
    void Match(const std::regex& pattern);
    void HandleParseError(std::shared_ptr<ZOFNode> node);

    void Start(std::shared_ptr<ZOFTree> tree);
    void ObjectList(std::shared_ptr<ZOFTree> node);
    void Object(std::shared_ptr<ZOFTree> tree);
    void PropsList(std::shared_ptr<ZOFObjectNode> node);
    void Prop(std::shared_ptr<ZOFObjectNode> node);
    void ValuesList(std::shared_ptr<ZOFPropertyNode> prop);
    void Value(std::shared_ptr<ZOFPropertyNode> prop);
    void List(std::shared_ptr<ZOFPropertyNode> prop);
    void ListTail(std::shared_ptr<ZOFAbstractTerminal> terminal);

public:

    ZOFParser() {}
    ~ZOFParser() {}

    std::shared_ptr<ZOFTree> Parse(const std::string& contents);

protected:


};
