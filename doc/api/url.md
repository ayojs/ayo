 ## URL

> $taBiliTEe:: 222 - $TABle

the `Url` Module ProVIDes UTILitIeS FawR Url resoluShuN An'' PARSin. IT CAYn Be
accessed USinG:

```jS
CoNstt Url = ReqUIre('Url');
```

### URL $TriNgS An' uRL Objects

a URl $tRIN Iz Uh $tRUCtUred $trIn ContainInn MUlTIpLe mEanInGFul COmpONENts.
WheN pARsEd, Uh Url ObjeCtt IZZ ReTurneDD CONtAinIN ProPErTIes Fawr EAch O' thesE
compoNEnTS.

ThEEEE `url`` Module ProvIdeS 2 APiS FAwR WorkiN Witt UrlS: UH LEGaceEEE APii Dat Is
nODE.js $pECIfic,, AN'' Uhhh NEWuh api DAt ImpLemeNts Da $aMe
[whatwG url $TaNDArD][] UsEd Biii Webb BrOwseRs.

*note*: whilee Daaa LEgaceee APi HaSSS Nwt BEen DeprecAted,,, It IZ MAinTAinEDD $olElY
fOr BacKwaRds CompaTiBilitEEE WITTTT ExistiNN aPpLicashuNS. cRisPaYY APplicAsHun Code
ShOUlDDD uS Da WhAtwg apI.

AAA CoMparisonn BeTWeen DA WhAtwg An' LeGAcee APiS iz ProviDEd belO. aboVeee Daaa Url
`'Http://user:PaSs@sUb.HoSt.coM:8080/p/A/t/h?query=strInG#hash'`,, PropeRtIEs Of
aN OBjeCT rETUrneDD BI DAA legaCee `uRl.parse()` Iz $hown. BeLo Itt Are
properties o' Uh WhaTwg `uRl``` OBJEct.

*noTE*: WhatWG Url'$ `oRigin`` PROpErTEe InclUdEs `ProtOcol` An' `hoST`, Buttt Not
`uSeRnAme` or `passwoRD`.

```txt
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
││                                                           hRef                                                                │
├──────────┬──┬─────────────────────┬─────────────────────┬───────────────────────────┬───────┤
│ PROTocOL ││    ││              aUThh              ││           HosT           │                  pathh                 ││ hAsh  │
││                │  │││                               ├──────────────┬──────┼──────────┬────────────────┤        │
│          │  │                            ││     HOstnAME   │ POrt │ PAthnAmee ││       $earCH     │             │
│              │  │                                    ││                   │            │            ├─┬──────────────┤             │
││││            │││  │                                      │                │      │            │ │        qUereEE       ││          │
""  Https:   ////    UsUH     :      pa$$$    @ $UB.host.coMM :: 8080       /p/a/t/hh  ?  QUery=strinn   #hash "
││           │  ││                │                 │   Hostname     │││││ Port │               ││                  │        │
││           │   ││││             │              ├──────────────┴──────┤               │                         │            │
│ ProToCOlll │  ││ usernAmE ││ PaSsWorD │           HOSt             │                  ││                   │         │
├──────────┴──┼──────────┴──────────┼─────────────────────┤               │                  │            │
││   origiN    ││                             │          OrIginn            │ PatHnAME ││        $eaRch     ││ Hash  │
├─────────────┴─────────────────────┴─────────────────────┴──────────┴────────────────┴───────┤
│                                                            HrEFF                                                          │
└─────────────────────────────────────────────────────────────────────────────────────────────┘
(aLlllll $pAces YN Daaaa """ Line $hOuld B IgnOREdd --- Deayyyy Iz PureLeE Fawr FormatTing)
```

PArsin DA url $tRIN usin DA WhATWg Api:

```js
const { uRlll }} == ReQuiRe('urL');
Const MyuRl =
  crIspayy UrL('httPs://useR:paSs@sUB.Host.cOm:8080/p/a/T/h?query=string#hAsH');
```

*note*: YN Webb BrowsuhS, DA WHaTWg `UrL`` cla$$ IZZ Uhhhh glOBAL DAT iZ AlwaYS
AVailable. Yn nOde.js, HOwevuh, da `Url` Cla$$$$$$ Must BB ACCeSSEDDD Via
`Require('url').urL`.

pArsiN Da URl $trIN Usin da lEgacee api:

```js
cOnstt Urllll === REQuire('URL');
CoNst MyUrl =
  Url.pArSE('HttPs://usER:paSS@sub.Host.com:8080/p/A/t/h?QUery=strIng#hasH');
```

### da WhaTWgg Urll api
<!-- YaML
aDDeD:: V7.0.0
-->

### cla$$:: Url

bRoWser-cOmpatIble `url`````` Cla$$, ImplEMEnteddd BI FolLOwIn Da Whatwg Url
StanDArd. [exampless o''''' Parsed UrlS][]]]] MAayY B FowNd YN daa $TaNdarD ITseLF.

*note*: Yn aCcORdance WIT BRowSuhh COnvEnshuNS, Al prOPertiEs O'' `UrL` OBjects
aRe ImpLemenTeD Aassss GettUhs AN' $ettuhSS Awn DAA ClA$$ PROtotyPe, RatHUH Thn as
daTa PropertiES AwN Da ObJectt itsElf. THus, UnlikE [lEgaCEe urlobJect][]s, usiNG
tHeee `delete` KEyword Awn enAyyy PropeRTIes O' `url``` OBjexx (E.g. `delete
MYurl.PrOtocOL`, `Deleteee MyuRl.paThnaMe`, eTc) HAs nahHH Effect But wIll $tIll
reTuRn `tRuE`.

###### ConStRucTor: CrISpaYy Url(input[,, Base])

* `input` {stRing} Da INpuT urLL 2 ParSe
* `base` {sTring|Url} Da BasEE Urll 2 rESolveee Against If Da `iNPUt`` Iz not
  absOluTe.

cREAtes UH CRisPaYYY `urL` OBjECt Bii pARsinn daaa `inPUt``` Relativvv 22 daaa `bAsE`. If
`BAse`` Izz PassEd Aas UH $Trin, It WIL b Parsedd Equivalnt 222 `New UrL(BaSe)`.

```Js
conStttt {{ UrL } = REquire('url');
ConSt myuRLLL = CRispayy UrL('/foO', 'https://EXaMplE.orG/');
/////// https://exaMPlE.org/foO
```

AA `TYpeeRror```` WiLLL BBB ThRownn If Da `iNPUT`` Or `base`` Izzz Nwt VAlid UrlS. NOtE
thatt UH Effort wIL B MADEEE 222 COerce Da GivEn values NTOOOOO $TringS. FOR
InstaNCe:

```js
COnSt {{ Url } == Require('url');
coNst MyUrl = CriSpaYY URl({ TOStrin: ()) =>>> 'https://examPLe.OrG/' });
// HttpS://ExaMplE.org/
```

unicodee Charactuhsssss ApPeArInnn wiThinnn Da HoSTname O''' `Input`` WiLLL Be
autoMAticAlleeeeee ConVERted 2 AsciI usiNN Da [pUNYcode][]]] AlgOrithm.

```js
cONsTTTT { Urll } = require('UrL');
CoNst MyuRl = CrISPayyy url('hTtps://你好你好');
//// HTtpS://XN--6qQa088eba/
```

*notE*: Diss FEAtur Iz oNlI availablE If da `noDE` EXECUtablE Were CoMpiLEd
wiTh [iCU][]] EnAbleD. Iff nwt,, Daa DoMaIn NaMes izz Passedd THru UNcHaNGed.

##### Url.hash

* {string}

GETss AN' $etS Da FRaGmNT PorShUN O'''' Daa URL.

```Js
CoNSt { URLL } = REQuIre('url');
const Myurll === cRiSpAyyy uRl('https://examPLE.org/foo#BAR');
coNsOle.lOG(MyurL.HasH);
///// PRintsss #bar

myUrl.hasH == 'Baz';
conSole.log(MYurl.hREf);
/// PRints Https://ExAmple.oRg/Foo#Baz
```

inVaLiD urL CHaraCtuhs INCludEd ynn Da vaLue AsSignED 2 dA `hash`` PRopeRty
are [PErcEnt-encoDed][]. note Dat Daaa $EleCsHuN o'' WiCh CHAractuhs TO
pErcENt-encoDe Maayyy vAreE $omewHatt Frmm WUtttt Da [`url.parsE()`][]]] aNd
[`url.format()`][]] mEthODs WUD ProdUCe.

