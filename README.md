# HLITB - Human-Like Intelligence Testbed

#### HIPOTEZA:
  _Preživeti Turingov test lahko nadomestimo z naborom kratkih testov._

GPT je sicer sposoben rešiti Turingov test, hkrati pa dela še preveč neumnosti, da bi lahko obveljal kot dokončna rešitev problema računske inteligence. Da bi vedeli, ali nam je uspelo oziroma ali smo sploh na pravi poti, rabimo novo metriko. 

Projekt HLITB vključuje 10 domnevno potrebnih pogojev za inteligenco. Verjamem, da jim lahko zadosti tudi klasičen imperativni program, vendar nas zgodovina uči, da je to slepa ulica; zanima me, ali zmore kdo ustvariti nevronsko mrežo, ki reši HLITB teste?


## Cilj
Izdelati nevronsko mrežo (_class Brain_), ki na vhodu sprejme vhodni signal (_class Signal_) in na izhodu uspešno napove *naslednji* vhodni signal. Sposobnost napovedovanja prihodnosti je dosežena z zaznavo krajših (_temporal_sequence_length_) cikličnih serij vhodnih vzorcev.

## API
Vsak signal sestoji iz več bitov (na primer 6 bitov v 2x3 matriki), ki predstavljajo senzorične vhode; pomen bitov ni pomemben, število bitov je poljubno, a predefinirano v okviru razreda _Signal_. Razred _Signal_ mora omogočati kreacijo naključnih signalov s pomočjo dveh statičnih _random_ funkcij:

1. _Signal::random()_ vrne objekt, kjer so vsi biti naključno postavljeni; in
2. _Signal::random(Signal mask)_ vrne objekt z naključno postavljenimi zgolj tistimi biti, ki so postavljeni tudi že v maski, ostali bodo 0.

Dodatno mora razred Signal omogočati osnovne bitne manipulacije (binarna operatorja | in & ter unarni komplement ~) ter kopiranje in primerjavo.

### Predpostavka
Počitek nevrona po proženju (_"refractory period"_) ni zgolj fiziološka, ampak tudi informacijska nujnost. Zahteva #7 določa, da se vsak prožen bit takoj resetira, kar simulira refractory fazo v delovanju nevrona (glej _Human_like_intelligence_benchmark::random_sequence()_).




### Sintaksa

Razreda _MojBrain_ in _MojSignal_ morata omogočati naslednjo kodo:
<pre>
MojBrain A, B = A;
MojSignal prvi_vzorec = MojSignal::random(), drugi_vzorec = MojSignal::random(~prvi_vzorec);<br/>
&nbsp;
bool enaki_mozgani = A == B;
bool enak_signal = prvi_vzorec == drugi_vzorec;
MojSignal novi_signal = prvi_vzorec | drugi_vzorec & prvi_vzorec;
MojSignal prazen_signal = MojSignal{};
MojSignal poln_signal = ~prazen_signal;<br/>
&nbsp;
A << prvi_vzorec << drugi_vzorec;
MojSignal napoved = A.predict();
</pre>

Trenutno so vsi testi v C++, so pa izjemno preprosti in v toliko verjamem, da ne bo težav pri njihovem razumevanju ter posledično prevajanju v jezik po tvoji izbiri. Če ne poznaš "modernega" C++, lahko ignoriraš "_concept_" in "_requires_" kodo.


## Primer glavnega programa

<pre>
#include "hlitb.h"<br/>
class MojSignal {...};<br/>
class MojBrain {...};<br/>
...
int main()
{
	using Testbed = sprogar::Testbed&lt;MojBrain, MojSignal, 500\*SimulatedInfinity*/&gt;;
	Testbed::verify(3/*temporal_sequence_length*/);
	Testbed::verify(4/*temporal_sequence_length*/);
	return 0;
}
</pre>


