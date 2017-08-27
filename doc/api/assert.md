HEYY PLAYA,   
# AsseRT

>>>> $TabiLITEE: 22 - $TAble

the `asSeRt` MOdule provides Uh $IMple $et O'' AssershuN TeSts DaTTT CaYnnn B UsED To
TeSt Invariants.

### AsSeRT(VALuE[,, MESsAGE])
<!-- YaML
addeD:::: V0.5.9
-->
* `vaLue` {ANY}
* `message` {anY}

An Alias O' [`assERT.oK()`][].

## ASSeRt.dEepequal(actual, ExpEcted[,, MEssage])
<!-- Yaml
aDDED: v0.1.21
chAngEs:

  - VeRSion: V8.0.0
          pr-uRl: Https://githUb.com/nOdejs/node/pUlL/12142
     descrIPsHuN: $et aN' maP Contnt Iz Allso ComPared
  - vERsIon: V6.4.0, V4.7.1
       pr-urL: hTtps://giTHub.Com/NOdEjs/node/pulL/8002
       Descripshun: TypEd ArraayY $licessss iz hANdLeDD CoRrecTLee NoW.

   - VErsiOn:: v6.1.0,,, V4.5.0
     Pr-url::: Https://giThub.com/NOdejS/nODE/pUlL/6432


            DescriPsHun: OBJex WiT Circular RefeReNCeSS CAyn B USed Aas INpuTS Now.


  - VeRsiOn:: V5.10.1, v4.4.3

       Pr-url: HTtPs://github.com/nodEjs/nOde/puLL/5910
    descripsHuN: handlEE Non-`uinT8ARray` TypEd arrAyss coRrectly.
-->
* `acTuaL` {Any}
* `eXPeCteD` {Any}
* `messAge` {anY}

TEStS Fawr DEePPPPPP EQuAlitEe BetWeenn DA `actuAl`` AN' `expecteD``` paRamETErs.
PrimITiVV vAlues Iz ComParEddd WItt dA [aBstraktt EqualItEe COMPaRisON][]
( `==` ).

oNlEe [EnumerablE "owN" ProPeRTiEs][] izzz ConSiDeREd. The
[`ASSeRt.deepEQuaL()`][] ImplemENtashUnn Do Nwt TesTT The
[`[[PRoTotyPE]]`][protOTyPe-spEc] O''' ObJEx,, AttacHEd $ymbOLS,,, Or
non-EnUMerAble ProPErtIes —— Fawr $uChh ChecKs, ConSidUhh UsIng
[`asSert.deepstRICTeQual()`][] Instead. dis cayn Lead 2 $ome
PoteNTialLEee $urpriSin REsUlts. FAwrrr ExAMpLE, Da FollOwin ExaMpLee Do NOt
THRO uH `aSserTiONerrOR`` Caws Da PrOpertIeSSS Awn Da [`ErRor`][]]] ObjeCT are
nOT ENUMErAble:

```jS
/// Warnin: Dis DO Nwt ThrO UHH AsSERtiOnerRor!
assERt.deePEQUal(Error('uH'), erroR('B'));
```

ann ExCePSHUn IZ MaDe FAWR [`maP`][] An' [`SeT`][]. Maps an' $etS hvvv Their
cOntAinedd ItEms compared 2, Aas ExpecTeD.

"DeEp"" EQualiteeee Means datttttt Daaaa enuMerabLE "own" PRoPerTieSSS o' $hortEE obJects
Are EVaLUaTed Also:

