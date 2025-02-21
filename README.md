# AGITB - Artificial General Intelligence Testbed  

#### THESIS:  
  _AGI needs a metric._  

Large language models may seem capable of passing the Turing test, yet they still make too many mistakes to be considered the definitive solution to artificial intelligence as envisioned by A. Turing. To determine whether we have created a reasoning machine—or even if we are on the right path—we need a clear metric.  

The AGITB project defines 12 (presumably) necessary conditions for intelligence. While a classical imperative program might technically satisfy these conditions, history suggests this approach is a dead end; only a neural network-based solution holds real potential.  

## AGITB Goal  
To provide a simple yet effective set of tests that support AGI development. The well-defined AGITB criteria are designed to be both efficient and insightful, helping to identify the essential characteristics of a final AGI solution.  

## NN Solution  
A neural network that takes an input pattern _p<sub>t</sub>_ and successfully predicts the *next* input pattern _p<sub>t+1</sub>_. The core challenge lies in understanding why a particular input pattern occurs and leveraging that understanding to anticipate future patterns. If the prediction is not entirely accurate, the goal is for it to be as close as possible—ensuring the network generalizes learned patterns and forecasts the most plausible external state based on past events.  

### Assumptions  
AGITB assesses predictive ability by evaluating sequences of varying lengths {_p<sub>0</sub>, p<sub>1</sub>, ..., p<sub>k</sub>_}, which represent a temporal stream of input data into the brain. Each pattern always consists of _n_ bits (|_p_| = _n_) as transmitted by all input sensors.

AGITB simulates the behavior of the natural cortex at a structural level, where information is encoded using _spikes_. In AGITB, a set bit in a pattern represents a neural spike. Crucially, the meaning of individual bits within a pattern is not predefined and must remain undisclosed; otherwise, the system would impose explicit semantics, leading to the Symbol Grounding Problem (SGP). Instead, any necessary correlations between patterns emerge through a continuous exploratory search process.  

AGITB also assumes that a neuron's _refractory period_ after firing is not just a physiological constraint but an essential informational mechanism. Consequently, Requirement #6 mandates that any bit set in a pattern must be reset in the next time step, mirroring the refractory phase of the corresponding control neuron (see the _Testbed::random_sequence_ function): 

_p<sub>t+1</sub>[i] = 0, if p<sub>t</sub>[i] = 1_  

Example of a time sequence with 5 patterns (_n_=3) that is **not** circular:  
<pre> {{101}, {010}, {100}, {001}, {100}} </pre>  
Example of an equally long circular sequence:  
<pre> {{101}, {010}, {100}, {001}, {010}} </pre>  

## API  
The _Pattern_ class must satisfy the _std::regular<Pattern>_ C++ concept, provide access to the pattern’s bit size, and allow read/write access to each bit:  
1. _size_t Pattern::size() const_ returns the number of bits in the pattern (_n_).  
2. _bool Pattern::operator[](size_t index) const_ allows reading any bit in the pattern.  
3. _Pattern::reference Pattern::operator[](size_t index)_ allows writing to any bit in the pattern.  

In addition to satisfying the _std::regular<Cortex>_ C++ concept, the _Cortex_ class must accept a _pattern_ as input and predict the next pattern:  
1. _cortex << pattern;_ sends the pattern into the model.  
2. _cortex.predict();_ returns the predicted next pattern.  

## Example of the Main Program  

<pre>
#include "agitb.h"
class MyPattern {...};
class MyCortex {...};
...
int main()
{
	using Testbed = sprogar::AGI::Testbed&lt;MyBrain, MyPattern, 500/*SimulatedInfinity&gt;;
	
	Testbed::run();
	return 0;
}
</pre>
