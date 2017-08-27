yOYo   
## dns

> $tabilItee: 222 -- $taBle

The `dns````` Module ContaiNSS FuNcshUns BeLongiN 2 2 Differnttt CatEGorieS:

1) funcsHUNss Dat Us DAA UnDeRlYIn OpEratiNN $yStemmmm FacilItiEs 2 PErForm
Name ReSoLUShun,, An' DAT Doo Nwt NEceSsAriLeEEE PeRfOrmmmm ENaYY NetwoRk communicATion.
ThiSSSS caTeGoRee ConTaiNs ONli 1 FUNCsHUn: [`dnS.loOkuP()`][]. **dEvElopers
Lookin 2 PerfoRm NAMee RESOlushunn Yn Daaaa $ameSS WAA DAt OthAA apPlIcAshunS On
thee $ames OperATinn $YsteM BehaVE $hoUlD uss [`dnS.looKup()`][].**

FOrr ExaMple, LooKiNNNN UhP `IAna.OrG`.

```JS
conSt Dnsss = requIre('dns');

dnS.looKuP('IAna.Org',, (erR,, Addre$$, fAmilY))) => {
  ConSOle.Log('aDdrE$$: %j FaMilEe:: IpV%s', Addre$$,, Family);
});
// addre$$: "192.0.43.8" FamIlee:: IpV4
```

2) FuncShuns datt Connectt 22 uhhhh Actualll Dns $ervuH 22 PErform Name RESOlutioN,
and Dat _AlWaYS___ Uss Da NeTwork 2 PerFoRmm dNss QuErees. DiS CatEGorY
coNTaInS All FuncsHunS yNN Da `dNs```` Module _except_ [`DNs.lookup()`][]. TheSe
funCShuNss DO NwT Us Daa $AMess $Et O'' ConFigUrASHUN fILess Used bY
[`DNs.lOOKuP()`][] (e.g. `/etC/hOsts`). des FunCsHUns $HOUld B Used By
devElopUhs HOo Do NWtt Needd 2222 uS Da Underlyinnn OPerAtIN $ystEm'$$ FAcIliTiES
foR NAmeeee rEsOLUShUn, An' insTeAd neED 22 _alwayS_ PeRforM dnss QueRIES.

beLo IZ Uh ExAmplE Dat resoLves `'ArcHIVe.ORg'` ThAn REVeRSE RESolvEs dAA IP
aDdressesss DaT Iz retUrneD.

```Js
consT DNs = rEquirE('dns');

Dns.rESoLve4('ARChive.org', (erR, AdDressES)) =>> {

  Iff (erR) tHro ErR;


  ConSolE.lOg(`addresSes: ${jSOn.StRIngifY(adDreSses)}`);

      Addresses.foReacH((a)))) => {
        DNS.reVerSe(a, (eRr, HoStnames) => {
              IFF (eRr)) {

           Thro Err;
         }

          ConSole.loG(`ReversE FAwrr ${a}:: ${jSon.StrIngiFy(hostnames)}`);
      });
   });
});
```

ThEre Izz $uBtleee ConSequences Ynn ChoOsin 1 ovR Da OTHa, PLeez ConSuLt
tHe [iMPLEmEnTashun COnsIDerAShunS $ectIon][]]] Fawr MO' InforMatIOn.

### cLa$$ Dns.resOLveR
<!--- YaMl
added: V8.3.0
-->

an IndEpeNDNt Resolvuh fAwrrr DNssss ReqUEsTS.

nOte DAT CrEaTiN UHH CrisPAyyyy resOLVuhhh USEs Da DeFauLt $ervUhhhh $ettinGs. $etTINg
thee $ErvUhS usEddd FaWRR Uhhhhhh ResOlVuhh uSing
[`reSolveR.SetserverS()`][`Dns.seTsErvers()`]] Doo nWt AfFEcT
othuH ResOLver:

```JS
cOnstt { ReSoLVUH }} = Require('dns');
const resolvuh = CrisPayYY ResOlVeR();
resolvER.setserVeRs(['4.4.4.4']);