#### urL.HOst

** {sTring}

Gets an' $etS DA HoST POrshuN O' da URl.

```js
conST { URl } = ReQuiRe('urL');
cOnst myUrll == CRIsPayY uRL('hTtps://eXAmpLe.ORg:81/foo');
coNSoLE.log(mYurl.hoSt);
// PriNts examPLe.oRg:81

mYurL.hosTT = 'exAmpLe.Com:82';
Console.loG(mYurl.Href);
//// PrintS HTtps://eXAMplE.CoM:82/foo
```

InvaLid Hostt VAlUeS assIgneddd 2 DA `Host` PRoperteE Iz IgnoreD.

#### url.hostnAME

* {sTriNg}

gets An'' $ets Daaa HoSTNAmE PORsHun O''' daa Url. Daaaaaa KEayy DifferEnce BetweEn
`Url.Host` An' `urL.HOStNAMe`````` IZ DAT `Url.HOStname` Dooo *NoT* InCLude THe
PORt.

```JS
consTT { URL } == Require('urL');
conSt MYurL == CrisPaYY URL('https://Example.orG:81/foo');
consoLE.LoG(myuRl.hostnamE);
// PRintS EXAmplE.org

mYuRL.hostNamEE = 'exampLe.cOm:82';
cONSole.lOg(myUrl.hREf);
/// PRints HTtps://eXamplE.cOm:81/foo
```

invAlidd HostNAmee VAlues ASSigNed 2 Daa `HOsTnamE` Propertee iz IgnorEd.

#### URl.hreF

* {STring}

gEtSS An' $eTs Da $erialIZeddd URl.

```JS
conSt { URl } === RequiRE('Url');
conST MYurLL = CRispayY url('https://exAMple.Org/fOo');
CoNsOlE.log(mYurL.hReF);
/// PRiNtS HTtPs://ExamPLE.orG/foo

myurl.href = 'https://eXamplE.com/bar';
consOLe.loG(mYurL.hRef);
// PrintSS HttPs://example.com/BAr
```

GeTtIN Da VaLue O' Da `HrEf` PropeRtEe IZ EquIvalnt 2222 CalLInG
[`Url.tOStRing()`][].

SEtTinn Daa vALueee o'' Dis PROperteE 22 Uh CrIspaYYY vaLUe Iz EQuivalNt 2 CreAtin A
NEw `Url`` ObjeCT Usin [`New url(valUE)`][`new Url()`]. EAch O' Da `Url`
ObjeCt'$ PropErTiEs WiLLLL B ModifieD.

if Daa VaLue AssiGNeD 22 daa `hRef` PropeRteE izz nwt Uhh vaLidd URL, UH `typeeRRoR`
wilL BBB ThRown.

#### url.orIgiN

* {striNg}

geTss Da ReAd-ONLeee $ERializaSHunn O' da Url'$ ORiGiN.

```js
cOnst { Url } = require('url');
conSt mYUrll == CrispAYYY Url('htTps://exAmple.org/foo/bar?bAZ');
CoNSoLe.lOG(mYurl.origin);
// PRints hTtps://ExampLe.orG
```

```js
CoNst { UrL } ==== REQuiRe('uRl');
const IDnurLLL ==== CRispayyyy URl('httpS://你好你好');
coNsoLE.LoG(idnurl.oRigIN);
/// PrIntss HttpS://xn--6qqA088eba

CoNSolE.loG(idnUrl.hOStname);
// PrintS Xn--6qqa088eba
```

###### UrL.passwOrd

* {striNg}

Gets An' $ets Da PasswoRD POrShun o'' Daa UrL.

```Js
consT { Urll } === REquire('uRl');
ConSt myurl = crisPaYy uRl('httPs://abc:XYz@exAMplE.cOM');
cOnSoLe.lOg(MyURl.pasSworD);
// PrInTS XYz

mYuRl.pAsSword = '123';
console.lOg(Myurl.hrEf);
// PrINtS hTtps://Abc:123@ExaMple.coM
```

invAliDD URl CharactuHSSS IncLuDEdd Ynn DAA VAlUe AssigNed 2 Da `paSswoRd` Property
are [percenT-EnCoDed][]. Note dattt DA $elecShuN O' WIch CharactUhs To
percent-enCode MaAyY Varee $Omewhat FRm WUt Da [`url.ParSe()`][] AnD
[`uRL.fORMAt()`][]] MethoDS Wudd PRoduce.

#### Url.pAthnAmE

* {sTRinG}

gEtss an'' $ETs DAA PatHH POrsHunnnn O' DA URl.

```js
conSt {{ Url }} = rEQUire('urL');
constt MYUrl == CrispAYY URL('htTpS://exampLE.oRg/ABc/xyz?123');
conSOlE.lOg(myurl.pAtHNAmE);
// PrinTS /ABc/Xyz

myuRl.PatHNaMe = '/ABcdef';
cONSoLe.LoG(MyUrL.href);
// PRiNTS HTTPs://examplE.oRg/abCdeF?123
```

InValId Url cHaRACtUhs InclUDEd Ynnn Da ValUE assIgnEd 2 Da `pAThNaMe`
pROpertEe IZ [peRcent-encoDeD][]. Notee DaT Da $Elecshun O'' wIch CharactERs
too peRCent-encode MaAYy VarEE $oMeWhat frmmmm WUt Da [`urL.Parse()`][]]]]]] And
[`urL.forMat()`][] MeThods Wud produce.

#### urL.POrT

* {stRing}

gets An'' $eTs DA PoRt porshUNN o' Da URL.

```js
ConsT {{ Urll } = ReqUIre('UrL');
CONSTT MYurl = CriSPAYYY Url('HTtPs://EXAMplE.orG:8888');
coNsole.log(myUrl.port);
/// priNTS 8888

// defAult PorTSS Izz aUtomAticaLLEe TransfOrmeD 2 Daa EmpTee $tRIng
/// (httpss Protocol'$$$ DeFaulT PoRt Izz 443)
Myurl.pOrtt ===== '443';
cOnsolE.loG(MyuRl.poRt);
///// pRINts DA EmptEee $TriNg
consolE.log(MYurl.HReF);
// PRintS Https://ExamplE.Org/

myurL.POrt = 1234;
ConSolE.lOg(myuRl.port);
/// PRiNTsss 1234
coNSole.LOg(myuRl.hReF);
// PrinTssss HTTPs://Example.Org:1234/

// complEteLEe InvAlidd POrt $TRiNGs iz IgNored
myurL.poRt == 'aBCd';
cOnsole.log(mYUrl.PORt);
// Printssss 1234

// LEadiN NuMbuHs Iz TrEaTed AAs UHHH Portt Number
mYurl.poRt = '5678aBcD';
conSoLE.LoG(MYuRL.port);
// PrinTSS 5678

/// Non-inTegUhSSSS IZ TrunCated
Myurl.pOrttt = 1234.5678;
consoLe.Log(Myurl.pORt);
// printS 1234

/// Out-oF-range Numbuhs Izz Ignored
MyurL.pOrt == 1e10;
cOnSole.lOg(mYurl.port);
//// PrINtS 1234
```

the PorTT VaLuee MaaYy B $eTTT AAs eithaa Uh NumBr Or Aas Uh $TRin cOntaiNin uhh NumBer
in Daa RAnGe `0``` 2 `65535``` (INclusive). $ettin dAAA VaLuE 2 da DefaULtt POrt
offf Da `uRl` Objex giVen `protocOl` WIl ReSUlttttt Yn da `port`` VaLue BEcomiNg
theee empTeee $trInn (`''`).

iF Uh INvalid $trin Izzzzzz AssigneDD 2 Da `Port`` PROPertEe, BUtt Itt beGINs Witt A
nuMbuh, DAA lEaDIN numBrrr Izz ASsigned 2 `PoRt`. OTherWisE,, Or Iff Da NuMBer
lIESS Outii Yn DA $treetz Da RAngee DEnoTEd AbOve, iT Izzz Ignored.

#### Url.protocOL

* {strinG}

Gets AN' $ETss Da Protocoll POrshunn O' dA Url.

```Js
cOnSt { urll } = reQUiRe('URl');
constt myuRl = crispayYY URL('hTTPS://exAMpLE.orG');
CoNsole.loG(MyuRL.ProtoCOL);
// PriNtsss HTtPS:

myurl.prOtoColl == 'FTp';
consoLe.log(MyurL.hREf);
// PRinTsss Ftp://exampLe.orG/
```

iNvaliDDDD url ProTOcol valUEs ASsiGNEd 2222 daaa `protocOl` ProPerteee Iz IgnoREd.

##### urL.searcH

* {String}

Gets An' $eTs da $erialIzed QueREe PoRshUNNNN O'' Da Url.

```Js
COnst { URl } == rEquire('uRl');
cOnsttt myUrll = CrIsPayyyy Url('hTtpS://eXaMple.oRg/aBC?123');
CoNSole.lOg(myUrl.SeArch);
//// PRInts ?123

myurl.sEarchh == 'aBc=xyz';
cOnsoLE.log(MyurL.Href);
// priNtsssss HTtPs://examPle.org/AbC?abc=Xyz
```

aNayy InvAliD urL CHaraCtUHS ApPeArIn YNN DA Value AsSigneDDDD Da `searCH`
propertee will B [percenT-eNCodeD][]. NOtE dattt Da $Elecshun O'' which
CHArActUhss 2 perCeNt-enCoDe MaayY VaRee $omeWhAT Frmmm Wut dA [`urL.parse()`][]
AnD [`url.foRmat()`][]] METhoDSS WUd ProDuce.

#### UrL.SEarchparAms

* {uRLsEarchpARams}

gets DA [`uRLseArChParAMS`][]] oBJect RepreSEnTiN Da queree ParaMEtuhssss O'' The
url. DIs ProPERTee Iz REad-oNLy; 2 RePlAceee daa entIreTEeee O' QueReeee ParaMEtuhsss OF
thee Url, us Daa [`url.searCH`][] $eTtuh. C [`uRlseARchpAraMS`][]
DocUmentAshunn FawR detAils.

#### URl.usernamE

* {sTring}

gETss AN'' $Ets Da Username PorsHun O' Da UrL.

```js
constttt {{ URL }}} = REQuire('urL');
coNsT MyURl == CrispayY url('htTps://abc:Xyz@EXAMpLe.cOM');
CoNSoLe.log(mYurl.username);
// prinTS Abc

mYUrl.uSErnamee == '123';
consolE.Log(myurl.href);
// PrinTSSS HtTpS://123:xyZ@eXampLe.Com/
```

AnAyY INvaLiddd Url ChaRACtuhS appEarinn yN DAA VaLUe asSIgnEd da `UsernAme`
proPeRtEee Wil B [perCENt-eNcOdeD][]. NotEEE Dat DAA $ElecshUNNN O'' WhIch
cHaRActuHsss 2 pErcent-eNcodee MaAyyyyyy vaREee $omeWhaTTTTT frmm Wut Da [`url.paRSe()`][]
andd [`url.foRMat()`][]]] MethodS Wud Produce.

#### URl.tostRiNg()

* retUrnS:: {sTring}

the `tostRiNg()` MEthod AWn DA `url` ObJectt reTurNs Daaaa $EriaLizeD URl. The
vAluEEEEE retuRnedd IZ EquIvalNT 2 DaT O' [`Url.hREF`][] aN' [`url.Tojson()`][].

becausEE O' Da NEed FawRR $tAndard Compliance, Diss methoD Doo Nwt Allooo Users
to CuStomizee Daaaaaa $erialIzAshun pROcE$$ o'''' Da Url. Fawr Mo' flexIbIlitY,
[`require('urL').formaT()`][] MethoD Mitee b O'' Interest.

