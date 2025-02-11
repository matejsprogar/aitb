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
#include <functional>

#include "concepts.h"
#include "helpers.h"


#define ASSERT(expression) (void)((!!(expression)) || (std::cerr << red("Assertion failed ") \
	<< __FILE__ << "\nLine " << __LINE__ << ": " << #expression << std::endl, exit(-1), 0))



namespace sprogar {
    inline namespace human_like_intelligence {
        using namespace std;

        template <typename Cortex, typename Pattern, size_t SimulatedInfinity = 500>
            requires InputPredictor<Cortex, Pattern> and BitProvider<Pattern>
        class Testbed
        {
        public:
            static void run()
            {
                const time_t temporal_sequence_length = achievable_sequence_length();

                clog << "Human-like Intelligence Testbed:\n"
                    << "Conducting tests on temporal sequences of length " << temporal_sequence_length << endl << endl;

                for (const auto& test : testbed)
                    test(temporal_sequence_length);

                clog << green("PASS") << endl << endl;
            }

        private:
            using time_t = size_t;
            template<typename T> using Sequence = std::vector<T>;

            static time_t achievable_sequence_length()
            {
                for (time_t length = 2; length < SimulatedInfinity; ++length) {
                    Cortex C;
                    const Sequence<Pattern> sequence = circular_random_sequence(length);
                    if (!learn(C, sequence))
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
            static Sequence<Pattern> random_sequence(time_t temporal_sequence_length)
            {
                assert(temporal_sequence_length > 0);
                Sequence<Pattern> sequence;
                sequence.reserve(temporal_sequence_length);

                sequence.push_back(random_pattern());
                while (sequence.size() < temporal_sequence_length)
                    sequence.push_back(random_pattern(sequence.back()));              // see #7

                return sequence;
            }
            static Sequence<Pattern> circular_random_sequence(time_t circle_length)
            {
                assert(circle_length > 1);
                Sequence<Pattern> sequence = random_sequence(circle_length);

                sequence.pop_back();
                sequence.push_back(random_pattern(sequence.back(), sequence.front()));    // circular stream; #7

                return sequence;
            }
            static Sequence<Pattern> any_learnable_sequence(time_t temporal_sequence_length)
            {
                while (true) {
                    Cortex C{};
                    Sequence<Pattern> sequence = circular_random_sequence(temporal_sequence_length);
                    if (learn(C, sequence))
                        return sequence;
                }
            }
            static Cortex random_cortex()
            {
                Cortex C{};
                C << random_pattern();
                return C;
            }

            static Sequence<Pattern> predict_sequence(Cortex& C, const Sequence<Pattern>& input_sequence)
            {
                Sequence<Pattern> predictions;
                predictions.reserve(input_sequence.size());

                for (const Pattern& in : input_sequence) {
                    predictions.push_back(C.predict());
                    C << in;
                }
                return predictions;
            }
            static time_t time_to_repeat(Cortex& C, const Sequence<Pattern>& input_sequence)
            {
                for (time_t time = 0; time < SimulatedInfinity; time += input_sequence.size()) {
                    if (predict_sequence(C, input_sequence) == input_sequence)
                        return time;
                }
                return SimulatedInfinity;
            }
            static bool learn(Cortex& C, const Sequence<Pattern>& input_sequence)
            {
                return time_to_repeat(C, input_sequence) < SimulatedInfinity;
            }
            static Sequence<Pattern> behaviour(Cortex& C, time_t behaviour_length = SimulatedInfinity)
            {
                Sequence<Pattern> predictions;
                predictions.reserve(behaviour_length);

                while (predictions.size() < behaviour_length) {
                    predictions.push_back(C.predict());
                    C << predictions.back();
                }
                return predictions;
            }

            static inline const std::vector<void (*)(time_t)> testbed =
            {
                [](time_t) {
                    clog << "#1 Unbiased (Truly unbiased cortices are fundamentally identical and have no Symbol Grounding Problem.)\n";

                    Cortex C;

                    ASSERT(C == Cortex{});
                },
                [](time_t) {
                    clog << "#2 Information (Input creates bias.)\n";

                    Cortex C;
                    C << random_pattern();

                    ASSERT(C != Cortex{});
                },
                [](time_t) {
                    clog << "#3 Determinism (Equal state implies equal life.)\n";
                    const Sequence<Pattern> life = random_sequence(SimulatedInfinity);

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
                    const Sequence<Pattern> life = random_sequence(SimulatedInfinity);

                    Cortex C, D;
                    C << initial_condition << life;
                    D << ~initial_condition << life;

                    ASSERT(C != D);
                },
                [](time_t) {
                    clog << "#6 Refractory period (Each spike (1) must be followed by a no-spike (0) event.)\n";
                    Pattern no_spikes{}, single_spike{}; single_spike[0] = true;
                    const Sequence<Pattern> no_consecutive_spikes = { single_spike, no_spikes };
                    const Sequence<Pattern> consecutive_spikes = { single_spike, single_spike };

                    Cortex C, D;

                    ASSERT(learn(C, no_consecutive_spikes));
                    ASSERT(not learn(D, consecutive_spikes));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#7 Universal (Brains can learn to predict also longer sequences.)\n";
                    auto learn_a_longer_sequence = [&]() -> bool {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            Cortex C;
                            const Sequence<Pattern> longer_sequence = circular_random_sequence(temporal_sequence_length + 1);
                            if (learn(C, longer_sequence))
                                return true;
                        }
                        return false;
                    };

                    ASSERT(learn_a_longer_sequence());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#8 Ageing (You can't teach an old dog new tricks.)\n";
                    auto learn_forever = [&](Cortex& dog) -> bool {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            Sequence<Pattern> new_trick = any_learnable_sequence(temporal_sequence_length);
                            if (not learn(dog, new_trick))
                                return false;
                        }
                        return true;
                    };

                    Cortex C;

                    ASSERT(not learn_forever(C));
                },
                [](time_t temporal_sequence_length) {
                    clog << "#9 Data (Learning time depends on the sequence content.)\n";
                    auto different_sequences_can_have_different_learning_times = [=]() -> bool {
                        Cortex C{};
                        const time_t default_time = time_to_repeat(C, circular_random_sequence(temporal_sequence_length));
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            Sequence<Pattern> another_sequence = circular_random_sequence(temporal_sequence_length);
                            Cortex X{};
                            time_t another_time = time_to_repeat(X, another_sequence);
                            if (default_time != another_time)
                                return true;
                        }
                        return false;
                    };
                    
                    ASSERT(different_sequences_can_have_different_learning_times());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#10 State (Learning time depends on the cortex state.)\n";
                    auto different_cortices_can_have_different_learning_times = [&]() -> bool {
                        Cortex C{};
                        const Sequence<Pattern> target_sequence = any_learnable_sequence(temporal_sequence_length);
                        const time_t default_time = time_to_repeat(C, target_sequence);
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            Cortex X = random_cortex();
                            time_t another_time = time_to_repeat(X, target_sequence);
                            if (default_time != another_time)
                                return true;
                        }
                        return false;
                    };

                    ASSERT(different_cortices_can_have_different_learning_times());
                },
                [](time_t temporal_sequence_length) {
                    clog << "#11 Advantage (Training improves predictions.)\n";
                    
                    size_t trained_score = 0, untrained_score = 0;
                    for (time_t time = 0; time < SimulatedInfinity; ++time) {
                        const Sequence<Pattern> fact = any_learnable_sequence(temporal_sequence_length);
                        const Pattern disruption = random_pattern();

                        Cortex T{}, U{};
                        learn(T, fact);
                        T << disruption << fact;
                        U << disruption << fact;
                        trained_score += helpers::count_matching_bits(T.predict(), fact[0]);
                        untrained_score += helpers::count_matching_bits(U.predict(), fact[0]);
                    }

                    ASSERT(trained_score > untrained_score);
                },
                [](time_t temporal_sequence_length) {
                    clog << "#12 Unobservability (Different internal states can lead to identical behaviours.)\n";
                    auto forever = [=](Cortex& C, const Sequence<Pattern>& sequence) {
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            if (sequence != predict_sequence(C, sequence))
                                return false;
                        }
                        return true;
                    };
                    // Null Hypothesis: "Different internal states *always* result in distinguishable behaviours."
                    std::function<std::pair<Cortex, Cortex>(time_t)> counterexample = [&](time_t length) -> std::pair<Cortex, Cortex> {
                        ASSERT(length > 1);
                        for (time_t time = 0; time < SimulatedInfinity; ++time) {
                            const Sequence<Pattern> target_behaviour = any_learnable_sequence(length);

                            Cortex C{}, R{random_cortex()};
                            learn(C, target_behaviour);
                            learn(R, target_behaviour);
                            if (forever(C, target_behaviour) and forever(R, target_behaviour))
                                return { std::move(C), std::move(R) };
                        }
                        return counterexample(length - 1);
                    };
                    auto [C, D] = counterexample(temporal_sequence_length);

                    ASSERT(C != D and behaviour(C) == behaviour(D));    // reject the null hypothesis
                }
            };
        };
    }
}