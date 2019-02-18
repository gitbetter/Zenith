//
//  ZOFParser.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include <fstream>
#include <regex>

// Forward Declarations
class ZOFTree;

// Class and Data Structure Definitions
class ZOFParser {
private:
  std::string currentToken_;
  ifstream zof_;
  regex id_ = regex("[^\d\s]\S*");
  regex number_ = regex("\d+\.?\d*");

  std::string Scan(ifstream file);
  void Match(std::string);
  void Match(regex pattern)

  void Start(ZOFTree* tree);
  void ObjectList(ZOFTree* tree);
  void Object(ZOFTree* tree);
  void PropsList(ZOFTree* tree);
  void Prop(ZOFTree* tree);
  void ValuesList(ZOFTree* tree);
  void Value(ZOFTree* tree);
  void List(ZOFTree* tree);
  void ListTail(ZOFTree* tree);

public:
    ZOFParser() { }
    ~ZOFParser() { }

    ZOFTree* Parse(std::string zofFile);

protected:

};
