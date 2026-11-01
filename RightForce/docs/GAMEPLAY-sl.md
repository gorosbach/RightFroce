# RightForce — podrobna navodila in tehnične opombe

## Zagon in upravljanje

Zaženi **RightForce.prg** na PAL C64 ali v VICE. Joystick v portu 2:
smeri premikajo raketo, FIRE strelja. Na začetnem zaslonu FIRE začne igro.
SPACE sproži WARP; za ponovitev preslednico spusti in znova pritisni.
Dol + FIRE normalno premika in strelja.

Začneš s tremi življenji in tremi WARP-i. Vsake tri sestrelitve obnovijo en
WARP, do največ treh. Med WARP-om si zaščiten, ne streljaš, napadalci in
njihovi izstrelki izginejo. Med bossom in njegovo eksplozijo je WARP blokiran
in se ne porabi. Po WARP-u imaš še sekundo zaščite ob vrnitvi.

Ob izgubi življenja najprej izgubiš enega Sidewinderja; če ga nimaš, eno
stopnjo laserja. Eksplozija blokira premik,
nihanje, streljanje in WARP ter odstrani tvoje izstrelke. Zaščita po zadetku
traja 150 logičnih sličic, približno tri sekunde pri 50 Hz.

Po zadnji eksploziji se pokaže GAME OVER; približno tri sekunde pozneje se
igra vrne na splash s skladbo Task Force. Za novo igro spusti in znova
pritisni FIRE. Izhod je z resetom računalnika oziroma emulatorja.

HUD: val, štiri stopnje laserja, življenja, WARP, sekunde do bossa (180–000),
sestrelitve. Med bossom zadnje polje kaže njegov preostali oklep. Prikaz
številk se ustavi pri 999, valovi pri 99. Raketa rahlo niha in plamen migota.

## Vrste NLP in triminutni valovi

Navadni NLP prihajajo **samo z desne proti levi**, v skupinah po pet ali šest.
Vstopijo izven roba in na levi povsem odidejo. Ne obračajo se in ne streljajo.
Njihov sinus ima valovno dolžino 512 pik, približno 0,6 periode čez igralno
polje. Člani se razporedijo v tri višinske pasove.

Prvi triminutni val ima tri obdobja:

- **0:00–1:00, Scout:** klasičen krožnik, en zadetek, odmik največ osem pik.
- **1:00–2:00, Dart:** cian puščica, hitrejša, en zadetek, odmik največ štiri pike.
- **2:00–3:00, Manta:** vijolična krilata ladja z dvema točkama oklepa.

V naslednjih valovih se vrstni red vrst ciklično zamenja. Vse tri so navadni
nasprotniki in ostajajo brez strelov. Barve in hitrosti napredujejo z valovi.

Vsak peti prihod je lahko rdečkast elitni NLP s petimi točkami oklepa.
V prvih dveh valovih je hkrati dovoljen največ en, od tretjega vala naprej
največ dva. Če je omejitev dosežena, pride navaden NLP. Rdeči lahko vstopi
z leve ali desne, leti hitreje, kombinira dva sinusna odmika in ostane v
boju, dokler ni uničen. Ne morejo postopoma zasesti vseh šestih mest.

**Streljajo samo elitni NLP in boss.** Elitni NLP pred prvim strelom počaka
100–140 logičnih sličic. V prvem valu ima nato med streli 46–86 sličic
premora, v naslednjih se ta postopoma skrajša. Smer in čas streljanja
ostajata nepredvidljiva. Zadetek sproži blisk, uničenje eksplozijo.

Boss **Obsidian Core** se pojavi po **treh minutah vsakega vala**, neodvisno
od števila sestrelitev. Čas meri PAL raster, zato se zaradi počasnejšega
koraka logike ne podaljša. Števec je viden v HUD-u. Ob prihodu se preostali
NLP in njihovi izstrelki odstranijo; rdečih ni treba najprej uničiti. Raketa
dobi vsaj dve sekundi zaščite, da se umakne iz območja prihoda. Če takrat
še traja WARP, boss počaka do njegovega konca.