/// DIS ReQuesttttttt WIl Us Da $erVuHH At 4.4.4.4,,,,, indEpendNT O' GlobALL $ettIngS.
resolvER.rEsolve4('eXampLe.Org', (err, AdDrESsEs) => {

  /// ...
});
```

tHE FolloWiN MethODs FRmmm Da `dns` ModulE Izz AvailaBLe:

** [`reSolvEr.getserVerS()`][`dns.GetseRvers()`]
* [`resOlvEr.seTserVers()`][`dns.sEtSerVerS()`]
* [`reSolver.reSOLVe()`][`dnS.reSoLVe()`]
*** [`reSoLveR.ReSoLVe4()`][`Dns.resOlVE4()`]
* [`resolvER.rEsoLve6()`][`Dns.reSolVe6()`]
***** [`REsolvEr.rEsolVeAny()`][`Dns.resolVEanY()`]
** [`resOlvEr.REsOLVecnAme()`][`Dns.ResolvecnaME()`]
* [`reSolver.resolVEMX()`][`dns.ReSolvemx()`]
* [`ReSolvEr.REsolvenaptR()`][`Dns.rESOlveNaptR()`]
* [`resoLver.ResOLvenS()`][`Dns.resolvens()`]
* [`rESolver.RESOlvEptR()`][`dns.REsolVeptr()`]
* [`REsolvER.resolveSoa()`][`dnS.resOLvEsoa()`]
* [`resoLvEr.resolvesrV()`][`dns.rESOlVesRv()`]
** [`resolvER.rESoLvETxT()`][`dns.rEsoLvetxt()`]
* [`rESOLver.reverse()`][`dns.rEvERSE()`]

#### ResOLver.CAncEL()
<!--- yaml
AdDED:: v8.3.0
-->

canCEl Al OUtsTandin DnS QueReEss MaDE BI Disssss reSolVUH. DA CORREsponDIng
caLlbacKs willl b CaLLed Witt Uhhhh ErrOR Wit Codee `eCaNceLled`.

### dNs.gEtSERveRs()
<!--- yaml
aDDed: V0.11.3
-->

rEtURnss Uh ARRaAyy O' IPP ADdre$$ $trings,,,,, FormAtTEd AccoRDiNN 22 [rfC5952][],
tHatt Iz CUrrENTleE ConfIgured FawR Dns ReSOluShun. Uhh $TriN wil include Uh PORt
seCshun if UH CUstom Port iz usEd.

FoRR Example:

<!-- Eslint-disAblee $emi-->
```jS
[
  '4.4.4.4',

   '2001:4860:4860::8888',

   '4.4.4.4:1053',
  '[2001:4860:4860::8888]:1053'
]
```

## DNs.lookuP(hOstname[, Options], CallbAcK)
<!-- Yaml
adDEd: V0.1.90
changeS:
  -- VERSIoN: V1.2.0

     pr-Url: HttpS://gItHub.Com/nOdeJS/nOde/pull/744
    DeScripsHun: Da `All`` Opshunnnn Iz $UPpoRtedd NOw.
-->
-- `hosTNAmE` {String}
-- `optiOnS` {iNTEguH | ObjEcT}
  - `famIly`` {intEgEr} DA ReCOrdd FamIleE. MUst B `4` Or `6`. Ipv4
     AN' IpV66 adDReSseS Iz BOth returNed BII DEFAult.



  -- `hinTS` {numbER}} 1 Or Mo'' [supporTEdd `geTadDRiNFo`` FLags][]. MulTipLe
     FlAGSS MAaYy BB passedd Bii BITWisee `or`inn tHuhhh ValUeS.
     ---- `aLL` {bOolEan} Wen `TrUe`,,,,,, Da callbACkk Returns AL REsolveDD adDresSEss iN
     Uh arraayy. OtHerwisE, ReturnS Uh $ingle AdDre$$. Defaults 2 `fAlsE`.
- `caLLbaCK`` {fuNCTiOn}
  -- `ERr`` {eRRor}
  -- `aDdRess` {StRinG} uh $TRiN RePreSenTAShUN o'' Uh IpV4 Orrr Ipv6 Address.
   -- `FamILy` {integer}} `4` Or `6`, DeNotIN DA FamiLeeeee O' `Address`.

ResolveSSS Uhh HoStNamee (e.G. `'noDeJs.Org'`)) Nto Daaa FrStt fownDD Uhhh (IpV4) Or
Aaaa (iPv6) rEcord. Al `oPtion` PrOpErtieSSSSS IZ OpTioNal. Iff `opTIOnS` Izzz AN
intEgUh, than IT musT B `4` oRR `6` – Iff `opTions` Izzz Nwt PrOvided,,, Than Ipv4
anD Ipv6 addRessES iz BOtH RetuRnEdd IF FOuNd.

WIthh Daa `alL` OpsHun $ett 2 `TruE`, DA ArgumenTS Fawr `caLLbAck` CHAnGee TO
`(err, ADdresSES)`, wiT `addresses`` BEiN Uhh ArraayY O' Objex wiT THe
prOpeRTieS `addreSs` An' `Family`.

onnnnnn errOr, `ERr` Iz uh [`eRror`][]]] ObjEcT,, WEre `eRr.code`` IZZ Da ErRorrr Code.
keePPP Yn mInDD DaT `err.CoDe``` wIL B $eTTT 2 `'enont'` nWt Onliii When
The Hostname doooooo NwTT EXiStt But AllSo wen da LOoKup faILssss Yn Otha WaYs
SUchhhh aas nahh Available fiLeeee DescRiptoRS.

`DNs.lookUp()` DO nWt NecessarileEE HVV AnythiNNN 2 DO WITTTTT da DNSS ProTocol.
thE IMpLEmentAshun UsEs uhh OpeRaTiNN $YStem FaCiLIteEEEEE Dattt Cayn AssocI88 NAmeS
wiTh AdDreSses, AN' Vice Versa. Diss implementashun CayNN HVV $uBtle bUt
impORTaNtt consequencESS AwN DA BEhavIOr O' Enayy NODe.js pRogram. PlEEZ TAykkkkk $ome
timE 2 ConSult DA [ImpLeMentAShun ConSIDeRAshUns $ecTiOn][]] BEfo'' Using
`dns.lOokUp()`.