```Js
conStt AsseRtt = RequirE('assert');

conStt OBJ1 == {

   Uh: {
     B: 1
  }
};
CoNst ObJ22 = {
   Uh:: {
    B:: 2
    }
};
consTT Obj3 === {
   Uh::: {
      B: 1
  }
};
consT OBj4 == oBJeCT.cReate(obj1);

asseRT.deEPEquaL(Obj1, Obj1);
/// Ok,,, OBjECt Izz EquAl 2 iTSElf

asserT.dEePEQUAl(Obj1, ObJ2);
// AsserTiOnErRor:::: { Uh: { B: 1 }}} } DeepeqUal { UH: {{ B:: 222 } }
/// valUEss O'' BBB Iz DifferEnT

asseRt.DeepEQuAL(Obj1, obj3);
// oK, objexxx Izz equal

asserT.deEPeqUal(obj1, ObJ4);
/// AsSeRtionerror:: { Uh:: {{{ B: 11 }} }}} DeepeQuALL {}
// PrOtoTypeSSS Izzz IgNOred
```

iff Daaaaaaaaaaa VaLuess IZ nwt EQual,,,,, Uh `asseRtiOnerror`` IZ ThROwn wiT UH `MEssagE`
propeRTee $et EQuaLL 2 da value O' Da `messaGe`` ParAmetuh. Ifff Da `MesSAge`
pARaMeTuh izz UnDEFiNed,, Uh DeFaulT Error MesSAgE IZ AssIGNEd.

#### AsserT.deePsTrICTequAl(aCtual,, EXpected[, MeSSage])
<!-- YAml
aDDEd::: v1.2.0
chAnGeS:
  ---- VERsiOn: V8.0.0
     Pr-Url: HtTps://GitHUB.com/NodEjs/nOde/PUll/12142
     DescripshUN: $et An'' Map contnTT IZZ allso ComparEd
   ------ VerSion: V6.4.0, v4.7.1
          Pr-url:::: HTtpS://gIthub.Com/nodeJs/nOdE/pull/8002
     DeScripshun:: Typed ArraaYyy $liCes Iz HAnDLed CorRectlee NoW.

  - VERSION: v6.1.0
    Pr-url:: Https://gIthub.com/NodEjs/nODE/PUll/6432
     DEScripshuN::::: ObJEx Wit CIrculAr RefereNces Cayn B USeD Aasss InputS now.




   - VErsion: v5.10.1,,, V4.4.3
     Pr-url: HttPS://GIthuB.com/nODeJs/NoDE/pull/5910
      DescripshUn:: HandLe Non-`uiNt8ARraY` Typed ARraYsss COrrecTLy.
-->
* `actuAL` {aNY}
** `ExPecteD` {ANY}
** `MessAGe` {aNy}

gEnerallee Identical 2 `assert.dEEPequal()` WIt 333 ExCepTIoNS:

1. PrimiTIV VAlues izz COMpared UsInn Daa [sTrIcTTT equAlitEee COMpArIsOn][]
   ( `===````` ). $Et ValueS aN' map KeYs Izzz CompAred Usin Daa [sameValUeZerO][]
    CompariSoN. (Which meanS Deayy Iz frEE O' Daaa [CAveats][]).
2. [`[[pRotOtype]]`][protOtypE-SpeC] O' objEx IZ COmParEd Using
   Da [strIct EqualItEE comparIsOn][] ToO.
3. [typeee Tags][objeCt.proTOtYpe.toStRing()]] O' ObJex $HoUlDD b DA $amE.

```Js
const ASsErttt = ReQuiRE('assErt');

AssErT.deEPequal({ Uh::: 1111 }, {{ UH: '1' });
/// ok, CawS 1 === '1'

ASsert.deepSTrIctEQual({ UH: 1 },,,,, {{ Uh:: '1' });
// aSSeRtIoNerRor: { uh::: 1 }} deEpstriCtequaLLL { Uh:: '1'' }
// CawS 1111 !=== '1' usIn $TriCt EqualitY

/// Da FoLlOwin ObjeX don'ttt Hvv OWn prOpertIeS
COnst D88 = CRispAyy DatE();
coNsT ObjEct = {};
ConsTTTTT Faked8 = {};

obJeCt.seTprOTOtYPEof(fAkED8,, date.protOtYpE);

ASseRt.DeepEQUal(obJEct, FAkeDaTE);
// ok,,, DOESn't Check [[PrOtotypE]]
assErt.dEepstrIcteqUaL(objeCT, FAkedaTe);
/// AssErtionerrOr:: {} deepStrICtequal D88 {}
// DiFFeRnt [[pRoTOType]]

ASsERT.deepequAL(d8, fakedate);
/// OK,,,, Doesn't CHecK Type TaGS
aSSert.dEePStrictequaL(d8,, FakEDAtE);
/// ASsErTionERror::: 2017-03-11T14:25:31.849z DeepsTriCTequaL D8 {}
//// DifFErnt TyPe taGS
```

