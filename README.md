# HLITB - Human-Like Intelligence Testbed

#### HIPOTEZA:
  _Preživeti Turingov test lahko nadomestimo z naborom kratkih testov._

GPT je sicer sposoben rešiti Turingov test, hkrati pa dela še preveč neumnosti, da bi lahko obveljal kot dokončna rešitev problema računske inteligence. Da bi vedeli, ali nam je uspelo oziroma ali smo sploh na pravi poti, rabimo novo metriko. 

Projekt HLITB vključuje 10 domnevno potrebnih pogojev za inteligenco. Verjamem, da jim lahko zadosti tudi klasičen imperativni program, vendar nas zgodovina uči, da je to slepa ulica; zanima me, ali zmore kdo ustvariti nevronsko mrežo, ki reši HLITB teste?


## Cilj
Izdelati nevronsko mrežo (_class Brain_), ki na vhodu sprejme vhodni vzorec (_class Signal_) in na izhodu uspešno napove *naslednji* vhodni vzorec. Sposobnost napovedovanja prihodnosti je preverjana s pravilnim napovedovanjem zaporedij vhodnih vzorcev, **ista** mreža pa mora biti sposobna napovedati tudi (različno dolge) vhodne sekvence.

## API
Vsak vzorec sestoji iz več bitov (na primer 6 video bitov v 2x3 matriki), ki predstavljajo senzorične vhode; pomen bitov ni pomemben, število bitov je poljubno, a predefinirano v okviru razreda _BitPattern_. Razred _BitPattern_ mora privzeto omogočati kreacijo praznega vzorca (vsi biti 0), dostop do vsakega bita v vzorcu preko operatorja [] in primerjavo vzorcev:

1. _BitPattern()_ konstruktor resetira vse bite v vzorcu (_false_);
2. _BitPattern::operator[](size_t index)_ omogoča branje/pisanje poljubnega bita v vzorcu;
3. _BitPattern_::size() const_ vrne število bitov v vzorcu.
4. operator == (const _BitPattern_& lhs, const _BitPattern_& rhs)
5. operator != (const _BitPattern_& lhs, const _BitPattern_& rhs)

### Predpostavka
Počitek nevrona po proženju (_"refractory period"_) ni zgolj fiziološka, ampak tudi informacijska nujnost. Zahteva #7 določa, da se vsak prožen bit takoj resetira, kar simulira refractory fazo v delovanju nevrona (glej _Human_like_intelligence_benchmark::random_sequence()_).


### Sintaksa

Razreda _MojBrain_ in _MojBitPattern_ morata omogočati naslednjo kodo:
<pre>
MojBrain A, B = A;
MojBitPattern prazen_vzorec;
	
assert(A == B);						// primerjava mozganov
assert(prazen_vzorec == prazen_vzorec);			// primerjava bitnih vzorcev
assert(2*3 == MojBitPattern::size());			// velikost vzorca v primeru video 2x3
// assert(prazen_vzorec[i] == false) za vsak i v intervalu [0: MojBitPattern::size()-1];

A << prazen_vzorec << prazen_vzorec;			// A << {0b000000} << {0b000000};
MojBitPattern napoved = A.predict();
</pre>

Trenutno je vsa koda C++; upam, da ne bo težav pri razumevanju ter posledično prevajanju testov v jezik po tvoji izbiri. Če ne poznaš "modernega" C++, lahko ignoriraš "_concept_" in "_requires_" kodo.


## Primer glavnega programa

<pre>
#include "hlitb.h"
class MojBitPattern {...};
class MojBrain {...};
...
int main()
{
	using Testbed = sprogar::Testbed&lt;MojBrain, MojBitPattern, 500/*SimulatedInfinity*/&gt;;
	
	Testbed::run();
	return 0;
}
</pre>


