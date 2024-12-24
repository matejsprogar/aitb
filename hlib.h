/*
 * Copyright 2024 Matej Sprogar <matej.sprogar@gmail.com>
 *
 * This file is part of HLIB - Human Like Intelligence Benchmark.
 *
 * HLIB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 * */
#pragma once

#include <functional>
#include <iostream>
#include <vector>

#define ASSERT(expression) (void)((!!(expression)) || (std::cerr << "\033[91mAssertion failed\033[0m\n" \
	<< __FILE__ << "\nLine " << __LINE__ << ": " << #expression << std::endl, exit(-1), 0))


namespace sprogar {
inline namespace benchmark {
    using namespace std;

    template <typename T, ranges::range Range>
    T& operator << (T& target, Range&& range) {
        for (auto&& elt : range)
            target << elt;
        return target;
    }

    template <typename Cortex, typename Pattern>
    concept brainable = requires(Cortex cortex)
    {
        { cortex << Pattern{} } -> convertible_to<Cortex&>;
        { cortex.predict() } -> convertible_to<Pattern>;
        // axiom(const Cortex cortex) { cortex.predict(); }
    };

    template <typename Pattern>
    concept processable = requires(const Pattern p)
    {
        { Pattern::random() } -> convertible_to<Pattern>;
        { Pattern::random(p) } -> convertible_to<Pattern>;
        { ~p } -> convertible_to<Pattern>;
        { p & p } -> convertible_to<Pattern>;
        { p | p } -> convertible_to<Pattern>;
        // axiom(Pattern mask) { Pattern::random(Pattern{}) == Pattern{}; (Pattern::random(mask) & ~mask) == Pattern{}; }
        // axiom(const Pattern p, const Pattern r) { p == p; ~~p == p; (p & p) == p; (p & r) == (r & p); (p & Pattern{}) == Pattern{}; (p | Pattern{}) == p; }
    };


    template <typename Cortex, typename Pattern>
        requires std::regular<Cortex> && std::regular<Pattern> && brainable<Cortex, Pattern> && processable<Pattern>
    class Human_like_intelligence_benchmark
    {
    public:
        Human_like_intelligence_benchmark(unsigned sequence_length = 3, unsigned simulated_infinity = 500) :
            SequenceLength(sequence_length), SimulatedInfinity(simulated_infinity)
        {
        }
        void run()
        {
            for (auto test : tests)
                test();

            clog << endl << "\033[92mPASS\033[0m" << endl;
        }

    private:
        using time_t = size_t;

        static vector<Pattern> random_sequence(time_t length)
        {
            if (length == 0) return vector<Pattern>{};

            vector<Pattern> seq;
            seq.reserve(length);

            seq.push_back(Pattern::random());
            while (seq.size() < length)
                seq.push_back(Pattern::random(~seq.back()));                // see test #7

            return seq;
        }
        static vector<Pattern> learnable_random_sequence(time_t length)
        {
            if (length <= 1) return vector<Pattern>{length, Pattern{}};

            vector<Pattern> seq = random_sequence(length);

            seq.pop_back();
            seq.push_back(Pattern::random(~(seq.back() | seq.front())));    // circular refractory periods; #7

            return seq;
        }
        bool equal_behaviour(Cortex& A, Cortex& B) const
        {
            for (time_t time = 0; time < SimulatedInfinity; ++time) {
                const auto prediction = A.predict();
                if (prediction != B.predict())
                    return false;
                A << prediction;
                B << prediction;
            }

            return true;
        }
        bool adapt(Cortex& B, const vector<Pattern>& experience) const
        {
            for (time_t time = 0; time < SimulatedInfinity; ++time) {
                bool all_predictions_correct = true;
                for (const Pattern& pattern : experience) {
                    if (all_predictions_correct and pattern != B.predict())
                        all_predictions_correct = false;
                    B << pattern;
                }
                if (all_predictions_correct)
                    return true;
            }
            return false;
        }

        const unsigned SequenceLength, SimulatedInfinity;
        const vector<std::function<void()>> tests =
        {
            [&]() {
                clog << "#1 Start (no bias)\n";

                Cortex A, B;

                ASSERT(A == B);
            },
            [&]() {
                clog << "#2 Information (input creates bias)\n";

                Cortex A, B;
                B << Pattern::random();

                ASSERT(A != B);
            },
            [&]() {
                clog << "#3 Determinism (equal state implies equal life)\n";
                const vector<Pattern> life = random_sequence(SimulatedInfinity);

                Cortex A, B;
                A << life;
                B << life;

                ASSERT(A == B);
            },
            [&]() {
                clog << "#4 Cause (equal behaviour implies equal state)\n";
                const vector<Pattern> kick_off = random_sequence(SequenceLength);

                Cortex A;
                A << kick_off;
                Cortex B = A;

                ASSERT(equal_behaviour(A, B));
            },
            [&]() {
                clog << "#5 Time (the ordering of inputs matters)\n";
                const Pattern any = Pattern::random();

                Cortex A, B;
                A << any << ~any;
                B << ~any << any;

                ASSERT(A != B);
            },
            [&]() {
                clog << "#6 Sensitivity (brains are chaotic systems, sensitive to initial conditions)\n";
                const Pattern initial_condition = Pattern::random();
                const vector<Pattern> life = random_sequence(SimulatedInfinity);

                Cortex A, B;
                A << initial_condition << life;
                B << ~initial_condition << life;

                ASSERT(A != B);
            },
            [&]() {
                clog << "#7 Refractory period (every spike (1) must be followed by a no-spike (0) event)\n";
                const vector<Pattern> learnable = learnable_random_sequence(2);
                const vector<Pattern> unlearnable = { learnable[0], learnable[0] };		// no refractory periods

                Cortex A, B;

                ASSERT(adapt(A, learnable));
                ASSERT(not adapt(B, unlearnable) or unlearnable[0] == Pattern{});
            },
            [&]() {
                clog << "#8 Ground truth (develop and establish beliefs about the world)\n";
                const vector<Pattern> ground_truth = learnable_random_sequence(SequenceLength);

                Cortex B;

                ASSERT(adapt(B, ground_truth));
            },
            [&]() {
                clog << "#9 Progress (learn new tricks)\n";
                const vector<Pattern> ground_truth = learnable_random_sequence(SequenceLength),
                                        new_trick = learnable_random_sequence(SequenceLength);

                Cortex B;
                adapt(B, ground_truth);

                ASSERT(adapt(B, new_trick));
            },
            [&]() {
                clog << "#10 Ageing (you can't teach an old dog new tricks)\n";
                auto forever_adaptable = [&](Cortex& dog) -> bool {
                    for (unsigned tricks = 0; tricks < SimulatedInfinity; ++tricks) {
                        vector<Pattern> new_trick = learnable_random_sequence(SequenceLength);
                        if (not adapt(dog, new_trick))
                            return false;
                    }
                    return true;
                };

                Cortex B;

                ASSERT(not forever_adaptable(B));
            }
        };
    };
}
}