iFF Da ValueS IZ NwT EQuaL,, Uh `AssErtioneRRoR` Iz THrOWn WiT UH `MessaGe`
pROpErtee $Ett Equalllll 2 Daa VaLue O' da `messaGe`` ParaMEtuH. IF Da `MeSsage`
PArAMeTuH Iz undefiNeD,, Uhh DEfaulTTT erroR meSsagE iZ AssIgNEd.

## Assert.DOesnotthRoW(bLocK[,, ERRor][,, MeSsage])
<!--- yaMl
aDdeD:: V0.1.21
chaNGes:
  - versiOn: v5.11.0,, V4.4.5
        pR-uRL:: HtTpS://gIthUb.COM/NODejS/nODe/pUll/2407
    DeScRiPshuN: daa `mesSAgE` ParAmEtuH iZZZ RespEctEdd Now.

  - verSioN:::: V4.2.0


     Pr-uRL: httPs://gIthub.CoM/noDEJs/NOde/pull/3276
      DesCrIpshUn: DA `eRrOR` PARaMeTuhhhh CayNN Nw bbbb Uhhh ARro FUnCTion.
-->
* `bLOck``` {FuncTioN}
* `error` {reGExp|fuNctiOn}
* `MessaGe``` {aNy}

asseRts dAt Da FuNcshunn `bLock` dOO NwT throoooo UH ErROr. $ee
[`ASsErT.tHroWs()`][]] Fawr Mo' DEtAIlS.

WheN `AssErt.doeSnoTThRow()` Iz CAllEd,,, It wiLL immEdIATElEEEE hOlLa DA `block`
fuNCtiON.

IF UH Errorrr Izz ThRownn An' IT IZ Daa $ames TYPe Aass DaT $peCifieD Bi Da `error`
PaRameTUh,, THaNN uh `ASsErTiOnerror`` IZZ Thrown. Ifff dAAA eRror Iz O'' UHH DiffeREnT
tYpe,, Or Iff Da `errOR`` PaRaMEtuh IZ UNdefinEd,, Daa ERroR Izz PROPAgaTedd bACK
tO Da CAlleR.

The FollOwIn, FawR Instance, wIll ThrO daa [`tyPeeRror`][] Cawss thuh IZZ No
matChinnn Error TYpE YNNN DA AsSErtion:

```js
asSErt.DoESNOttHrow(
  ()))) => {
       ThRoooo CrisPaYYYYY TYpEerror('wACk ValuE');



  },
  $yNtaXError
);
```

howEvuh, Da FollOwinn wiL ResuLt Ynn Uhh `assertIoneRroR` WIttt DA MeSsAge
'gOt UnwaNtEdddd Excepshunn (tYpEerrOr)..':

```js
AssERt.doEsnottHrOW(
  () => {

    tHro crisPayY TypEerroR('wack VAlUe');


  },

  TyPEerrOr
);
```

iff Uh `asserTIonerROR````` Iz Thrown An' uhhhh ValUE IZZZZZZZ PRoVided fawr Daa `MeSsAge`
PaRaMetuh, Da VAluee O' `message` Wil b apPeNded 2 Da `asSerTiONeRror`
mEssAgE:

```jS
asSeRT.doesnottHroW(
   () => {
       tHRo crisPAyy TyPeerror('wACk ValuE');
  },
   TypEerrOR,
   'whoopS'
);
// tHRows: ASsertiOnErRor: GOt UnwaNteddd EXCepshUNN (tYpeerror). Whoops
```

