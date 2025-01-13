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
#include <vector>
#include <format>

#define ASSERT(expression) (void)((!!(expression)) || (std::cerr << red("Assertion failed") \
	<< __FILE__ << "\nLine " << __LINE__ << ": " << #expression << std::endl, exit(-1), 0))


inline std::string red(std::string_view msg) { return std::format("\033[91m{}\033[0m", msg); }
inline std::string green(std::string_view msg) { return std::format("\033[92m{}\033[0m", msg); }


namespace sprogar {
    inline namespace human_like_intelligence {
        using namespace std;

        template <typename T, ranges::range Range>
        T& operator << (T& target, Range&& range) {
            for (auto&& elt : range)
                target << elt;
            return target;
        }

        template <typename Cortex, typename Pattern>
        concept PatternProcessor = requires(Cortex cortex)
        {
            { cortex << Pattern{} } -> convertible_to<Cortex&>;
            { cortex.predict() } -> convertible_to<Pattern>;
            // axiom(const Cortex cortex) { cortex.predict(); }
        };

        template <typename Pattern>
        concept PatternManipulations = requires(const Pattern p)
        {
            { Pattern::random() } -> convertible_to<Pattern>;
            { Pattern::random(p) } -> convertible_to<Pattern>;
            { ~p } -> convertible_to<Pattern>;
            { p & p } -> convertible_to<Pattern>;
            { p | p } -> convertible_to<Pattern>;
            // axiom(Pattern mask) { Pattern::random(Pattern{}) == Pattern{}; (Pattern::random(mask) & ~mask) == Pattern{}; }
            // axiom(const Pattern p, const Pattern r) { p == p; ~~p == p; (p & p) == p; (p & r) == (r & p); (p & Pattern{}) == Pattern{}; (p | Pattern{}) == p; }
        };


        template <typename Cortex, typename Pattern, size_t SimulatedInfinity = 500>
        requires std::regular<Cortex>&& std::regular<Pattern>&& PatternProcessor<Cortex, Pattern>&& PatternManipulations<Pattern>
            class Testbed
        {
        public:
            static void verify()
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
                for (time_t tm = 1; tm < SimulatedInfinity; ++tm) {
                    Cortex C;
                    const vector<Pattern> sequence = circular_random_temporal_sequence(tm);
                    if (!adapt(C, sequence))
                        return tm-1;
                }
                return SimulatedInfinity;
            }

            /*
            * #7: Temporal signals incorporate an absolute refractory period following each spike.
            */
            static vector<Pattern> random_temporal_sequence(time_t length)
            {
                if (length == 0) return vector<Pattern>{};

                vector<Pattern> seq;
                seq.reserve(length);

                seq.push_back(Pattern::random());
                while (seq.size() < length)
                    seq.push_back(Pattern::random(~seq.back()));                // see presumption #7

                return seq;
            }
            /*
            * Enables circular processing of the same sequence of patterns.
            */
            static vector<Pattern> circular_random_temporal_sequence(time_t circle_length)
            {
                if (circle_length <= 1) return vector<Pattern>{circle_length, Pattern{}};

                vector<Pattern> seq = random_temporal_sequence(circle_length);

                seq.pop_back();
                seq.push_back(Pattern::random(~(seq.back() | seq.front())));    // circular stream; #7

                return seq;
            }
            static vector<Pattern> behaviour(Cortex& C, time_t length)
            {
                vector<Pattern> predictions;
                predictions.reserve(length);

                while (predictions.size() < length) {
                    predictions.push_back(C.predict());
                    C << predictions.back();
                }

                return predictions;
            }
            static vector<Pattern> predict(Cortex& C, const vector<Pattern>& inputs)
            {
                vector<Pattern> predictions;
                predictions.reserve(inputs.size());

                for (const Pattern& in : inputs) {
                    predictions.push_back(C.predict());
                    C << in;
                }

                return predictions;
            }
            static bool adapt(Cortex& C, const vector<Pattern>& experience)
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
                    ASSERT(C.predict() == Pattern{});
                },
                [](time_t) {
                    clog << "#2 Information (input creates bias)\n";

                    Cortex C;
                    C << Pattern::random();

                    ASSERT(C != Cortex{});
                },
                [](time_t) {
                    clog << "#3 Determinism (equal state implies equal life)\n";
                    const vector<Pattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << life;
                    D << life;

                    ASSERT(C == D);
                },
                [](time_t temporal_sequence_length) {
                    clog << "#4 Substitutability (equal behaviour implies equal state)\n";
                    const vector<Pattern> kick_off = random_temporal_sequence(temporal_sequence_length);

                    Cortex C;
                    C << kick_off;
                    Cortex D = C;

                    ASSERT(behaviour(C, SimulatedInfinity) == behaviour(D, SimulatedInfinity));
                },
                [](time_t) {
                    clog << "#5 Time (the ordering of inputs matters)\n";
                    const Pattern any = Pattern::random();

                    Cortex C, D;
                    C << any << ~any;
                    D << ~any << any;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#6 Sensitivity (brains are chaotic systems, sensitive to initial conditions)\n";
                    const Pattern initial_condition = Pattern::random();
                    const vector<Pattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << initial_condition << life;
                    D << ~initial_condition << life;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#7 Refractory period (every spike (1) must be followed by a no-spike (0) event)\n";
                    const vector<Pattern> learnable = circular_random_temporal_sequence(2);
                    const vector<Pattern> unlearnable = { learnable[0], learnable[0] };		// no refractory periods

                    Cortex C, D;

                    ASSERT(adapt(C, learnable));
                    ASSERT(not adapt(D, unlearnable) or unlearnable[0] == Pattern{});
                },
                [](time_t temporal_sequence_length) {
                    clog << "#8 Universal (predict longer sequences)\n";
                    auto learn_a_much_longer_sequence = [&](const Cortex& C) -> bool {
                        for (time_t tm{}; tm < SimulatedInfinity; ++tm) {
                            Cortex CC = C;
                            const vector<Pattern> longer_sequence = circular_random_temporal_sequence(temporal_sequence_length * 2);
                            if (adapt(CC, longer_sequence))
                                return true;
                        }
                        return false;
                    };

                    Cortex C;
                    ASSERT(learn_a_much_longer_sequence(C));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#9 Ageing (you can't teach an old dog new tricks)\n";
                    auto forever_adaptable = [&](Cortex& dog) -> bool {
                        for (time_t tm{}; tm < SimulatedInfinity; ++tm) {
                            vector<Pattern> new_trick = circular_random_temporal_sequence(temporal_sequence_length);
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
