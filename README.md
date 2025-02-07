# HLITB - Human-Like Intelligence Testbed

#### TEZA:
  _AI potrebuje metriko._

Veliki jezikovni modeli so sicer navidezno sposobni rešiti Turingov test, a delajo še preveč neumnosti, da bi lahko predstavljali dokončno rešitev problema umetne inteligence, kot jo je dojemal A. Turing. Da bi vedeli, ali nam je uspelo ustvariti razumen stroj oziroma ali smo sploh na pravi poti, rabimo metriko. 

Projekt HLITB definira 12 (domnevno) potrebnih pogojev za inteligenco. Verjetno jim lahko zadosti tudi klasičen imperativni program, vendar nas zgodovina uči, da je to slepa ulica; potencial ima le rešitev z nevronsko mrežo.

## HLITB Cilj
Preprost in uporaben sklop testov v pomoč pri razvoju umetne inteligence. Razumljivi HLITB kriteriji nudijo hiter odgovor in identificirajo potrebne lastnosti končne AI rešitve. 

## AI Rešitev
Nevronska mreža _C_, ki na vhodu sprejme vhodni vzorec _p<sub>t<sub>_ in na izhodu uspešno napove *naslednji* vhodni vzorec _p<sub>t+1<sub>_. Glavna težava je razumeti, zakaj se nek vhodni vzorec pojavi, in  izkoristiti to razumevanje za napoved "prihodnosti". Če napoved ni točna upamo, da je vsaj čim bolj pravilna, da torej mreža generalizira naučene vzorce in napoveduje najbolj smiselno zunanje stanje glede na preteklo dogajanje.

### Predpostavke

HLITB preverja sposobnost napovedovanja prihodnosti s pomočjo različno dolgih zaporedij {_p<sub>i</sub>, p<sub>i+1</sub>, ..., p<sub>j</sub>_}, ki predstavljajo časovno sekvenco vhodnih podatkov v možgane. Vsak vzorec vedno sestoji iz _n_ bitov (|_p<sub>t</sub>_| = _n_), kot jih v trenutku _t_ pošljejo vsi vhodni senzorji. 

HLITB simulira obnašanje naravnega korteksa na najnižjem - fizičnem - nivoju, ki podatke kodira s pomočjo _spike_-ov. Spike je v HLITB predstavljen kot informacija v velikosti enega bita - to omogoča obdelavo signala, hkrati pa pomen bitov v HLITB ni in ne sme biti znan, ker bi sicer HLITB postal žrtev Symbol Grounding Problema (SGP). Morebiti potrebne korelacije med vzorci so zato pridobljene s simuliranim "neskončnim" iskanjem.

HLITB tudi predpostavlja, da _refractory period_ nevrona po proženju ni zgolj fiziološka, ampak tudi informacijska nujnost. Zahteva #7 zato določa, da se vsak postavljen bit v vzorcu v naslednjem časovnem trenutku obvezno resetira, kar simulira refractory fazo v delovanju krmilnega nevrona tega bita (glej funkcijo _Testbed::random_sequence_):

_p<sub>t+1</sub>[i] = 0, if p<sub>t</sub>[i] = 1_ 

Primer časovne sekvence s 5 vzorci (_n_=3), ki pa **ni** krožna: <pre> {{101}, {010}, {100}, {001}, {100}} </pre>
Primer enako dolge krožne sekvence: <pre> {{101}, {010}, {100}, {001}, {010}} </pre>

## API
Razred _Pattern_ mora zadoščati _std::regular<Pattern>_ konceptu, nuditi dostop do velikosti vzorca v bitih in r/w dostop do vsakega bita v vzorcu:
1. _size_t Pattern_::size() const_ vrne število bitov v vzorcu (_n_).
2. _bool Pattern::operator[](size_t index) const_ omogoča branje poljubnega bita v vzorcu;
3. _Pattern::reference Pattern::operator[](size_t index)_ omogoča pisanje poljubnega bita v vzorcu;

Razred _Cortex_ mora zadoščati _std::regular<Cortex>_ konceptu, omogočati sprejem vzorca ter napoved prihodnjega vzorca:
1. _cortex << pattern;_ pošlje vzorec v model;
2. _cortex.predict();_ vrne napoved prihodnjega vzorca.




## Primer glavnega programa

<pre>
#include "hlitb.h"
class MyPattern {...};
class MyCortex {...};
...
int main()
{
	using Testbed = sprogar::Testbed&lt;MyBrain, MyPattern, 500/*SimulatedInfinity&gt;;
	
	Testbed::run();
	return 0;
}
</pre>