Boss ima obliko meduze: svetel klobuk gleda proti raketi, lovke valovijo
proti desni. Oba dela sta povezana tudi čez mejo 256 pik. Izmenjuje merjene
strele, naključne smeri in rafale ter ima 36–80 točk oklepa. Po uničenju
se začne nov val in triminutni števec se ponastavi.

## Nagrade in laser

Elitni NLP ob sestrelitvi pusti nagrado. Običajno je to ikona **L**, vsaka
tretja nagrada pa srček za dodatno življenje. Nagrado pobereš z raketo;
čez nekaj časa izgine. Življenj je lahko največ devet. Po polni nadgradnji
laserja naslednja dva L odpreta Sidewinderja. Ko sta tudi ta že oba prisotna,
L doda življenje. Če je nagrada že prisotna, se nova dodeli neposredno,
prejšnji predmet pa ostane na igralnem polju.

Laser ima osnovno stopnjo in tri nadgradnje:

1. Daljši žarek in krajši premor med streli.
2. Pet pik debel svetlo rdeč žarek z belim jedrom in dvema točkama poškodbe.
   Manta pade z enim zadetkom, rdeči NLP s tremi neposrednimi zadetki (5 → 3 → 1 → 0).
3. Prebojni žarek, ki nadaljuje skozi NLP in lahko uniči vseh šest v vrsti.

Posamezen žarek vsako tarčo zadene samo enkrat. Sestrelitve obnavljajo WARP
tudi s prebojnim laserjem. Debeli laser ostane rdeč tudi na prebojni stopnji.

### Stranska spremljevalca Sidewinder

Po tretji laserski nadgradnji naslednji L doda zgornjega, še naslednji
spodnjega Sidewinderja. Nagrade še vedno prihajajo od uničenih rdečih NLP.
Največ sta dva. Majhni cian raketici sledita tvoji raketi nekoliko zadaj,
ena zgoraj in druga spodaj. Ob robovih se njun razmik prilagodi.

Streljata z običajnim FIRE, brez novih tipk. S polno floto sta hkrati v
zraku največ dva glavna žarka, ki potujeta hitreje, in dva stranska izstrelka. Vsak spremljevalec ima največ en izstrelek
naenkrat, torej skupaj dva. Izstrelka potujeta šest pik naprej in eno piko
navzgor oziroma navzdol na korak. Povzročita eno poškodbo in se ustavita
ob prvi tarči, tudi če ima glavni laser preboj. Zadeneta lahko tudi bossa;
neuničljivi asteroid ju prestreže.

Sidewinderja ne povečata območja trka rakete. Ob izgubi življenja izgubiš
enega spremljevalca pred stopnjo glavnega laserja. Med eksplozijo in WARP-om
sta skrita in ne streljata. Eksplozija in začetek WARP-a počistita vseh pet
igralčevih izstrelkov, zato se streljanje med uničenjem ne more nadaljevati.

## Posebni WARP: Enterprise-D

Vsak **četrti uspešno sprožen WARP** traja 230 namesto 75 logičnih sličic
(približno 4,6 namesto 1,5 sekunde pri 50 Hz). Izza rakete prileti stilizirana
USS Enterprise-D, jo prehiti in izgine na desni. Števec se ponastavi ob novi
igri; neuspešen poskus pri bossu se ne šteje.

Spodaj ostane oster, stacionaren napis:

    SPACE, THE FINAL FRONTIER
       SIR PATRICK STEWART
            1940-2026

Enterprise sestavljajo trije sprite-i, vse tri vrstice napisa sedem hires
sprite-ov. Letnici sta v manjši, ostri petpikselni pisavi. Ozadje
se nadaljuje; asteroidi so med tem prizorom skriti, da ostane napis celovit.
Raketa je zaščitena. Dogodek ne porabi dodatnega WARP naboja.

