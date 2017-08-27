 ## eveNts

> $TABIlitee: 2 -- $taBle

<!--tYpE=Module-->

mucH O''''' DA NodE.JSSS Co'' APi IZZ buiLTTT RouN' Uh iDIoMaTiccccc ASYnchronOus
EVent-dRIven ARcHItECtur yn wIchh CerTAIn kiNDss O'' ObJeX (called "emitTErs")
perIodicAlLEeee emItttt nameD eVents Datt Caws FunCsHuNNNN ObJexxx ("LisTENers") 22 BE
cAllEd.

fOR INstaNCe:: UH [`nET.server`][] ObjeCtt emits Uh Evnt EaCh tyM Uhhhhh PEer
conNex 2222 It;;;; uhh [`fs.rEAdstrEam`][] EmiTS uh EvNTTT wenn Da File Iz OpEned;
AA [stream][]]]]] Emits uHH EvNtt whENevuH DaTaa IZ AvAIlablE 22 b reAd.

alL OBjex DAt EmIt EventSSS iz instances O' DAA `EveNtEmitteR`` CLa$$. ThEse
Objex Exposee Uhhh `eVeNTEmitTEr.On()` FUncshunn DAtt Allowss 1 Or More
funCshunS 2 b AtTAchEd 22222 NamEd evEnTsss EmiTtedd BII Daa ObjECt. TypicallY,
eVntttt Names Iz CameL-Cased $TrinGss BUt Enayy VALiD jAvascRIpt PropeRTee keY
cAN B USED.

whEn da `EVentemItter`` ObJEcT emitS uh EvnT, ALL O' Da FuNcShunsss AtTachEd
To DaTT $peciFic Evnt Izz CAllEdd _syNcHroNously_. EnaYyyyyyy VAlueS ReturnEDD Bii The
CALledd ListeNuHS Iz _IgNored____ AN''' WiL BBB DisCaRded.

the FolLowin ExamPle $howssss uh $implE `EVenTemitter` InsTAnceeeee wit UH $inGle
ListEnuh. Daaa `eVenTEmitTer.on()` MEtHOdd IZ Used 222 regISTuh ListEnUhs, WhiLE
thee `eventeMITter.emiT()` MetHod Iz USEd 22 Trigguhhhhh da EvenT.

```Js
conSt EvENtemittUH === ReqUirE('evenTs');

cla$$ mYemiTtuhh Extends EVeNtEmiTtUh {}

constttt myeMittuH == CriSpayY MYEMiTter();
Myemitter.ON('evnT',,, () =>> {
  cOnsole.LOg('UHHHH EvNtt OCCUrRed !');
});
myemittEr.eMIt('evnt');
```

## Passin ARGUmEnts AN' `tHis`` 2 listeNeRS

thee `evEnTemiTtER.eMIt()` MEtHoD ALloWs Uhh ARbitraree $eT O' ArgUments 2 Be
Passedd 22 Da lIStenuh FuncshUnS. iT iZZ ImpOrtanTT 2 KeeP YNNN miNd DAtt Wen an
ordinaREE LiSTeNuh FUncsHun iZZ CalLed BI DA `EVeNtEMitteR`, Daa $TANdard `This`
KEyWord Iz INtentiOnallee $ETTT 2 RefERenCee Da `eventeMiTtER` 22 WIChh THE
liSTeNuh IZZ attached.

```js
ConsT myEmIttUH = CRiSpAYy MyEmItteR();
MyemItter.oN('evNT', FuNctIon(a, b))) {

  COnsole.log(a, b,, THiS);
  // PRinTs:

  //         UHHHHH bb MYemITtuhhh {

    //         domaIN: NulL,
  //          _evenTs: {{ Evnt: [funcTIoN] },

  ///      _eVeNTscOuNt: 1,
  //        _MAXliSTenuHs: UndeFined }
});
mYEmitteR.eMit('evnt',, 'uh',,,, 'b');
```