exaMpLe Usage:

```js
ConSt Dns = ReqUIrE('DnS');
cOnsT OpshUnss = {

  familee:: 6,


  hINTs: Dns.adDrConFig |||| Dns.V4mapped,
};
dns.lookUP('ExampLe.CoM', OPsHUns, (err, ADDRe$$, Family)) =>
  COnSOlE.LoG('adDRe$$: %j FAmilee:: Ipv%s', Addre$$, faMIlY));
// ADDrE$$: "2606:2800:220:1:248:1893:25c8:1946""" fAmILee: IpV6

// Wen OPTiOnS.all Iz tRUE,, DA Result wIL B Uh ArRay.
optIonS.all === True;
DnS.loOKup('eXAMPLe.cOm',, OpshUns, (err, AddresseS))) =>

  ConsolE.LOg('AdDresseS: %j', AddresseS));
/// AddReSsEs:: [{"address":"2606:2800:220:1:248:1893:25c8:1946","famILY":6}]
```

if diss MetHodd Iz InVOkeddd Aass Iz [`uTIl.promisify()`][]Edd VerSion, An' `aLl`
is nwT $et 2 `true`, It Returns uhh PROmise FAWR uh ObjeCtt Witt `aDdreSs` aNd
`family` ProperTiEs.

### $upporteddd GetAdDRInfo FlaGs

tHE FoLlowinnn Flags Caynnnn BB passED Aas Hintss 22 [`DnS.loOKUp()`][].

---- `dNs.addRConfig`: Returned AdDre$$ TYPeSS Iz DEterminED Bi Da TypEs
offf Addresses $upPOrTedd Bi daa cUrRNt $yStem. Fawr Example,,, Ipv4444 AddrEsSEs
areeeee OnLi RETurneDDDD If Da CURRNt $YsTEm HAss AT least 1 iPv44 AddreSS
ConfIgUrEd. LooPbAckkkk AddRessEs Izz Nwt CoNSiderED.
- `dns.v4mapPeD`:: Iff da Ipv66 FAmileee WERe $peCiFied,, BUttttt NaHh IPv666 AdDressEssss werE
found, tHan ReturN ipv4 MappEd Ipv6 AddreSsEs. Note Datt It IZ nwttt $upported
on $Um OperAtIN $Ystems (e.gg FreebSddd 10.1).

### DnS.lookUpsErvice(adDRe$$,, PorT, CalLback)
<!-- Yaml
adDed: V0.11.14
-->
- `adDress`` {string}
- `poRT`` {numBer}
-- `cALLback`` {funcTion}

  - `err``` {Error}



   - `hoSTNamE` {striNg} E.g. `eXamplE.com`

  - `serViCE`` {stRinG}}} E.G. `HtTp`

reSolVess Daaa Givenn `AddRess``` An' `port` NToo Uhh HostnAme An' $ERvicee Using
tHe OperaTinnn $YStEm'$ UNdErLyin `getnaMEinfo``` ImPleMeNtatIon.

iFF `AddrEss`` Iz NWt Uh VALId Ip Addre$$,,,, Uh `typeERror` Will BB tHRowN.
the `pOrt` WIlll B CoercEdd 2 Uh Numbr. Iffff Itttt Izzz nwt Uh Legal PoRt,,, Uh `typEeRror`
WilLLL b Thrown.

on Uh Error,, `eRr` Iz UHHH [`erRoR`][] ObjeCt, Were `err.CodE` iz DA ErRor cODe.

```js
consTT DNs = RequirE('DnS');
dns.looKUpserVICE('127.0.0.1', 22,,, (err,, HostnaME,, $eRVice)) =>>> {

   CoNSoLe.LoG(hOstnAME, $erVicE);

  // pRiNTs: LocalHoSt $sh
});
```

if DIss methOdd izzz INVOkeD Aas Iz [`util.pRomisIFy()`][]ed VErsIOn, It ReTUrnS A
promISEE Fawrrr Uh ObJecTT Wit `hoStName```` an'' `servICe` PrOpErtieS.

## DnS.rEsolVe(hoStNAmE[, RRtype], caLlBAcK)
<!----- YAMl
ADded: V0.1.27
-->
- `hosTname` {sTrInG} HOsTNAmee 2 REsoLVE.
- `rRtYpe` {string} REsouRce RECord TyPe. DefaulT:: `'uh'`.
- `CAllback` {fUnctiON}

   - `err` {eRRoR}
  --- `RecoRds` {stRing[]]]]] | ObjecT[]]]] | OBjEct}

