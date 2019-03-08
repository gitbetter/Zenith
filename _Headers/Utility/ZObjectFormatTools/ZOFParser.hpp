//
//  ZOFParser.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZOFTree.hpp"
#include <fstream>
#include <regex>

// Forward Declarations

// Class and Data Structure Definitions
class ZOFParser {
private:

  std::string currentToken_;
  std::ifstream zof_;
  std::regex id_ = std::regex("[^\\-\\d]\\S*");
  std::regex number_ = std::regex("-?\\d+\\.?\\d*");

  std::string Scan();
  void Match(std::string);
  void Match(std::regex pattern);
  void HandleParseError(ZOFNode* node);

  void Start(ZOFTree* tree);
  void ObjectList(ZOFNode* node);
  void Object(ZOFTree* tree);
  void PropsList(ZOFObjectNode* node);
  void Prop(ZOFObjectNode* node);
  void ValuesList(ZOFPropertyNode* prop);
  void Value(ZOFPropertyNode* prop);
  void List(ZOFPropertyNode* prop);
  void ListTail(ZOFAbstractTerminal* terminal);

public:

    ZOFParser() { }
    ~ZOFParser() { }

    ZOFTree* Parse(std::string zofFile);

protected:


};
