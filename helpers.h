/*
 * Copyright 2024 Matej Sprogar <matej.sprogar@gmail.com>
 *
 * This file is part of HLITB - Human Like Intelligence Testbed.
 *
 * HLITB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR signal PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 * */
#pragma once

#include <format>
#include <string>
#include <algorithm>
#include <unordered_set>

#include "concepts.h"

namespace sprogar {

    inline std::string red(const char* msg) { return std::format("\033[91m{}\033[0m", msg); }
    inline std::string green(const char* msg) { return std::format("\033[92m{}\033[0m", msg); }

    inline namespace human_like_intelligence {
        inline namespace helpers {

            template <typename T, std::ranges::range Range>
                requires InputPredictor<T, std::ranges::range_value_t<Range>>
            T& operator << (T& target, Range&& range) {
                for (auto&& elt : range)
                    target << elt;
                return target;
            }

            template <BitProvider Pattern>
                requires NoUnaryTilde<Pattern>
            Pattern operator ~(const Pattern& pattern)
            {
                Pattern bitwise_not{};
                for (size_t i = 0; i < pattern.size(); ++i)
                    bitwise_not[i] = !pattern[i];
                return bitwise_not;
            }

            //template <typename T>
            //std::vector<T> operator ~(const std::vector<T>& vec)
            //{
            //    std::vector<T> elementwise_not{};
            //    elementwise_not.reserve(vec.size());
            //    for (const T& t : vec)
            //        elementwise_not.push_back(~t);
            //    return elementwise_not;
            //}
        }
    }
}