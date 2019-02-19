//
//  ZOFTree.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZOFTree.hpp"

template<> std::string ZOFNumber::ToString() {
  std::string valueString = " " + std::to_string(value);
  return valueString;
}

template<> std::string ZOFString::ToString() {
  std::string valueString = " " + value;
  return valueString;
}

template<> std::string ZOFNumberList::ToString() {
  std::string valueString = " [";
  for (float v : value) {
    valueString += std::to_string(v) + ", ";
  }
  valueString = valueString.substr(0, valueString.length() - 2);
  return valueString + "]";
}

template<> std::string ZOFStringList::ToString() {
  std::string valueString = " [";
  for (std::string v : value) {
    valueString += v + ", ";
  }
  valueString = valueString.substr(0, valueString.length() - 2);
  return valueString + "]";
}