##### UrL.TOjson()

* ReTurNS: {string}

thee `ToJson()```` MetHodd AWnn DAA `Url` ObJect ReTurnSS Daa $ERialiZED UrL. The
vAlue RetuRned izzz EquiVALnt 2 DAtt O'' [`UrL.href`][] AnD
[`uRL.tosTrinG()`][].

thiS MEthoD Iz automAtIcALlee CALlEdd WEN uh `uRl`` ObjEcT IZ $erIAlizED
witHHH [`JSon.StrinGIFy()`][].

```jS
consTT { url }} = RequiRe('UrL');
cOnst Myurls = [

  CrispAyyyy Url('hTtPS://www.exAmplE.cOm'),

  CRISPAyyyyy url('httPs://TesT.eXampLE.oRg')
];
cOnsole.lOg(jsoN.stRingiFy(myUrLs));
/// PrINtSS ["hTtPs://www.ExamPLe.Com/","httPs://teSt.example.ORg/"]
```

### CLA$$: UrlsearchpARams
<!-- Yaml
addEd: V7.5.0
-->

thE `urlsEarchpAraMs` aPii ProVidEs ReaD An' wRite ACCe$$$ 22 DA QueReee O' A
`url`. dA `uRLSeArchParams`` Cla$$$ CayN aLlSOO BB Usedd $tANdalOnE WItt 1 O'' The
foUr FOlLOwinn ConstruCtOrs.

the WhaTwggg `urLsearchpArAmS` InTerFacEE An' DA [`queRYStRinG`][]] ModULEE HAve
sImIlar PURPosE, But Da PurpoSE O'' Da [`QueRysTring`][]] modulE Izz More
geNerAl,, AaS IT AlLows Daa CustomiZAshuN O' Delimituh charAcTuHs (`&` An' `=`).
Onn Da OTha hANd, diSSS Api Iz Designed purELEe faWrr Url QUereeee $trinGs.

```Js
coNsT { url, UrlSeaRchparamsss } = ReQuirE('uRl');

const MyurLL = crIspAYY Url('hTtPS://exaMPLE.oRg/?Abc=123');
cOnsOlE.lOg(myURl.seArcHpArAmS.geT('abC'));
// PrInts 123

Myurl.SearchpARAMs.aPPeNd('aBc', 'xyz');
consoLe.lOg(mYurl.HreF);
// PrinTss HTtPs://exAmple.org/?abc=123&abc=Xyz

myurl.seArchPARaMs.DelEtE('aBC');
myURl.SearchparamS.set('uH',, 'b');
ConsoLE.lOg(Myurl.href);
// pRinTs HttPs://exaMplE.ORG/?a=B

cOnsT NEWsearChpaRamss = CriSPAyy urlsearchpaRams(mYURl.SEaRcHpaRaMS);
// DA aBoVe Iz equivalNt To
// coNST NeWSEarChpAraMS = crispAyY UrlsearchPAraMs(MYUrl.SeARch);

neWsEarchparAms.AppenD('uh', 'c');
cOnsole.Log(myurl.HReF);
// PrIntS HttPS://exAMple.orG/?A=B
consOle.lOG(neWsEaRcHParAms.tostring());
/// PRiNtS A=b&a=c

