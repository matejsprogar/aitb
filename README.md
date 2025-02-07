# HLITB - Human-Like Intelligence Testbed

#### TEZA:
  _AI potrebuje metriko._

Veliki jezikovni modeli so vsaj navidezno sposobni rešiti Turingov test, hkrati pa delajo še preveč neumnosti, da bi lahko predstavljali dokončno rešitev problema umetne inteligence. Da bi vedeli, ali nam je uspelo ustvariti razumen stroj oziroma ali smo sploh na pravi poti, rabimo metriko. 

Projekt HLITB vključuje 12 (domnevno) potrebnih pogojev za inteligenco. Verjetno jim lahko zadosti tudi klasičen imperativni program, vendar nas zgodovina uči, da je to slepa ulica; potencial ima le rešitev z nevronsko mrežo.


## Cilj
Nevronska mreža _N_, ki na vhodu sprejme vhodni vzorec _P<sub>t<sub>_ in na izhodu uspešno napove *naslednji* vhodni vzorec _P<sub>t+1<sub>_. Glavna težava je razumeti, zakaj se nek vhodni vzorec pojavi, in  izkoristiti to razumevanje za napoved "prihodnosti". Če napoved ni točna upamo, da je vsaj čim bolj pravilna, da torej mreža generalizira naučene vzorce in napoveduje najbolj smiselno zunanje stanje glede na preteklo dogajanje.


HLITB preverja sposobnost napovedovanja prihodnosti s pomočjo različno dolgih zaporedij _P<sub>i</sub>,...,P<sub>i+n</sub>_, ki predstavljajo časovno sekvenco vhodnih podatkov v možgane. Posamezen vzorec sestoji iz več bitov, ki jih v trenutku _t_ posredujejo vsi vhodni senzorji. Število bitov v vzorcu je nespremenljivo. Pomen bitov za HLITB ni pomemben, ker testi ustvarijo morebiti potrebne korelacije kar s pomočjo naključja. 


### Predpostavka
Testi preverjajo obnašanje korteksa na najnižjem - fizičnem - nivoju, ki kodira informacije s pomočjo _spike_-ov. Predpostavljam, da _refractory period_ nevrona po proženju ni zgolj fiziološka, ampak tudi informacijska nujnost. Zahteva #7 zato določa, da se vsak postavljen bit (_spike_) v vzorcu v naslednjem časovnem trenutku obvezno resetira, kar simulira refractory fazo v delovanju krmilnega nevrona za ta bit (glej funkcijo _Testbed::random_sequence_).

Primer časovne sekvence s 5 vzorci, ki pa **ni** cikel: <pre> {{101}, {010}, {100}, {001}, {100}} </pre>
In še primer enako dolge sekvence, ki je hkrati tudi cikel: <pre> {{101}, {010}, {100}, {001}, {010}} </pre>

## API
Razred _Pattern_ mora privzeto omogočati kreacijo praznega vzorca (vsi biti _off_), dostop do vsakega bita v vzorcu preko operatorja [] in primerjavo vzorcev:
1. _Pattern::Pattern()_ konstruktor resetira vse bite v vzorcu (_false_);
2. _bool Pattern::operator[](size_t index) const_ omogoča branje poljubnega bita v vzorcu;
3. _Pattern::reference Pattern::operator[](size_t index)_ omogoča pisanje poljubnega bita v vzorcu;
4. _size_t Pattern_::size() const_ vrne število bitov v vzorcu.
5. bool operator == (const _Pattern_& lhs, const _Pattern_& rhs)
6. bool operator != (const _Pattern_& lhs, const _Pattern_& rhs)

Razred _Brain_ mora zadoščati _std::regular<>_ konceptu in dodatno omogočati še sprejem trenutnega ter napoved prihodnjega vzorca:
1. _brain << vzorec;_ pošlje vzorec v model;
2. _brain.predict();_ vrne napoved prihodnjega vzorca.




### Sintaksa

Razreda _MyBrain_ in _MyPattern_ morata omogočati naslednjo kodo:
<pre>
MyBrain A, B = A;
MyPattern prazen_vzorec;				// prazen_vzorec == {000}
	
assert(A == B);						// primerjava mozganov
assert(prazen_vzorec == prazen_vzorec);			// primerjava vzorcev
assert(2*3 == MyPattern::size());			// velikost vzorca v primeru video 2x3

prazen_vzorec[0] = true;				// sprememba vzorca {000} -> {001}

A << MyPattern{} << prazen_vzorec;			// A << {000} << {001};
MyPattern napoved = A.predict();
</pre>


## Primer glavnega programa

<pre>
#include "hlitb.h"
class MyPattern {...};
class MyBrain {...};
...
int main()
{
	using Testbed = sprogar::Testbed&lt;MyBrain, MyPattern, 500/*SimulatedInfinity&gt;;
	
	Testbed::run();
	return 0;
}
</pre>


