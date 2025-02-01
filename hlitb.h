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
                    if (!predictable(C, sequence))
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

                Pattern pattern{};
                for (size_t i = 0; i < Pattern::size(); ++i)
                    if (!(false | ... | off[i]))
                        pattern[i] = bd(generator);

                return pattern;
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

            static vector<Pattern> prediction(Cortex& C, const vector<Pattern>& input_sequence)
            {
                vector<Pattern> predictions;
                predictions.reserve(input_sequence.size());

                for (const Pattern& in : input_sequence) {
                    predictions.push_back(C.predict());
                    C << in;
                }
                return predictions;
            }
            static time_t time_to_adapt(Cortex& C, const vector<Pattern>& input_sequence)
            {
                for (time_t time = 0; time < SimulatedInfinity; time += input_sequence.size()) {
                    if (prediction(C, input_sequence) == input_sequence)
                        return time;
                }
                return SimulatedInfinity;
            }
            static bool predictable(Cortex& C, const vector<Pattern>& input_sequence)
            {
                return time_to_adapt(C, input_sequence) < SimulatedInfinity;
            }
            static vector<Pattern> behaviour(Cortex& C, time_t sequence_length = SimulatedInfinity)
            {
                vector<Pattern> predictions;
                predictions.reserve(sequence_length);

                while (predictions.size() < sequence_length) {
                    predictions.push_back(C.predict());
                    C << predictions.back();
                }
                return predictions;
            }

            static inline const vector<void (*)(time_t)> testbed =
            {
                [](time_t) {
                    clog << "#1 Knowledgeless start (Presume that the unbiased cortex outputs an empty pattern.)\n";

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
                [](time_t) {
                    clog << "#4 Time (The ordering of inputs matters.)\n";
                    const Pattern any = random_pattern(), other = ~any;

                    Cortex C, D;
                    C << any << other;
                    D << other << any;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#5 Sensitivity (Brains are chaotic systems, sensitive to initial conditions.)\n";
                    const Pattern initial_condition = random_pattern();
                    const vector<Pattern> life = random_temporal_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << initial_condition << life;
                    D << ~initial_condition << life;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#6 Refractory period (Each spike (1) must be followed by a no-spike (0) event.)\n";
                    Pattern no_spikes{}, single_spike{}; single_spike[0] = true;
                    const vector<Pattern> learnable = { single_spike, no_spikes };
                    const vector<Pattern> unlearnable = { single_spike, single_spike };

                    Cortex C, D;

                    ASSERT(predictable(C, learnable));
                    ASSERT(not predictable(D, unlearnable));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#7 Adaptable (Able to prediction sequences.)\n";

                    ASSERT(temporal_sequence_length > 1);
                },
                [](time_t temporal_sequence_length) {
                    clog << "#8 Universal (Able to prediction longer sequences.)\n";
                    auto learn_a_longer_sequence = [&]() -> bool {
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
                            Cortex C;
                            const vector<Pattern> longer_sequence = circular_random_temporal_sequence(temporal_sequence_length + 1);
                            if (predictable(C, longer_sequence))
                                return true;
                        }
                        return false;
                    };

                    ASSERT(learn_a_longer_sequence());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#9 Ageing (You can't teach an old dog new tricks.)\n";
                    auto adaptable_forever = [&](Cortex& dog) -> bool {
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
                            vector<Pattern> new_trick = circular_random_temporal_sequence(temporal_sequence_length);
                            if (not predictable(dog, new_trick))
                                return false;
                        }
                        return true;
                    };

                    Cortex C;

                    ASSERT(not adaptable_forever(C));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#10 Data (The adaptation time depends on the sequence content.)\n";
                    auto adaptation_time = [=]() -> time_t {
                        Cortex C;
                        return time_to_adapt(C, circular_random_temporal_sequence(temporal_sequence_length));
                    };
                    auto adaptation_time_can_vary = [&](const time_t reference_time) -> bool {
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
                            if (adaptation_time() != reference_time)
                                return true;
                        }
                        return false;
                    };

                    ASSERT(adaptation_time_can_vary(adaptation_time()));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#11 Cortex (The adaptation time depends on the accumulated knowledge.)\n";
                    auto adaptation_time = [=](const vector<Pattern>& sequence) -> time_t {
                        const vector<Pattern> knowledge = circular_random_temporal_sequence(temporal_sequence_length);
                        Cortex C; 
                        C << knowledge;
                        return time_to_adapt(C, sequence);
                    };
                    auto adaptation_time_can_vary = [&](const vector<Pattern>& sequence, time_t reference_time) -> bool {
                        for (size_t attempt = 0; attempt < SimulatedInfinity; ++attempt) {
                            if (adaptation_time(sequence) != reference_time)
                                return true;
                        }
                        return false;
                    };
                    const vector<Pattern> sequence = circular_random_temporal_sequence(temporal_sequence_length);

                    ASSERT(adaptation_time_can_vary(sequence, adaptation_time(sequence)));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#12 Unobservability (Different internal states can lead to identical behaviours.)\n";
                    auto repeats_forever = [=](Cortex& C, const std::vector<Pattern>& sequence)
                    {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            if (sequence != prediction(C, sequence))
                                return false;
                        }
                        return true;
                    };
                    // Null Hypothesis: "Different internal states always result in distinguishable behaviours."
                    auto counterexample = [&]() -> std::pair<Cortex, Cortex> {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            Cortex C, D;
                            C << random_temporal_sequence(temporal_sequence_length);
                            const auto target_behaviour = circular_random_temporal_sequence(temporal_sequence_length - 1);

                            if (predictable(C, target_behaviour) and repeats_forever(C, target_behaviour) and
                                predictable(D, target_behaviour) and repeats_forever(D, target_behaviour)) {
                                return { std::move(C), std::move(D) };
                            }
                        }
                        ASSERT(false);
                    };
                    auto [C, D] = counterexample();

                    ASSERT(C != D and behaviour(C) == behaviour(D));    // reject the null hypothesis
                },
            };
        };
    }
}