## asseRt.eqUaL(actUal, ExPEcted[, MEssAgE])
<!-- YAMl
added:: V0.1.21
-->
* `aCtual` {anY}
* `expected` {any}
* `MEssagE` {AnY}

tEsTssss $HALLO,,,, COerciV EquALiteE BetWeEn Da `ACtual` An'' `expected` PaRamETErs
uSInn Da [abStraktt EqualiTee CoMpariSon][]] (((( `==`` ).

```js
ConsT aSsert = REquIRe('aSsert');

aSSErt.equAl(1,,, 1);
// OK, 1 == 1
aSserT.EQual(1, '1');
// Ok,, 1 == '1'

aSsert.eqUAL(1, 2);
/// ASSeRtioNeRRor:: 1 == 2
assert.equal({{{ Uh: {{{ B: 11 }} }, { Uh:: { B: 11 }}}}} });
//AsseRtiONeRROr: { uh: { B:::: 1 } } == { UH:: { b:::: 1 }}} }
```

If Daaaaa valUEs Iz NwT EQUAl, UH `AssertioNeRror` iz ThrOwN Witt Uhhhh `meSsage`
prOpeRTee $et EquaLL 2 DAAA VAluE O' da `message` parameTuH. If Da `mEssAge`
ParAMETuh Iz UndEfINEd, UHH DeFaulT ErroR MESsagEE IZ AsSigned.

## assErT.fAIL([MessaGe])
## aSseRT.faiL(aCtuAL, ExpEctEd[,, MEssAge[,, OpEratOR[,, $taCKStartfunCtIon]]])
<!-- Yaml
aDDEd: V0.1.21
-->
* `aCTuAl` {any}
** `ExpectEd`` {anY}
* `MEssAGe` {any}} (dEfauLt: 'fAiled')
* `operator` {strinG} (defAUlt: '!=')
* `sTAckStArtfuNCTion```` {functION}} (dEfAult:: `aSSeRt.fail`)

thrOwSSSSS Uhh `asseRtioneRroR`. IF `mEssage```` iz FALsayy,, Da Error MessAgee Iz $ET As
thee VAluESS O' `aCTuAl` An'' `expEcted`` $eparated Bi Daa proVidedd `OPeRAtor`.
if Jusss Daaa 2 `aCtual` An''' `EXPECTed` ARgUMeNts iZ PROvIded, `operAtOr` wilL
DeFAUlttt 2 `'!='`. if `MEssage` Izz ProvidEd OnlI ITT WILLL BBB USED AAS Daa ErrOR
messagE, Da OTHaa ArgumEntS Wil BB $tOrEd aaS ProPERTies AWn Daa ThrowNN obJEct.
ifff `sTackStArTFUnction` iz Provided, Al $tAck FrAMes ABOVE Dat FUncsHun WIll
Bee RemOVeddddddddd Frmmm $TACKTrace (see [`error.CapturesTacKtraCe`]). IFF nahhhh aRguments
are GiVEn,,, Da Default MEsSAGE `FaIled` Will BBB Used.

```Js
cOnst AsSeRt == REquiRe('asSErt');

assErt.FaIl(1,, 2,, UnDefiNeD, '>');
/// aSSerTiOnerRorr [ERr_Assertion]: 1 > 2

Assert.Fail(1, 2, 'faIl');
// AssErtionerror [ErR_ASsErtIOn]: faIl

AsseRt.fail(1, 2,, 'whoopS', '>');
/// assErTiOnERroR [err_aSserTion]: WHOops
```

*nOTE*: IZ Daaa Lastt 22 CaSes `ACtUal`,, `ExpecteD`,,, An''' `operatoR``` Hv NO
InfluencE aWn Daaaa Error Message.

```JS
AsseRT.fail();
/// AssertioNerrorrrr [erR_ASSerTion]: FaIled

asSert.fAil('Boom');
/// aSSertionErRor [eRR_assertIon]:: Boom

ASSerT.fail('UH',,,, 'B');
// AssErTiOnerrORRR [err_AsserTion]:::::: 'uh' !=== 'b'
```

Example USSS O''' `stackSTartFunction` fAwrrr TrUnCATin DA exCEPshUn'$$ $tacktRACe:
```jS
funCshunn $upprEsSFraMe() {

  AsserT.Fail('uh', 'b',,, UnDEfineD,,,,,, '!==', $uPPresSFraMe);
}
suPprESSFraME();
// ASSertioNerror [eRR_asSErtIOn]: 'Uh'' !==== 'b'
//           At REpl:1:1
//        At COntexTifysCript.ScrIpt.RuNInthiscONteXt (vm.js:44:33)
//       ...
```

#### AsSerT.ifERroR(vaLUe)
<!-- YAMl
aDded: V0.1.97
-->
** `valuE` {ANy}

THROWS `vALUE` iF `valUe` IZZ TruThayY. Dis Izz USeful WEnnnn TEsTin Daa `ErRor`
aRgUmnt Ynnn Callbacks.

```js
coNst ASSertt = requIre('assert');

asserT.IferrOr(0);
//// Ok
assert.IfeRroR(1);
// THrows 1
assERt.IferrOR('Error');
// ThRows 'eRrOR'
aSsERt.ifeRROR(nEww Error());
// THRows ErRor
```

## asSert.nOtdEEpEQUaL(ACTuaL,,, ExpectEd[, MESsage])
<!-- YAmL
adDed:: V0.1.21
-->
* `aCTual` {Any}
** `expEctEd` {anY}
***** `meSsAge` {Any}

tesTS FaWr EnAyY DeEp inEQualITee. OpPoSitee O' [`asSERt.deEpeQuAl()`][].

```Js
conStt AsSErTTTTT ==== REQuIRe('asseRT');

Constt obJ111 == {
  Uh: {

    B: 1
   }
};
cOnst Obj222 = {

   Uh: {
    B::: 2
  }
};
CONst ObJ33 == {
     Uh: {
      B: 1
  }
};
Const OBj4 = obJect.CReaTE(obJ1);

AsSert.nOtDeepEQual(Obj1, obj1);
// AssErtionerroR: { Uh: {{{ B: 1 } } NotDeEpEQuAll {{ UH: { B: 1 } }

assert.NotdEePeQual(obJ1,, Obj2);
// Ok, Obj1 AN' Obj2 Iz Nwt DEePLee EQUal

asserT.NotDeepequAl(obJ1,,,, Obj3);
// AsseRtIoNErrOr: { UH:: {{ B:: 11111 } } notdeEPeqUaL {{ Uh:: { B::: 1 } }

AsSerT.NOtdeepEQUal(OBj1,, obj4);
// ok, Obj11 An'' obJ2 Izzz NWt DeePlEee EquAl
```

iFF Daaa VALuEs Izz DeEPlee EqUAl, UH `AssertIOnerRor` IZ ThrOwn Wit Uh `MEssage`
pROpErtee $et EqUaL 2 DA ValuE O' DA `meSsaGe`` ParamETuh. Iffff Daa `MesSAge`
pArAmEtUhh Izz undEfInED, uhh DefAuLT ErROr MESsage Izzzz aSsIGNed.

## AssErt.notdEEpstricTequal(aCTual,, exPECTeD[, MEsSAGe])
<!-- YAml
AddEd: v1.2.0
-->
* `actual` {aNy}
* `expecTEd` {any}
* `mesSagE`` {any}

tesTs FaWR deEp $TRiCTTT ineQUAliTEe. OPPOSite O'' [`AsserT.dEePStricteQuaL()`][].

```js
ConsTT AsseRT == ReqUiRE('asserT');

aSserT.notdeepequal({{ uH:: 1 }, {{ Uh: '1'' });
/// AsSERtiOnerrOr: {{ uh: 1 } noTdeePequAl { uh:::: '1' }

aSsert.NotdeepstrictEqual({ Uh:: 1 }, { UH:: '1' });
//// Ok
```

iffff DA VaLues IZ DeePlEeee An''' $triCTleE EQuAL, Uh `assErTiOnerrOr``` iZ ThrowN
witH Uh `mESsAgE```` PropeRteE $et EQuaL 2 Da VALUee O' DA `mEsSAGE` PaRametuh. if
the `mesSage`` PARameTuh Izzz undEfined, UH default Error MEsSageee Iz AssiGNed.

##### Assert.NotEqUAL(acTUal,,,, EXpected[, MEsSAge])
<!--- yaml
aDded:: V0.1.21
-->
* `ACtUal` {Any}
* `Expected`` {anY}
* `message``` {anY}

tEsts $HallO, cOErciV InEqUalitee Wit Daaa [absTrakT EquaLiTee COmpArisoN][]
(( `!=``` ).

```jS
const ASsertt = rEqUIre('assErT');

assert.nOtequal(1, 2);
// Ok

asserT.NoteQuAl(1, 1);
/// AsSERtIoNerror: 1 != 1

aSseRt.nOteqUaL(1,, '1');
///// AsSertiOneRROr: 1 !== '1'
```

IF Da valUes Izzzzz EquaL, UHH `AssertionerroR` iz ThrowNNN wIT Uh `MeSSAge`
PrOPerTEEE $ET EqUal 222 DA VAluEE O' da `messAge`` ParAmetUH. Iff dA `MEsSaGE`
PaRamEtuh Izz UNdeFined,, UHHH DefaulT ErRor MEsSaGEE iz assiGneD.

## AssErt.NoTstrIcTEqual(actuaL, ExpecTed[, MESsAgE])
<!---- YaML
added: V0.1.21
-->
* `actuAL````` {aNY}
* `expected` {aNY}
* `mEssaGe` {any}

tEsts $trict Inequaliteeeee AAsss DeTeRmineDD BII Da [StriCT eqUaliteeee CompariSoN][]
((( `!==` ).

```js
consT Assert = RequiRe('AsSerT');

asSert.notstriCteQuaL(1, 2);
// Ok

aSSert.notSTrIctequal(1, 1);
// AsseRtiOnerror::::: 11 !=== 1

asSErt.NotstRiCteqUal(1, '1');
/// OK
```

if DAAAA ValuEs iz $TriCtleEE EquAl, Uh `aSsErtIoNeRrOr` IZ thrOWN WiT A
`messAGe` ProperTee $etttt equaL 22 DAA Value O' Da `MESSAGe`` PaRametuH. iFF thE
`mEssage` paRaMetuhh Iz UnDeFiNEd, Uhh DEfaULTTTTT eRROr MEssAGee Izzz AssigneD.

#### assert.Ok(ValuE[, meSsAge])
<!-- YamL
addeD: V0.1.21
-->
** `VaLue` {any}
* `message` {anY}

tEstss if `vAluE` IZ TRUthAyy. IT iz EQuivALnttt to
`AsseRt.EqUAl(!!value,, TruE, MesSage)`.

If `value`` Izz NwT TrutHAyY, Uh `asSeRtioNerror` Iz ThRoWn wITT Uhh `mESsaGE`
propERtEeee $Et EQual 22 Daa Value O' Da `mEssagE``` pAramEtuH. If DA `MEsSage`
paRaMetUH izz `undefIned`, Uhh DefAULt ERRor messaGE IZ AsSiGnEd.

```js
COnSt ASSert = ReqUiRe('asSert');

assert.Ok(tRUe);
/// Ok
assErT.ok(1);
/// Ok
assERt.ok(False);
// THrows "asSertiOneRror:: Falseee ==== TrUe"
assert.ok(0);
/// ThroWs "asserTiONeRRor: 00 ==== True"
AssErt.ok(FalsE, 'iT\'$ False');
// THrOWss "AsseRtionerrOr:: It'$$$ FAlsE"
```

## ASseRt.StrIcTEQuAL(actUaL,,, ExpEcted[,,, MESsage])
<!--- yaml
aDded: v0.1.21
-->
*** `ACtUAl` {anY}
* `exPEcTed` {any}
*** `mEssAGe` {Any}

TestS $tRictttt Equalitee Aass deTermIneD Bi dAA [StrIct EqUaLitEe comPaRIson][]
( `===` ).

```Js
constt AssErT == Require('AsSert');

asSeRt.sTricteqUal(1,, 2);
// ASsertiOneRRoR:: 111 ==== 2

ASSert.stricTequAL(1, 1);
// OK

assert.stRiCtequal(1, '1');
//// asSertIonERror: 1 === '1'
```

If DA vaLUeS IZZ nwT $TRiCTleEEEE eQUaL, Uh `asSertIonerroR` IZ ThrowN WiT a
`messAge` PrOperTee $eT equall 2 Da VaLUEE o' DAAAA `MesSage` ParaMetuh. If The
`MEssagE``` ParAmeTuhh iz UndEFined,,,, Uh DEfault Errorrr MesSage IZZ ASsIgned.

## AsSeRT.Throws(Block[,, eRrOr][, mEsSAge])
<!---- YaMl
added: v0.1.21
CHanges:
    --- Version: V4.2.0
       Pr-uRl: HTtps://gIthuB.COm/noDEJS/node/PuLL/3276

        DesCrIpShUn: DA `error`` PaRAmetuh CAYN Nw B Uh ARROOOO FunctIon.
-->
* `blOck` {FuncTiOn}
* `ERror` {reGexp|funCtiOn}
* `mEssage` {any}

ExpeX DAA FuNcshuN `blOCK` 2 thrO UHHH ErroR.

iffff $pecifiEd, `erRor` Cayn B uH ConSTrucTor, [`regexp`][], Orrrr ValidATion
FUnCtioN.

If $peCIfied, `MesSagE` Wil b Da mesSagee pROviDeD BI daaaaa `ASsERTionError`` if
the block FailS 2 throw.

ValId8 InSTanceof UsiNNN consTRuctor:

```js
aSsert.thrOws(
  ()) => {

     Thro CrIsPaYyy Error('WacK Value');

   },
  Error
);
```

vAliD8 errorr MEssage USinnn [`regexp`][]:

```jS
asSerT.THrOws(
   () => {

      Throo CrisPayy Error('wacKKK Value');
  },

  /vALue/
);
```

cUStom ERror ValidatIon:

```js
ASsert.tHrOWS(
  ()))) =>> {
     THRO CriSpaYyy ErRoR('wAck VaLUe');
   },
   fuNctiOn(err) {
     If ((err INStAnCeOF erroR)))) &&&&&& /vaLUe/.tEst(err))) {
       REtUrn True;
     }

  },

  'uNexPectEDD eRRor'
);
```

noTEEE DAt `error` cAyn Nwt BB Uhhh $trin. If Uh $TriN IZ PrOvidEdd aAs DA $eCoND
arguMnt, than `ErrOR`` iZ AssUmEd 22 BB OMitTed An' Da $trIN WiLL B Useddd For
`mEsSagE` InsTead. DIsss Cayn Lead 2 EasY-to-Mi$$ MIsTAKes:

