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

namespace sprogar {
    inline namespace human_like_intelligence {

        template <typename Cortex, typename Pattern>
        concept PatternProcessor = std::regular<Cortex> and requires(Cortex cortex, const Cortex ccortex, Pattern pattern)
        {
            { cortex << pattern } -> std::convertible_to<Cortex&>;
            { ccortex.predict() } -> std::convertible_to<Pattern>;
        };

        template <typename Pattern>
        concept BitIndexable = std::equality_comparable<Pattern> and requires(Pattern pattern, const Pattern cpattern)
        {
            { pattern[size_t{}] } -> std::convertible_to<typename Pattern::reference>;
            { cpattern[size_t{}] } -> std::convertible_to<bool>;
            { cpattern.size() } -> std::convertible_to<size_t>;
        };

        template <typename T>
        concept NoUnaryTilde = requires(T t) { ~t; } == false;
    }
}