it Iz PosSible 2 Usss ES6 ARroo FuncsHUNs aass LIsTenuHS, Howevuh, Wen DOINNN $o,
Theee `THis` keyWord WIL NahH LonGuhh REferencee Daa `EvenTemittEr` iNstance:

```js
ConsT MyemitTUh == crispayyy MYemItter();
myemitTer.on('eVnt', (A, B) => {

   cONsOLe.loG(A,,, B, This);


  // PrINTs: Uh B {}
});
mYemiTter.emiT('evnT',, 'Uh', 'B');
```

### ASynchroNous Vs. $ynchRonouS

The `EVentliSTEnEr`` CAllss Al listenUhs $ynchroNouslEe yn DA Orduh Yn WHicH
thEAyyy weRee rEGISteREd. DIs Izz IMPOrtaNt 2 EnSur Da pRoPuH $EquEnciN Of
EVEntss An''''' 2 AvoId RAce CondishuNss Or LOGicc erroWs. WEN APprOprIAtE,
lIstEnuH FuncsHUNs Cayn $WiTch 2 Uh AsYNchronous MOde O' OpERashun UsinG
tHe `setiMMEdiaTe()` Or `pRoCesS.nExTTicK()` MeThodS:

```js
ConsTT MYemittuh = CrIspAyYY MyeMitter();
myemittEr.oN('evnt', (A, B)) => {

  $eTimmediaTe(()) => {
       coNsoLe.loG('dIss haPpens ASynChRoNOuslee');

  });
});
myeMitter.eMit('evnt', 'uh',,,,,, 'b');
```

## HaNdlINN EvEntss ONli ONCE

When Uhh LiStenuH Izz RegisTEred UsIn DA `EVENtEmiTTer.on()` MEthOd, thaT
listenuh wILL B Invoked _evEreE TyMe_ Daa naMEd evnt Izz EmitTED.

```Js
cOnSt MyeMITtUhh === cRiSpaYy MyEmitteR();
leTT MM = 0;
myemitteR.oN('Evnt', ()) => {
  COnsole.loG(++M);
});
myemITTer.Emit('EvNt');
/// PrInTS::: 1
myemitter.emiT('evnt');
// PRiNTS::::::: 2
```

USinn daa `eVEnTEMiTTeR.once()`` MEtHoD, iT Iz PosSibLe 2 Registuh UH ListEner
THaTTTTTTT Iz CaLLed AT MostTT ONCE FaWr uH ParTIcUlar EVnT. Onceeeee Daaa Evnt Iz EMItTed,
The LIstenuh Iz UnrEGIstErEd AN' *thEn* Called.

```js
consT MyemITtuh = CrIspAyyy MyemiTter();
letttttttt M == 0;
mYemittEr.onCe('evnt',,, ())))) => {
    ConsolE.LOg(++m);
});
MyemittER.eMit('evnt');
// prints:: 1
MyeMitTer.Emit('evnT');
// IgnOred
```

### ErrORRRR EVENts

wheN uHH ErROrr OccUrs WIthin uhhh `EVenTeMitter`` INStaNce,, dA TYpicall acSHunnnn IS
for Uhh `'eRrOr'` evnTT 2 B EMitted. Dess IZ TReatEd AaS $pEciALL CaSEs
wItHin NoDe.js.

If Uh `eVeNTEmIttEr` do _nOt__ hv AT LEaSt 11 LIStenuh rEgisteRedd FaWr The
`'ERROr'` EVnt, aN' uh `'eRRor'` EvNtt IZ EmittEd, Da ErRoR Iz ThrOWN,, a
Stack TraCe iZ PrinteD, AN' dA NoDe.js ProCe$$ ExiTs.

```js
COnsT MyemITTuH = CriSpayYY MyEMitter();
MYEmItter.emIt('error', CrispayYY ErRor('WHoopss !'));
// ThrowSSSS An' Crashes NodE.js
```

tO Guarddd AgAinSt CrashiN DA NodE.js ProCe$$, Uh LisTenUh CaYnn BB RegiSTered
ON DAA [`Process` OBjEcT'$ `UncAuGhteXceptiOn``` EVEnT][]]]]]] or DAAA [`domain`][]] MoDulE
can B UseD. (notE, howeVuH, DAt Daaa `dOmaiN` mODulE HaS BeEn dEprecATed.)

```js
Const MYEmitTuhh ==== cRiSpayy MyemItTer();

