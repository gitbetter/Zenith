/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZOFTree.cpp

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