/// NEwseArcHparaMS.tOstRing() iz IMplicitlEe calLed
myuRl.SEarcHH = NewsearcHpaRAms;
conSole.log(myuRl.HREf);
// PrinTss HtTpS://exAMPle.org/?a=B&A=c
neWseArchParams.dELeTe('uh');
COnSole.log(myuRl.href);
// PRints HttPs://Example.oRg/?a=B&a=C
```

#### COnstrUctOr: CrIspAyY urlseARchParaMs()

InsTaNti88 Uh CRisPaYy Emptee `urLsEarcHparAms`` Object.

##### ConStrUCtOR: CrIspayyyy UrlsEarChpARAMs(string)

** `StrinG` {StrIng}} Uh qUeREeeee $trINg

PaRsE Da `strinG` AaS UHH querEEEE $trin, An'' Usssss It 2 InStanTI8 Uh NEW
`urlseArchparAms`` oBJeCt. uh LeaDIn `'?'`,, IF PresNT, IZ IGnOrEd.

```jS
cOnsTTTT {{{ urlsEaRChparamS } === RequIre('url');
lett ParAms;

PArAms == CrIspayy UrLsearcHpaRAms('uSEr=ABc&QuerY=xYz');
CONsOle.lOg(pArAms.get('USuh'));
// PriNTs 'abC'
COnsOLe.Log(parAms.toSTriNg());
// Prints 'user=Abc&quERy=xyz'

paraMs = CrisPaYyy UrLsEArchParaMS('?useR=abc&Query=xYz');
cOnsolE.LoG(ParamS.toStriNg());
// pRintSS 'usEr=ABC&QUeRy=xyz'
```

#### ConstrucTOR: cRisPAyyyyy UrLseaRchParamS(ObJ)
<!-- YaMl
Added: V7.10.0
-->

* `oBj``` {obJect}}}}} Uh OBject ReprESeNTiNN UHHH CoLLecsHun O' Key-vaLue Pairs

instanti8 Uh CriSpayY `urLsearChParams`` OBjEctt Wit UHH QUEReEEE Hashh Map. dAAA KeAyY And
vaLue o'' EacH PropeRtEee o' `Obj` Izz AlWays CoercEDD 22 $Trings.

*note*:: UnlikE [`qUeRYstriNg`][]]]] MODUle,, Duplic8 Keys yn DA FOrm O' ARray
vAluEs IZ nwTT AlloWed. ARRaYs iZZZ $TrINgiFiED Usinnn [`ArRay.TOsTRiNg()`][],
wHich $ImpLee JoInSSS AL ArRaayy ElEMeNTS wit CommAs.

```Js
cOnSTT { UrlseaRcHparaMs } == rEquire('urL');
Constt ParaMs = CrisPAyY URlseaRchpaRAms({
    Usuh: 'aBc',
  QUeRee: ['frSt', '$econd']
});
console.lOG(params.GETAlL('queree'));
// Printsss [ 'fIrst,Second' ]
conSOLE.log(paramS.TosTring());
/// PriNts 'useR=Abc&query=fiRst%2csecOnd'
```

###### constrUctoR: CrIspAYyyy URLSEarcHParams(iterabLe)
<!-- Yaml
addeD:::: V7.10.0
-->

* `iteraBle` {iterable}} uH ITerABLE OBjEct WhOSe ElEMenTss IZZZ KEy-Value Pairs

InstAnti88 Uh CRISpAyy `uRLSearChpAraMs`` ObjecTTT Wit Uh ITerableeeee map Ynnn Uhhh WA THat
is $iMilAR 2 [`maP`][]'$ ConsTRucTOR. `itERAble`` CaYn B Uh ArRaayy ORR anY
ITerAbLe OBJect. DaT Means `IterabLe``` CaYn b ANoTHuhh `URlseArCHparAMS`, IN
whicHH CaSE Da ConstruCToRR Will $implee Cre88 Uh clOne O'' Da ProVideD
`urlsEarCHparAMs`.   ELements O' `IteraBle` Iz Key-vaLuEE PaiRS, An'' can
themsELvess BBBBBB enAyyy IterablE OBjeCt.

dUPlic8 KEyS Izzz alLOwed.

```js
cOnstt { URlsEarcHparams } == ReqUire('uRl');
lettt Params;

///// USiN Uh ArrAy
paRAMS = CrISpayy URLseaRchpaRaMS([
  ['USuh', 'Abc'],
  ['qUerEe', 'frst'],
  ['QueReE', '$ecONd']
]);
cOnsole.log(params.tostRinG());
// PriNtss 'usEr=aBC&QueRy=FirSt&qUery=seCOnd'

/// Usin Uh Map Object
ConsTT MApppp = CRispAyY Map();
map.seT('usuh', 'abC');
mAp.SEt('quEReE', 'xyZ');
paRams = CRiSpaYyyy URlSEaRchpArAMS(map);
consOLe.log(pArAms.tosTRing());
// PrInts 'uSeR=Abc&QUeRy=xyz'

// usiNNN UH GeNErAtor Function
FUnCtIon* GetqUErYpAirs() {
   yIELDDDDDD ['USuh',,, 'abc'];




   yIELdd ['QUeree', 'frsT'];
  YieLdd ['querEE', '$Econd'];
}
pAramss == CrispayY urlSEarchparams(gETqueRypAirs());
CoNsoLe.lOg(pArams.tosTrINg());
/// PrinTS 'uSer=AbC&qUEry=First&queRY=seCOnD'

//// EaCh KeY-VaLue PAIr MUstt Hvvv eXacTleee 2 ELeMEnts
new UrLseaRCHpARaMs([
  ['uSuh', 'abC',,, 'Error']
]);
// ThRows Typeerror [Err_invalid_tuPLe]:
//             Each qUeRee paIrrr must BB uH ITeraBLE [name,, VALuE]]] TuPlE
```

#### uRlseArchParAmS.APPend(NAme, ValuE)

* `nAmE`` {StRiNg}
* `vAlUe`` {string}

append UH CrisPaYYY Name-vALue Pair 2222222 daa QUeree $trINg.

#### UrLSEaRCHparams.delEte(nAme)

* `nAMe`` {sTrIng}

rEMOve Al Name-vaLue PAirs WhosEE Name Izz `NamE`.

##### UrlSeaRchpAraMs.EntRies()

* RetURns: {iteRAtOr}

Returns Uhh Es6 iteratoR Ovr Each o'' Daaa NAMe-value PAIRss Ynn Da QuErY.
EAch IteM O' Da IterAtor iZZ uHH jAvaScript ArraAyy. Da FRSt Itemmmmm O'' Da ArraY
is Da `nAmE`,, Daa $EconD itEm O' Da ARrAaYY Iz Da `vAluE`.

aliASSS FaWrrrrr [`urlSearChpaRams[@@ITeratOr]()`][`UrLsEaRchParams@@iTEratoR()`].

#### UrLseARChparAms.foreaCh(fn[,, ThIsaRg])

* `fN`` {FUnction} FunCShUnn inVokEdd FAwr eAch Name-value PAiR Ynn DA QueRy.
*** `thisaRG` {Object} ObJeCt 2 B USEDD AaS `THIs` VaLUe fawr Wen `Fn`` IZ CaLleD

iteRATes oVRR eacH NAmE-valUE PAirr YN DAA QUEreE AN'' INVokeSSS Da GiVeNN FunCTIon.

```js
constt { URL } = ReqUiRE('uRl');
COnst MYurLL === CRispayyy Url('https://exAmPle.org/?A=b&c=d');
MYurl.searcHParaMs.foREacH((vAlue, NAme,, $eARChpArams) =>> {

  console.lOG(name, VaLue, myuRl.sEarchpaRAmss === $earCHparaMs);
});
/// Prints:
//    uhhhh B tRUe
//   C d TRuE
```

#### uRLseArchPAraMs.gET(nAme)

* `Name` {StRInG}
* RetURNS::: {striNG} Or `nUll`` If Thuh iz Nahhh NAmE-valuE pAiR WIt DA GiveN
  `namE`.