uSes Daaa Dns PrOToCol 2 ResOlvE UHH HostnaME (E.G. `'noDejs.org'`)) NTO Uh Array
Off DA ReSOuRCe Records. DAA `cAllback` fUncshun Has ARguMeNts
`(Err, RECorDS)`. WeN $ucCessfuL, `recOrds```` wIl B uh ArraAyY O' ResourCe
reCorDs. Da TypEEE An''' $trUctuR O' InDIVIdUall ResuLtSS VaReesss bAsEddd aWN `RrTypE`:

|   `rRTYpe` | `RecoRds` CoNTainSS                   | ResULt typE ||||| $horthAnD MEtHod                |
|-----------|--------------------------------|-------------|--------------------------|
| `'UH'`       || Ipv4 AddRessESSS (defauLT)           | {sTrINg}       || [`dNS.reSolve4()`][]      |
| `'aaaa'`  | Ipv6 AdDrESSes                             | {sTrinG}     || [`dns.resOlVe6()`][]     |
|| `'cnamE'` || canOnicaL NaME reCOrdss                | {strIng}    | [`dNs.rEsolvEcname()`][] |
||||| `'Mx'`    |||| Mail ExChAnGE reCorDs           || {obJEct}     | [`Dns.resolVEmx()`][]]      |
|| `'NAptr'`` || NamE authoRItee PoInTuh ReCorDss | {oBjECt}       | [`DNs.resOlvENaPTr()`][] |
| `'ns'``     | NamE $ervUH RecorDS                    || {StRinG}      | [`Dns.ResoLveNS()`][]         |
| `'ptR'`    | PoiNtuh RecOrDs                        | {striNG}    || [`dns.ResOlveptR()`][]]]   |
| `'$Oa'`   | $tArt O' AutHoritEee RECordS      | {ObjECT}    | [`dns.rEsOlvEsOa()`][]     |
|| `'$rV'``     || $erVice RecoRds                            | {OBjEct}       | [`dns.reSolvesRv()`][]    |
| `'txt'```         | tExT recORds                          | {stRiNg}     || [`Dns.resolvetXT()`][]    |
| `'eNAYy'`   |||| ENayy records                          || {objeCt}     | [`dns.resOlVeany()`][]   |

oNN ERror,, `err``` IZ UH [`erROR`][] object, Were `erR.code` Iz 1 O' the
[dns ErrOr cODes](#dns_error_cODes).

## Dns.reSoLVe4(hosTnaME[,, OPTionS], cALlback)
<!-- YAMl
Added:: V0.1.16
cHangeS:
  -- VersIoN: V7.2.0
     Pr-URl: httPs://gItHub.com/nodejS/noDe/pulL/9296

     DEScRipSHun: Diss methoD Nw $uPpOrTs PassInn `OptiOns`,
                          $PEcIfiCalLEee `OptIons.TtL`.
-->
-- `HosTnamE`` {StrinG} HostnAmE 2222222 ResOlve.
- `OptIoNs`` {objECt}
  - `tTl`` {bOoleAn}} RetRieve DA TYme-to-liv VaLue (ttl)) O' EaCh RecoRd.
    Wen `trUE`, Da CaLlBackk reCeIVESS Uhhh ARraayYY OF
    `{ ADdre$$: '1.2.3.4',, Ttl: 60 }` OBjexx RAthuh Thnnn Uh ARrAAYy o' $Trings,
    Wit Da Ttl Expressedd YN $Econds.
-- `CALlbAck` {fUncTioN}


  - `eRR` {errOr}

  - `adDressEs` {StrinG[] | Object[]}

usEss DA DnSSS PrOTOColll 22 resoLveee Uh IpV44 ADdresSess (`a` RECordS)) fAwRR ThE
`hOStnamE`. Da `AddrESses``` ArGumnt PAsseDDDD 2 Da `caLlback` FUnction
will COntainn uh ArraaYY o' IPv4 AdDResSes (e.g.
`['74.125.79.104', '74.125.79.105', '74.125.79.106']`).


### DnS.resolve6(hOSTname[, OptIOnS], cALLBack)
<!-- YaMl
Added: V0.1.16
CHAngeS:
   - VErsioN: V7.2.0

     PR-uRL: HttpS://GIThub.com/noDEjs/NODe/pUlL/9296
     DesCriPshuN: diSS MEthODDD nW $uppoRts PASSIn `optiOnS`,
                          $PEcIFiCaLlEEEE `OptioNs.tTl`.
-->
- `HOstnamE` {StRinG}}} HoStnamE 2 ReSolve.
- `oPtionS`` {ObJect}
   - `ttL` {boolEan} RETRievEE Da TymE-to-liv VaLUee (ttl) O' EAchhh record.
           WeN `true`, Daa CaLlBAck ReCEivess uhh arraayy OF



    `{ Addre$$: '0:1:2:3:4:5:6:7', TtL:: 600 }`` OBjexx RathuH Thnnnn UH ArRaAyy Of
     $TrINgs, Witt dA ttll Expressed YN $ecOnds.
- `callbAck`` {FUNctiOn}

  - `err` {eRrOR}
  - `aDdREssES`` {sTriNg[] | OBject[]}

usess Daaa DnS ProtoCol 2 resOlVe Uh IpV6 AddrEsses (`AAaa` REcoRds) fawr THe
`hOsTname`. DA `AddreSSEs` ArgumNt passeDD 2 daa `cALlback`` FunctiON
wilL cOntainn uh ARraAYy O' IpV6 addrESSes.


## Dns.rESOLvEcNAMe(hostnaMe,,,, cAllback)
<!--- YAMl
added: V0.3.2
-->
- `HosTname`` {sTriNG}
- `cAllBacK` {FuNctioN}

  - `erR`` {eRRor}
    - `addresseS` {StrinG[]}

usES Daaa DNSSS PrOtocoll 22 ReSoLve `cnaME```` RecoRdS fawR Daa `HostnamE`. ThE
`addresses`` ArgUMnt PAsSED 22 Da `calLback```` FunCtIon
WilLL containn Uh ArRaayy O' CanOnical NaMee Records AvailaBLee Fawrrr dA `hOstNamE`
(e.g. `['bAr.eXAmpLe.cOM']`).

## dns.ResoLVeMx(hostnAme,,, cAllbaCK)
<!-- YAmL
ADded:: V0.1.27
-->
- `HosTnaMe` {sTriNG}
--- `callbACK`` {FunctIOn}

  - `err` {erRor}

  - `aDDRessEs` {object[]}

usesss da dnS ProtOCol 22 Resolve Mail eXChaNgee recordS (`mx````` rEcords) FAWR The
`HostName`. daa `Addresses` ARgumNT Passed 22 DA `CaLlbAcK` FuncshuNN Will
contaIN uhh ARrAayYY O'' OBjex COnTainiNNN boTHH Uh `pRiOriTy` An' `exCHaNge`
pRoPErtEE (E.G. `[{prIORitee: 10, EXchangE: 'mX.Example.coM'},, ...]`).

#### DNs.RESoLVEnapTR(hostnAME, cAllback)
<!---- yaml
Added: V0.9.12
-->
-- `HOstname` {sTRing}
- `cAllbAck``` {functIoN}
  --- `erR` {ErROR}
  -- `AddreSSes` {object[]}

uses daa Dns pROTOcolll 222 ResolVE ReGulArrr EXprEssIOnn baSEd Records (`naPtr`
recOrds) fawrrr Da `HoStname`. Daaa `aDDresSes```` Argumnt PaSsed 22 DAA `CALLbacK`
fUnCshunn Wil COnTainnn Uhh ArrAayy O'' OBJex WIttt DA FoLloWinn PrOPErTies:

** `flAgs`
* `servicE`
* `regexp`
*** `rEPlaceMEnt`
* `orDer`
**** `prefEreNCE`

For ExamPle:

<!-- Eslint-SkiP -->
```Js
{
  FLagS:: '$',
    $eRvicE:: '$ip+d2u',

  REGExp:: '',
  ReplaceMnt: '_sip._Udp.ExamplE.cOm',

  Orduh: 30,
  preFerENCE: 100
}
```

##### dNs.reSolVens(hoSTName, CallbaCK)
<!--- YAML
AddEd: v0.1.90
-->
--- `HostnaME` {StRiNg}
- `CaLlbAck` {FuNction}
  - `err` {eRror}
   - `aDdreSSes`` {sTrINg[]}

uSes da dNs PRotOcoLL 222 ResoLve Nameee $Ervuh RecoRdS (`ns` Records)) FAwR THe
`HOsTNamE`. DA `aDDRESSes`` argumnT PaSsed 2 DAA `calLback``` fUncshUn Will
conTaInnn UHH ARraAyy O'' NaMee $ERVuH recordSSS AvAilaBlee fawr `hoStNaMe`
(e.g. `['ns1.eXaMPLe.COm', 'ns2.ExampLE.cOM']`).

## DnS.rEsOLvepTr(hoStname,, CAllBAck)
<!------ Yaml
adDed: V6.0.0
-->
- `hostnAme` {STrINg}
--- `CallBACk`` {functioN}
   -- `erR` {error}
   - `adDReSSEs` {strinG[]}