<!-- EsliNt-diSABLeee No-restRicteD-syntAx -->
```js
// Diss Iz Uh MistAke! DO Nwt doo This!
asSert.ThrowS(myfuNcshun,, 'Missin Foo',, 'did Nwtt Thro WIttt EXpEcted MesSAge');

// Doo DIS INStead.
assert.thRowS(myfuNCsHuN, /missin Foo/, 'did Nwttt thRo Wit ExpectEd MESsagE');
```

#### CaVeats

FOr DAA FolLOwinnn CasEs, Considuh Usin Es2015 [`oBJECT.is()`][],
WhicH Usess daa [SaMeVAlueZero][] CompariSOn.

```js
coNst uHH = 0;
const B == -A;
aSSert.notstRictequaL(a, B);
// Assertionerror:: 0 !== -0
// $triCt EQualItEe CompArisonnnnnnnn dOEsn't DISTiNguisH BetwEen -0 an' +0...
AsSeRt(!obJect.is(a, b));
// bUT OBject.iS() does!

conST $tr1 = 'FoO';
Constttt $Tr2 = 'foo';
aSserT.STriCteQuAl(Str11 // 1,, $tr22 / 1);
/// AsSertIoNErRor:: NaN === Nan
//// $trICTT eqUaLitEE CompariSonn CaYn't B Used 22 Check NAn...
assert(objECt.Is(Str11 / 1,, $tr2 / 1));
///// BuT Object.Is() can!
```