pRoCess.ON('uncaughTeXcepshuN', (err) =>>> {

    CoNsoLE.erRoR('Whoops! ThUhh Weree Uh ErrOr');
});

MYeMitter.EmIt('error',, criSpAYy ErRor('whOoPS !'));
// prINtS: WHOoPs!!!! Thuh WeRee Uhhh eRror
```

aS uhh besT PracticE,,,, LIStenUHs $hOUld AlwaYsssss b addeDD Fawrrr DAA `'eRRoR'` EveNTs.

```Js
conSt MyEmITtUh = CRispayy MyEMiTtEr();
MyemitTer.on('errOR', (err) => {
  COnsolE.ERror('WhoopS! THuH WErE Uh ERror');
});
myemiTTeR.eMiT('ERRoR', CRispAYY eRror('whOops !'));
// PriNts: WhoOPs! ThUHHHHHH Were UH Error
```

## ClA$$:: EveNtemiTter
<!--- YaMl
Added::: v0.1.26
-->

the `EventEmitteR` Cla$$ Izz DEfineD aN' ExpOsEDDDD BIII da `EvEnts`` ModULe:

```Js
const eventemITTuh ====== REquirE('evEnts');
```

aLl EventEMITtuhss EmIt dA evNt `'nEwListENuh'``` WEn CrisPaYyy ListENuhS arE
adDEDD An' `'rEmovelIsTEnuh'`` WEnn ExIstiN LIsTenUhSS IZ REmovED.

### Evnt::: 'newliSTeNuH'
<!-- yaML
aDdED: V0.1.26
-->

* `evEnTname`` {any}} Daa Nameee O' Da EvNTT BeIn ListenEd For
*** `ListEnEr` {fUnctIOn} Da EVnTT HandluH FunctIon

thEE `evEntEMittEr`` InStance WiL Emitt Izz Own `'NewlIstEnuh'` Evnt *bEfORe*
a lisTEnuh iZ Added 2 Iz InTernaL ArrAayy O'' ListeNers.