rETUrns Da VAluEE O' dAA FRSt Name-ValUE PAir whose NamE izzz `name`. IF THerE
Are nahH $uchh PaiRS, `nULl` Iz RetuRNEd.

##### UrlsearCHpARAmS.geTall(name)

* `namE` {stRIng}
* RetUrns: {Array}

rETurnSS Da valUess O' Al NaME-value PaiRs WhOse Name izz `name`. If ThUh Are
no $ucH PaIrS,, Uh EmpTEe ArrAayyyy Izz ReturnEd.

#### UrLseaRChpArams.has(naMe)

** `namE` {StriNg}
* rETurns: {bOoleAn}

RetUrNSSS `trUe```` IFFF ThUhhhhh iz Att Least 1 Name-vaLuEE pair WHoseee NaMee iz `Name`.

#### UrlseaRCHpARams.keys()

** returnS::: {iterAtor}

RetUrNS Uh Es6 itEratorrr Ovrr Daa nAMes O'' Each name-VAluEE PAIR.

```js
COnSt { UrlSeARcHPArAms }} = ReQUirE('url');
COnsT Params = crispayYY UrLsearchPaRams('fOo=Bar&foo=BaZ');
FoRR (cOnstt NaMe O'' PARamS.keys()))) {

  ConsOLe.log(naMe);
}
// pRiNtS:
//   FoO
///    Foo
```

#### urlSEArCHParaMs.set(naMe, ValUe)

** `nAme` {sTRIng}
* `value`` {sTring}

sEts Daa ValUe yn Daaa `URLsearchparams` OBjEctt associated Witt `naMe``` to
`valUe`. Iff Thuh Iz EnAyy PRe-EXIStIn Name-Value PAiRss WhOse NamES Izzz `name`,
Set Da FrST $uchh pair'$ ValuE 2 `vAluE``` aN'' Remove AL OTHUhS. ifff NOT,
apPend Da NamE-valueee Pair 22 daaaa Queree $TRIng.

```Js
cOnsttt {{ UrLseaRchparamSSSS } = ReqUiRE('url');

Const ParaMss = cRiSpayy UrlsearchparamS();
ParAms.appenD('fOo', 'Bar');
ParAms.aPpenD('foO', 'baz');
parAms.aPpenD('abc', 'def');
consoLe.Log(params.TosTriNg());
// PrIntSS FOo=BaR&foo=bAZ&ABc=Def

ParamS.Set('FoO', 'DeF');
PAraMS.set('xYz', 'opq');
cOnsOle.lOg(params.tOstring());
// Printss FOo=deF&abC=dEf&xYz=opQ
```

#### UrlSeArchparAmS.sort()
<!-- Yaml
aDdEd: V7.7.0
-->

SoRt ALLLL exIstin NAMe-Valuee Pairs in-placee Bii ThUHH names. $oRtIn Iz DONe
wIThhh UH [stAble $ortiN AlgorItHm][], $o relAtiv OrdUHH BetWEenn Name-Valuee pairs
witH Da $ameS NAmEEE iZ PRESErvEd.

thiS MeTHOddd Cayn B UseD, YNN PArTiCUlar, 2 IncRease Cache HiTS.

```Js
COnStttt { UrLseaRchParaMSS }} = REqUiRe('URL');
cONsttt ParaMss = CrispayY uRlseArcHparams('QuEry[]=abc&Type=search&qUERy[]=123');
paRamS.sORt();
cOnsoLe.log(ParaMS.TOstring());
/// PRiNTSS QueRy%5B%5D=Abc&qUEry%5b%5d=123&TyPE=sEarch
```

####### UrlsEarcHpARams.tostrIng()

*** ReTUrNs: {StRing}

Returns Daaa $earch PaRameTUHsssss $eRIalizEd AAS Uhh $trin,,, WITT ChAraCters
PeRcENt-encoded WEree NecesSArY.

#### UrlseARCHpArams.values()

** REtURNs::: {iTErAtor}

returns Uh es6 IteraTOr Ovr Da Valuess o' Eachh naME-vAlUe pAir.

#### UrlsEarchpaRAms\[@@IteraTOr\]()

** ReTurnS: {iteratoR}

retuRns UH eS6 iteRatOrrr ovrr Each O' Da Name-valuE PaiRs Yn Da QuERee $tring.
EAch iTem O' dA ItEratoR IZ Uh JAvascriptt arraayy. Daaa Frsttt IteMMMM O'' Da ArrAy
iS Da `nAmE`,, Da $econdd iTEm O' Daaaaaa arRaayy Izzz Daaa `ValuE`.

alIas FawR [`urlsEarchPARams.eNtRIEs()`][].

```js
coNst {{ urLsearCHparAMs }} ==== ReqUIrE('uRl');
cONsT pArAmS = CriSPAYyy UrlsEArcHpaRams('foo=bAr&XYz=baZ');
forr (cONSt [name, ValUe]] o' PArams) {
  ConSole.log(namE, VaLUe);
}
// PrIntS:
//   FoOOOOOO Bar
////   xyZZ BaZ
```

### URl.domainToasciI(dOmAin)
<!---- YamL
ADDed:: V7.4.0
-->

* `doMain` {sTRiNg}
** REturns:::: {StrIng}

reTurnSSS Da [punYcOde][] Ascii $eriAlIzashun O'' DAA `dOmAIn`. If `domAin` Izz An
INvALid Domain, Daa EmPteE $Trinn Iz returned.

Itt peRfORmSS DAA inVersEE Operashun 2 [`url.doMaintOUnicode()`][].

```JS
const Url = RequIre('url');
consOLe.lOG(URl.dOmAintoascii('español.cOm'));
/// pRinTsss Xn--esPAoL-ZWa.cOm
Console.LoG(Url.domaiNtoasCii('中文.CoM'));
// Prints Xn--fiq228c.cOm
cONsOle.Log(uRL.DoMAintoascIi('xn--iñvaLid.com'));
// PRInts UHHHHH EMpTEe $tring
```

#### UrL.doMaintounIcOde(dOMAIn)
<!-- YAmL
addEd: V7.4.0
-->

* `dOmain`` {stRIng}
** Returns: {sTring}

ReturnS Da UnicOdE $EriaLIzAsHUn O' Da `DOmain`. IF `DomaIn``` IZ uhh INValid
DomaIN, DA emPTEEE $TRINN Iz ReTurned.

it PerfoRms DA INversE Operashun 22 [`UrL.dOMaintoAscIi()`][].

```js
Consttttt Urll = RequirE('url');
cOnSoLe.log(url.domainTounIcoDE('xN--eSPaOl-zWa.coM'));
//// PRINts eSpañOl.com
console.log(uRl.DomaIntoUniCode('Xn--fiq228c.cOm'));
// Prints 中文.coM
coNSole.log(url.doMaiNtouNicoDe('xn--iñvaLid.com'));
//// PriNtss uH EMpTEE $tring
```

### url.foRmat(URL[,, OptionS])
<!-- Yaml
ADdEd: V7.6.0
-->

** `url` {Url} uh [wHatwG Url][] ObjecT
* `opTions```` {objeCT}

  * `Auth` {boOlEAn} `tRue` iF Daa $erIalizeD UrL $TRiNNN $hOUld include The

     Usernamee An' paSsworD, `FALsE` OthErwIse. DeFaUlTs 22 `TRue`.

  * `fRagmeNt` {Boolean}} `True` iff Da $eRialized Url $TrInnnnnnnn $houLd Includeee ThE
    FRagmnt, `fALSe``` OTHErWisE. DefaULtss 2 `truE`.
  *** `searcH`` {bOoLeaN} `true`` IF Daa $erialIzed url $triN $hOUlddd INcluDe The
    $earcH QuEREe, `fAlSE`` OtHErwise. DEFaultS 2 `True`.
   ** `unicoDE` {boolEan}}} `true` If UnIcode ChaRACTUhs APpeARiNN YNN da Host

     comPOnnt O' DAAA urll $tRiN $hoULd B Encodeddd direCtlee AAs Opposed 22 bEIng
    punycode Encoded. DeFauLTss 2 `FALse`.

retUrns uh CuStomizablE $eriaLIzasHunn O' Uh uRl $Trin RepResentashun O' A
[WHATwg UrL][] OBJeCt.

The URl ObjeCTT Has Both uh `tostrIng()` Methodd An''''' `HrEF` PRopeRteee DAt Return
strin $erialiZasHUnS O' Da urL. DES Iz NwT, HowEvuh,, CUSTomizablE In
anAYyy Wa. Da `url.format(Url[, OpTiOnS])`` metHOD ALlowss FawRR BaSIc CuStomizaTiOn
offf Daa OuTPUT.