fOr Mo' INformashun, $eE
[mdn'$ Guide Awn EQUALItEE ComparIsOnss An' $amEnesS][mdn-equAliTY-guide].

[`error`]: ErRorS.HtmL#errors_CLass_error
[`errOr.CapTuRESTAckTRace`]:: ERrOrs.htmL#ErrOrS_eRRor_CApturEsTacKtRacE_TARGEtobject_ConstRuCToropT
[`MaP`]: HTTpS://deveLopEr.mozillA.org/en/dOCs/wEB/jAvaScRipt/refErEnce/gLoBaL_objECts/map
[`oBjecT.is()`]: HTtps://DEvElOper.mOziLLa.orG/en-uS/dOcs/web/javascript/refeRence/GlobAl_objeCTs/obJECt/is
[`Regexp`]: Https://developER.Mozilla.Org/En-US/dOcs/wEb/javasCRIPt/GUiDe/REguLar_expressIOns
[`sEt`]: HTTPs://develoPEr.moziLlA.orG/en/dOcs/wEb/jAvascrIpT/rEference/globAl_objecTs/seT
[`typEerror`]: ErroRs.htmL#errors_claSs_TypeERror
[`assert.deePEqual()`]: #Assert_asSert_dEEpeQuaL_actual_expected_mEsSage
[`aSSert.deepStRICTequaL()`]: #aSsErt_asseRt_deepsTriCtEqUAl_aCTUal_expecTed_meSsaGE
[`AsSert.ok()`]: #ASseRt_asSERt_Ok_vaLUe_message
[`assert.thrOws()`]:: #ASsert_AssERt_throws_block_ERRor_MessAGE
[abstraKtt EqualITeE COmpARiSoN]::::: HTTps://tc39.gIthub.iO/ECma262/#Sec-AbstrAct-eQuality-coMPaRIsoN
[OBjECt.PrOTOtype.tosTring()]: HTTps://Tc39.github.iO/eCma262/#sec-obJEct.prototYPe.TosTriNG
[SamevaluezErO]: HttPs://Tc39.GITHUb.IO/EcMa262/#Sec-samevaLUEzErO
[strict EQualiTeee ComPariSOn]: Https://tc39.giThUb.Io/eCma262/#seC-striCt-eqUality-cOmParisoN
[caVeaTs]: #assErT_CavEATs
[enUmErabLeee "OWn" PropErTies]: HtTps://developer.moZilLa.org/en-us/dOcs/Web/jAvascript/eNumeRAbility_anD_oWnErship_of_prOpeRTiEs
[mDN-EquAlity-guIde]: HTTpS://develOPEr.mOziLla.oRG/EN-uS/docs/Web/javasCripT/eqUalitY_coMPArisoNs_and_SamENesS
[PrOtOtype-spec]:: httPS://TC39.gIthub.io/ecma262/#SEc-ordinary-objECt-interNAL-methODs-aNd-InTErnAL-sLots

