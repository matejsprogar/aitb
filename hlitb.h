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

#include "concepts.h"
#include "helpers.h"

#define ASSERT(expression) (void)((!!(expression)) || (std::cerr << red("Assertion failed") \
	<< __FILE__ << "\nLine " << __LINE__ << ": " << #expression << std::endl, exit(-1), 0))



namespace sprogar {
    inline namespace human_like_intelligence {
        using namespace std;

        template <typename Cortex, RandomAccessible Pattern, size_t SimulatedInfinity = 500>
            requires InputPredictable<Cortex, Pattern>
        class Testbed
        {
        public:
            static void run()
            {
                time_t temporal_sequence_length = max_learnable_temporal_sequence_length();
                ASSERT(temporal_sequence_length > 1);

                clog << "Human-like Intelligence Testbed:\n"
                     << "temporal_sequence_length = " << temporal_sequence_length << endl << endl;
                
                for (const auto& test : testbed)
                    test(temporal_sequence_length);

                clog << green("PASS") << endl << endl;
            }

        private:
            using time_t = size_t;
            
            static time_t max_learnable_temporal_sequence_length()
            {
                for (time_t tm = 2; tm < SimulatedInfinity; ++tm) {
                    Cortex C;
                    const vector<Pattern> sequence = circular_random_temporal_sequence(tm);
                    if (!adapt(C, sequence))
                        return tm - 1;
                }
                return SimulatedInfinity;
            }

            static Pattern random_pattern()
            {
                static const Pattern no_off_bits{};
                return helpers::random_pattern(no_off_bits);
            }

            // #7: Temporal bits incorporate an absolute refractory period following each spike.
            static vector<Pattern> random_temporal_sequence(time_t length)
            {
                assert (length > 0);
                vector<Pattern> seq;
                seq.reserve(length);

                seq.push_back(random_pattern());
                while (seq.size() < length)
                    seq.push_back(helpers::random_pattern(seq.back()));              // see #7

                return seq;
            }
            static vector<Pattern> circular_random_temporal_sequence(time_t circle_length)
            {
                assert (circle_length > 1);
                vector<Pattern> seq = random_temporal_sequence(circle_length);

                seq.pop_back();
                seq.push_back(helpers::random_pattern(seq.back(), seq.front()));    // circular stream; #7

                return seq;
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
                    C << random_pattern();

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
                    clog << "#4 Observability (equal behaviour implies equal state)\n";
                    auto equal_behaviour = [&](Cortex& C, Cortex& D) {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            const auto prediction = C.predict();
                            if (prediction != D.predict())
                                return false;
                            C << prediction;
                            D << prediction;
                        }

                        return true;
                    };
                    const vector<Pattern> kick_off = random_temporal_sequence(temporal_sequence_length);

                    Cortex C;
                    C << kick_off;
                    Cortex D = C;

                    ASSERT(equal_behaviour(C, D));
                },
                [](time_t) {
                    clog << "#5 Time (the ordering of inputs matters)\n";
                    const Pattern any = random_pattern(), other = ~any;

                    Cortex C, D;
                    C << any << other;
                    D << other << any;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#6 Sensitivity (brains are chaotic systems, sensitive to initial conditions)\n";
                    const Pattern initial_condition = random_pattern();
                    const vector<Pattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << initial_condition << life;
                    D << ~initial_condition << life;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#7 Refractory period (every spike (1) must be followed by a no-spike (0) event)\n";
                    const Pattern all_zero{}, all_ones = ~all_zero;
                    const vector<Pattern> learnable = {all_zero, all_ones };
                    const vector<Pattern> unlearnable = { all_ones, all_ones };

                    Cortex C, D;

                    ASSERT(adapt(C, learnable));
                    ASSERT(not adapt(D, unlearnable));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#8 Universal (predict longer sequences)\n";
                    auto learn_a_longer_sequence = [&]() -> bool {
                        for (time_t tm{}; tm < SimulatedInfinity; ++tm) {
                            Cortex C;
                            const vector<Pattern> longer_sequence = circular_random_temporal_sequence(temporal_sequence_length + 1);
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
