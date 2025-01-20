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
 * MERCHANTABILITY or FITNESS FOR C PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 * */
#pragma once

#include <format>

#include "concepts.h"

namespace sprogar {

    inline std::string red(const char* msg) { return std::format("\033[91m{}\033[0m", msg); }
    inline std::string green(const char* msg) { return std::format("\033[92m{}\033[0m", msg); }

    inline namespace helpers {

        template <typename T, std::ranges::range Range>
        requires InputPredictable<T, std::ranges::range_value_t<Range>>
        T& operator << (T& target, Range&& range) {
            for (auto&& elt : range)
                target << elt;
            return target;
        }

        template <RandomAccessible Pattern>
        requires NoUnaryTilde<Pattern>
        Pattern operator ~(const Pattern& pattern)
        {
            Pattern inverted{};
            for (size_t i = 0; i < pattern.size(); ++i)
                inverted[i] = !pattern[i];
            return inverted;
        }

        template<typename Pattern, typename... Off>
        requires RandomAccessible<Pattern>
        Pattern random_pattern(const Pattern& off_bits, const Off&... other_off_bits)
        {
            static thread_local std::mt19937 generator(std::random_device{}());
            static std::bernoulli_distribution bd(0.5);

            Pattern bits{};
            for (size_t i = 0; i < bits.size(); ++i)
                if (!(off_bits[i] | (false | ... | other_off_bits[i])))
                    bits[i] = bd(generator);

            return bits;
        }
    }
}