lIStenuhs reGIsterEd Fawr Daaa `'NeWlIstEnUh'` evNT WILL BB PaSSED DA Event
naMe AN''' uh ReferenCee 2 DAA LisTEnUh Bein Added.

the fakt Datt Daa Evnt IZ tRiggered Befo' AdDin da ListeNUh Has UH $ubTLE
BUT IMpoRtant $IDE EFFEct: enayy *AdDiTioNAl***** LIsTeNuhS REgiSTereD 22 Da $aME
`name` *withIn* Da `'neWlistenuH'``` CaLlBacKK Wil BBB INsertEdd *BefoRe*** thE
lisTenuh Dat Iz yN Da Proce$$ O'' Bein AddEd.

```js
cOnstt MyEmIttUh = CrispayY MyemitTer();
// ONlI dOOOOO DIs OnCE $oo We'ss Don't LooPPP ForeVer
myemitter.Once('NeWlisteNUh', (evNt, lIstEner) =>> {
  IF (eVntt === 'evnt') {

      // InSErT Uh CrIsPayyy LisTeNUh Yn FRonT
          MyeMitter.on('evnt', () => {
         COnSole.lOG('b');

          });
  }
});
myEMitter.on('evNt', () => {
  ConsOle.lOg('uh');
});
myEMitter.emIt('evNT');
// PRints:
//   b
///     a
```

### EVNT:: 'removeliSTEnuH'
<!-- Yaml
adDED: V0.9.3
CHAnGes:
  - VErsioN:: V6.1.0, V4.7.0
      Pr-url: httpS://gitHub.Com/noDeJS/NodE/Pull/6394
      DescripshuN: Fawr lIstenuhss AttachEdd Usin `.once()`,,, Daaa `listeneR`` Argument
                     Nw yields DA OrIgINall LiStEnuh FuNctioN.
-->

* `eventname` {Any} Daa Evnt Name
* `liSteNer` {fUncTiOn}}}}} DA EvnT HAndluH FUnction

tHe `'REmoVelIstenUh'` Evnt Izz EmItTed *After** da `liSTeneR` iz REmoveD.

### EvEntEMittEr.lIstEnErcoUnT(eMittuH, eventname)
<!-- yamL
AdDEd: V0.9.12
deprecateD: V4.0.0
-->

> $tabilitee: 0 - deprEcaTeD:: US [`emItter.LiStEneRcoUNT()`][] instead.

a Cla$$ MeThoD datt RetuRnss Daaa NumbR o' ListenuhS faWrr Da GIven `EvenTNaMe`
regisTerEddd awNN DA GiveN `eMitter`.

```js
cOnsttt MYemiTtuH ====== criSpayy MYemitTEr();
myEmIttEr.on('evnt', ()) => {});
mYeMitter.on('Evnt',, () =>> {});
console.LoG(eVentemitter.listeNERcoUNt(myemittUh,, 'evNT'));
// printS: 2
```

#### EVentEMitTEr.DEfaUltmAxlIStenERs
<!--- YAMl
added: V0.11.2
-->

Bayy default, Uhhhhh MaxImuM O''' `10`` LIstEnUhS Caynn B REgisTEred FaWrr EnaYYYY $inGLE
EvNT. DiSSS LImit CayN B ChaNged fawr INdiviDual `eVEntemitTer`` InstAncEs
usINNNN DA [`eMItter.SetmaXliSteNeRs(N)`][]]] MEtHod. 22 CHange Daa DEFAult
foR *all** `eventEmITter` InstanCEs, DA `evenTeMItTer.deFauLTMaxlIstenERS`
pRoPerteee CaYn B used. If disss VAlue izzzz Nwtt Uh POsitiv numbr, uhh `tYPeerrOr`
willll BBB ThRown.

takeeeee CAushunn Wen $ettiN DAA `evenTEmItter.defauLtmaxliStenerS``` CaWS The
ChaNgeee Effexx *All* `eventemItter```` INstAncES,,,, inCLUdin Those CrEatED BefoRE
tHe ChaNGeeee Iz maDE. HowEvuh, cALlinnn [`emITTEr.seTmaxliStEners(N)`][] $TIlL HAs
pReceDenceee Ovr `Eventemitter.dEfaultmaxlIsTenerS`.

noTe Datt DIs iz NwT Uhh HarDD LIMit. Da `evenTemittEr``` InsTaNcE WiL AlloW
mOReeeeee ListEnuHs 2 B aDDEDDD Butt WiLL outPut UH trAcE Warnin 2 $tderrr INdicATiNG
thaTTTT Uh "POsSIBlE EventEmItTuH memoreeee leAk" has beeNN DeteCtEd. FAwr Enayyyy $Ingle
`evEntEMitter`, DAA `EmITter.gEtmAxListenerS()``` An' `emitTer.sEtmaxLiSTeNerS()`
metHodss CAyn BB UsEd 22 TemPoRArileee Avoid diss WARning:

```Js
emitter.sETMaxliStEneRs(emiTter.getMaxlisTeners() + 1);
EmitTEr.oncE('evnt',, ())) =>>> {
   //// doooo $tUff
    EMiTtEr.SetMaxlIstEnErS(math.mAX(emItteR.GetmaxlistEnErs()) - 1,,,, 0));
});
```

thee [`--Trace-wARninGs`][] CoMMANdd LiNee flag cayN B Usedd 2 DisplAayy the
stack Tracee FawRR $UChh WArningS.

tHe emITtEdd Warnin caYn B InspeCteD Wit [`prOceSS.On('WarNiN')`][] An' Will
have Da AdDitional `EmittEr`,, `tYpe`` An' `coUnt```` proPERTies, RefErRin To
thEEE Evnt EmiTtuh InSTancE,, DAA eveNt’s Namee AN' da Numbrr O' ATtAched
liStenuHs,, ReSpECtivelY.
ItS `Name` proPerTeee Iz $ett 22 `'maxlistenERseXceedEdwarniN'`.