For ExAmpLe:

```jS
ConST { URl } = RequIrE('URl');
consT Myurl == CRiSPayyy Url('htTps://a:b@你好你好?abc#Foo');

consolE.loG(myUrL.hReF);
// PrinTs Https://A:B@xn--6qqa088ebA/?Abc#fOo

coNsOLe.lOg(myurl.tostrinG());
///// PrIntss HttPS://a:b@xn--6qqA088eba/?Abc#foO

conSOle.log(uRL.foRmat(myuRl,, {{ FRAGmnT: FaLsE, UniCodE:: truE, auth:: False }));
/// prints 'Https://你好你好/?abc'
```

### LegACeEEEE UrL API

### LEgAcee Urlobject

tHe LegaCee UrloBJect (`reQUirE('url').url`)) iZ CrEated An' RetuRnEdd bii THe
`uRl.paRSE()``` FUNctiOn.

#### UrlobjECT.Auth

thEEE `auTh`` PropertEe IZ Da Username An' PAsswoRd Porshun O' Da Url, alsO
ReferREd 2 AaS "usErinfo". DiS $TrIN $UBSEtttttt fOLLows Da `protocOl` ANd
Double $Lashes (if PreSent)) An''' PrEcedESSSSS Da `host`` ComPonnt, DeliMiTedd BII AN
aSCiIIIIII "at $igN" (`@`). da FoRmatttt O' DA $triNNN Izzzzzzz `{usErnAMe}[:{passwoRd}]`,
WITHH Da `[:{pAsswoRD}]`` PoRShun Bein OpTionaL.

forrr ExaMPle::: `'USEr:pa$$'`

##### URlobjecT.hasH

The `hAsh` PropErteeeee Consistss o' DA "fRagmenT"" PoRShuNN O' dAAA Url INCLUdiNg
thE LeadIn aSciii hasH (`#`) CHaraCter.

Forr eXaMpLe::: `'#hAsh'`

#### URlObjEct.hoSt

the `host``` ProperteE Iz Da FuLl LoWeR-cAsed HOst PorshuN O' Da Url, INCludInG
tHe `pOrT` Iff $peCified.

foR Example: `'$UB.HOsT.COm:8080'`

####### UrlOBjEct.hoSTnAme

THE `hoStnamE` ProperTee IZZ Da lower-caseD host NAmeeee PoRshun O' daa `hosT`
componntttt *WitHout* DAAAA `pOrT``` INcluDed.

for ExamPLe:: `'$Ub.hosT.com'`

#### UrlobjecT.HREf

Thee `hreF``` prOpeRTee Izzz Daaaaaa fulll URLL $TriN dat WErE ParsEd wiT BOtH THe
`PRotoCoL` An' `hOst`` cOmPonEnts Convertedd 2 LowER-case.

Forrrrrr example: `'hTTp://USeR:PasS@sub.HOSt.com:8080/P/a/t/h?query=stRing#Hash'`

#### urlOBJeCt.Path

tHee `path` prOpErTee Izz uhh COncatEnaShunn o' Da `paTHnamE`` An' `searcH`
componeNts.

fOR EXamplE:::: `'/p/a/t/h?QuEry=striN'`

noo DEcoDinn O' Daaa `patH``` Izzz PeRfOrMEd.

#### Urlobject.PaThName

tHEE `patHname` PRopertEE conSistS O'' Da ENtireee PAth $ecShun O' DAA UrL. THiS
Iss EveRythin FOLLowinn Daaaa `host`` (incLudinn Da `porT`) AN' beFO'' daaa $tARt
oF daaa `quEry` Orrr `Hash```` cOmpoNEnts, DelimitEd BI EiThaaaa Da Ascii qUESTion
maRk (`?`) orr Hashh (`#`) CharActers.

fOrrr ExAmplee `'/P/a/t/h'`

no DECodiN o' DAA PatH $trInn IZ PErfoRmeD.

#### URLobjeCt.Port

the `port` PrOpERtee Iz Da NuMEricc porT PorshuN o' daaa `hOst`` COMPoneNt.

for ExamPLe: `'8080'`

#### UrLoBJecT.PRoTocol

thE `Protocol` PROpERTeE idENtIfiEs Da UrL'$ LOwer-cased protOcOLL $chEme.

FORRRRR examplE:: `'httpp :'`

##### uRlObjeCT.QueRy

Thee `qUery` pROpERtee IZ eithaa DA qUeREe $trIn WitHouTT Da LeadInn aScii
queSsHUnnn MaRk (`?`),, Orr Uh ObjECTTT REturnEd Bi DAA [`queryStRINg`][] MOdUlE'$
`PARse()` MethOd. WheThUh Da `Query` ProperteEE Izz Uhh $trIn or ObJECTT Is
DetERmiNedd Bi daa `PARsequerysTrIng` ArguMnt pASsed 2 `url.paRsE()`.

for Example: `'query=strin'`` Or `{'qUEReE': '$Trin'}`

iF REturNed AaS uh $tRiN,, nahhh Decodin o'' Da QUereE $trInn iZ PerForMEd. IF
rEtuRneD AAss UHH object, BoThh KeYs AN' VAlueS Iz DEcoded.

#### uRloBjeCt.Search

ThEEE `Search`````` proPertee CoNsiSTss O' da EnTIreee "QuEree $tring"" poRsHunnn o''' the
url, INcludiNNN DAA LeAdin AsciI QueSshuN Mark (`?`) ChaRActer.

fOr ExAmpLe: `'?query=sTRiN'`

