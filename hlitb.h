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

#include <iostream>
#include <cassert>
#include <random>
#include <vector>
#include <format>

#define ASSERT(expression) (void)((!!(expression)) || (std::cerr << red("Assertion failed") \
	<< __FILE__ << "\nLine " << __LINE__ << ": " << #expression << std::endl, exit(-1), 0))


inline std::string red(std::string_view msg) { return std::format("\033[91m{}\033[0m", msg); }
inline std::string green(std::string_view msg) { return std::format("\033[92m{}\033[0m", msg); }


namespace sprogar {
    inline namespace human_like_intelligence {
        using namespace std;

        template <typename Cortex, typename BitPattern>
        concept PatternProcessor = requires(Cortex cortex, const Cortex ccortex, BitPattern pattern)
        {
            { cortex << pattern } -> convertible_to<Cortex&>;
            { ccortex.predict() } -> convertible_to<BitPattern>;
        };

        template <typename BitPattern>
        concept BitsetLike = requires(BitPattern pattern, const BitPattern cpattern)
        {
            { pattern[size_t{}] } -> convertible_to<typename BitPattern::reference>;
            { cpattern[size_t{}] } -> convertible_to<bool>;
            { cpattern.size() } -> convertible_to<size_t>;
        };

        template <typename T, ranges::range Range>
            requires PatternProcessor<T, std::ranges::range_value_t<Range>>
        T& operator << (T& target, Range&& range) {
            for (auto&& elt : range)
                target << elt;
            return target;
        }

        template <typename T>
        concept NoUnaryTilde = requires(T t) { ~t; } == false;

        template <BitsetLike Pattern>
        requires NoUnaryTilde<Pattern>
        Pattern operator ~(const Pattern& pattern)
        {
            Pattern inverted{};
            for (size_t i = 0; i < pattern.size(); ++i)
                inverted[i] = !pattern[i];
            return inverted;
        }


        template <typename Cortex, BitsetLike BitPattern, size_t SimulatedInfinity = 500>
            requires std::regular<Cortex> && PatternProcessor<Cortex, BitPattern> && std::equality_comparable<BitPattern>
        class Testbed
        {
        public:
            static void run()
            {
                time_t temporal_sequence_length = learnable_temporal_sequence_length();
                ASSERT(temporal_sequence_length > 1);

                clog << "Human-like Intelligence Testbed:\n"
                     << "temporal_sequence_length = " << temporal_sequence_length << endl << endl;
                
                for (auto test : testbed)
                    test(temporal_sequence_length);

                clog << green("PASS") << endl << endl;
            }

        private:
            using time_t = size_t;
            
            static time_t learnable_temporal_sequence_length()
            {
                for (time_t tm = 2; tm < SimulatedInfinity; ++tm) {
                    Cortex C;
                    const vector<BitPattern> sequence = circular_random_temporal_sequence(tm);
                    if (!adapt(C, sequence))
                        return tm-1;
                }
                return SimulatedInfinity;
            }

            template<typename... T>
            requires (std::same_as<T, BitPattern> && ...)
            static BitPattern random_pattern(const T&... off_bits)
            {
                static std::random_device rd;
                static thread_local std::mt19937 gen(rd());
                static std::bernoulli_distribution bd(0.5);
                
                BitPattern bits{};
                for (size_t i=0; i<bits.size(); ++i)
                    if (!(off_bits[i] | ...))
                        bits[i] = bd(gen);
                    
                return bits;
            }
            static BitPattern random_pattern()
            {
                static const BitPattern off_bits{};
                return random_pattern(off_bits);
            }

            // #7: Temporal signals incorporate an absolute refractory period following each spike.
            static vector<BitPattern> random_temporal_sequence(time_t length)
            {
                assert (length > 0);
                vector<BitPattern> seq;
                seq.reserve(length);

                seq.push_back(random_pattern());
                while (seq.size() < length)
                    seq.push_back(random_pattern(seq.back()));             // see #7

                return seq;
            }
            static vector<BitPattern> circular_random_temporal_sequence(time_t circle_length)
            {
                assert (circle_length > 1);
                vector<BitPattern> seq = random_temporal_sequence(circle_length);

                seq.pop_back();
                seq.push_back(random_pattern(seq.back(), seq.front()));    // circular stream; #7

                return seq;
            }


            static vector<BitPattern> behaviour(Cortex& C, time_t length)
            {
                vector<BitPattern> predictions;
                predictions.reserve(length);

                while (predictions.size() < length) {
                    predictions.push_back(C.predict());
                    C << predictions.back();
                }

                return predictions;
            }
            static vector<BitPattern> predict(Cortex& C, const vector<BitPattern>& inputs)
            {
                vector<BitPattern> predictions;
                predictions.reserve(inputs.size());

                for (const BitPattern& in : inputs) {
                    predictions.push_back(C.predict());
                    C << in;
                }

                return predictions;
            }
            static bool adapt(Cortex& C, const vector<BitPattern>& experience)
            {
                for (time_t time = 0; time < SimulatedInfinity; ++time) {
                    if (predict(C, experience) == experience)
                        return true;
                }
                return false;
            }

            static inline const vector<void (*)(time_t)> testbed =
            {
                [](time_t) {
                    clog << "#1 Knowledgeless start (no bias)\n";

                    Cortex C;

                    ASSERT(C == Cortex{});
                    ASSERT(C.predict() == BitPattern{});
                },
                [](time_t) {
                    clog << "#2 Information (input creates bias)\n";

                    Cortex C;
                    C << random_pattern();

                    ASSERT(C != Cortex{});
                },
                [](time_t) {
                    clog << "#3 Determinism (equal state implies equal life)\n";
                    const vector<BitPattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << life;
                    D << life;

                    ASSERT(C == D);
                },
                [](time_t temporal_sequence_length) {
                    clog << "#4 Substitutability (equal behaviour implies equal state)\n";
                    const vector<BitPattern> kick_off = random_temporal_sequence(temporal_sequence_length);

                    Cortex C;
                    C << kick_off;
                    Cortex D = C;

                    ASSERT(behaviour(C, SimulatedInfinity) == behaviour(D, SimulatedInfinity));
                },
                [](time_t) {
                    clog << "#5 Time (the ordering of inputs matters)\n";
                    const BitPattern any = random_pattern(), other = ~any;

                    Cortex C, D;
                    C << any << other;
                    D << other << any;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#6 Sensitivity (brains are chaotic systems, sensitive to initial conditions)\n";
                    const BitPattern initial_condition = random_pattern();
                    const vector<BitPattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << initial_condition << life;
                    D << ~initial_condition << life;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#7 Refractory period (every spike (1) must be followed by a no-spike (0) event)\n";
                    const BitPattern all_zero{}, all_ones = ~all_zero;
                    const vector<BitPattern> learnable = {all_zero, all_ones };
                    const vector<BitPattern> unlearnable = { all_ones, all_ones };

                    Cortex C, D;

                    ASSERT(adapt(C, learnable));
                    ASSERT(not adapt(D, unlearnable));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#8 Universal (predict longer sequences)\n";
                    auto learn_a_longer_sequence = [&]() -> bool {
                        for (time_t tm{}; tm < SimulatedInfinity; ++tm) {
                            Cortex C;
                            const vector<BitPattern> longer_sequence = circular_random_temporal_sequence(temporal_sequence_length + 1);
                            if (adapt(C, longer_sequence))
                                return true;
                        }
                        return false;
                    };

                    ASSERT(learn_a_longer_sequence());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#9 Ageing (you can't teach an old dog new tricks)\n";
                    auto forever_adaptable = [&](Cortex& dog) -> bool {
                        for (time_t tm{}; tm < SimulatedInfinity; ++tm) {
                            vector<BitPattern> new_trick = circular_random_temporal_sequence(temporal_sequence_length);
                            if (not adapt(dog, new_trick))
                                return false;
                        }
                        return true;
                    };

                    Cortex C;

                    ASSERT(not forever_adaptable(C));
                }
            };
        };
    }
}
