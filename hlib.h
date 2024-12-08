/*
 * Copyright 2021 Matej Sprogar <matej.sprogar@gmail.com>
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


namespace sprogar
{
	using namespace std;

	template <typename T, ranges::range Range>
	T& operator << (T& target, Range&& range) {
		for (auto&& elt : range)
			target << elt;
		return target;
	}

	template <typename Brain, typename Signal>
	concept brainable = requires(Brain b)
	{
		{ b << Signal{} } -> convertible_to<Brain&>;
		{ b.predict() } -> convertible_to<Signal>;
		// axiom(const Brain b) { b.predict(); }
	};

	template <typename T>
	concept signalable = requires(const T t)
	{
		{ T::random() } -> convertible_to<T>;
		{ T::random(t) } -> convertible_to<T>;
		{ ~t } -> convertible_to<T>;
		{ t& t } -> convertible_to<T>;
		{ t | t } -> convertible_to<T>;
		// axiom(T mask) { T::random(T{}) == T{}; (T::random(mask) & ~mask) == T{}; }
		// axiom(const T p, const T r) { p == p; ~~p == p; (p & p) == p; (p & r) == (r & p); (p & T{}) == T{}; (p | T{}) == p; }
	};


	template <typename Brain, typename Signal>
	requires std::regular<Brain>
		and std::regular<Signal>
		and brainable<Brain, Signal>
		and signalable<Signal>
		class Human_like_intelligence_benchmark {
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

			static vector<Signal> random_sequence(time_t length)
			{
				if (length == 0) return vector<Signal>{};

				vector<Signal> seq;
				seq.reserve(length);

				seq.push_back(Signal::random());
				while (seq.size() < length)
					seq.push_back(Signal::random(~seq.back()));

				return seq;
			}
			static vector<Signal> cyclic_random_sequence(time_t length)
			{
				if (length <= 1) return vector<Signal>{length, Signal{}};

				vector<Signal> seq = random_sequence(length);

				// cyclic continuity from back to front
				seq.pop_back();
				seq.push_back(Signal::random(~(seq.back() | seq.front())));

				return seq;
			}
			bool equal_behaviour(Brain& A, Brain& B) const
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
			bool adapt(Brain& B, const vector<Signal>& experience) const
			{
				for (time_t time = 0; time < SimulatedInfinity; ++time) {
					bool all_predictions_correct = true;
					for (const Signal& signal : experience) {
						if (signal != B.predict())
							all_predictions_correct = false;
						B << signal;
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
					clog << "#1 Default (equal initial states)\n";

					Brain A, B;

					ASSERT(A == B);
				},
				[&]() {
					clog << "#2 Information (input creates difference)\n";

					Brain A, B;
					B << Signal::random();

					ASSERT(A != B);
				},
				[&]() {
					clog << "#3 Determinism (equal state implies equal life)\n";
					const vector<Signal> life = random_sequence(SimulatedInfinity);

					Brain A, B;
					A << life;
					B << life;

					ASSERT(A == B);
				},
				[&]() {
					clog << "#4 Cause (equal behaviour implies equal state)\n";
					const vector<Signal> kick_off = random_sequence(SequenceLength);

					Brain A;
					A << kick_off;
					Brain B = A;

					ASSERT(equal_behaviour(A, B));   // ASSERT(behaviour(A) == behaviour(B));
				},
				[&]() {
					clog << "#5 Time (inputs' ordering matters)\n";
					const Signal any = Signal::random();

					Brain A, B;
					A << any << ~any;
					B << ~any << any;

					ASSERT(A != B);		// ASSERT(not equal_predictions(A, B, SimulatedInfinity));
				},
				[&]() {
					clog << "#6 Sensitivity (brains are chaotic systems)\n";
					const Signal initial_condition = Signal::random();
					const vector<Signal> life = random_sequence(SimulatedInfinity);

					Brain A, B;
					A << initial_condition << life;
					B << ~initial_condition << life;

					ASSERT(A != B);		// ASSERT(not equal_predictions(A, B, SimulatedInfinity));
				},
					// ADAPTATION
					[&]() {
						clog << "#7 Refractory period (signal's spike (1) must be followed by a no-spike (0) event)\n";
						const Signal any = Signal::random();
						const vector<Signal> learnable = { any, Signal::random(~any) };
						const vector<Signal> unlearnable = { any, any };		// no refractory periods

						ASSERT((learnable[0] & learnable[1]) == Signal{});

						Brain A, B;

						ASSERT(adapt(A, learnable));
						ASSERT(not adapt(B, unlearnable) or any == Signal{});
					},
					[&]() {
						clog << "#8 Ground truth (establish beliefs about the world)\n";
						const vector<Signal> ground_truth = cyclic_random_sequence(SequenceLength);

						Brain B;

						ASSERT(adapt(B, ground_truth));
					},
					[&]() {
						clog << "#9 Progress (teach new tricks)\n";
						const vector<Signal> ground_truth = cyclic_random_sequence(SequenceLength),
									new_trick = cyclic_random_sequence(SequenceLength);

						Brain B;
						adapt(B, ground_truth);

						ASSERT(adapt(B, new_trick));
					},
					[&]() {
						clog << "#10 Ageing (can't teach an old dog new tricks)\n";
						auto forever_adaptable = [&](Brain& dog) -> bool {
							for (unsigned tricks = 0; tricks < SimulatedInfinity; ++tricks) {
								vector<Signal> new_trick = cyclic_random_sequence(SequenceLength);
								if (not adapt(dog, new_trick))
									return false;
							}
							return true;
						};
						const vector<Signal> first_trick = cyclic_random_sequence(SequenceLength);

						Brain B;
						adapt(B, first_trick);

						ASSERT(not forever_adaptable(B));
						ASSERT(adapt(B, first_trick));
					}
			};
	};
}