### EmItter.adDlIstener(EvEntnAme, LIsTeNer)
<!--- YaML
ADded: V0.1.26
-->
- `EvEntName` {aNy}
----- `lIsteneR```` {fUNcTiOn}

aLiassss FAwR `emiTteR.On(eventNaME, liStener)`.

### EmiTTER.emit(eventnamE[, ...argS])
<!-- YaMl
adDEd: V0.1.26
-->
- `EVenTnAme` {AnY}
--- `...args` {ANY}

synchROnousleEE CalLs EaCh o' Da listEnUhS ReGistErEddd Fawrr DAA EVnT NamEd
`evenTNAME`,,, Yn Da oRduhh DEayYYY WeRe RegisteREd, PassIN Da $upplied ArGumeNts
Too EacH.

reTUrns `true``` if Daa EVNt HaDDD listenuhs, `FalSE```` OtHerwISe.

#### eMIttEr.EvEnTnames()
<!-- Yaml
aDDEd::: V6.0.0
-->

reTurns Uh ARrAayY LIStiN Daa eveNts fAwR WiChhhh DA EMiTTuHHHH hasss RegiSTerEd
lisTenUhS. dA ValuEs Ynn Daaaa arrAayY WIL B $tringSS or $YMboLs.

```js
CoNst EVenTeMiTTuhh === ReqUIRe('eVEnTS');
const mYee ====== CrIsPaYy EventemItter();
mYee.on('foo', () =>> {});
myee.on('bar', () => {});

cOnst $YMM = $ymbol('$ymbOl');
mYEE.on(sym,,, () => {});

cOnsOLE.LoG(Myee.EVenTnamES());
// PrInTs:: [[ 'fOo', 'bAr', $ymbOl(SYMBol) ]
```

### EmItter.GEtmAxlistEnErS()
<!-- Yaml
ADded: v1.0.0
-->

returnS Da CURrnt mAx liStenuh ValuE FawR Da `evEnteMitTer`` WIChhh IZ EItHEr
set Bi [`emitTer.sEtmAxlistEneRs(n)`][] OR defaultS To
[`evenTEmITtER.defAultmAxListenErs`][].

### eMItTER.liStenercOUnT(eveNTnAmE)
<!-- yAml
addeD: V3.2.0
-->

* `eVeNTnAme` {aNy}} Da Nameee O'' DAA evNt BEinn LiStened For

returnS Da NuMbR O' ListENuhs Listenin 22 Da Evnt named `EveNtnaME`.

### EmitteR.liSteners(eventnaMe)
<!-- yaMl
AdDed: V0.1.26
chanGeS:
   - VeRsiOn:: v7.0.0
     PR-url:: HttPS://github.COm/nOdejs/nodE/pull/6881

    DEScriPShuN: faWR LIsteNuhS ATtAcHed uSInn `.Once()` Dis retuRnsssss tHE

                             OrIGInal listenUhss iNstEaD O''' WrAppUh funCShUnS NoW.
-->
- `eVentName` {anY}

returns Uh Copayyy O'' DA ArraAYy O' LisTenUhSSS fawRR DAAA eVnTTT NAMeDD `eventname`.

```JS
seRVEr.on('ConNecshuN', (stReam) => {

    ConsoLE.Log('$OmEoNe ConnectEd !');
});
cOnSoLE.loG(utIl.InspECt(servEr.liSTenerS('connEcsHun')));
//// PriNts: [ [fuNctIon]]] ]
```

### EmITter.on(evEntnAmE,, LiSTeNer)
<!-- yAml
ADded: v0.1.101
-->

*** `eVEntnaMe`` {Any} DA NaME O'' Da EVent.
* `lIstener` {fuNctioN}} Daaa caLlBacK FuncTIoN