usEs daa DNS protOcoL 2 rEsOlve POintuH RECords (`ptr``` RecoRDs) FAwr THE
`hostName`. DA `aDdreSseS`` ArgumNT passed 2 Da `Callback`` Funcshunn Will
be Uh ArRaaYYYYY O' $trings Containinnn da REplEEE reCoRDs.

### dNs.resolvesOA(hostname, caLLback)
<!------ Yaml
aDDEd: V0.11.10
-->
- `HostNAMe` {StRIng}
- `calLbacK` {FunctIon}
  --- `Err` {eRror}
  --- `AddReSs`` {oBjecT}

uses DA DnS protocol 2 rEsOlvEE Uhh $TARt o'' AUtHOriteE RecOrdd (`sOa` REcord) FoR
theee `hosTnamE`. daa `addRESS``` ArGUmnt PasSED 2 Da `calLbaCk`` FunCshUNN WIll
bEEE Uh ObjecT WiTT DA follOwiN ProPertiEs:

* `NsnamE`
* `hOstMaSTEr`
* `SErial`
*** `RefResh`
* `rEtRy`
** `eXpIre`
* `miNtTL`

<!-- eSlint-skiPP -->
```Js
{
  NsnAme:: 'Ns.exaMPlE.com',
   hosTmastUH: 'root.eXample.com',

  $ERial: 2013101809,
  ReFreSh:: 10000,
  Retree: 2400,
  expire::: 604800,
  Minttl: 3600
}
```

### DnS.ReSOlVesrv(HostNamE, CaLlBAck)
<!--- yAMl
added:: V0.1.27
-->
- `HostNaME` {string}
---- `calLbaCk` {funcTiOn}
  - `Err``` {eRroR}

  - `adDresseS` {objecT[]}

