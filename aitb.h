/*
 * Copyright 2024 Matej Sprogar <matej.sprogar@gmail.com>
 * 
 * This file is part of AITB - Artificial Intelligence Testbed.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * */
#pragma once

#include <iostream>
#include <cassert>
#include <random>
#include <vector>
#include <functional>

#include "concepts.h"
#include "helpers.h"


#define ASSERT(expression) (void)((!!(expression)) || (std::cerr << red("Assertion failed ") \
	<< __FILE__ << "\nLine " << __LINE__ << ": " << #expression << std::endl, exit(-1), 0))



namespace sprogar {
    namespace AI {
        using namespace std;

        template <typename Cortex, typename Pattern, size_t SimulatedInfinity = 500>
            requires InputPredictor<Cortex, Pattern> and BitProvider<Pattern>
        class Testbed
        {
        public:
            static void run()
            {
                const time_t temporal_sequence_length = achievable_sequence_length();

                clog << "Artificial Intelligence Testbed:\n"
                     << "Conducting tests on temporal sequences of " << temporal_sequence_length << " patterns\n\n";

                for (const auto& test : testbed)
                    test(temporal_sequence_length);

                clog << green("PASS") << endl << endl;
            }

        private:
            using time_t = size_t;
            template<typename T> using TemporalSequence = std::vector<T>;

            static time_t achievable_sequence_length()
            {
                for (time_t length = 2; length < SimulatedInfinity; ++length) {
                    Cortex C;
                    const TemporalSequence<Pattern> TemporalSequence = generate_circular_random_sequence(length);
                    if (!adapt(C, TemporalSequence))
                        return length - 1;
                }
                return SimulatedInfinity;
            }

            // Each bit in the pattern is set randomly unless explicitly required to remain off.
            template<std::same_as<Pattern>... Patterns>
            static Pattern generate_random_pattern(const Patterns&... off)
            {
                static thread_local std::mt19937 generator{ std::random_device{}() };
                static std::bernoulli_distribution bd(0.5);

                Pattern pattern{};
                for (size_t i = 0; i < Pattern::size(); ++i)
                    if (!(false | ... | off[i]))
                        pattern[i] = bd(generator);

                return pattern;
            }
            static TemporalSequence<Pattern> generate_random_sequence(time_t temporal_sequence_length)
            {
                assert(temporal_sequence_length > 0);
                TemporalSequence<Pattern> TemporalSequence;
                TemporalSequence.reserve(temporal_sequence_length);

                TemporalSequence.push_back(generate_random_pattern());
                while (TemporalSequence.size() < temporal_sequence_length)
                    TemporalSequence.push_back(generate_random_pattern(TemporalSequence.back()));

                return TemporalSequence;
            }
            static TemporalSequence<Pattern> generate_circular_random_sequence(time_t circle_length)
            {
                assert(circle_length > 1);
                TemporalSequence<Pattern> TemporalSequence = generate_random_sequence(circle_length);

                TemporalSequence.pop_back();
                TemporalSequence.push_back(generate_random_pattern(TemporalSequence.back(), TemporalSequence.front()));

                return TemporalSequence;
            }
            static TemporalSequence<Pattern> generate_any_learnable_sequence(time_t temporal_sequence_length)
            {
                while (true) {
                    Cortex C{};
                    TemporalSequence<Pattern> TemporalSequence = generate_circular_random_sequence(temporal_sequence_length);
                    if (adapt(C, TemporalSequence))
                        return TemporalSequence;
                }
            }
            static Cortex generate_random_cortex(time_t random_strength)
            {
                Cortex C{};
                C << generate_random_sequence(random_strength);
                return C;
            }

            static TemporalSequence<Pattern> behaviour(Cortex& C, time_t output_size = SimulatedInfinity)
            {
                TemporalSequence<Pattern> predictions;
                predictions.reserve(output_size);

                while (predictions.size() < output_size) {
                    predictions.push_back(C.predict());
                    C << predictions.back();
                }
                return predictions;
            }
            static TemporalSequence<Pattern> behaviour(Cortex& C, const TemporalSequence<Pattern>& inputs)
            {
                TemporalSequence<Pattern> predictions;
                predictions.reserve(inputs.size());

                for (const Pattern& in : inputs) {
                    predictions.push_back(C.predict());
                    C << in;
                }
                return predictions;
            }
            static time_t time_to_repeat(Cortex& C, const TemporalSequence<Pattern>& inputs)
            {
                for (time_t time = 0; time < SimulatedInfinity; time += inputs.size()) {
                    if (behaviour(C, inputs) == inputs)
                        return time;
                }
                return SimulatedInfinity;
            }
            static bool adapt(Cortex& C, const TemporalSequence<Pattern>& inputs)
            {
                return time_to_repeat(C, inputs) < SimulatedInfinity;
            }


