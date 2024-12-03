<h1>Human like intelligence benchmark</h1>

<h4>HIPOTEZA:</h4>
<p>
  <em>Preživeti Turingov test lahko nadomestimo z naborom kratkih testov.</em>
</p>

<p>
GPT je sicer sposoben rešiti Turingov test, hkrati pa dela še preveč neumnosti, da bi lahko obveljal kot dokončna rešitev problema računske inteligence. Da bi vedeli, ali nam je uspelo oziroma ali smo sploh na pravi poti, rabimo novo metriko. 
</p>
<p>
Projekt HLIB vključuje 10 domnevno potrebnih pogojev za inteligenco. Verjamem, da jim je mogoče zadostiti tudi s klasičnim programiranjem, vendar nas zgodovina uči, da je to slepa ulica; zanima me, ali zmore kdo ustvariti nevronsko mrežo, ki reši HLIB teste?
</p>

<h4>Cilj</h4>
<p>
Izdelati nevronsko mrežo (<em>class Brain</em>), ki ima dve enostavni funkcionalnosti:
</p>
  <ol>
<li>sprejme lahko vhodni signal (<em>class Signal</em>); in</li>
<li>napove lahko naslednji signal.</li>
  </ol>
<p>
Vsak signal je skupina bitov, ki predstavljajo senzorične vhode. Pomen bitov ni pomemben, število bitov pa je poljubno. Razred Signal mora omogočati kreacijo naključnih signalov s pomočjo statične <em>random</em> funkcije; slednja mora imeti dve obliki:
  </p>
  <ol type="a">
<li> <em>Signal::random()</em> vrne objekt, kjer so vsi biti naključno postavljeni; in</li>
<li> <em>Signal::random(Signal mask)</em> vrne objekt, kjer so naključno postavljeni zgolj tisti biti, ki so del maske.</li>
  </ol>
<p>
Dodatno mora razred Signal omogočati osnovne bitne manipulacije (binarna | in & ter unarni komplement ~). HLIB testi generirajo kratka (<em>SequenceLength=3</em>) zaporedja naključnih signalov, ki imajo vedno neke odvisnosti. 
</p>

<h4>Primer glavnega programa</h4>
<p>
<pre>
#include "hlib.h"<br/>
...<br/>
    sprogar::Human_like_intelligence_benchmark&lt;MojBrain, MojSignal&gt; hlib;
    hlib.run();
</pre>
</p>
<p>
Trenutno so vsi testi v C++, so pa izjemno preprosti in v toliko verjamem, da ne bo težav pri njihovem razumevanju ter posledično prevajanju v jezik po tvoji izbiri. Če ne poznaš "modernega" C++ lahko enostavno ignoriraš "concept" in "requires" kodo.
</p>