Poklon se nanaša na kapitana Jean-Luca Picarda in Sira Patricka Stewarta.
Vir preverjene novice: [Associated Press](https://apnews.com/article/8b38054ff74d4187a32158d78e345ad7).

## Asteroidi in ozadje

Asteroidi uporabljajo strojne sprite-e v treh globinah:

- Majhen, temnejši in počasen: oddaljeno ozadje.
- Srednji: srednja globina in hitrost.
- Velik, svetlejši in najhitrejši: neuničljiva ovira.

Velik asteroid odvzame življenje ob trku z raketo, prestreže tudi prebojni
laser in uniči NLP, ki se vanj zaleti. Takšno uničenje ne prinese točk,
WARP naboja ali nagrade. Tudi boss lahko trči vanj. Prva dva sloja sta
okrasna. Prihodi imajo naključne zamike in višine.

Ohranjena sta spiralna galaksija z belo-rumenim jedrom in zelen kraterast
planet po priloženih referencah. Planet ima tri velikosti, teksturo,
senčeno polovico ter paleto v dvobarvnih celicah. Višino spremeni samo ob
novem prihodu. Potuje približno pol počasneje kot najpočasnejše zvezde;
ko povsem odide, počaka 150–300 logičnih sličic. Ne ovije se na drugo stran.

Zvezde imajo tri plasti s po štirimi zvezdami: temnejše enopikselne,
srednje in svetle bližnje sledi. Premikajo se izmenično po polovici na
sličico, v WARP-u se raztegnejo. Galaksija in planet sta bitmap, ostala
premikajoča se plovila, izstrelki, nagrade, asteroidi in eksplozije so sprite-i.
Splash ostaja multicolor bitmap s čistimi belimi navodili na črnem.

## Glasba

Splash: `assets/music/Task_Force.sid` — Task Force, Rob Hubbard & Jason Page, 2018 Project Hubbard.
Igra: `assets/music/Lightforce_2006.sid` — Lightforce 2006, Glenn Rune Gallefoss, 2006 Nostalgia.
Pri obeh se predvaja prva skladba. Krediti in opozorilo glede pravic so v
`MUSIC-CREDITS.md`.
Laser, WARP, eksplozija in pobrana nagrada začasno prevzamejo tretji glas SID,
nato se ta vrne glasbi. 

## Prevajanje in viri

PowerShell:

    ./scripts/build.ps1 -Compiler 'C:/pot/do/oscar64.exe'

Preverjeno z Oscar64 v1.32.273, C in 6502 ASM, brez __asm volatile.
Za običajno gradnjo zadostujejo priloženi binarni viri; Python ni potreben.
Za ponovno izdelavo grafike uporabi Python 3 s Pillow in NumPy:

    python tools/generate_assets.py
    python tools/convert_splash.py

Ta obnovi vseh 34 sprite slik, planete, meglico, pakirane planete in SID
payloade. `tools/convert_splash.py` obnovi splash iz ilustracije; v Windows uporablja
Arial Bold. `tools/c64_art.py` pretvori grafiko v dejanske dvobarvne celice VIC-II.
`tools/generate_nebula.py` in `tools/generate_planets.py` sta samostojna generatorja;
`tools/pack_planets.py` stisne planet po omejenih kosih za sprotno dekodiranje.

`src/main.c` povezuje igro in HUD; `src/gameplay.h` vsebuje formacije, orožje, nagrade,
tribute in trke; `src/encounter.h` valove in meduzo; `src/renderer.h` ASM razporejevalnik
sprite-ov, IRQ in sekundni števec; `src/hit_test.h` hiter ASM preizkus zadetkov; `src/engine.h` zvezde; `src/background.h` ozadje;
`src/sound.h` učinke; `src/ambience.h` glasbo in vire; `src/title.h` splash ter GAME OVER.
`.asm/.map/.lbl/.int` so izpisi prevajalnika v mapi `build/`.

## Tehnične opombe in preverjanje

PAL raster IRQ poganja glasbo in izris s 50 Hz. Števec minut uporablja to
prekinitev, enkrat na PAL sličico. V naslovnem zaslonu in med bossom miruje;
ob novi igri in po bossu se ponastavi. Na NTSC igra in glasba tečeta hitreje.

Seznam virtualnih sprite-ov se pripravi vsak drugi logični korak, priprava
in razporejanje sta razdeljena na izmenična koraka. IRQ vmes izračuna
horizontalni položaj med starim in novim ciljem. Višina ostaja skladna z
razporedom rastrskih vrstic, da se ponovno uporabljeni kanali ne prekrivajo.
Gibanje NLP se računa izmenično po treh; raketa, streli in izstrelki se
premikajo vsak korak. Glavni žarki in stranski izstrelki preverjajo zadetke pri 25 Hz, na
izmeničnih korakih, da se obremenitev porazdeli. Žarki se
med posnetki prekrivajo tudi pri največji hitrosti. Navpični preizkus pred
horizontalnim in pripravljene bitne maske zmanjšajo obremenitev; preverjanje
vseh tarč za žarek zdaj izvede namenska rutina 6502 ASM.

Planet se riše po dve oziroma tri vrstice vsak korak. Kos se odkodira samo
enkrat za dva koraka, paleta pa enkrat na znakovno vrstico. Tako se izris
enakomerneje razporedi med sličice.

Največ 22 virtualnih sprite-ov se razporedi na osem strojnih kanalov s
ponovno uporabo po rastrskih vrsticah. Ob prevelikem prekrivanju imajo
prednost raketa, boss, nevarni asteroid in sovražni izstrelki; okrasni
asteroidi najnižjo. Če je s spremljevalci zaslon poln, se oddaljena
okrasna asteroida začasno izpustita; nevarni asteroid ostane viden. Sedem delov napisa dobi daljši čas za preklop
kanalov, da so vse črke in letnici izrisane pred svojo rastrsko vrstico.
Skriti NLP in izstrelki ne povzročijo trka z raketo.
Ob največji gneči so še možne upočasnitve logike; izris in SID ostaneta
vezana na raster. Vse kombinacije niso zaklenjene na 50 logičnih korakov/s.

VICE 3.10 PAL: preverjeni so poln triminutni prelet do časovnega prihoda
bossa, prihod neodvisno od sestrelitev in preostalih rdečih NLP, ponastavitev
časa po bossu, tri obdobja plovil, smer z desne in odsotnost navadnih strelov.
Preverjeni so redki četrti WARP, prehitevanje Enterprise, sedem celih sprite-ov
napisa tudi ob raketi spodaj ter čist povratek. Preverjeni so še trije zadetki debelega rdečega laserja proti elitnemu NLP,
celotno zaporedje L do obeh spremljevalcev, obe smeri izstrelkov, ena poškodba
brez preboja, trki z asteroidom, izguba spremljevalca ob smrti in blokada
streljanja med eksplozijo. Preverjeni so še laserji,
nagrade, asteroidi, meduza, blokada WARP-a pri bossu, GAME OVER, obnovitev
obeh skladb in nova igra. Vse tri velikosti planeta se ujemajo z izvorno
sliko in paleto; preverjeni so prelet, čiščenje in odloženi novi prihod.

Preizkusi vključujejo pripravljena stanja v monitorju emulatorja in daljše
samodejno igranje. Vseh kombinacij ne pokrivajo. Na pravem C64 še ni preverjeno.
Diagnostika missedFrames/ticks in waveSeconds je v .lbl.

Obe skladbi uporabljata $1000. Task Force sega do $212f, igralna koda se začne
pri $2400. Pomožna koda naslovnega zaslona uporablja prosti prostor
$0880–$0fff, ločeno od zagonske kode in obeh skladb. Njegova kopija je pod I/O v RAM $cd00–$de2f. Kopiranje poteka z
izklopljenimi prekinitvami in začasno skritim I/O. Štiriintrideset sprite slik
zaseda $c400–$cc7f. V PRG so stisnjene z LZO; 2176 bajtov se razširi neposredno
v pomnilnik VIC-II ob zagonu. Zaslon je $c000, vidni bitmap $e000, kopija
ozadja brez zvezd pa $a000–$bf3f. Grafični ASM uporablja $f9–$fe.
Task Force uporablja zero page $f0–$f7, Lightforce pa ohranjeni $84–$8f in
rezervirani $90–$f3. Ne dodajaj -xz ali samodejne rabe zero page brez pregleda.
Program ne uporablja dinamičnega pomnilnika; programski sklad je 1024 bajtov.

Planeti imajo višine 40/32/32 pik. Izvorni planet.bin ima 8320 bajtov,
planet-palette.bin 1040. Sprotno se dekodira največ 50 bajtov; zapakirana
slika ima 4675 bajtov in indeks 384. Pri spremembah ohrani skladnost s
tabelami v planetSpawn() in v pack_planets.py.
