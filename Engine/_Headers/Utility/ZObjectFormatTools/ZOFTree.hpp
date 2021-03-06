/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZOFTree.hpp

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
#include "ZCommon.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

// Forward Declarations
struct ZOFNode;
struct ZOFObjectNode;
struct ZOFPropertyNode;
struct ZOFAbstractTerminal;
template<typename T> struct ZOFValueTerminal;

// Class and Data Structure Definitions
typedef std::map<std::string, std::shared_ptr<ZOFNode>> ZOFChildMap;
typedef std::map<std::string, std::shared_ptr<ZOFPropertyNode>> ZOFPropertyMap;
typedef std::vector<std::shared_ptr<ZOFAbstractTerminal>> ZOFAbstractTerminalList;

struct ZOFNode
{
    std::string id;
    std::shared_ptr<ZOFNode> root;
    ZOFChildMap children;

    virtual ~ZOFNode() {}

    void Clear()
    {
        children.clear();
    }

    virtual std::string ToString()
    {
        std::string objString = (root == nullptr) ? std::string(1, NEWLINE) : "";
        ZOFChildMap::iterator it = children.begin();
        for (; it != children.end(); it++) objString += it->second->ToString();
        return objString;
    }
};

struct ZOFAbstractTerminal
{
    std::shared_ptr<ZOFNode> root;
    virtual ~ZOFAbstractTerminal() {};
    virtual std::string ToString() { return ""; }
};

template<class T>
struct ZOFValueTerminal : public ZOFAbstractTerminal
{
    T value;
    ~ZOFValueTerminal() {}

    std::string ToString() override { return ""; }
};

struct ZOFPropertyNode : public ZOFNode
{
    ZOFAbstractTerminalList values;

    ~ZOFPropertyNode()
    {
        values.clear();
    }

    bool HasValues() const { return values.size() > 0; }

    unsigned int ValueCount() const { return values.size(); }

    template<typename T>
    std::shared_ptr<T> Value(unsigned int index)
    {
        if (index >= values.size()) return nullptr;

        std::shared_ptr<T> val = std::dynamic_pointer_cast<T>(values[index]);

        return val;
    }

    std::string ToString() override
    {
        std::string propString = "\t:" + id;
        for (std::shared_ptr<ZOFAbstractTerminal> val : values) propString += val->ToString();
        propString += ZOFNode::ToString();
        propString += ":" + NEWLINE;
        return propString;
    }
};

struct ZOFObjectNode : public ZOFNode
{
    ZOFPropertyMap properties;

    ~ZOFObjectNode()
    {
        properties.clear();
    }

    std::string ToString() override
    {
        std::string objString = id + NEWLINE;
        for (ZOFPropertyMap::iterator it = properties.begin(); it != properties.end(); it++)
            objString += it->second->ToString();
        objString += ZOFNode::ToString();
        objString += "-" + NEWLINE;
        return objString;
    }
};

typedef ZOFValueTerminal<float> ZOFNumber;
typedef ZOFValueTerminal<std::string> ZOFString;
typedef ZOFValueTerminal<std::vector<float>> ZOFNumberList;
typedef ZOFValueTerminal<std::vector<std::string>> ZOFStringList;
typedef ZOFNode ZOFTree;

// Template specializations
template<> std::string ZOFNumber::ToString();
template<> std::string ZOFString::ToString();
template<> std::string ZOFNumberList::ToString();
template<> std::string ZOFStringList::ToString();

template<> inline std::string ZOFNumber::ToString()
{
    std::string valueString = " " + std::to_string(value);
    return valueString;
}

template<> inline std::string ZOFString::ToString()
{
    std::string valueString = " " + value;
    return valueString;
}

template<> inline std::string ZOFNumberList::ToString()
{
    std::string valueString = " [";
    for (float v : value)
    {
        valueString += std::to_string(v) + ", ";
    }
    valueString = valueString.substr(0, valueString.length() - 2);
    return valueString + "]";
}

template<> inline std::string ZOFStringList::ToString()
{
    std::string valueString = " [";
    for (std::string v : value)
    {
        valueString += v + ", ";
    }
    valueString = valueString.substr(0, valueString.length() - 2);
    return valueString + "]";
}