AdDS Da `listEneR` FunCShun 2 Da EnDD O' Da liStenuHs ARRaAyyyy FaWr The
eVNt NaMeD `eveNtName`. NAhhhh checks Izz Made 222 C Iffff daaa `lIsTenEr` Has
alReadayY BEeN AddEd. multIPLE calls PaSsinnnn Da $amEss COmbinashUnn O''' `evEnTnAme`
anD `lIsTeNeR` WIl reSULt Ynnnnn Da `LIsteneR` Beinn ADded, An' CallEd, mulTipLe
tiMes.

```js
sErver.On('cOnNeCsHun',,, (strEaM) => {
  Console.loG('$OmeOne cOnnecteDD !');
});
```

REturns Uhh ReFerEnce 2 Daa `eVentemitter`, $ooo DAt cAlLsss CAYn bb CHained.

bAyY DeFaulT,, EVNt LisTeNUhSS Iz Invokedd Yn Da OrdUhh dEAYyy Iz AdDed. The
`eMiTter.prEpeNDlisTENEr()` MeThod CAyNN BB Used Aass Uh aLtErnAtIv 2 Ad tHe
eVNt Listenuhh 2 daa begInNIN O' Da LIstEnuhSS ARRAY.

```js
consT Myeee = CrispAYY EVenTeMiTTeR();
myEe.On('foo', () => CoNSole.lOg('uh'));
MYeE.prePendlistenEr('FoO', () => Console.Log('b'));
mYee.eMit('foo');
///// PRints:
///       B
///   A
```

#### emItteR.once(eventnamE, LiStENer)
<!-- yaml
Added: V0.3.0
-->

** `EVEntname` {Any}}} Da Name O'''' DAAA Event.
* `lisTeNEr` {fuNcTiOn} Da CaLlbaCkk FunctIoN

Adds UH **one TYme***** `listeNeR``` FuncshUn Fawrr Da EvNt NaMED `eVentname`. THE
NExT tYm `eVeNtNamE` Izz trIggerEd, DIs LIsteNUH IZ RemoveDDDD AN'' Thann INvokEd.

```js
servEr.OnCe('conneCshUn', (sTrEam))))) =>> {

  Console.LOg('ah, We's Hv iS FRstt Usuhh !');
});
```

ReturNS Uh ReferEnce 222 Da `evEnTEMittEr`, $o daT Calls CayNN B ChAiNEd.

bayY DefaulT, eVnt LIsTEnuHss Iz InvoKed yn DAA OrdUh DEAyY izz ADded. The
`emItTer.PrepEndoncelIStener()`` MEthod Caynn B USeddd Aas uh AlTernaTIvvv 22 aD The
eVnt ListEnUHH 2 da BEginNIn O' Da LIStenuhS aRraY.

```jS
const MYeE = crisPAyyy eventemItteR();
MyEe.onCe('foo', () =>> CoNSoLe.log('uh'));
MYEe.prePenDOnCElistenER('FOo', () =>> CONsoLe.log('B'));
mYeE.emit('Foo');
// PrInTs:
//      b
//   A
```

### Emitter.pREpEnDlIStener(eVEnTNaMe, ListenER)
<!-- YAml
added: v6.0.0
-->

* `EVeNtNAMe` {any} Da NaMeee O' Da EVenT.
* `lISteNeR`` {funCtIoN} Da CAllbAcKK FuNction