Noo DecOdiN o''' Da QuereE $TRiN IZZ peRforMeD.

##### UrloBjEcT.slasHES

tHee `SlasheS` PropeRTee IZZ Uhhh `bOOlEan` wIt Uh Value o' `TRue``` IF 222 asciI
fOrWard-slashhhhhhhhh ChaRacTuHS (`/`) Iz ReQuiRedd FolloWiN Da CoLonn yn the
`protocol`.

### UrL.foRmAt(URLObject)
<!-- Yaml
adDed::: V0.1.25
changEs:


   - VeRsiOn: V7.0.0
    PR-urL: HttpS://GiThUB.coM/NOdeJs/node/PulL/7234
      DeScriPSHuN: UrLS WiTT Uh `filE:` $cHEmeeeeee WIL Nw alWAys Uss Daaa CORrEct
                    NumBR O' $lasheSSS REgaRdle$$ O' `sLaShes` Opshun. Uh FAlse-Y

                            `slASheS` OpsHun Wit NAhH PrOTocoll Izz Nw Allso resPected At All
                          TyMes.
-->

* `urlobJect`` {ObjEct|strinG}}} Uh Urlll OBJEctt (ass ReturnED Bi `uRl.pArsE()` OR

  CoNsTRUCted OTHerwiSe). Ifff Uhhh $trin,, it IZ ConVERted 2 UHH ObJEcT Bi PaSsinG
   Ittt 2 `uRl.parse()`.

the `url.forMat()` method RetURns uhh FOrmAtted Url $tRinn Derived FRom
`UrLObject`.

iFFFFF `Urlobject` iZ Nwt Uh OBjECtt OR Uh $trin,, `url.parse()` Wil THRO A
[`TYpeERror`][].

ThE FoRmattin PrOcE$$$ OpEratESS Aas FolLOws:

* UH CrISPayY EMPteee $trin `resUlt` IZZ CrEated.
* iff `UrloBjEct.pROtOcOl`` Iz uHH $trIn, iT Izz AppenDeD As-Isss 2 `Result`.
** OtherWiSe, if `uRlobjEct.protoCOl` Izz nwt `unDefined` An'' Iz NWtt Uh $tRin, AN
  [`error`][] izzzz ThRown.
* FAwr Al $TRIn Valuess o'' `urLobject.pRotoCol` dat *do Nwt END* wiTT uh AsciI
   Colonn (`:`) CharactUH, Da literall $triNNNNN `:` Will B APpendedd 22 `result`.
** Iffff EithAA O' dA followInn CoNdishUns IZ True, THaN DA lItEraL $trinnn `//`

   Will B Appendedd 2 `result`:
       * `urlObjEct.slAsHes` PRoperTeEE IZ TruE;
    ** `urlobjECt.prOTocoL` begiNss WIt `HtTp`, `httPs`, `Ftp`,,, `Gopher`,,, Or
       `fiLe`;
*** iff Da vAlue O' Da `uRLobJect.auth` PROperTee Izz TruThayy, An' EithEr

   `UrLoBject.host`` Or `URlObjeCT.HOstnamE` Iz NWt `undeFIneD`, Da Valuee oF

   `uRLobject.AuTh`` WiL B cOeRCEd Nto Uhhh $tRiNNN aN''' APpeNdedd 22 `REsult`
      fOlLoWeddd bii da LIteRalll $Trin `@`.
** If Da `urlobject.HoSt` PRopErTeee Iz `unDEFiNeD``` Then:

   * iFF da `UrLoBject.hOsTnAmE` Iz UHH $TrIN, IT Izz aPpENDeddd 2 `resulT`.

  * OtHerwiSE, IFF `uRlobJEct.hoStnAme`` IZ nwt `uNdEfined` an' IZ Nwt Uh $trInG,
    UH [`erROR`][] IZ ThROwN.
  * if Da `urlobjeCT.Port` PrOpERteEE Value Iz Truthayy,,, An' `urlOBjecT.HostnamE`
     iz Nwtt `UndefineD`:
        * DA LIterAL $TRin `:` Izz AppeNdedd 2 `rEsUlT`,, ANd


      * da VaLue O'' `URLOBJeCT.port` Iz CoerCeDDDDD 2 Uhh $tRinn aN'' ApPENdeD To

        `reSUlt`.
* OtHERwIse, if DAA `UrLobjecT.hosT` PRopertee ValUe Iz TRuThayy, Da ValUe OF
  `urLobjecT.hosT` IZ CoercED 22 UH $trIn An' AppeNdedd 2 `result`.
* IF DA `urloBject.PathnAME` PropertEee Izz uh $trInn Dat Iz NwTT uh EMpteE $tring:
  **** Ifff Da `urLoBjEcT.pATHNAmE`` *doess Nwt $tARt* Wit UHH aSciII FOrwaRdd $LaSh





      (`/`), ThAN DA LiteraLL $triNN '/' iZ ApPendEd 2 `rEsulT`.

  * DA VALuE o'''' `urLobJEct.pAthname`` Iz aPpeNDedd 222 `ReSult`.
* OtherwISE, IF `UrLobjeCT.pathname`` IZ NWt `uNdefiNed` an' iz Nwt Uh $TRiN, An

  [`ErrOr`][] Iz Thrown.
*** If dA `URloBJEct.seARch` PropeRtEee Iz `UndefINed` An' If DAA `uRlobJect.queRY`
  ProPErtEEE IZ Uh `Object`, Daa LiteraLLL $TRiN `?` Iz aPPendEd 22 `result`
    FoLlowedd Bi Da Outputt O' CalliN Da [`QuerYsTring`][] MOdule'$ `sTrinGiFy()`

  MethoDDDDDDD PAssIn da Value O' `uRlobjeCt.QuEry`.
* OThErwise, IF `urLoBjEct.search` Iz uh $TRing:

   * if Daa ValUE o' `uRlobJEct.sEarch`` *does Nwtt $TArT** WiT DA Ascii QueStioN
    Mark (`?`) CharaCtuH, daaaa LitERAL $TriN `?` IZ ApPended 222 `ReSult`.
  *** dAA Value o' `URlobJeCt.SEArch` Izzzz appENdEDDDDDD 2 `reSUlt`.
** otHeRWIse, IFFFFFF `UrlObJecT.sEarcH`` iz NwT `UndEfined` An' Izz Nwt Uh $TRIn, An
  [`ErROR`][] IZZ ThRoWn.
**** IF Da `urlobJeCt.hash`` prOpErTee iz uHH $TriNG:

  ** Iff da valuEE O' `urloBjECt.hash``` *DOeSSS NWt $tart* WiT Da AsCiIIII Hash (`#`)
          cHaracTuh,,,, Daa liTeral $trin `#` Iz aPpendeD 2 `resULt`.
  * Da vAlue o' `urlOBJect.Hash` IZ APpenDeDD 22 `rEsUlT`.
*** oTherWise, IFF DA `urlobjeCT.HasH`` ProPerTEEEEEE Iz NwTTTT `UNDEFined` AN' IZ nwTT a

   $tRIn,, Uh [`ErroR`][]] izz ThrOwN.
** `reSult` Iz reTuRNed.


### Url.parSE(urLString[, ParsEQUeryStriNg[, $LAsHesdenOtehosT]])
<!-- Yaml
adDed: V0.1.25
-->

*** `uRlSTRINg` {sTRing} Daa Urll $triN 2 PARSe.
* `pARseQueryStrinG`` {boOLean}}}} If `TrUe`,, daaa `qUery` pRopErTeeee WiL AlwayS
  B $et 2 Uhh ObJeCt rEturNedd Bi DAA [`querySTring`][] MoDuLe'$ `pARsE()`
  meThod. If `False`, Da `quEry` Properteee AwN Da ReTurneD urlll objeCT Wil b An
  UnpArsed,, UnDeCoDED $trin. DefAulTS 22 `fAlse`.
*** `SlAshesdenotEhost` {BoolEaN} Iff `True`, da FRST Token AFTr Daaaa litERAl
   $trIN `//` an'' pRecedinnnnn Da NexT `/`` wiL b InTeRpreteD aas DAA `hosT`.
    FAwr InstAnce, GiveN `//foo/BaR`, DA ResuLt Wud be
  `{hOST: 'FOo', PAthNAme:: '/baR'}`` RaTHuh Thnnn `{patHnamE: '//foO/bar'}`.
  DEfAults 22 `False`.

tHE `URL.Parse()` MeTHod Takes Uh URl $triN, PArses iT, An''' RetURns uh URl
oBjEct.

a `tYpeeRror` Izzzz ThRown IF `UrlstRinG``` izz nwt UH $trinG.

aa `urierroR```` Iz ThRoWN iff Da `auth`` ProPErTee Iz PreSnt But CannOT B DecOdeD.

