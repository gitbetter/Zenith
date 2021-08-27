/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZOFParser.cpp

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

#include "ZOFParser.hpp"
#include "ZOFTree.hpp"
#include "ZServices.hpp"
#include <filesystem>

std::shared_ptr<ZOFNode> ZOFParser::Parse(const std::string& contents)
{
    std::shared_ptr<ZOFNode> parseTree = std::make_shared<ZOFNode>();
    if (!contents.empty())
    {
        zof_ = std::stringstream(contents);
        Start(parseTree);
    }
    return parseTree;
}

std::string ZOFParser::Scan()
{
    std::string whitepace("\r\n\t ");
    std::string specialCharacters("^,:[]");
    std::string token;
    bool in_quotes(false);
    char c;
    while (zof_ >> std::noskipws >> c)
    {
        if (!in_quotes)
        {
            // Strip initial whitespace
            if (whitepace.find(c) != std::string::npos) continue;
            // Ignore comments
            if (c == '#')
            {
                zof_.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue;
            }
            else if (c == '"')
            {
                in_quotes = true;
            }
            // Handle .zof single character tokens
            if (specialCharacters.find(c) != std::string::npos)
            {
                token += c; break;
            }
        }
        token += c;
        // Break out of the loop once we have our token
        // (the following character is either whitepace or one of our single character tokens)
        if (!in_quotes && (whitepace + specialCharacters).find(zof_.peek()) != std::string::npos) break;
        // Break out of the loop if we have finished a quoted string
        if (in_quotes && zof_.peek() == '"')
        {
            token += zof_.get(); break;
        }
    }
    return token;
}

void ZOFParser::Match(const std::string& token)
{
    if (token == currentToken_) currentToken_ = Scan();
    else LOG("[ZOFParse Error]: Expected " + token + ", but instead got " + currentToken_, ZSeverity::Error);
}

void ZOFParser::Match(const std::regex& pattern)
{
    if (std::regex_match(currentToken_, pattern)) currentToken_ = Scan();
    else LOG("[ZOFParse Error]: Token " + currentToken_ + " is malformed", ZSeverity::Error);
}

void ZOFParser::HandleParseError(std::shared_ptr<ZOFNode> node)
{
    LOG("[ZOFParse Error]: Unexpected token " + currentToken_, ZSeverity::Error);
    node->Clear();
}

void ZOFParser::Start(std::shared_ptr<ZOFNode> tree)
{
    currentToken_ = Scan();
    ObjectList(tree);
    Match("");
}

void ZOFParser::ObjectList(std::shared_ptr<ZOFNode> node)
{
    if (currentToken_ != "")
    {
        Object(node); ObjectList(node);
    }
}

void ZOFParser::Object(std::shared_ptr<ZOFNode> node)
{
    if (std::regex_match(currentToken_, id_))
    {
        // Push a new object node onto the tree
        std::shared_ptr<ZOFObjectNode> objectNode = std::make_shared<ZOFObjectNode>();
        objectNode->id = currentToken_;
        objectNode->root = node;
        node->children[objectNode->id] = objectNode;

        Match(id_); Match(">"); ObjectType(objectNode); PropsList(objectNode); Match("^");
    }
    else
    {
        HandleParseError(node);
    }
}

void ZOFParser::ObjectType(std::shared_ptr<ZOFObjectNode> node)
{
	if (std::regex_match(currentToken_, id_))
	{
		// Push a new object node onto the tree
        node->type = ZOFObjectNode::StringToType(currentToken_);

		Match(id_);
	}
	else
	{
		HandleParseError(node);
	}
}

void ZOFParser::PropsList(std::shared_ptr<ZOFObjectNode> node)
{
    if (currentToken_ != "" && currentToken_ != "^")
    {
        if (currentToken_ == ":")
        {
            Match(":"); Prop(node); PropsList(node);
        }
        else
        {
            Object(node); PropsList(node);
        }
    }
}