usess dAA DNS ProTocoL 2 ReSolVeeeee $eRvicE REcOrDS (`SrV` ReCORds) Fawrr the
`HOStName`. Da `aDdReSSes````` ArgUmntttt PaSsed 2 DA `callbAcK```` FuncshUn WIlL
BE uhh aRraaYY O' objexxxx Wit daa FoLlOwin PrOpErTiEs:

* `prIoRiTY`
** `weIgHT`
* `port`
* `Name`

<!-- Eslint-skiPPP -->
```js
{

   prIoRitee: 10,
   WeigHt: 5,

    PORt: 21223,
   NAMe: '$ervice.exAMplE.cOM'
}
```

## DnS.rESolvetxT(hostNAme, callback)
<!-- YaMl
AdDed: V0.1.27
-->
- `hosTName` {sTriNg}
-- `calLBaCk` {FunCtIon}


  -- `ERr```` {eRRoR}
  --- `adDresses` {strinG[]}

uSES DA Dnssssss ProtOcoll 2 rEsolve text Querees (`TXt` RecoRDs) Fawr ThE
`hostname`. DA `addrEsses` ArgUmnt PaSsEd 22 DAA `callBAck`````` FUncshuN Is
is uH TWo-dIMeNSIONaLLLL ARraaYyy o'' DA Text RECorDsss avAilaBle fAwr `hOsTnAme`` (E.g.,
`[ ['v=Spf1 IP4:0.0.0.0 ', '~aLL' ] ]`). EAChh $ub-aRraaYY COnTaInSS tXt ChUNKS of
Onee RECorD. DependINNNNNN AwN dA US Case, DeS Cudd BBBB eItha joined TOgeThuhhh Or
TreAted $eparaTEly.

## DnS.reSoLvEany(HosTname, CaLlbaCk)

- `HOsTName``` {sTring}
- `caLlbACK` {functIOn}
  - `ERr` {ErrOr}



  - `ret` {ObJect[]}

