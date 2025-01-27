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


#define ASSERT(expression) (void)((!!(expression)) || (std::cerr << red("Assertion failed ") \
	<< __FILE__ << "\nLine " << __LINE__ << ": " << #expression << std::endl, exit(-1), 0))



namespace sprogar {
    inline namespace human_like_intelligence {
        using namespace std;

        template <typename Cortex, typename Pattern, size_t SimulatedInfinity = 500>
            requires InputPredictor<Cortex, Pattern>and BitProvider<Pattern>
        class Testbed
        {
        public:
            static void run()
            {
                const time_t temporal_sequence_length = achievable_temporal_sequence_length();

                clog << "Human-like Intelligence Testbed:\n"
                    << "Conducting tests on temporal sequences of length " << temporal_sequence_length << endl << endl;

                for (const auto& test : testbed)
                    test(temporal_sequence_length);

                clog << green("PASS") << endl << endl;
            }

        private:
            using time_t = size_t;

            static time_t achievable_temporal_sequence_length()
            {
                for (time_t length = 2; length < SimulatedInfinity; ++length) {
                    Cortex C;
                    const vector<Pattern> sequence = circular_random_temporal_sequence(length);
                    if (!adapt(C, sequence))
                        return length - 1;
                }
                return SimulatedInfinity;
            }

            // Each bit is set randomly unless explicitly required to remain off.
            template<std::same_as<Pattern>... Patterns>
            static Pattern random_pattern(const Patterns&... off)
            {
                static thread_local std::mt19937 generator{ std::random_device{}() };
                static std::bernoulli_distribution bd(0.5);

                Pattern bits{};
                for (size_t i = 0; i < bits.size(); ++i)
                    if (!(false | ... | off[i]))
                        bits[i] = bd(generator);

                return bits;
            }

            // #7: A temporal sequence incorporates an absolute refractory period following each spike.
            static vector<Pattern> random_temporal_sequence(time_t length)
            {
                assert(length > 0);
                vector<Pattern> seq;
                seq.reserve(length);

                seq.push_back(random_pattern());
                while (seq.size() < length)
                    seq.push_back(random_pattern(seq.back()));              // see #7

                return seq;
            }
            static vector<Pattern> circular_random_temporal_sequence(time_t circle_length)
            {
                assert(circle_length > 1);
                vector<Pattern> seq = random_temporal_sequence(circle_length);

                seq.pop_back();
                seq.push_back(random_pattern(seq.back(), seq.front()));    // circular stream; #7

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
            static time_t time_to_adapt(Cortex& C, const vector<Pattern>& experience)
            {
                for (time_t time = 0; time < SimulatedInfinity; time += experience.size()) {
                    if (predict(C, experience) == experience)
                        return time;
                }
                return SimulatedInfinity;
            }
            static bool adapt(Cortex& C, const vector<Pattern>& experience)
            {
                return time_to_adapt(C, experience) < SimulatedInfinity;
            }

            static inline const vector<void (*)(time_t)> testbed =
            {
                [](time_t) {
                    clog << "#1 Knowledgeless start (No bias.)\n";

                    Cortex C;

                    ASSERT(C == Cortex{});
                    ASSERT(C.predict() == Pattern{});
                },
                [](time_t) {
                    clog << "#2 Information (Input creates bias.)\n";

                    Cortex C;
                    C << random_pattern();

                    ASSERT(C != Cortex{});
                },
                [](time_t) {
                    clog << "#3 Determinism (Equal state implies equal life.)\n";
                    const vector<Pattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << life;
                    D << life;

                    ASSERT(C == D);
                },
                [](time_t temporal_sequence_length) {
                    clog << "#4 Observability (Equal behaviour implies equal state.)\n";
                    auto equal_behaviour = [&](Cortex& C, Cortex& D) {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            const Pattern prediction = C.predict();
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
                    clog << "#5 Time (The ordering of inputs matters.)\n";
                    const Pattern any = random_pattern(), other = ~any;

                    Cortex C, D;
                    C << any << other;
                    D << other << any;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#6 Sensitivity (Brains are chaotic systems, sensitive to initial conditions.)\n";
                    const Pattern initial_condition = random_pattern();
                    const vector<Pattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << initial_condition << life;
                    D << ~initial_condition << life;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#7 Refractory period (Each spike (1) must be followed by a no-spike (0) event.)\n";
                    const Pattern no_spikes{}, one_spike = helpers::single_random_spike<Pattern>();
                    const vector<Pattern> learnable = { one_spike, no_spikes };
                    const vector<Pattern> unlearnable = { one_spike, one_spike };

                    Cortex C, D;

                    ASSERT(adapt(C, learnable));
                    ASSERT(not adapt(D, unlearnable));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#8 Adaptable (Able to predict sequences.)\n";

                    ASSERT(temporal_sequence_length > 1);
                },
                [](time_t temporal_sequence_length) {
                    clog << "#9 Universal (Able to predict longer sequences.)\n";
                    auto learn_a_longer_sequence = [&]() -> bool {
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
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
                    clog << "#10 Ageing (You can't teach an old dog new tricks.)\n";
                    auto adaptable_forever = [&](Cortex& dog) -> bool {
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
                            vector<Pattern> new_trick = circular_random_temporal_sequence(temporal_sequence_length);
                            if (not adapt(dog, new_trick))
                                return false;
                        }
                        return true;
                    };

                    Cortex C;

                    ASSERT(not adaptable_forever(C));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#11 Content matters (Some experiences are easier to learn.)\n";
                    auto one_time = [=]() -> time_t {
                        const vector<Pattern>& experience = circular_random_temporal_sequence(temporal_sequence_length);
                        Cortex C;
                        return time_to_adapt(C, experience);
                    };
                    auto adaptation_time_can_differ = [&]() -> bool {
                        const time_t reference_time = one_time();
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
                            if (one_time() != reference_time)
                                return true;
                        }
                        return false;
                    };

                    ASSERT(adaptation_time_can_differ());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#12 Everything matters (Other experiences affect adaptation time.)\n";
                    auto adaptation_time_can_change = [&](const vector<Pattern>& sequence, time_t reference_time) -> bool {
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
                            Cortex C;
                            const vector<Pattern> other = circular_random_temporal_sequence(temporal_sequence_length);
                            C << other;
                            if (time_to_adapt(C, sequence) != reference_time)
                                return true;
                        }
                        return false;
                    };
                    const vector<Pattern> sequence = circular_random_temporal_sequence(temporal_sequence_length);

                    Cortex C;
                    time_t default_time = time_to_adapt(C, sequence);

                    ASSERT(adaptation_time_can_change(sequence, default_time));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#13 ?(??)\n";
                    const vector<Pattern> truth = circular_random_temporal_sequence(temporal_sequence_length);

                    Cortex C;
                    adapt(C, truth);
                    C << ~truth;

                    ASSERT(time_to_adapt(C, truth) <= time_to_adapt(C, ~truth));
                }
            };
        };
    }
}