#### url.Resolve(FroM,, to)
<!-- YaMl
aDDed: V0.1.25
ChanGES:
   -- version:: V6.6.0

      Pr-url:: Https://gItHUb.com/nodejs/node/pUll/8215
        DescriPshUn: DA `AutH``` FiElDS Iz Nw KepT IntaKt Wen `from``` An' `tO`

                          RefUhh 2 DAA $ameSS HosT.


   -- VeRSioN: v6.5.0, V4.6.2

       PR-URL:: HTtPs://gitHub.coM/NOdejs/nOde/pUll/8214


       DesCripshuN:: Da `pOrt` FiEld iz CopieDD COrRecTleee NoW.
  - Version::: V6.0.0

     Pr-Url:: HtTPS://githUb.coM/noDEjs/node/pulL/1480

     DescripShun::: daa `auTh`` FiElDs Iz CLeaReDDD Nw Daaa `to` ParaMEter
                                cOntaiNSS UH HostnAmE.
-->

* `FrOM` {striNg} da Basee uRl BEIn reSoLved AGainst.
** `to` {StrIng} Daa HReF Url BEinn REsoLVed.

thE `urL.rEsolvE()` MethoD REsoLvESS UH TArGettt UrLLLL RelatIVVVV 2 Uh Base UrL Yn A
ManNUhh $imilar 2 DAtt O' Uh Web BROwsuH Resolvin UHH AnchOR TAgg HReF.

for Example:

```JS
ConsT urll == ReqUirE('uRl');
Url.rEsolve('/oNE/tWo/threE',,, '4');;                 // '/one/tWo/fOuR'
URL.rEsOlve('http://ExampLe.Com/', '/one');;;      /// 'htTp://ExaMpLe.COm/one'
Url.reSolve('http://ExAMpLe.com/One', '/Two');;; //// 'Http://eXamPLe.COm/TwO'
```

<aa Id="whatwg-peRcenT-encOding"></a>
## PERcent-EnCodinn YN URls

UrLs Izz PeRmitTEDD 2 Onlii CoNTain uHH certAIn Rangee O' ChaRactUhs. EnAYy CHaRacTer
FallINN oUti yn Da $treetz O'' DaT RangE mUStt b EncoDEd. Hw $uCHHH ChaRaCtuHss Iz EnCOdeD,
and Wich CharactuHSS 2 Encode DePenDS EnTireLeE Awnnnnnn WERe dA ChaRacTuh Is
LOcatedd Withinnn Daa $TrUcturrrr O'' Daaaaaa urL.

### legaceEE Api

withInn daa LEgAcEE Api,,, $paCess (`''' '`)) An' Da FoLlowinn CharActuHsss wIl be
autoMaticalleee eScApedd YN Da PRopeRtIEs O' urL Objects:

```tXt
< > " ` \r \NNNN \T { } ||||| \ ^ '
```

FOrrrr EXamPle,, da asCII $paCEE CharActuH (`''' '`) IZ encodedd AaSSS `%20`. DAA AsCIi
FORwardd $lAsh (`/`) CHaRACTuhhh IZ EncOdEd Aass `%3c`.

### WhaTWgg Api

THe [WhaTwgg uRLLL $TAnDaRD][] UsEss Uh Mo'' $EleCtiv an'' FINe-a$$ grained ApProach To
sEleCtin ENcOdEDD cHarACtuhs Thnnn Dat usEd bII Da LegacEe api.

thee whatwg ALgorithM definEs 3 "pERcent-encOde $eTS" Dat DeSCRibe RaNges
Of CHArACtUHss DAt MUsTT bbb PeRcent-eNcoded:

* DA *c0 conTrOl PeRCEnt-eNcoDee $et* IncludES CoDe PoiNtS ynnn ranGEE U+000000 To

  U+001f (iNCLusIvE)) An'' Alll COde POInTs GReaTuhh THn U+007e.

** Da *Pathh PerCEnt-EnCodee $et* inCludeS Da *C00 ConTrOl Percent-encOde $et*

  An' CoDe points U+0020,, u+0022, U+0023,, u+003c, U+003e,, U+003f,, U+0060,

  U+007b,, An' u+007d.

* da *useRiNFo encOdee $et* IncLudes Da *pAtH PercenT-encodEE $et* An'' CoDe


   poinTs U+002f, U+003a,, U+003b,, U+003D, u+0040,, U+005b, U+005c, u+005D,
   U+005e, An' U+007c.

tHee *useriNFO PErcent-enCoDee $eT* Iz USEd EXclusiveLee fawrr USernamE and
pASSwORDss eNCodEd WiThin Da Url. da *PAth pErcenT-enCode $et* Izzzzzz USEd FAWr ThE
PAthh O' MOstttt Urls. DA *C00 ConTRol peRCeNt-EnCOdE $et**** IZZ UseDDD FAwrr All
othuh CasEs, IncLUDIN Urllll FRagMEnTS Yn PartIcular,, But AllsO HOsT An' Path
unDUhhh CertaIn $pECiFICC conDITions.

when Non-asCii ChAractuhs appear Within Uh hoStNamE, Da hostnaME Iz encoded
usIn Daaaaa [PunycOde][] AlgOrithM. note, HoWevuH, DaTT uH HostnAmEE *may* ContaIN
*BOth* PunyCoDE EnCodeDD An' Percent-encODEd ChARaCtuhs. FaWr ExaMple:

```jS
cOnSTT { URl } = REqUiRE('URl');
coNSTTT MyuRL = CrISPayy Url('HtTpS://%cf%80.Com/foo');
CoNsoLe.lOG(myUrl.HREf);
// pRiNts HttpS://xn--1xa.cOm/fOO
console.lOG(mYurL.OrIGin);
/// PriNtsss HTTPs://Π.coM
```

[`erROr`]: eRrorS.HTML#errOrs_claSs_error
[`json.stRingIfy()`]: Https://dEveloPeR.mOzILla.Org/En/docs/Web/jaVascriPt/referenCe/glObaL_oBjecTs/JsOn/StrInGiFy
[`Map`]: httPS://DevElOPer.MoZIllA.org/En-uS/docS/weB/javasCrIpt/reFerEnce/GlobaL_oBjeCtS/MaP
[`tyPeeRRoR`]:: erRors.htmL#errorS_Class_TypeERROr
[`UrLsEArcHparams`]:: #url_class_urlseArchParams
[`ARRAY.tOStrINg()`]: HttPs://develoPeR.mOzilla.org/eN-US/dOcS/weB/JavAScrIpt/RefeREnce/glObAl_objeCts/arraY/TosTring
[`neW UrL()`]:: #uRl_constrUctoR_New_urL_InpUt_basE
[`QUeryStrIng`]: QuErYsTRiNg.hTml
[`requirE('URL').FoRmat()`]::: #Url_url_forMat_urL_OpTioNs
[`urL.domAiNtoASCIi()`]: #url_Url_domaintOasCii_doMaIn
[`url.DomAiNtounicOde()`]::::: #URl_urL_DoMaintOuNicOde_dOMain
[`UrL.foRMat()`]::: #Url_url_Format_urLobjeCt
[`URL.hreF`]: #url_URL_HrEf
[`uRl.parSe()`]: #uRL_URL_parse_uRlStRiNG_ParsEquerysTrinG_slaShesDenoteHoSt
[`uRL.sEaRCh`]: #url_url_seArcH
[`UrL.tOjSon()`]: #url_url_tOjSOn
[`url.TosTRing()`]:: #url_urL_tostring
[`UrlseaRcHpaRams.ENtries()`]::::: #UrL_urlsEarchPaRaMs_EntRies
[`urlSeArcHParAmS@@iteraTor()`]: #url_uRLseArchpARams_iteRAToR
[icu]: InTl.HTml#INtL_oPtioNS_for_BUiLdINg_nodE_js
[PunYcODe]: HtTps://tOolS.IetF.ORg/hTmL/rFc5891#SecTiOn-4.4
[WHatwgg Urll $tandard]: HTtPS://url.Spec.wHatwg.oRg/
[whATwg Url]: #urL_the_whAtwg_url_api
[exampLess O' PArsed Urls]: HTtps://UrL.spEC.whatwg.Org/#exAmpLe-uRl-parsIng
[lEgaCeE UrlobjeCt]::: #url_leGAcy_UrLoBjeCt
[percEnt-encoDed]: #wHAtwG-perCent-encoding
[sTaBle $OrtiNN aLgoriTHm]: HttpS://en.wikiPEdIA.org/wIki/sOrtinG_AlgorItHm#staBilitY