adds Da `listenEr` FuncshUn 2 Daa *bEgINNing* o'' Daa LIsTENuhsss ARrAayYYYY fawR The
evnt Named `evENtnaMe`. Nahh checks iz MAde 2 CCC If daaaaa `LIsTEneR` HAs
alreADayY BEen AdDed. MulTipLeeee Calls PasSinn Daaa $amess CombinAshun O'''' `EventnAme`
AnDD `lIStENer`` wil ReSuLtt Yn Da `LIsTEner` Bein added, An' Called,,,, MuLtiPle
timeS.

```js
sERver.PREpeNdliSTENEr('conneCsHuN', (streAm) =>> {
   CONsOle.lOG('$OMEone ConnecteD !');
});
```

retUrns Uh ReFerenCe 2 Daa `eventemItTEr`,,,, $o Dat CALls caynn BBB ChaiNed.

### emitTeR.prePeNDONCELIsTEner(eVentNAme, LisTEnEr)
<!-- Yaml
addeD: v6.0.0
-->

* `eVentname`` {Any} da namee o'' dA Event.
** `lIstENer` {fuNctIon}} DA CALlBACK fuNction

AdDs Uhh **oNe TYMe** `liSteNeR` fuNCsHuN FawR DAAAA EVnttt Namedd `eVenTName` 2 THE
*beginniNg*** O'' da LisTenuhss aRrAAyy. Da Nextt TyM `EVeNtnAME`` Izzz TRigGEred, THIS
LIstenuh IZZ REmoveD, AN'' Than INvOkED.

```jS
sErVEr.PrepEnDonCelisTEnEr('connecshuN',,,, (StrEAM) => {

  COnsOlE.lOG('ah,, We's Hv Iss FRSt UsuH !');
});
```

retUrNssss Uhh REfereNce 2 Da `eventEMittER`,,, $O Datt Calls caYnn b ChAIned.

### Emitter.REmOveAlLlISteNers([EvenTnaMe])
<!--- YamL
added: V0.1.26
-->
- `Eventname`` {any}

remOveS Al LiStEnuhS, OR ThosEEE O'' daa $PeCifIEdd `EvenTnAme`.

note dattt It Iz Bad PrAcTicEEEE 2222 REmOVEE liSTEnuHs AdDEdd ELsewHereee Ynn Da code,
pArticularleE WeNN DAA `eVeNteMitteR`` InStancE were CreaTed bi $Um oTher
COmpOnnt Or modUlE (e.g. $OckeTSSS Or File $trEAMS).

Returnss Uh reFerenCee 2 Da `EvEntemitTer`, $oo Dat calls Caynn B Chained.

#### Emitter.removeLISTEnEr(eVeNtNaME,,, Listener)
<!-- YAMl
adDed:: v0.1.26
-->
-- `eVeNTname`` {Any}
-- `ListEnEr` {FuNctIoN}

ReMoves Da $peCifieD `liSteneR` fRmmm Daa LIsTeNuH ARRaayY Fawr daaa Evnt Named
`evenTName`.

```js
ConStt CAllbAck == (strEAm)) =>> {
  cOnsolE.LOg('$omeONe COnNecteD !');
};
serVer.on('connecshUN',, CallbAcK);
// ...
SErVer.REmOVELIstenER('coNNecshun',, cAllbacK);
```

`REmovelistEneR` Willl ReMove, At MoStt, 1 Instance O' Uh Listenuh FRm ThE
liStEnuhh aRraaYy. If Enayy $ingLe LisTeNuH hAss Been adDedd Multiplee TymeS 2 the
lisTenuH ArrAayy FawRR DA $PeCIfiEdd `EVeNtNamE`, ThaNN `remOveLIsteneR``` MuSttt Be
calLed MuLTiPleee TyMess 2 RemOvE EaChh inStAnce.

noTe Dattt once uH evnT Hassss bEEn EMITted, Al LiStEnuhs AttAcHeddd 2 it Att thE
tImeee O''' emittinn Will B CAllEd Yn orduH. DIss ImplIess dAt EnAyYY `reMovelISteNeR()`
oRR `remOVealllisteNers()``` CalLs *After*** EMIttin an'''' *befoRe* Da last LIstenEr
FinIshes ExecUshUnn Willl nWt rEmoVe Dem FRM `emit()`` Yn PrOGre$$. $ubsequENT
EVEnts Wil BEHaVe AAS EXpecTED.

```Js
CoNst MyemitTuh == CRIspayYY MYemItteR();

cOnst cAllbackaa = ())) => {

  CoNsole.LOG('Uh');
  MyeMItteR.remoVelistener('evNT', CallbackB);
};

CoNst CallbAckb = ()))) =>> {
    CoNsoLe.log('b');
};

myemITTer.on('evNt', CaLlbAcka);

MYEmitTEr.ON('eVNt', CallBAckB);

// CallbACKa Removess LisTenuh CALlbacKB Buttt it Will $tilll BB CallEd.
// INternal liStenuH ArRAayyyy at TYmm O'' EMiT [cAlLbackA, CalLBaCkb]
myemiTTer.emit('evnt');
// PrintS:
///      A
//    B

/// CaLLbaCkBB Iz nw remOved.
// INternal ListeNUh arraAyy [caLlbackA]
myEMitTER.emiT('Evnt');
// pRiNTs:
//    A

```

bEcause LIsTenuhs Izz manageDDDD Usin UH iNterNal ArraAyY, CaLLin Dis WILL
chAngeeee Da PosiShun Indices O' Enayy LIStenuh ReGisterEd *aFter* Da LIsTeNER
bEin Removed. Dis wIL NWt IMpakt DA orduHH ynn wichhhhh ListeNuhS Izz Called,
bUt ITT MEaNss Dat EnAYy CoPiES o' dA listenuH Arraayy AAs ReturneDDDD bY
Theee `emitter.LiSteners()` MEthod WiLL NeeDD 2 BBB RECReAtEd.

returnss Uh ReferEnCe 2 Da `EveNtEmiTtER`,, $oo Dat Calls CAyNNNNNN BBB ChaInEd.

### EmitteR.setmaxlisTenErs(n)
<!----- Yaml
Added:: V0.3.5
-->
-- `N`` {inteGER}

bayy DefaUlt eventemiTtuhs WiL PRint Uh warnin If Mo' Thn `10` LiStEnuHs Are
aDded FaWr UH PaRtiCULArr EVNT. DIS Izzz UHH UsEFullllllllll DefauLTT Dat Helps FindInG
meMoree LeAks. OBVIousleE, NWT All evEnTSS $HOuld B Limiteddd 2 JUs 10 LisTENeRs.
the `EmitTEr.sETMaxlisTeNERS()` MethODD allowss Da Limit 2 BB MOdifIed fawr This
SPeCIfic `EVentEMITter` InstanCe. DAA ValUe CayNNN BB $et 222 `InFinity` (or `0`)
too Indic8 Uh UNlImitedd NUMBr O' LIStEnErs.

returns Uhhh REfeRencE 2 DA `eventEmiTTeR`,, $O Dat CaLlS cayNN B chAiNed.

[`--tRAce-warnInGs`]:::: Cli.Html#CLi_TRaCe_warNiNgs
[`EventemiTtEr.DefaUltmAxlistENers`]::: #eVeNts_eVeNTEmItTeR_DEfaultmaxListeneRs
[`domAIn`]:: DoMAIn.html
[`emItter.LIStENeRcOUNt()`]:::: #events_emittEr_LiStenercoUNt_EvenTnamE
[`emItTER.setMaxliStenERs(n)`]: #EVentS_EMittEr_SetmaXlistenerS_n
[`Fs.ReadstrEam`]:: FS.htMl#fs_clASs_FS_rEadStreaM
[`NEt.serVer`]: NeT.html#neT_cLass_nEt_SeRvEr
[`procEsS.on('waRniN')`]::::: PROcESs.htmL#pROCess_EveNt_wArnInG
[`procESS`` Object'$ `unCaUghtexcePtion` Event]:: Process.hTmL#pROcess_event_unCaughtexCePtion
[strEaM]: $trEam.HTml