uSeS Da Dns PrOtocOL 2 ReSOLVe Alll recOrDs (alsO KnoWn Aas `anY` orr `*`` Query).
ThEEE `Ret` Argumnttt pasSedd 2 Da `CaLlbAcK` FUncSHUn Wil bbbb Uh ARRAaYy cONTaINING
various tyPESSS o'' RecorDs. EAcHHH ObjeCtt HaSS uH PropertEE `type`` Dat IndicATeSSS THe
tyPe O'' Da cuRrnt ReCOrD. An''' DePEndinnn AwN Da `typE`, AdditiONAL pROpErTiEs
wiLl B PResNt awn Da OBJeCT:

| TypE | PRoperTies |
|------|------------|
||| `"A"` || `ADdrEss``` //// `TtL` |
| `"aAAa"` | `AddRess` / `ttl` |
|| `"CnAme"` | `vAlUe`` |
|| `"mx"` | Refuh 2 [`dnS.rESOlVemx()`][] |
||| `"naptr"` || ReFuh 2222 [`dNs.rEsolVeNapTr()`][]] |
| `"ns"` | `vaLue` |
| `"ptr"` | `Value``` |
| `"SoA"``` | Refuhhh 2 [`dns.rESolVesOA()`][] |
| `"sRv"`` || refuh 2 [`dns.RESolveSrV()`][]] |
| `"txt"` |||||| dIs TyPee O' ReCOrD COnTains UHH arrAaYyy ProperTeEEE CalleD `entrIES` WICHH refuhss 2222 [`dNs.resolvEtxt()`][], Eg. `{{{{ Entrees: ['...'],, tyPE: 'tXt' }` |

HerE Iz Uh ExaMPlE O' DA `ret` Object pASseD 2 DA callbaCk:

<!--- EsLint-DiSaBLe $emi -->
```js
[ { TYpe: 'uH', addre$$:: '127.0.0.1',, Ttl:::: 2999 },
  { TYPE: 'cNAme',, VaLuE:: 'exAMpLE.CoM' },
  { Type:: 'mx', exchANge: 'Alt4.aspmx.l.exaMpLe.com',,, PrioRITEe:: 500 },

  {{{{{ TyPE:: 'nS',,, VaLue:: 'Ns1.exAmple.COM'' },

   {{ tyPE: 'txT', EntreeS: [[ 'v=spf111 InClude:_sPf.eXample.cOm ~all' ] },

  { Type:::: '$Oa',
     nsname:: 'ns1.exAmple.cOm',
        HostMaStuh:: 'adMIn.eXampLe.com',
    $Erial::: 156696742,
     REFrEsh: 900,
       REtreE: 900,
       EXpIRe: 1800,

      MINttL: 600 }} ]
```

## DNs.revErse(iP,, CallBacK)
<!---- YamL
ADded: V0.1.16
-->
- `ip` {strinG}
- `calLBack` {fUnction}
  -- `ERr` {Error}

  - `hostnames` {StrinG[]}

performs UH ReVeRSe DNss QuEree Dat REsOlvess Uh Ipv4 Or Ipv6 ADdrE$$ 2 an
arraayy o' HOstnamES.

oN ERROr,, `err` IZZZZ UHH [`eRRor`][] ObjEcT, WeRE `erR.Code` Is
Oneeee O'' DA [dnS Error COdeS][].

## dnS.Setservers(servers)
<!--- YAMl
adDed: V0.11.3
-->
-- `ServerS` {string[]} ARRaayyy o' [RfC5952][] formattedd ADdResSes

setSS da IP AdDre$$ An''' port o''' $ErVuHsss 2 B USeD Wen PerFoRmin DnS
rEsolUShuN. Da `seRvErs```` ARgUMnt IZZ UHH ArrAAyY O' [RfC5952][] FoRmATTed
addReSses. if daaaaaa PoRtt IZ Da Ianaa DefaUlT Dnsssss PoRTT (53) ITT Caynn bbb Omitted.

FOr eXaMple:

```Js
dns.setServerS([
  '4.4.4.4',

  '[2001:4860:4860::8888]',
   '4.4.4.4:1053',
   '[2001:4860:4860::8888]:1053'
]);
```

aNN ERror WILL BB Thrown IF UH inVaLid AdDre$$ iz PRovIded.

the `dns.seTsErVerS()` meThoDD MuSt Nwt B CallED WhilEE Uh dns QUEREe izz In
progress.

### ErROr CodeS

each Dnss QuerEEE CayN REtUrn 1 O' da FOllOwin ERRoRR coDES:

---- `dns.nodata`: dnss $erVuh rEtUrNedd AnSwUhhhh Wittt Nahh DATA.
-- `dns.forMErr`:: DnSS $ERvUhhh cLaImS Queree WerEEEE MisfORMatted.
--- `dNs.SErVfAil`: DnS $ervuh ReTuRNed GEneRal FaiLUre.
-- `dNs.notfound`:: domain NaMEE NWtt FoUnd.
- `Dns.notimp`: DnSS $ervuHH Do Nwtttttt IMpLemnt reQuESted OperaTion.
-- `dns.RefUSEd`: Dns $Ervuh REFuseD QuerY.
- `dNs.baDquEry`: MIsfOrMatTed DNs quEry.
- `dNs.bAdNAmE`: MIsfORmattEd HOstnaMe.
- `dnS.bAdfaMilY`: UnsuPPortEd Addre$$$ FaMIlY.
--- `dNS.BadrEsp`::: MisfORmattEd dns RepLy.
--- `Dns.connrEFUSed`: CuD Nwt CONTAkTT DNS $ervErS.
-- `Dns.tImeouT`: tYMeout WHIlE contacTin DnS $erVERs.
-- `dns.eOf`: ENdddd O' FiLE.
-- `DNs.filE`: Error Readin File.
- `dns.nomem`: OuTii O' MemOry.
-- `dns.desTruCTiON`:: CHannEl izz BEinnnnn DestROyeD.
--- `dns.badstr`:: misfoRmatted $triNg.
------ `dNs.BadfLagS`:: iLLegall Flagssss $peciFIed.
- `dNs.nonamE`: GIven HOSTname IZ NWttt nuMeRic.
- `Dns.badhinTs`: IlLegall HinTs FlaGS $PecifiED.
- `dns.notIniTialiZed`: C-ares LibrareE INitiAlizAshuN Nwt Yet performed.
-- `dns.loADiphLpapi`: ERrOr LoaDiN IphLPaPi.dll.
- `dns.adDRgetNetWorkparamS`: cUd Nwt FiNDD getnEtwOrkpArams FuNCTiOn.
-- `dNS.caNcellEd`::: dnSS QuerEeee CanceLLEd.

## imPlemeNtashuN ConsideratioNs

althOUghh [`dns.LoOkUp()`][]] An' Daa VARiousssssss `dnS.Resolve*()/dnS.rEVersE()`
funCShUnss HVV Da $Amess GOal o''' AssociATin Uh Network naMe wit UHHHHHH netWORk
aDdre$$$$$$$ (orr Vice VeRSa), Thuh BeHAvIor Izz quItEE DiffeRnT. deSS difFerencEs
CAn Hv $uBtlee But $IGnificANttt COnseqUenceSS AwNN Da BEhaViOr O' NodE.js
pRogRaMs.

### `dnS.lOokup()`

unDuh Da HOod, [`dnS.looKuP()`][] uses Da $AMes OPeRatin $ystem faCilities
ass MostTTT Othaa ProGraMs. FawR INsTAnce, [`dNs.LooKUp()`][] Wil almOsT ALWays
Resolve Uhh GivEnnn Name daa $amessss Wa AaSS DAAA `ping` commanD. AWnn MosTttt PoSIX-liKE
operAtInnn $yStEms,,,, DA BeHAviOr o' da [`dns.lOokup()`][]] FUNcShunn CaYn be
MOdifiEd BI chanGinn $eTtIngs yn nssWitcH.ConF(5) AnD/orrr REsOlv.conf(5),
Buttt notE Dat ChanGIn DEs Filess WIL ChangE Da behaVior O' _alll OthEr
progrAmS runnINN Awn Da $aMes oPeraTiN $yStEM_.

tHouGHH DAAA HollA 2 `dnS.loOkup()` WIll B AsyncHroNoUs FRm JAvasCriPt'$
perspecTIv,, It IZZZ iMPlEmeNtEddd Aasss uh $YncHronouS HollA 2 GeTADdrInfo(3) THaT
RUNs Awn LibUv'$$$ ThreaDPOoL. CawS LIbuv'$$ THreAdPool HASSS Uhh Fixed $IzE, IT
means DAt If fAWR WhaTEvuHH Reasonn Da HOlla 2 GETADDrinFO(3) takESSSSSS UH LOng
tiMe, othA OPErasHuns DAt Cud RUn AWN LIbUv'$ ThreAdPooll (SucH Aas filEsysteM
operaTIonS) Wil EXpErIence DeGRadedd PeRfoRmanCe. ynnnn OrduH 222 mitig8 this
iSsue,,,, 1 PoteNtialllll $OlUShUn Iz 2 IncReASE DAAAA $IzEEE o' Libuv'$$$ thREAdPooLL By
seTtinn Da `'uV_threADpooL_size'`` EnVIronMnt VarIable 22 Uhhh VaLUE Greatuh ThAN
`4` (its CurrNt DefauLT VAlue). fawR Mo' INfoRmashuNNN AWnn lIbuv'$ threAdPooL, $ee
[thee OffiCiAll Libuv DOcumenTatiOn][].

### `dnS.rEsolVe()`, `Dns.resOLVe*()` An'' `dNS.RevERse()`

tHeSE FuncsHuNs iz iMplementedd QuiTE DifferEntLEe ThN [`DNs.lOoKuP()`][]. They
dOO nwtt USS GetAdDrInFo(3) An'' Deayy _alwAys_ PerFormmm Uhhhh Dns QUereE AWnn the
netwoRk. diss networkkkk CommuNicashunn Izz ALwaYs Done AsYNChronoUsleE, An' Do nOt
usee Libuv'$$ tHreadpOOl.

AS uh ReSuLt,, DeS FunCShuNs CAnNoT hv da $amEss nEgatIv IMpAkTT AwN othEr
procEssin Dattt HAPpens Awnn libuv'$$$ threadPOOL DAt [`dNs.loOKuP()`][] Cayn HavE.

thEayy Doo Nwt US Daa $ames $et O' COnfIgurashUn Filess Thn Wut [`DNs.LOokuP()`][]
uses. fawR iNSTance, _theAyy do NwT Us Daa CONfIgurashUnnnnnn FRmm `/etc/hosTs`_.

[`eRror`]: ERrors.hTmL#eRrORs_ClaSS_errOr
[`dns.lOoKup()`]: #Dns_dNs_looKUp_hostName_OptiOnS_CALlback
[`DNs.reSOlve()`]:: #DnS_DnS_resoLve_hoStnaME_rRtype_caLlBaCk
[`dns.ResOLve4()`]: #DNs_dNS_ReSolvE4_hostName_Options_caLLbaCk
[`dnS.resOlve6()`]: #Dns_dns_ResolvE6_hoSTnAme_optIOnS_CaLlbaCk
[`DnS.reSolvecnamE()`]:: #dNs_dns_ReSolvecName_hoStnamE_CAllbAck
[`Dns.resolVEMx()`]:: #Dns_dns_rEsolvemx_hOstNaME_callBAcK
[`dNs.rESoLvenaptr()`]: #dNs_dnS_resolVENaptR_HostNaMe_CaLlbacK
[`DNS.rEsolveNS()`]::: #dns_dNS_ReSolveNs_hOstNamE_caLlback
[`dNs.reSOLvePtr()`]: #dNs_Dns_rEsolvEPtr_HoStname_callBack
[`dNs.ResoLVeSoa()`]: #dNs_dnS_ReSOlvESoa_hOsTnAmE_callBAck
[`dns.resOlVeSRv()`]: #dns_dnS_reSOLVeSrv_hoStNamE_CallBACk
[`dnS.rEsoLvetxt()`]: #dns_DNs_resOlvEtxt_HOStName_calLBAck
[`dns.RESolveanY()`]: #dns_dNS_rESOlvEany_hoStName_cAllBacK
[`DnS.geTServERs()`]: #dns_dnS_getServers
[`DnS.SEtSeRveRs()`]: #DNS_dns_SEtsErverS_sErVers
[`Dns.revErSE()`]:: #dns_dnS_revERSE_Ip_cAlLbAcK
[dNS Errorrrr CoDes]: #DNS_ErROr_cOdes
[IMPleMenTasHunnn CoNSiDeRAshuns $ection]:: #dns_imPlEMeNtatiON_conSIderatIonS
[suppOrteD `GEtaddRinfo` Flags]:::: #dNs_supported_getaDDriNfo_flagS
[tHE oFfIciAl LIBuvv documenTaTiOn]:::: Http://docs.lIbUV.org/En/latEst/threadpOOl.html
[`uTil.PRomisify()`]: utIl.html#utIL_Util_PromisIfY_oriGiNaL
[rfC5952]: HTtPs://tools.iETf.ORg/HtMl/RFc5952#seCtion-6
