//
//  ZOFParser.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZOFParser.hpp"
#include "ZOFTree.hpp"
#include <regex>

ZOFTree* ZOFParser::Parse(std::string zofFile) {
  ZOFTree* tree = new ZOFTree;
  if (!zofFile.empty()) {
    zof_.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      zof_.open(zofFile);

      currentToken_ = Scan();
      Start()

      zof.close();
    } catch (std::ifstream::failure e) {
      _Z("There was an error reading the .zof object file. Please consult the code.", ZERROR);
    }
  }
  return tree;
}

std::string ZOFParser::Scan() {
  std::string token;
  char c;
  while(zof.get(c)) {
    // Strip initial whitespace
    if (std::string("\n\t ").find(c) != std::string::npos) continue;
    // Ignore comments
    if (c == "#") {
      zof.ignore(numeric_limits<streamsize>::max(), '\n'); continue;
    }
    // Handle .zof single character tokens
    if (std::string(",:[]").find(c) != std::string::npos) {
      token += c; break;
    }
    token += c;
    // Break out of the loop once we have our token
    // (the following character is either whitepace or one of our single character tokens)
    if (std::string("\n\t :,[]").find(zof.peek()) != std::string::npos) break;
  }
  return token;
}

void ZOFParser::Match(std::string token) {
  if (token == currentToken_) currentToken_ = Scan(zof);
  else _Z("[ZOFParse Error]: Expected " + token + ", but instead got " + currentToken_, ZERROR);
}

void ZOFParser::Match(regex pattern) {
  if (regex_match(currentToken_, pattern)) currentToken_ = Scan(zof);
  else _Z("[ZOFParse Error]: Expected " + token + ", but instead got " + currentToken_, ZERROR);
}

// Recursive descent parser to recognize and parse .zof files
void ZOFParser::Start(ZOFTree* tree) {
  ObjectList(tree);
  Match("");
}

void ZOFParser::ObjectList(ZOFTree* tree) {
  if (currentToken_ != "") {
    Object(); ObjectList();
  }
}

void ZOFParser::Object(ZOFTree* tree) {
  if (regex_match(currentToken_, id_)) {
    Match(id_); PropsList();
  } else {
    _Z("[ZOFParse Error]: Unexpected token " + currentToken_, ZERROR);
  }
}

void ZOFParser::PropsList(ZOFTree* tree) {
  if (currentToken_ != "" && currentToken_ != "]") {
    if (currentToken_ == ":") {
      Match(":"); Prop(); PropsList();
    } else {
      ObjectList(); PropsList();
    }
  }
}

void ZOFParser::Prop(ZOFTree* tree) {
  if (regex_match(currentToken_, id_)) {
    Match(id_); ValuesList();
  } else {
    _Z("[ZOFParse Error]: Unexpected token " + currentToken_, ZERROR);
  }
}

void ZOFParser::ValuesList(ZOFTree* tree) {
  if (currentToken_ != "" && currentToken_ != "]") {
    Value(); ValuesList();
  }
}

void ZOFParser::Value(ZOFTree* tree) {
  if (currentToken_ == "[") {
    Match("["); List(); Match("]");
  } else if (regex_match(currentToken_, id_)) {
    Match(id_);
  } else if (regex_match(currentToken_, number_)) {
    Match(number_);
  } else {
    _Z("[ZOFParse Error]: Unexpected token " + currentToken_, ZERROR);
  }
}

void ZOFParser::List(ZOFTree* tree) {
  if (currentToken_ != "]") {
    if (regex_match(currentToken_, id_)) {
      Match(id_); ListTail();
    } else if (regex_match(currentToken_, number_)) {
      Match(number_); ListTail();
    } else {
      _Z("[ZOFParse Error]: Unexpected token " + currentToken_, ZERROR);
    }
  }
}

void ZOFParser::ListTail(ZOFTree* tree) {
  if (currentToken_ != "]") {
    if (currentToken_ == ",") {
      Match(","); ListTail();
    } else if (regex_match(currentToken_, id_)) {
      Match(id_), ListTail();
    } else if (regex_match(currentToken_, number_)) {
      Match(number_), ListTail();
    } else {
      _Z("[ZOFParse Error]: Unexpected token " + currentToken_, ZERROR);
    }
  }
}
