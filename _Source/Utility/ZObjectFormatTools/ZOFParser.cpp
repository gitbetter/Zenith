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

ZOFTree* ZOFParser::Parse(std::string zofFile) {
  ZOFTree* parseTree = new ZOFTree;
  if (!zofFile.empty()) {
    zof_.exceptions(std::ifstream::failbit);
    try {
      zof_.open(zofFile);

      Start(parseTree);

      zof_.close();
    } catch (std::ifstream::failure e) {
      _Z("There was an error reading the .zof object file. Please consult the code.", ZERROR);
    }
  }
  return parseTree;
}

std::string ZOFParser::Scan() {
  std::string whitepace("\n\t ");
  std::string specialCharacters("^,:[]");
  std::string token;
  char c;
  while(zof_ >> c) {
    // Strip initial whitespace
    if (whitepace.find(c) != std::string::npos) continue;
    // Ignore comments
    if (c == '#') {
      zof_.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue;
    }
    // Handle .zof single character tokens
    if (specialCharacters.find(c) != std::string::npos) {
      token += c; break;
    }
    token += c;
    // Break out of the loop once we have our token
    // (the following character is either whitepace or one of our single character tokens)
    if ((whitepace + specialCharacters).find(zof_.peek()) != std::string::npos) break;
  }
  return token;
}

void ZOFParser::Match(std::string token) {
  if (token == currentToken_) currentToken_ = Scan();
  else _Z("[ZOFParse Error]: Expected " + token + ", but instead got " + currentToken_, ZERROR);
}

void ZOFParser::Match(std::regex pattern) {
  if (std::regex_match(currentToken_, pattern)) currentToken_ = Scan();
  else _Z("[ZOFParse Error]: Token " + currentToken_ + " is malformed", ZERROR);
}

void ZOFParser::HandleParseError(ZOFNode* node) {
  _Z("[ZOFParse Error]: Unexpected token " + currentToken_, ZERROR);
  node->Clear();
}

void ZOFParser::Start(ZOFTree* tree) {
  currentToken_ = Scan();
  ObjectList(tree);
  Match("");
}

void ZOFParser::ObjectList(ZOFNode* node) {
  if (currentToken_ != "") {
    Object(node); ObjectList(node);
  }
}

void ZOFParser::Object(ZOFNode* node) {
  if (std::regex_match(currentToken_, id_)) {
    // Push a new object node onto the tree
    ZOFObjectNode* objectNode = new ZOFObjectNode;
    objectNode->id = currentToken_;
    objectNode->root = node;
    node->children[objectNode->id] = objectNode;

    Match(id_); PropsList(objectNode); Match("^");
  } else {
    HandleParseError(node);
  }
}

void ZOFParser::PropsList(ZOFObjectNode* node) {
  if (currentToken_ != "" && currentToken_ != "^") {
    if (currentToken_ == ":") {
      Match(":"); Prop(node); PropsList(node);
    } else {
      Object(node); PropsList(node);
    }
  }
}

void ZOFParser::Prop(ZOFObjectNode* objectNode) {
  if (std::regex_match(currentToken_, id_)) {
    // Push a new prop onto the object node
    ZOFPropertyNode* propNode = new ZOFPropertyNode;
    propNode->id = currentToken_;
    propNode->root = objectNode->root;
    objectNode->properties[currentToken_] = propNode;

    Match(id_); ValuesList(propNode); Match(":");
  } else {
    HandleParseError(objectNode->root);
  }
}

void ZOFParser::ValuesList(ZOFPropertyNode* prop) {
  if (currentToken_ != "" && currentToken_ != ":") {
    Value(prop); ValuesList(prop);
  }
}

void ZOFParser::Value(ZOFPropertyNode* prop) {
  if (currentToken_ == "[") {
    Match("["); List(prop); Match("]");
  } else if (std::regex_match(currentToken_, id_)) {
    // Push a new string onto the prop
    ZOFString* terminal = new ZOFString;
    std::string s = currentToken_;
    s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
    terminal->value = s;
    terminal->root = prop->root;
    prop->values.push_back(terminal);

    Match(id_);
  } else if (std::regex_match(currentToken_, number_)) {
    // Push a new float onto the prop
    ZOFNumber* terminal = new ZOFNumber;
    terminal->value = std::stof(currentToken_);
    terminal->root = prop->root;
    prop->values.push_back(terminal);

    Match(number_);
  } else {
    HandleParseError(prop->root);
  }
}

void ZOFParser::List(ZOFPropertyNode* prop) {
  if (currentToken_ != "" && currentToken_ != "]") {
    if (std::regex_match(currentToken_, id_)) {
      // Push a new list of strings onto the list
      ZOFStringList* terminal = new ZOFStringList;
      std::string s = currentToken_;
      s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
      terminal->value.push_back(s);
      terminal->root = prop->root;
      prop->values.push_back(terminal);

      Match(id_); ListTail(terminal);
    } else if (std::regex_match(currentToken_, number_)) {
      // Push a new list of floats onto the list
      ZOFNumberList* terminal = new ZOFNumberList;
      terminal->value.push_back(std::stof(currentToken_));
      terminal->root = prop->root;
      prop->values.push_back(terminal);

      Match(number_); ListTail(terminal);
    } else {
      HandleParseError(prop->root);
    }
  }
}

void ZOFParser::ListTail(ZOFAbstractTerminal* terminal) {
  if (currentToken_ != "" && currentToken_ != ":" && currentToken_ != "]") {
    if (currentToken_ == ",") {
      Match(","); ListTail(terminal);
    } else if (std::regex_match(currentToken_, id_)) {
      // Push a new string onto the list of strings
      ZOFStringList* term = dynamic_cast<ZOFStringList*>(terminal);
      if (term != nullptr) {
        std::string s = currentToken_;
        s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
        term->value.push_back(s);
      }

      Match(id_), ListTail(term);
    } else if (std::regex_match(currentToken_, number_)) {
      // Push a new float onto the list of floats
      ZOFNumberList* term = dynamic_cast<ZOFNumberList*>(terminal);
      if (term != nullptr)
        term->value.push_back(std::stof(currentToken_));

      Match(number_), ListTail(term);
    } else {
      HandleParseError(terminal->root);
    }
  }
}
