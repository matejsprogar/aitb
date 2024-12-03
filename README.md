<h1>HLIB - Human-Like Intelligence Benchmark</h1>

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
Izdelati nevronsko mrežo (<em>class MojBrain</em>), ki ima dve enostavni funkcionalnosti:
</p>
  <ol>
<li>sprejme lahko vhodni signal (<em>class MojSignal</em>); in</li>
<li>napove lahko naslednji signal.</li>
  </ol>
<p>
Vsak signal sestoji iz več bitov (na primer 6 bitov v 2x3 matriki), ki predstavljajo senzorične vhode; pomen bitov ni pomemben, število bitov je poljubno. Razred Signal mora omogočati kreacijo naključnih signalov s pomočjo dveh statičnih <em>random</em> funkcij:
  </p>
  <ol type="a">
<li><em>Signal::random()</em> vrne objekt, kjer so vsi biti naključno postavljeni; in</li>
<li><em>Signal::random(Signal mask)</em> vrne objekt z naključno postavljenimi zgolj tistimi biti, ki so postavljeni tudi že v maski, ostali bodo 0.</li>
  </ol>
<p>
Dodatno mora razred Signal omogočati osnovne bitne manipulacije (binarna operatorja | in & ter unarni komplement ~) ter kopiranje in primerjavo. HLIB testi generirajo kratka (<em>SequenceLength=3</em>) zaporedja naključnih signalov, ki imajo vedno neke odvisnosti.</p>
<p>
  Predpostavka je, da počitek nevrona po proženju ("neuron refractory period") ni zgolj fiziološka nujnost, ampak tudi informacijska nujnost, zato test #7. Hkrati so vse časovne sekvence signalov kreirane s tem v mislih (<em>Human_like_intelligence_benchmark::random_sequence()</em>).
</p>

<h4>Koncepti</h4>

<p>Če nimaš C++20 prevajalnika, HLIB pričakuje naslednje funkcionalnosti v tvojih razredih <em>MojBrain</em> in <em>MojSignal</em>:</p>
<pre>
MojBrain A, B = A;
MojSignal prvi_vzorec = MojSignal::random(), drugi_vzorec = MojSignal::random(~prvi_vzorec);<br/>
  
bool enaki_mozgani = A == B;
bool enak_signal = prvi_vzorec == drugi_vzorec;
MojSignal novi_signal = prvi_vzorec | drugi_vzorec & prvi_vzorec;
MojSignal prazen_signal = MojSignal{};
MojSignal poln_signal = ~prazen_signal;<br/>

A << prvi_vzorec;
MojSignal napoved = A.predict();
</pre>

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
Trenutno so vsi testi v C++, so pa izjemno preprosti in v toliko verjamem, da ne bo težav pri njihovem razumevanju ter posledično prevajanju v jezik po tvoji izbiri. Če ne poznaš "modernega" C++ lahko ignoriraš "<em>concept</em>" in "<em>requires</em>" kodo.
</p>
