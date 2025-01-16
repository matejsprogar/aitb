# HLITB - Human-Like Intelligence Testbed

#### HIPOTEZA:
  _Preživeti Turingov test lahko nadomestimo z naborom kratkih testov._

GPT je sicer sposoben rešiti Turingov test, hkrati pa dela še preveč neumnosti, da bi lahko obveljal kot dokončna rešitev problema računske inteligence. Da bi vedeli, ali nam je uspelo oziroma ali smo sploh na pravi poti, rabimo novo metriko. 

Projekt HLITB vključuje 9 domnevno potrebnih pogojev za inteligenco. Verjamem, da jim lahko zadosti tudi klasičen imperativni program, vendar nas zgodovina uči, da je to slepa ulica; zanima me, ali zmore kdo ustvariti nevronsko mrežo, ki reši HLITB teste?


## Cilj
Izdelati nevronsko mrežo (_class Brain_), ki na vhodu sprejme vhodni vzorec (_class BitPattern_) in na izhodu uspešno napove *naslednji* vhodni vzorec. Sposobnost napovedovanja prihodnosti je preverjana s pravilnim napovedovanjem zaporedij vhodnih vzorcev, **ista** mreža pa mora biti sposobna napovedati tudi (različno dolge) vhodne sekvence. 

Vzorec je skupina bitov, ki predstavlja vhode vseh senzorjev v izbranem časovnem trenutku, zaporedje teh vzorcev pa predstavlja stream vhodnih podatkov v možgane. Pomen bitov pravzaprav ni pomemben, število bitov je poljubno majhno, a predefinirano v okviru razreda _BitPattern_. Kratke sekvence majhnih vzorcev bi morale biti večinoma naučljive, pa čeprav bi vsebovale tudi beli šum. (Domen?)

### Primer
Recimo, da en vzorec sestoji iz 6 bitov v 2x3 matriki, ki jih producira primitiven video senzor. Vzorec tako vedno vsebuje natanko 6 bitov z zalogo vrednost {_0b000000_, ..., _0b111111_}. V možgane lahko zaporedoma pošiljamo različne vzorce in temu zaporedju v času pravim časovna sekvenca (_temporal_sequence_). Glavna težava je razpoznati ponavljajoče se sekvence in nato napovedati "prihodnost". Če napoved ni točna upamo, da je vsaj čim bolj pravilna, da torej mreža generalizira naučene vzorce in napoveduje najbolj smiselno zunanje stanje glede na preteklo dogajanje.

### Predpostavka
Počitek nevrona po proženju (_"refractory period"_) ni zgolj fiziološka, ampak tudi informacijska nujnost. Zahteva #7 zato določa, da se vsak sprožen bit (nevron) v naslednjem časovne trenutku obvezno resetira; takšne sekvence simulirajo refractory faze v delovanju nevrona (glej funkcijo _Testbed::random_temporal_sequence_).

## API
Razred _BitPattern_ mora privzeto omogočati kreacijo praznega vzorca (vsi biti _off_), dostop do vsakega bita v vzorcu preko operatorja [] in primerjavo vzorcev:
1. _BitPattern()_ konstruktor resetira vse bite v vzorcu (_false_);
2. _bool BitPattern::operator[](size_t index) const_ omogoča branje poljubnega bita v vzorcu;
3. _BitPattern::reference BitPattern::operator[](size_t index)_ omogoča pisanje poljubnega bita v vzorcu;
4. _BitPattern_::size() const_ vrne število bitov v vzorcu.
5. operator == (const _BitPattern_& lhs, const _BitPattern_& rhs)
6. operator != (const _BitPattern_& lhs, const _BitPattern_& rhs)

Razred _Brain_ mora omogočati sprejem enega vzorca in napoved prihodnjega vzroca:
1. _brain << vzorec;_ pošlje vzorec v model;
2. _brain.predict();_ vrne napoved prihodnjega vzorca.




### Sintaksa

Razreda _MojBrain_ in _MojBitPattern_ morata omogočati naslednjo kodo:
<pre>
MojBrain A, B = A;
MojBitPattern prazen_vzorec;
	
assert(A == B);						// primerjava mozganov
assert(prazen_vzorec == prazen_vzorec);			// primerjava bitnih vzorcev
assert(2*3 == MojBitPattern::size());			// velikost vzorca v primeru video 2x3
// assert(prazen_vzorec[i] == false) za vsak i v intervalu [0: MojBitPattern::size()-1];
prazen_vzorec[0] = true;				// spreminjanje vzorca po bitih

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


