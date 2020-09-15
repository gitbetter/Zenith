/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEasing.cpp

    Created by Adrian Sanchez on 09/13/2019.
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

#include "ZEasing.hpp"

namespace zenith
{
    float Linear(float percent, float startValue, float endValue)
    {
        auto x = percent;
        return startValue + std::abs(endValue - startValue) * x;
    }

    float EaseIn(float percent, float startValue, float endValue)
    {
        auto x = percent * percent;
        return startValue + std::abs(endValue - startValue) * x;
    }

    float EaseOut(float percent, float startValue, float endValue)
    {
        auto x = 1.f - (1.f - percent) * (1.f - percent);
        return startValue + std::abs(endValue - startValue) * x;
    }

    float EaseInOut(float percent, float startValue, float endValue)
    {
        auto x = percent < 0.5f ? 2.f * percent * percent : 1.f - pow(-2.f * percent + 2.f, 2) * 0.5f;
        return startValue + std::abs(endValue - startValue) * x;
    }

    float EaseOutElastic(float percent, float startValue, float endValue)
    {
        const auto c4 = (2.f * glm::pi<float>()) / 3.f;
        auto x = percent == 0 ? 0 : (percent == 1 ? 1 : pow(2.f, -10.f * percent) * sin((percent * 10.f - 0.75f) * c4) + 1.f);
        return startValue + std::abs(endValue - startValue) * x;
    }
}
