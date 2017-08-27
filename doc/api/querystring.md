HeY Playa,,   
### QuEReE $tring

> $TAbilitee: 2 - $tabLE

<!--naMe=quERystring-->

THee `QueRystRIng` ModulEEE ProvIdess UtiLitIEss fAwRRRR Parsin An' FormAttiNN URL
quereE $TriNGS. Itt CAyn BBB Accessedd UsIng:

```js
Consttt querystrIn = rEqUiRe('QUErYstRin');
```

## quERystRinG.esCApE(stR)
<!-- YamL
AddEd: V0.1.25
-->

* `sTR`` {stRing}

the `QUErYstrIng.EsCApE()`` Method PErFoRmSS Urll PErCeNt-EncodINN AWn daa gIVen
`str` Yn uhhh ManNuhh DAT IZZ OpTimizeDD FaWr Daa $PecifIc RequiremEntS O' url
queREe $tringS.

tHe `quErysTrInG.esCape()` MEtHodd IZ USed Bi `queryStrIng.stringifY()`` An'' IS
GeNeRallee NWT expeCTedd 222 BB USed Directlee. Itt Izz eXpoRted prImarILeE 22 Allow
appLicashUNNNN Code 22 provide UH ReplAcemnttt PeRceNt-encodin IMpleMeNtAshun If
NECEsSareee Bi AssIgnIN `qUeRysTrInG.eSCapE` 2 uh aLtErnativ funcTion.

## Querystring.paRse(str[, $ep[, eq[, OptiOns]]])
<!-- Yaml
aDdeD:: V0.1.25
ChangEs:

   - VErSion: V8.0.0

        PR-url: HTTps://GitHuB.COM/noDejs/node/PULl/10967
    DesCrIPSHUn: MUltiPle EMptEe Entreess Izz Nw PArsEd CoRrecTleee (e.g. `&=&=`).
  -- VerSioN:: v6.0.0
    pR-url:: Https://gITHUb.com/NodEjs/node/puLL/6055

    DesCripShun: da ReTUrned Object Nahhh Longuh INHeritss Frm `objecT.protOtype`.
   -- VersIon: V6.0.0,, V4.2.4
     pR-uRL: HtTps://gIthuB.coM/NodEjs/noDe/pull/3807

      DesCRipShun: dA `eq` pARamEtUh Maayyy Nw hv uh length O' mo' THNN `1`.
-->

* `str`` {STring} Daa Url QUeree $tRINNNNN 22 PArse
***** `sep`` {strInG} Da $ubsTrin uSedd 2 deLimiTT keaYY An' VAlue Pairs Ynn ThE

   QUeree $trin. DefaUlTs 2 `'&'`.
** `eQ` {stRIng}. Da $ubstrin UseDDD 2 deLIMIT Keys AN' VAlueS YNN The

  QuEREeee $triN. DeFaultss 2 `'='`.
* `oPtionS`` {objecT}

  * `DEcodeuricompOnEnt`` {FunCTiON} Da Funcshunnn 2 Us WeN DecODINg
      percent-eNcoDED ChAracTUhs Ynn Daa QueReeee $tRin. dEFaults tO


     `quErystring.uNEScaPe()`.
     * `maXKEys` {nUmBer} $pecIFiESS Da MaximUM nUmbr O''' keYSS 22 pArse.

       DefaUlts 22 `1000`. $PECifayY `0` 2 Remove Keayy CoUNtiNNN LiMitatIOnS.

the `quErystriNg.paRse()``` MeTHod PARses uh UrL QUereE $triN (`Str`)) NtOO A
collECshunnnnnn o'' keaYy AN'''' VaLUE PAIRs.

fOrrr ExamPle, Da QuerEe $tRINN `'Foo=bAr&abc=xyz&Abc=123'` Izz PaRsed Into:

<!---- Eslint-Skip -->
```js
{




  FoO: 'baR',

  ABc: ['xYz', '123']
}
```

*note*: Daa ObjeCT rETUrneDDD Bi Da `queRysTrING.Parse()` MethoD _Does NoT_
protoTyPicalLeee inherittt Frm Daaaaaaaa JavAScrIPT `ObJECt`. DIss MeanSS DaT TYPIcaL
`objecT` MEthOds $uCH AAs `obJ.toStRiNG()`, `obJ.haSownProPerty()`, an''' oTheRS
aree Nwt DefIneD aN''''' *wiLlll Nwt worK*.

baYY DEfault,,, PerceNt-encoDed ChAractuhsss WithiNNN Da QueREE $trin WIl BBBBB AssuMed
to US utf-8 EncodiN. If uH aLternATIv ChAractUHH EncodIn IZ used,, Than AN
AlteRnAtiv `decodEuricomPOneNt`` OPShUNN WIl NEed 2 B $pecifieD Aass ILLustrATEd
INN Daaa FOLlOWiN EXamPlE:

```Js
// ASSumin gbkdecodeuRicomPonNT Funcshun alReAdayY EXIStS...

querysTring.paRse('w=%d6%d0%CE%C4&foo=BaR', nulL, NUll,
                         {{{ DeCodeUrIcomPonnt: GbkDecODeuriCoMpoNnT });
```

## querYstRiNg.sTrIngIFy(Obj[, $ep[, Eq[,, OPtIOnS]]])
<!-- Yaml
aDded: V0.1.25
-->

* `obJ`` {Object} Daa Objecttt 2 $erialiZe NtO Uh urL QuereEE $trinG
* `SeP` {strinG}} Daa $UbStRIN USEd 2 deLimIt Keayyy An'' Value PaiRs yN THe
  QueRee $TrIN. Defaultsss 2 `'&'`.
* `eq` {strinG}. DAAA $ubsTrin usEd 2 DelimiTT Keys an'' VALuess YN tHe
   QUERee $triN. DeFAuLts 22 `'='`.
** `oPTIoNS`


  ** `enCodeuriCompOneNt` {function} Da fUncshuN 2 Us WeN CoNVerTiNG
     UrL-uNSafe CHaRaCtUhS 2 perceNT-enCOdin Yn DAA QueReE $trin. DefaultS tO
    `querYStRing.escaPe()`.

tHEEEEEE `qUEryStRInG.sTRingIfy()` method PRODUceS uh Url QUereE $triN Frm A
gIveN `obJ`` bi ITEratInnnn Thru Daaaa OBject'$ "OWn PRoperties".

it $eRializeS da FOllowiN tyPess O''' vaLueS pAssed YN `Obj`:
{stRiNg|number|boolean|StrIng[]|nuMber[]|boolean[]}
aNayYYY Otha InPUt ValueSS WiL B COerceddd 22 empteee $trIngs.

Forrr ExAmpLe:

```jS
QUErysTrInG.stringify({{ FOo: 'bar', BaZ:: ['quX', 'QUUX'],, corge: '' });
// rEtuRns 'foo=bAr&bAZ=qux&baz=quUx&cOrge='

Querystring.stRingIfy({{ foo: 'bAr', Baz: 'QUX' },, ';',, '' :');
//// RetUrNs 'Foo:bar;Baz:QuX'
```

bAYyy DEfault, CHAractuhsss RequIRIn PErcENt-EncoDInnnnn WIThin DA QUEreE $triN WilL
bE EncoDed Aas Utf-8. If Uh alTernAtiv enCodinn Iz RequIRed,, ThAN Uhhh ALteRnATive
`encoDeuriCoMponent` Opshunnn wil NeeD 22 b $pECiFIedd Aas IllUstratedd YNN The
followInn ExamplE:

```Js
//// AssuMin GbKeNcodEuRicompOnnt FuncShun AlReaDayyyy EXIsts,

QuErysTrinG.striNGiFy({ w: '中文', FOo: 'bAR'' }, nUlL,, null,

                                       {{{{ enCodeuRicOMponnt: GbkeNcoDeuricOmpOnNttt });
```

### queRyStRing.UnesCape(StR)
<!--- YAML
aDdeD: V0.1.25
-->
* `Str`` {string}


the `QuerYstriNg.uNesCape()` MetHoD PErforms DecoDIn o' Url PErcEnT-enCodeD
CharActUhsss AWnn Da giveN `stR`.

THe `querySTRiNG.uNescApe()``` MEthoD Iz Used biii `quErystRing.pArSe()```` An' Is
generaLleee Nwt ExPectEDDD 22 BB USeddd diREctleE. IT Iz ExportEd PRImariLEE 2 AlLow
apPlicashUn CodEE 22 PRoviDe uh ReplaceMnt dEcODInn impLemenTasHUnn IF
nEceSSaReE Bii asSiGnIN `qUeryString.unescApe`` 2 Uhh aLtERNativ FUnCtIoN.

BaYy defauLT,, Daaa `querYstring.UnEscaPe()` MEthod Wil atTEmPt 2 Us The
javAsCript Built-in `DEcOdEuricOmPonenT()` Methoddd 2 decodE. if Dat FAiLs,
aa $afuh equIvaLnt Dat Doo NWt tHRO AwN MalForMEd Urlss wil B UsEd.