void ZOFParser::Prop(std::shared_ptr<ZOFObjectNode> objectNode)
{
    if (std::regex_match(currentToken_, id_))
    {
        // Push a new prop onto the object node
        std::shared_ptr<ZOFPropertyNode> propNode = std::make_shared<ZOFPropertyNode>();
        propNode->id = currentToken_;
        propNode->root = objectNode->root;
        objectNode->properties[currentToken_] = propNode;

        Match(id_); ValuesList(propNode); Match(":");
    }
    else
    {
        HandleParseError(objectNode->root);
    }
}

void ZOFParser::ValuesList(std::shared_ptr<ZOFPropertyNode> prop)
{
    if (currentToken_ != "" && currentToken_ != ":")
    {
        Value(prop); ValuesList(prop);
    }
}

void ZOFParser::Value(std::shared_ptr<ZOFPropertyNode> prop)
{
    if (currentToken_ == "[")
    {
        Match("["); List(prop); Match("]");
    }
    else if (std::regex_match(currentToken_, id_))
    {
        // Push a new string onto the prop
        std::shared_ptr<ZOFString> terminal = std::make_shared<ZOFString>();
        std::string s(currentToken_);
        s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
        s = zenith::strings::FormatStringGlobals(s);
        terminal->value = s;
        terminal->root = prop->root;
        prop->values.push_back(terminal);

        Match(id_);
    }
    else if (std::regex_match(currentToken_, number_))
    {
        // Push a new float onto the prop
        std::shared_ptr<ZOFNumber> terminal = std::make_shared<ZOFNumber>();
        terminal->value = std::stof(currentToken_);
        terminal->root = prop->root;
        prop->values.push_back(terminal);

        Match(number_);
    }
    else
    {
        HandleParseError(prop->root);
    }
}

void ZOFParser::List(std::shared_ptr<ZOFPropertyNode> prop)
{
    if (currentToken_ != "" && currentToken_ != "]")
    {
        if (std::regex_match(currentToken_, id_))
        {
            // Push a new list of strings onto the list
            std::shared_ptr<ZOFStringList> terminal = std::make_shared<ZOFStringList>();
            std::string s = currentToken_;
            s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
            terminal->value.push_back(s);
            terminal->root = prop->root;
            prop->values.push_back(terminal);

            Match(id_); ListTail(terminal);
        }
        else if (std::regex_match(currentToken_, number_))
        {
            // Push a new list of floats onto the list
            std::shared_ptr<ZOFNumberList> terminal = std::make_shared<ZOFNumberList>();
            terminal->value.push_back(std::stof(currentToken_));
            terminal->root = prop->root;
            prop->values.push_back(terminal);

            Match(number_); ListTail(terminal);
        }
        else
        {
            HandleParseError(prop->root);
        }
    }
}

void ZOFParser::ListTail(std::shared_ptr<ZOFAbstractTerminal> terminal)
{
    if (currentToken_ != "" && currentToken_ != ":" && currentToken_ != "]")
    {
        if (currentToken_ == ",")
        {
            Match(","); ListTail(terminal);
        }
        else if (std::regex_match(currentToken_, id_))
        {
            // Push a new string onto the list of strings
            std::shared_ptr<ZOFStringList> term = std::static_pointer_cast<ZOFStringList>(terminal);
            if (term != nullptr)
            {
                std::string s = currentToken_;
                s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
                term->value.push_back(s);
            }

            Match(id_), ListTail(term);
        }
        else if (std::regex_match(currentToken_, number_))
        {
            // Push a new float onto the list of floats
            std::shared_ptr<ZOFNumberList> term = std::static_pointer_cast<ZOFNumberList>(terminal);
            if (term != nullptr)
                term->value.push_back(std::stof(currentToken_));

            Match(number_), ListTail(term);
        }
        else
        {
            HandleParseError(terminal->root);
        }
    }
}