            static inline const std::vector<void (*)(time_t)> testbed =
            {
                [](time_t) {
                    clog << "#1 Genesis (The system starts from a truly blank state, free of bias.)\n";

                    Cortex C{};

                    ASSERT(C == Cortex{});  // Requires deep comparison in operator==
                },
                [](time_t) {
                    clog << "#2 Emergence (Bias emerges from the inputs and experiences.)\n";

                    Cortex C{};
                    C << generate_random_pattern();

                    ASSERT(C != Cortex{});
                },
                [](time_t) {
                    clog << "#3 Determinism (Equal state implies equal life.)\n";
                    const TemporalSequence<Pattern> life = generate_random_sequence(SimulatedInfinity);

                    Cortex C{}, D{};
                    C << life;
                    D << life;

                    ASSERT(C == D);
                },
                [](time_t) {
                    clog << "#4 Time (The ordering of inputs affects the system's behavior.)\n";
                    const Pattern any = generate_random_pattern(), complement = ~any;

                    Cortex C{}, D{};
                    C << any << complement;
                    D << complement << any;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#5 Sensitivity (The system behaves as a chaotic system.)\n";
                    const Pattern initial_condition = generate_random_pattern(), altered_initial_condition = helpers::mutate_one_bit(initial_condition);
                    const TemporalSequence<Pattern> life = generate_random_sequence(SimulatedInfinity);

                    Cortex C{}, D{};
                    C << initial_condition << life;
                    D << altered_initial_condition << life;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#6 RefractoryPeriod (Each spike (1) must be followed by a no-spike (0).)\n";
                    const Pattern no_spikes{}, single_spike = helpers::mutate_one_bit(no_spikes);
                    const TemporalSequence<Pattern> no_consecutive_spikes = { single_spike, no_spikes };
                    const TemporalSequence<Pattern> consecutive_spikes = { single_spike, single_spike };

                    Cortex C{}, D{};

                    ASSERT(adapt(C, no_consecutive_spikes));
                    ASSERT(not adapt(D, consecutive_spikes));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#7 Scalability (The system can adapt to predict longer sequences.)\n";
                    auto can_adapt_to_longer_sequences = [&]() -> bool {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            Cortex C{};
                            const TemporalSequence<Pattern> longer_sequence = generate_circular_random_sequence(temporal_sequence_length + 1);
                            if (adapt(C, longer_sequence))
                                return true;
                        }
                        return false;
                    };

                    ASSERT(can_adapt_to_longer_sequences());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#8 Stagnation (You can't teach an old dog new tricks.)\n";
                    auto indefinitely_adaptable = [&](Cortex& dog) -> bool {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            TemporalSequence<Pattern> new_trick = generate_any_learnable_sequence(temporal_sequence_length);
                            if (not adapt(dog, new_trick))
                                return false;
                        }
                        return true;
                    };

                    Cortex C{};

                    ASSERT(not indefinitely_adaptable(C));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#9 Input (Learning time depends on the TemporalSequence content.)\n";
                    auto learning_time_differs_across_sequences = [=]() -> bool {
                        Cortex D{};
                        const time_t default_time = time_to_repeat(D, generate_circular_random_sequence(temporal_sequence_length));
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            TemporalSequence<Pattern> random_sequence = generate_circular_random_sequence(temporal_sequence_length);
                            Cortex C{};
                            time_t random_time = time_to_repeat(C, random_sequence);
                            if (default_time != random_time)
                                return true;
                        }
                        return false;
                    };
                    
                    ASSERT(learning_time_differs_across_sequences());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#10 Experience (Learning time depends on the state of the cortex.)\n";
                    auto learning_time_differs_across_cortices = [&]() -> bool {
                        Cortex D{};
                        const TemporalSequence<Pattern> target_sequence = generate_any_learnable_sequence(temporal_sequence_length);
                        const time_t default_time = time_to_repeat(D, target_sequence);
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            Cortex R = generate_random_cortex(temporal_sequence_length);
                            time_t random_time = time_to_repeat(R, target_sequence);
                            if (default_time != random_time)
                                return true;
                        }
                        return false;
                    };

                    ASSERT(learning_time_differs_across_cortices());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#11 Advantage (The adapted model outperforms the unadapted one.)\n";
                    
                    size_t adapted_score = 0, unadapted_score = 0;
                    for (time_t time = 0; time < SimulatedInfinity; ++time) {
                        const TemporalSequence<Pattern> facts = generate_any_learnable_sequence(temporal_sequence_length);
                        const Pattern disruption = generate_random_pattern(), expectation = facts[0];

                        Cortex A{};
                        adapt(A, facts);
                        A << disruption << facts;
                        adapted_score += helpers::count_matches(A.predict(), expectation);

                        Cortex U{};
                        U << disruption << facts;
                        unadapted_score += helpers::count_matches(U.predict(), expectation);
                    }

                    ASSERT(adapted_score > unadapted_score);
                },
                [](time_t temporal_sequence_length) {
                    clog << "#12 Unobservability (Different internal states can lead to identical behaviours.)\n";
                    auto forever = [=](Cortex& C, const TemporalSequence<Pattern>& TemporalSequence) {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            if (TemporalSequence != behaviour(C, TemporalSequence.size()))
                                return false;
                        }
                        return true;
                    };
                    // Null Hypothesis: "Different internal states always lead to different behaviors."
                    std::function<std::pair<Cortex, Cortex>(time_t)> counterexample = [&](time_t length) -> std::pair<Cortex, Cortex> {
                        ASSERT(length > 1);
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            const TemporalSequence<Pattern> target_behaviour = generate_any_learnable_sequence(length);

                            Cortex C{}, R = generate_random_cortex(temporal_sequence_length);
                            adapt(C, target_behaviour);
                            adapt(R, target_behaviour);
                            if (forever(C, target_behaviour) and forever(R, target_behaviour))
                                return { std::move(C), std::move(R) };
                        }
                        // recursively call for smaller lengths if no counterexample is found
                        return counterexample(length - 1);
                    };
                    auto [C, D] = counterexample(temporal_sequence_length);

                    ASSERT(C != D and behaviour(C) == behaviour(D));    // reject the null hypothesis
                }
            };
        };
    }
}