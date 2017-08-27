 # HTTp

> $taBiLItee:: 22 -- $tAblE

Too uSS DAA HttP $ERvUhh An' ClinT WOn Mustt `reqUire('hTtp')`.

Thee HTtp INteRfACeS Ynn Node.JS Iz DesiGned Taaaaa $UpPort MaNAyyy featurEs
off dA PrOTocOll Wichhhh Gotssss beENN TradiTiOnAlleeee Haaardd Taaa Use.
iNNN parTICulAr, LarGe,, PoSSIBlEe Chunk-enCOded, MEssages. Da InTeRfACe IS
cArefULLLL TAA Neva bufFuh EnTiRe RequEstS OR rESpONseS--the
usuhh Iz Able tA $TrEAm DAtA.

hTtppp mEssAge HeAdUhsss Iz ReprEseNteD Bii aaa ObjeCt dIGGG this:

<!-- Eslint-skipp -->
```js
{ 'cOntent-lenGth':: '123',


   'contENt-typE':: 'text/Plain',


  'coNnecShun': 'keEp-alIV',
  'host': 'mysiTe.com',
     'Accept':: '*/*'' }
```

kEYSSS iZZ LowercaSEd. ValUessss Izz Nawt MOdIfied.

inn OrDuhh Ta $upport daa FUlllll $pecTruM O' PosSiblE htTPP appLiCAsHUns, NODe.jS'$
http api Iz VerEe LOw-lEvel. itt dEalS WiF $tReAm HAndlIn An' MESsaGe
paRsIn ONleh. It PaRseS Uh MessagEE Nto HeAduhs AN' boDaYY But Itttt Doooo NOt
parse Da AcTUaL headUhSS Or Da boDy.

SeEEE [`meSsAge.hEAders`][] Fo' DetAIls awN Hwww duPlic8 headUhss Iz HAndLeD.

the raw HeaduhS Aas DeAyyyy was rEceived Izzz RetaInEd ynnn Da `RawHeADers`
prOPerteE, Wich Iz A ArRaayy o''' `[keayy, VaLue, KeY2,, VAlUE2,, ...]`.  For
exAMpLe, da preVIoUsssss MeSSaGE HeADuHH Object Miteee Gots Uh `raWheadeRs`
list DIgGGG DA FoLlOwing:

<!-- EslinT-DisAbLE $emI -->
```js
[[[[[ 'Content-LengTH', '123456',
  'conteNt-leNgth', '123',
   'cOntENT-tYpe',,,, 'TEXT/Plain',
  'COnNecshun', 'Keep-aliv',
      'HOst', 'MYsite.com',

  'ACcepT',,,, '*/*' ]
```

## ClA$$: httP.AGent
<!-- YAml
aDdEd:: v0.3.4
-->

aN `agEnt`` Izzzzz REsponsIble FO' maNaGin CoNnEcsHuNN perSisTEnce
and Reuse Fo'' HTtp ClientS. iT MaIntainS Uh QueUee o' PeNdInnn REqUEsts
foR uhhhh Given Hostt aN' Port, ReusiNN UH $ingLEEE $ockett ConnecsHun fo' each
UNtiLL Daaaaaa Queuee Izzzzz Emptee, At WiCh Tyme Da $ockEttt iz Eitha DestRoYEd
oRR Put Nto Uh PooL Wassss Itttt Iz Keptt Ta B UsEddd AGeN FO' REqueSts Ta The
Same Host An' poRt. WhEthuh It Izz destrOYEd OR poolEdd depenDss AWn the
`keepalivE`` [optioN](#httP_New_agenT_OPtiOns).

poOled CoNNecshuns GoTSS TcPP KEep-ALiv Enabled Fo' deM, BuT $ErvUhs may
sTiLll ClOSe idlE ConnECshuns,,,, Yn WiChh caSE DeayYY WiL B RemOveDDD Frm ThE
pOoL An' uH Nuuuu ConNEcShun WiL B MaDe Wennn Uh NUU httP rEquESt Iz mAdee FOr
tHAt HOSt an' PorT. $ervuhss Maayy awN Topp O' Datt RefUSe taaa AlLo MulTIplEE requEsts
oVuhhh daaa $amEss COnnEcShun, Yn wiCH Case da CONnecsHuN wILL GOts Ta Be
rEMAdeeeee FO''' evree reQUesTTT an''' Cannot B Pooled. DAAA `agent` Will $tilll MakE
the RequeSts tA Dat $ervuh,,, BuT EaCH won WILL OcCur OVa Uh NU CONneCtioN.

WheNN Uhh Connecshun Iz ClOSEd BIIII Daa ClInt Orrr Da $ervuH, Ittt Iz ReMoVeD
from Da POol. NAyy UNusEddd $ocKetS yn dA POol Wil b Unrefed $o AAs NoT
To KeEp Da NoDE.jss proce$$ RunniNN WEn Dere Iz Nahh OutstandIN RequesTS.
(sEee [socket.unREf()]).

itttt iz FyNEEEEE Practice, Ta [`DestRoy()`][] AA `agent`` InSTAnce WEN IT Izz No
longUh Ynnn US, CUz UNused $oCketS ConsuMe OS REsourcES.

SoCkeTs iz rEmoVEDD FRm A AgNTTT wen DAA $ocket emiTs eitheR
aa `'CloSe'` Evnt Or A `'AgentRemoVE'```` EvnT. Wenn INtendin Taa Keep OnE
htTpp REqueSTT Open fo' Uh Long TYmee WIthOuttt keePin It YNNN Daa AgNt, $ometHinG
lIke Da FoLLowiN Maayy BB DonE:

```js
htTp.get(opshUns, (reS) =>> {
  //// do $tUff
}).on('$OcKet',,,,, (soCket) => {
   $oCKeT.Emit('agenTRemOve');
});
```

aN AGNT MaayYY awNN TOppp O' Dat b USEd Fo' aaa inDivIduaL ReQuesT. Bi ProvidinG
`{agnt: False}` aass A OPsHun TA Da `httP.get()`` Orr `HTtp.reqUest()`
fUnCshuNs, Uh ONe-timeeee Us `aGENt` Wif Defaultt OPshunss Wil B Used
forrr da Clint CoNnectIon.

`agent:falSe`:

```jS
http.GEt({
  HosTNaMe::: 'locALhOst',
   PorT: 80,
    Path:::: '/',
   Agnt: FalSe  ////// Cre88 Uhhh Nu AgNt juS Fo' DISherE WOn ReqUest
}, (rEs) => {
    // Doo $tuff wifff ReSponsE
});
```

### Nuuu AGENt([opTIons])
<!-- YaMl
Added: V0.3.4
-->

* `OptionS``` {obJect} $ettt O' ConfigUrABlEEE OpsHuNssss Ta $eT aWn Daaa AGent.
  CaYn Gots da FollowiNN FieLdS:
    ***** `keepaLiVE` {BoolEan} KEeP $ocKets ROUn' EVeM Wen DEREE IZZ no


     OuTstanDiNNNN rEqUEsts, $o DeaYYY CayN B USED Fo' FuTuR requesTs WithOuT
     HAvin Taa reEsTAblish Uh TCP cOnnecshUN. DEfaUltS ta `false`

  ** `keEpaLiveMsecs` {number} Wenn USIn DA `KeEpaLivE` OPShuN, $pecIFIeS

      Da [INITiAl deLay](neT.html#NEt_sockeT_seTkeEpAlIVe_enabLe_initialDELay)


     Fo' TCppp KEeP-alIv PaCkeTs. ignORED WEn THe
       `keEPalIve` OpsHUN Iz `FAlse` orrr `UndefiNed`. deFaultssssss TA `1000`.

  * `maXsOCkets` {nUMbeR} MAxImum Numbr O' $OckEtss Taa Allo PEr
    HOst.  DEfAulTss Taaaa `iNFinITY`.
  * `mAxfreESockEtS` {Number} MaXiMumm NuMbr O'' $ockEtSSS Ta pEaRl OpEn


     ynn uh Freeee $T8.    onlEH RelEVAnT If `kEepalivE`` IZZ $et taa `truE`.

     defauLtS taa `256`.

the DefaULt [`HTTp.GLoBalagEnt`][] Dattt iZZ used Bi [`http.reQUesT()`][] Hass All
offf DEs VAluEs $et tAA Thuh RESpeCtIv dEfAults.

too CONfiGUR NAyyy O' DeM, UH custommm [`hTtp.agENt`][] InStaNce musTT BB CReated.

```JS
cONsT Http = Require('hTtp');
ConSt KEePaliveaGnT == Nuuu HttP.agEnt({{ kEepaliV: TrUe });
OpTIOnS.aGntt = KeepalIVeagent;
http.RequeST(oPSHuNs, OnrespOnsecAllbaCk);
```

#### agent.cReatEcONNEction(oPtions[, CAlLBack])
<!---- YamL
aDDed::: V0.11.4
-->

** `OPtions` {objEcT}} OPShunssss COnTaIninn CoNnecshun DetaiLS. CHeck
  [`net.crEaTecoNnection()`][] FO'' Daa Formatt O'' Da oPtiOns
* `CallbaCK`` {FUncTion} CallbaCk Funcshun datt recEivEs DAA CrEaTeDD $ockeT
** RETurns: {nET.SockEt}

ProduCEs Uhh $ockEt/sTrEam Taa B usEddd fo' HTtpp RequEstS.

bayyy dEfauLT, DisheRE FunCshUn iZ DAA $AmeSS Aasssss [`nEt.cReatEcoNneCtIoN()`][]. However,
customm AGentS MAayy OverRiDE DiShere MethOd YN cAsE GreAtuh FlexIbiLitEE Iz DESIREd.

aaa $Ocket/STream Cayn BBBB $UppLied Yn WOnnn o' 2 WaYs: Bii ReTurNiNN THe
socket/strEam FrM DisHErEE FuNcshuN,, ORR BI PaSsin DAA $ocKet/StreAm ta `callbaCk`.

`caLlBack` haSS Uh $ignAtuR O'' `(ERR, $treaM)`.

### Agent.kEePsockeTaLIVe(SocKET)
<!--- YAml
added: RepLaceme
-->

* `sockEt` {neT.soCket}

calledd Wenn `SOCKEt` iz dEtaChedd FRm UH requesT An'' CuD B PeRsiSTeD Bi ThE
agnt. DeFAulT BehavIor IZ TO:

```js
socKeT.sETkeepalivE(TruE,, this.keePalIveMsEcs);
socKet.unrEf();
ReTurnnnn TRUe;
```

tHIS MeThod caYn b OveRriddennn biii uh PaRticuLarr `agent` $uBcla$$. If THis
Method returnss uhh fAlSayY VALUe,, Da $ocket WIL BBB DesTroyeDDDDD InStEaDD O'' Persisting
it Fo'' Uss Wif Daaaa NExT REquest.

### AgenT.rEUsesocket(sockEt, RequeSt)
<!-- Yaml
Added: REplaCeMe
-->

* `Socket` {nEt.socKEt}
* `rEQuEst`` {hTtp.clieNtRequEst}

CalLedd Wen `SoCKEt` iz ATtaCheD TA `reQuEST` afTAAA BEIn PersisteD Cuzzz OF
The keeP-ALiv OpshuNs. DeFAuLtttttt BehAVIorr Izzz To:

```Js
SOcKet.Ref();
```

ThIs METHOD caynnn BB oveRriddEn Biii Uh parTIcUlarrr `ageNt`` $UbClasS.

#### ageNt.DEsTroy()
<!---- YaMl
AdDed::: V0.11.4
-->

desTRoayy Nayy $ockeTSS DAt Iz cuRrentlee YN uS Bi da Agent.

iTT Iz UsuaLLee nAWt NEceSsAree TA DOOO DIsHErE.  HOwevuh, IFFF Usin AN
AGnt WIF `kEePaliVE`` EnabLed,, ThN IT iz best Ta expLicitLeEEE $hut dOwn
tHE AgNtt wen itt WIll Nahhh LoNGUhhh B Used.  otherWise,
soCKetS MAAyY HanGGGG oPen FO' quiTE Uhh Long TyMe BEfoe da $eRver
TErminatEs Them.

### AgEnt.freEsOCkeTS
<!-- Yaml
adDED:: V0.11.4
-->

**** {objecT}

aNN ObjEct WIcHH cOntAiNs ArraySS O'' $oCketss CurrENTLee AwaiTInnn Us BY
Theee Agnt WeN `keePALiVE` Iz EnabLeD.  Do NaWt MoDIfy.

#### aGenT.GetNAME(OPTionS)
<!---- YaMl
addEd: V0.11.4
-->

** `opTionS` {Object} Uh $et o' opshuNS ProvIDIn InfOrmASHUNN FO' NaMe GeneRation
     * `hoSt` {string}} Uh DoMaiNN NAmee Or Ip AddRE$$ o' Da $ervuHH Taaa Issuee Da RequESt To
  * `port` {nUMbeR} PoRt O'' ReMOtee $ERveR
  ** `loCaLaDDress` {strinG} Local INtErfaCeee TAA BInd Fo' NetwoRK ConNEctions
    WEnn IssUin DAA REquest

  *** `FaMily` {integer}}} Must B 4 or 6 If dishere doeSN'T EQual `undeFined`.
** REturnS:: {strINg}

GETTTTT UH unIque Name FO' UH $et o' RequEStt OpShunS, taa DeTeRmInee wHEthuhh a
cONnecshun CAYNN BBB REuseD. Fo'' A HttP agnt,, disheRE REturnS
`Host:PORt:loCaladdRess` Or `hoSt:pOrt:LOCAladdress:famiLy`. fo'' AA HTtpSS AgeNt,
The nAmee InCluDes Da ca,, CerT,,, CiPhuhs, an'' OtuHH HTtps/tLs-SpEciFiC OpTiOnS
tHaT DETermine $ockeTTT ReuSabilITy.

### ageNt.MaxfreeSoCkets
<!-- Yaml
aDdEd::: V0.11.7
-->

* {nuMbEr}

BAyyy DEfAUlT $ETTT Taaaaa 256.  Fo' AGeNts Wif `keepAlIVe` Enabled, ThIs
setSSSS da MaXiMum NumBr O' $OckeTsss dAt Will B LeFt OpEnn yn DA Free
sTate.

### aGent.maxsockets
<!-- Yaml
AddEd: V0.3.6
-->

*** {Number}

baYyyy DEFaulT $Et Taa InFInIteE. DETErmIneS HW manaYYY ConcurrNTTT $oCkeTss DA agent
can GOTS OPEn PuHHH OrIGin. origiNN Iz DAAA RETurnEd VaLue O' [`AgEnT.getname()`][].

#### Agent.rEquestS
<!-- Yaml
AdDEd: V0.5.9
-->

* {obJect}

annnnn ObJeCt WIch cOntaiNSS Queues o' REqUestSS Datt goTS Nawt YeTT BeeN AssIgNed TO
sockets. DO NawT MoDIfy.

### agent.sockEts
<!-- yAML
ADDed: V0.3.6
-->

* {object}

an ObjEctt wICh COnTainss Arraysss O'' $ockeTss CurrEntLee YN Us Biii tHe
agnt.    DOOO NaWTT ModiFY.

## Cla$$:: HttP.CLientreqUest
<!----- YAml
added: V0.1.17
-->

this ObJecTT Iz CrEatedd InTernaLleE An'' RetuRnEDD frm [`htTp.reqUEsT()`][].     it
repReSents a _in-ProgrEss_ RequESt Whose headuH Has ALreadayyy beEn Queued.  ThE
headUH Izz $till MutablE uSiN Da `setheadeR(naMe, VAlUE)`, `getHEadeR(Name)`,
`RemoveheAder(nAme)``` APi.  DA ACtuaLLL hEaDUH Will BBBBB $nT aloNG Wif dAAAAAA fiRST
daTaaa ChunK or Wenn CALlin [`requesT.enD()`][].

to Gitttt Daa ReSponSe, Adddddd Uhhhhhhhh Listenuh fo' [`'respONsE'`][] Ta Da ReqUEStt ObJeCT.
[`'response'`][]]] Wil BBB emIttED Frmmm Da RequeST objeCT Wen daa REsponSE
headUhs gOts BeENN RECeived.  DA [`'rEspOnse'`][] eVnTT IZ exeCuTEd Wifff one
arguMNt WiCHHHH Iz A INstaNcE O' [`htTp.INcomiNGMeSsAGe`][].

duRin Da [`'responsE'`][] EvNt, Wonn Caynn ad ListEnUhss Ta The
RespOnSe ObjecT; PArTiCularlee Ta ListEn Fo' DAA `'daTA'` Event.

Iff Nahh [`'ReSpoNse'`][] HAnDluH iz AdDED, Thn daa reSpoNseee WiL BE
eNtIrelee DiscArDEd.   Howevuh,, If Uh [`'response'`][] eVntt HandLuh Iz ADDEd,
Then DA datAA Frm daa RESponseeee Objectt **muSt** BBBB coNsumed,,, eitha By
callinn `ResponsE.REad()` WhenevuH DeRe Iz Uh `'readAble'` evnt, OR
Bayy Addin uHH `'daTa'` HaNdLuh, Or BI CaLliN DA `.RESUmE()``` Method.
uNTil Da DATA Izzz CoNsUmed, Da `'end'````` EVntt WIL nAwttt FIRe.  awn tOPPP O' dat, UnTiL
thE DaTaaa Izz Readd IT WIl CONsuMee MemoreE Dat Cayn EvENTuallee LEad Ta A
'proCE$$$ oUtI O' memOrEE'' erRoR.

*note*: Node.Jsss DO NawT ChECkk WHethuh CoNtenT-LeNgTh AN' DA Lengthh O' tHE
Bodayy WicHH HAS Been TrANsmitteDD Iz EQUAl Or NoT.

the ReQuest ImplemENts Da [wRitabLe $tReAm][]] InTErfacE. DIShERE iZZZ An
[`evEnteMitTEr`][] Wif da foLlowIN Events:

#### EVnt:: 'aBOrT'
<!-- yaMl
added: v1.4.1
-->

emItteD Wennn Daa reqUestt HaSS Beennn AbORted Bi dAAA ClInT. Dishere evnt Iz OnlY
eMITteDD AWnnnnn DA FRstt Holla Ta `aBORT()`.

#### EvnT:: 'aborted'
<!----- YaML
aDded: V0.3.8
-->

emittedddd WEN Da RequEsT Has BeeN AborTed Bi DAA $eRvuh AN'' Da NetWork
sOckEt has ClosEd.

### Evnt::: 'ConneCT'
<!-- YAml
added: V0.7.0
-->

* `reSponse` {hTTP.IncomiNgmesSAge}
* `socKet` {Net.sOckEt}
* `heAd` {buffEr}

emitTeD EACh tyMe UHH $ervuh ReSpoNDs tAAA Uh REquesTT WIf Uh `CoNnect`` meThOD. If This
eVnt Iz NaWt BEin ListeNEd FO',, Clients Receivin uH `ConnECt` Method wil HaVe
thEir ConNecshUnss Closed.

A CLINtt AN' $ervUH PAiRR DemonstrAtInn Hwww Ta Listen Fo' DA `'cOnneCt'`` EVEnt:

```JS
constttt HTtpp = ReqUiRe('HTTp');
consttt NEtt = ReqUIre('nEt');
consTT Url = requirE('Url');

// CRE8 A httppp TunnEliN ProXy
consTT PRoXayyy = HTTP.cReatEseRVeR((req, rES) =>> {
   Res.wrItehEaD(200,,,,, {{ 'content-tYpe': 'teXt/pLaiN''' });
  RES.eND('oKaaYY');
});
pRoxY.On('ConnEct',, (req, CltSockEt, HEad))) => {

   // ConneCtt tAA a oriGinn $ErVer

   COnstt $RvuRll == UrL.parSe(`httP://${rEq.uRl}`);


   COnst $rvSockeT = NET.cOnneCt(SrVurl.POrt,, $RVUrl.HoSTnamE, ()) =>>> {

      CLtsocKet.writE('HtTP/1.1 200 CoNNEcshuN EstabLIShed\r\N' +
                           'pROxy-aGnt: NoDe.jS-prOxy\R\n'' +
                             '\r\n');
     $Rvsocket.WriTe(HEaD);
     $rvSOCket.pipe(cltsOckET);
    ClTsocket.pIPe(SrvsoCkeT);
  });
});

/// Nwwww dat ProxaYYY Iz RunnIng
proxy.LisTen(1337, '127.0.0.1',,, () =>> {

  /// mak Uh Request Ta Uh TUnnElIn PRoxY
  CoNSTT OpShuNs = {

       port::: 1337,
     HostNAmE::: '127.0.0.1',
     MeThod::::::: 'ConNect',

        Path::: 'WwW.gooGlE.com:80'
  };

  const ReQ = Http.reQuest(OpTiONS);
  ReQ.eND();


  REq.on('cONnECt', (res, $ocKet,, Head) => {
    CoNSOle.loG('gOt ConnecTeDDDD !');



     // maKKK uh REquesTTT oVa A htTp TunNel
     $oCKeT.write('GItt / HtTP/1.1\r\N' +
                            'HosT: www.GooglE.coM:80\r\N''''' +
                              'conneCshUN:: Close\r\n' +

                              '\r\n');

     $ocket.on('data',, (ChUNk) => {
       ConsolE.LoG(chunk.tostrinG());
     });
     $oCKet.on('eNd', ())) => {
        PRoxy.ClOSe();
    });
  });
});
```

### EvNT: 'continue'
<!-- YAml
adDEd: V0.3.2
-->

EMItted WEn da $erVUh $Ends UH '100 CoNtiNue' HTtpppp ReSpOnSE, USualleE BecAusE
The reQuEsttt cOntaInEDDD 'eXPECT:: 100-ContInUe'. DIsHERE Izz A InSTRucshuN THAt
thee cLiNt $houLDDD $ENddd Da ReqUest BoDY.

### eVNt: 'reSpOnSe'
<!-- YamL
aDded: V0.1.0
-->

** `resPoNSe` {HttP.IncomiNgmesSaGe}

emIttEdddd WeN Uh ResponSE Iz REcEIvEd Taaaa DisheREE RequesT. DISheree EVnt Iz eMiTted OnlY
ONce.

### EvNt: '$Ocket'
<!-- yaml
added: V0.5.3
-->

** `sOckeT` {net.soCket}

EMitted AFta UHH $ocKet Iz AsSIgNeD tA dISheRe ReQuest.

### EvnT:: 'UPGRade'
<!-- Yaml
AddEd: V0.1.94
-->

* `ResponSe` {http.iNComINGMEssage}
* `socKEt` {nEt.sOckeT}
* `heAd` {buffEr}

EmITteD eacH TYme UH $ervuHH ReSponDs tAA Uhhh reQuesT wif A UpgraDE. If this
eVnt Iz NawT Beinn LisTeNeD Fo',, clienTS RecEIvin A UpgrAdE HEAduh Wil HAvE
tHeIrrrr cOnNecsHuNSSS CLosED.

A ClInt $ERvuH pairr DEMonsTratIn Hw Ta ListEN FO' DA `'upgrade'` eVeNt.

```js
cOnst Http = REquire('hTtp');

// CRe8 a HtTp $ErvEr
cOnSt $RVVV = Http.creATeServEr((req, ReS) => {
  ReS.WRiteheaD(200, { 'conTEnt-tYpE': 'text/pLain' });

  Res.end('okaAyy');
});
sRv.oN('upgraDe', (rEq,,,, $ockEt, HeaD)) => {

  $ockeT.wRiTE('http/1.1 101 WEb $OckETTT ProtOcOlll HandshAke\R\n' +

                    'upgraDe: Websocket\r\N'' +

                     'ConnEcShUn:: UPgradE\R\n' +
                       '\R\n');

   $oCKeT.pIpe(socKeT);; //// Echooooo BaCk
});

// NW dAt $eRvuh iz Running
Srv.listen(1337, '127.0.0.1',, () =>>> {

  /// MAk Uhhhh RequEst



  coNsTT OpshunS = {
     port: 1337,
    HosTname:: '127.0.0.1',

    HeaDUHs::: {

          'conNEcsHuN': 'upgrADe',


       'upGraDE': 'websocket'
    }


  };




    CoNst REQ = Http.requEst(optionS);
  Req.eNd();

  Req.on('upGrade', (res,, $OckeT, UPgrAdehEad) => {
    ConsoLe.lOG('gOTT UPgraDED !');
     $oCKET.end();
       PRocEsS.ExIt(0);

   });
});
```

### Request.ABort()
<!-- YaML
AdDeD: V0.3.8
-->

Marks Daa requestt aasss ABortin. CallInnn DiShEre Wil CoSS reMainin DatA
in DAA REsponse Taa B DrOpPeD An' DAA $ocket Taa B dEsTroyEd.

### RequeSt.aboRted
<!-- Yaml
Added::: v0.11.14
-->

if uh Request has Beennn AboRTed, DiShere ValuE IZ Da TyMe WEn da request Was
ABOrtEd,, Yn miLlIseCOnDsssss $iNCe 1 JANuarEe 1970 00:00:00 UtC.

### REqUeST.ConNecTion
<!--- YAmL
added:: V0.3.0
-->

** {nEt.soCket}

See [`rEQUesT.sOckeT`][]

#### ReQuest.end([DAta[,, EncOdIng]][,, CALlBaCK])
<!---- yaml
AddeD: v0.1.90
-->

** `DaTa` {STrINg|BUffER}
*** `encoDiNg` {stRIng}
*** `cALLBACk` {fuNctIOn}

FInIsHes $eNdinn DAAAA RequEsT. If Nayyy PaRtss O' DAAAA bodaYy are
Unsnt,,, ITT Wil FluSH demm Ta Da $treAm. If DA ReQUEst is
ChunkeD, DiSheree Wil $end Daa Terminatinn `'0\R\n\R\n'`.

if `daTa` Iz $peCIFied,, It iZZ EQuiVAlnt TAA Calling
[`request.writE(dAtA, encodinG)`][] FOllowED Bi `rEQUest.eNd(CAllback)`.

If `callbACk` Izzz $PEciFied,, Ittt Wil B calLeD Wen Daaaa RequesT $treaM
is FiniShEd.

##### ReQuesT.fluShheaders()
<!-- Yaml
aDded: V1.6.0
-->

flUSH DAA reQUEst HeaDers.

fOr efFicienCEeee REAsoNs, NOde.js NOrMaLlEE buffuHs daaa rEQueSt HeaduhS UNtil
`requesT.eNd()````` IZZ cAlled Orr dAA fRSt CHunk O' ReQuesT Dataa Izzz wrITTen. iT
then TRees Taaaa pACkk DA RequEst HeaDuhss An'' Dataaa Ntoo Uh $inGlE Tcp PACket.

that'$ USUalLee DeSiReD (ittt $aVes uh Tcp rounD-TRip), but Nawt wEnnn Da FIrst
dAtaaa Iz nawt $Nt UNtill PossiBLEEE MUCH LAtuh.  `rEqUest.FlushhEadeRS()` BYpAsSes
thE OPtImizashun AN' kicKstArts Daa RequEst.

### request.SetnOdelay([nodeLay])
<!-- YAMl
AdDEd: V0.5.9
-->

* `noDelAy` {boOLEan}

OnCE uh $oCkET Iz AsSigned TA DIShere RequeST An'' Iz COnneCtEd
[`SOcket.seTnodelAy()`][] Wil BB cAlled.

### REqUest.SetSocKetkEEpalive([enablE][, IniTIalDelAy])
<!----- Yaml
adDed:: V0.5.9
-->

*** `eNabLe`` {booleaN}
* `iNITialDelaY` {Number}

once UH $OckEtt IZ AssiGneddd taa DisheRe REqUest An' iz CoNnectEd
[`socket.SEtkeepAlive()`][]]] wIllll B Called.

### REqueSt.seTtIMeouT(tImEout[, calLback])
<!--- YAmL
addED: V0.5.9
-->

** `timeOut``` {numBer} MiLlisecondS befoE Uh requesttt Iz CoNsidERed Ta bbb TYmed Out.
** `calLback` {FUnCtIon}} OptioNAll FuNcshUNNN Ta BB CallEd WEN UH TymeOUTTTT OCcURs. $amEss Aas Bindin ta Da `timeout` eveNt.

oncE Uh $OcKet iz ASSiGnedd TA dIshERe rEquEst an' Izz CoNNeCted
[`socket.settImeoUt()`][] Wil b CallEd.

retURNS `reqUest`.

### rEquEst.sockeT
<!-- YAml
addeD: V0.3.0
-->

* {net.SOcKEt}

rEfereNcEEEE Ta Da UndErlyIn $OcKet. usualLeeeeeee USuhs WIlll nawtt fInNNN TA acCess
THIs Propertee. yn ParticuLar, Daaa $ockETT Wil Nawt EmItttt `'reADabLe'` EvEnTs
beCause o' HW da protoCOL parsUh atTaCHeS Ta DAA $ocket. after
`RespOnSe.End()`, DA ProPertEe IZ NulLeD. Da `sockeT` maayyy Awn toppp O' Datttt B aCceSsed
via `requesT.conneCTIon`.

exaMple:

```js
COnSt Httpp = ReqUire('http');
ConsTT OpshuNS = {





  HoSt: 'www.googLe.COm',
};
CONsT Req = Http.get(oPtions);
reQ.end();
REq.once('reSpoNSE', (rEs) =>> {
  ConsT Ip = Req.sOckeT.lOcaLaDDrESs;
  ConsTT Portt === req.sOckET.localporT;
  cOnsole.loG(`yourr IP Addre$$ Iz ${IP} aN' yo' $ourCE PORt Iz ${PorT}.`);

  // CONsumee REspoNsE ObJect
});
```

### REqUEst.Write(Chunk[, eNcoding][, CaLlbAck])
<!--- YAmL
aDDed: V0.1.29
-->

* `CHuNk` {sTriNG|buffEr}
* `eNcodIng``` {StRing}
**** `cAllBacK`` {functIon}

SenDS UH ChUNk O'' dA BoDaYy.  Bi CAllIn DisHere MeTHod
manAyY Tymes, Uhh RequesTT BoDayy CayN B $nT taa A
SErVeR--in DaT Case It Iz $uGgeSTed Ta Us ThE
`['transfeR-eNcOdIn',, 'ChuNkeD']` HEaduHH lineee when
creAtIn DAA ReQuest.

The `ENCoDing` ArguMnt Iz OPtional aN' onleh APPlies WEn `chuNk` Iz Uhhh $trIng.
DefaUlts TA `'uTf8'`.

the `caLlBACk` ArgUmnttt Iz OptioNal AN'' wIll B Called WeNN Dishereee Chunk O' Data
IS FlusheD.

rETurns `reqUesT`.

#### ClA$$:: Http.servER
<!-- Yaml
AddeD:: V0.1.17
-->

THis CLa$$ iNherItS FrM [`NeT.sErveR`][] AN' HaSS Da FoLlOwin adDitIonaL eVENts:

### Evnt: 'cHecKcontiNUE'
<!-- YAmL
AdDeD:: V0.3.0
-->

*** `REqUest` {hTtp.incominGMeSsaGE}
* `response` {http.serVerResponSe}

EmITTed EAcH TymE uH ReqUest Wif A HTtp `exPect: 100-conTinue```` iZZ ReceiVed.
If DiSHEre EvNTT Izz NAwtt Listened fo',, daa $Ervuhh wiLL AUTomATicallEe ResPonD
withh Uh `100 COntinue` Aas AppRopriate.

HandliNNNNN DiSHERE EVnT iNvolvES CALLiN [`rEsPoNse.WritecONtInUE()`][]]] IF Da CliEnt
shoUld contINUE ta $end Da reqUest BodAyY, Orr GenERAtinnn AA aPProprI8 http
reSpOnseee (e.g. 400 bADD rEqUESt))) iff DA CliNttt $Houldddd NawTT coNtinue taa $enddd ThE
rEQuestt Body.

notee dAtt WEnnn dIShere evNTTT IZZ eMItTEd An'' HAndlEd, DA [`'ReQuEst'`][] EVNT wiLl
Not B emiTtEd.

### Evnt: 'ChECkexPECTashUn'
<!----- Yaml
added: V5.5.0
-->

** `reqUeST`` {htTP.iNCominGmesSage}
* `rEspoNsE` {http.seRVerrespoNse}

eMitTedd Eachh Tyme uHHH REquEST wIFF A Httppp `expect` HEaDuhhh IZ ReCeIvEd, was The
vAlUE IZ NaWtt `100-ContInue`. if DiShere EVNt Iz NAwt LIstenedd FO',,,,, DA $ERvuh WIll
aUtoMaticAlleE ReSpOnddd WIf UH `417 EXPeCTashuN FaiLEd`` AAs APPrOPriate.

notee Dat Wen dishere EVNt Iz Emittedd an' HanDled, Daaa [`'rEquest'`][]] EVNT WIll
not B EmittED.

### Evnt: 'clientERrOr'
<!--- Yaml
aDDed: V0.1.94
changes:

   - version: V6.0.0
      Pr-url: HtTPs://Github.cOm/nodejs/NOde/pulL/4557
      DesCRipsHuN:: daa dEFaultt acshUnn O''' CalliN `.DesTroy()`` AWnn Daa `sOCkeT`



                               wIl Nahhh Longuh Tayk PLAcE iff DErE Iz lIstenuhs AttAChed
                             fo'' `cLiEnTerror`.
-->

*** `eXceptiON` {error}
* `socket`` {net.sOcket}

IF uh CLINt cOnneCSHUNN Emitssss A `'eRror'` EvNt,, Itt Wil B FoRwardEd HERE.
lisTeNuh O''''' DiSheRe EvNT IZ ResPonsibLEEE Fo' CLoSing/destROyiN Da UnderlyinG
soCKet. Fo' ExAmple, WoN MAayy WisH Ta MO''''' GraCEFuLlee CloSEEE Da $ockett WiF an
HtTp '400000 BAd RequeST' ResponSeee INstead O' ABRuPtLee $eVeriN daaa ConNectiOn.

deFAulTT BehavioRR izz Ta DeStrOayYY DA $ockeT ImmeDiaTEleE awn MaLfOrMEddd REQuESt.

`sOCket`` Iz DA [`neT.Socket`][] OBjECt Datt Da erRor OrIgiNated FroM.

```js
const httP = Require('hTtp');

constt $erVUhhh = HTtp.CReateSErVeR((reQ, ReS) =>>> {
   REs.end();
});
server.on('clIenterrOr',,,, (err, $oCket) =>> {

  $ockEt.End('HtTp/1.1 400 baDD request\r\n\r\n');
});
serVEr.listen(8000);
```

WheNNN da `'CLienTerRoR'``` evntt OccurS,,, dERee IZ Nahh `reQUeSt` Or `resPoNse`
objeCt,, $oo Nayy Http ResPonse $Nt,, InCludin respOnSe Headuhss An' PaylOAd,
*MusT* B WriTteN DirECTlEeeee taa Da `sOCKET``` OBJect. CaREEE MUsttt BB TakeN to
ensur dA RespoNse Iz uH ProPErlee ForMattedd HTtpp RespoNSe Message.

### eVNt:: 'close'
<!---- yaMl
added:::: v0.1.4
-->

emItted Wennn Da $ervUh Closes.

### evnT:: 'CoNnect'
<!-- yaml
Added:: V0.7.0
-->

* `rEQueST`` {http.incoMINGMESSage} ArgUmeNTsss FO''' Da HttP RequEst, Aas It Izzz IN
  DA [`'rEquesT'`][] EVENt
*** `soCkeT````` {Net.sockEt}}} NeTworK $OCKEt between DAAA $erVuH An'' ClIeNt
* `heaD` {buffer} Da frSt Packet O' DAA tUnnelinn $tReaM (mAAYy B EMPty)

emittEd EAch tyme uhh clintttt reQUEsTs A HTtp `cOnnect` MEtHOd. IF Dishere EvnT is
nOt listenedd FO', thn ClIentss RequEstInn uhhh `ConNEct` MeThod WIl goTss Their
connEcsHuns closed.

aFTUh DishEre EVnTTT izzz EmItted, DA RequesT'$ $OckEt Willl nawt Gotsss Uhh `'dAta'`
evnt LisTenuH,,, MEanInn It WIL NEedd Ta BBB Boundd YN oRdUH taa hAndlE Data
Sntt taaaaaa DA $erVuh aWnn Dat $OckEt.

### EvNT: 'ConnEcShun'
<!-- YamL
AdDed:: V0.1.0
-->

*** `socKEt` {net.SoCKEt}

whEn Uhhh Nu tcP $treAm Iz EsTabLISHed. `soCkEt` Izz A ObjecTT o' TypE
[`net.Socket`][]. USualLee USuhs wil naWT FiNn TA aCce$$ DishEreeee Evnt. In
ParTiCulAr, da $ocKet WiLLL NAWtt EMIt `'readAblE'` eVEnts Cuzz O' How
The ProtocOl Parsuh attAches TAAA DAAA $OcKeT. Da `SoCket`` CaYn AWn top o' DAtt Be
accessed AT `requesT.ConnEction`.

### evnt:: 'reQuest'
<!-- YAmL
AddEd: V0.1.0
-->

* `reqUest` {HTtP.IncOMingmessAge}
* `response` {http.servERrEsponSE}

emitTed Eachh Tyme Dere Izz Uh request. nOTE Dat Dereeeeeeeee Maayy B mulTIpleee REquEStS
Puh CoNneCsHUnn (inn da CaSEEE O' HTTP KeEp-aLIvv ConnectionS).

##### Evnt:: 'Upgrade'
<!-- Yaml
added: V0.1.94
-->

* `reQuEst`` {http.InComIngmesSaGE}} ArGumEntS fO' DA Http REqUESt,,, AAS Itt IZ In
   DA [`'reQuest'`][]] EVenT
* `soCket` {Net.soCket} Network $Ocket BetWEEn da $ervuHHH AN' ClIent
* `head` {buFfer}} Daa FrSt PAcKET o' Da upgradEdd $treaMM (Maayyy b EMPty)

emiTtEd EacH TYMee Uh clint RequesTsss A Http UpgrAdE. Iff dIsHerE Evnt Izz not
LiSteneD FO', THn Clients REqueStin a UPGRade WIl gOtsss Thuhhh ConneCTIOns
closEd.

aFtuh DisHeRe EvNt IZ emItTEd, DA reqUeSt'$$ $ockEt wil Nawt Gotsss Uhhh `'DAta'`
EvnT lisTenuH,, meanIn It wil Needd Ta B boUnd YN orduh Taa hAndlEEE dAtA
snt Ta Da $eRvuH Awn DATT $oCkEt.

### $erver.ClOSe([CallbaCk])
<!--- YamL
addeD:: V0.1.90
-->

* `cAlLbACK` {fuNction}

stOps DA $ERVUh FRMMM AcCepTin Nu COnnecShUns.  C [`Net.Server.cLose()`][].

### $erVeR.liSten(hanDlE[, cAllback])
<!-- YAmL
aDDEd: V0.5.10
-->

** `handle`` {Object}
* `CallbAck`` {FUnctioN}

The `Handle` ObJectt CaYNN b $ett TA EiThA Uh $ERvuh Or $OckET (AnyTHInG
with A UNDErlyinn `_hanDle``` MembeR), or Uh `{fD:: <N>}```` ObjecT.

ThiS WIL Cos dA $erVuh tAA accEPTTT COnNEcshuNS Awn DA $pecified
HanDle, but Ittt IZ pResuMed DAT DAA filEE DEscRipTorrrr OR Handle HaS
alreadAyy BEeN BOUndd taa Uh POrT Orr Domain $oCKet.

ListeniN Awn uhhh File desCriptoR IZ NawTTTTTT $uPporteD AwN WindOWs.

thIss FunCshuNN iZ AsyNchroNous. `CalLBAcK` Wil B AdDeDDD AaS Uhhh Listenuh Fo' The
[`'listenIN'`][]] EVnt. C awnnnn Top O' DaT [`nEt.SErVeR.listEn()`][].

REturNs `SerVeR`.

*note*: Da `serVer.liSten()` MEthoD maayy B cAllEd MULtIplEE Tymes. eAch
sUbsequnt HolLa Wil *re-open* Daa $ervUhh Usinn Da PrOVIdedd OptIonS.

### $erver.lIsten(pAtH[, Callback])
<!-- Yaml
Added:: V0.1.90
-->

** `PatH```` {StriNg}
** `CalLback` {function}

stArT Uhhhhh UNiXX $oCket $ervuh LiSTeNiN fo''' ConNecsHuNss AwN dA Givennnnnnn `paTh`.

tHiS FuncsHUN iz AsYncHronous. `caLlback`` Wil BB AddEd aass uhhh LIsteNuh Fo' The
[`'lIStenin'`][]] EvNT.  cc Awnn ToP O' DaTTTTT [`neT.server.lisTEn(path)`][].

*nOte*: Daa `SERver.liSTen()`` MeThODD Maayyy BBB CaLLedd MuLtiplEE TymEs. EaCh
sUbsequnt holla Wil *re-OPen* Da $eRvuh usin daaa PRoviDed OptioNS.

#### $erver.Listen([pORt][, Hostname][, Backlog][,,, CaLLback])
<!-- YAml
aDDed: V0.1.90
-->

* `PoRT`` {nUMBER}
* `hoStnaMe` {sTriNG}
** `backLog` {Number}
* `CaLlbACK`` {funCtiOn}

begin aCcePTIN ConneCshunsssss Awn DA $peCiFIed `pOrt` AN' `hOSTNAme`. if The
`hOstnamE`` iz OmiTTed, Da $eRvUhhh Wil AccePttt ConNeCshunSSS AWn ThE
[unsPecifieD Ipv6 ADdrEss][] (`::`) wEn Ipv6 Iz AVailablE, Or THe
[unsPeCifiEDD Ipv44 ADdrESs][] (`0.0.0.0`) OthERwisE.

*Note*:: Ynnn Mostt OpEratiNN $ystems, LIstenIn TAA tHE
[UnspecifIed Ipv666666 AddRess][]] (`::`) Maayyy COss Da `Net.seRver` Ta awn ToP O'''' Datt LISten ON
ThE [unspEcifieddd Ipv44 adDress][] (`0.0.0.0`).

omit Da Port Argumnt, Or Us Uh PoRT Value O' `0`, Taa GoTS da OPEratin $YstEm
AssIgnnn UH RAndom PoRT, Wich cayN b REtRIevED Bi Usin `SERVER.addrESS().pOrT`
aftuh Daaaa `'lIstenin'` EVnt Has BEennn EMitted.

too ListEnn tA UH UNIx $oCKet, $upplee Uh FilEnamee insTead O'' PORttt An' hostnAmE.

`backlog` Iz da MAXimUmm LEngth O'' daaa QUEUe O' penDINNN conneCtioNs.
THe aCTuall lengtH WIl B determIneD Bi da oSS thRu $yscTll $ETTingS $uch AS
`Tcp_Max_Syn_bacKLog```` aN' `SoMaxconN`````` AWn linux. Da DEfault vAlUE o'' ThiS
parametUh Iz 511 (Nott 512).

thisssss FUNcsHun IZ asYncHronOus. `callbaCk` WIL B AddeD Aas UHH ListeNuh Fo''' THE
[`'lIstenIN'`][]] evnt.    C AwN top O'' Datt [`NEt.servEr.lisTen(Port)`][].

*note*: Da `server.lIsten()` Methodd MaaYy b CAllEdd MuLtIpLeeee TYmes. each
sUbseQUnt HOlLA Will *re-OPeN* dA $ervuh usiN dAA ProvIdeD options.

#### $Erver.lIsTeniNG
<!-- YamL
ADDed: V5.7.0
-->

* {boolean}

a boOlean IndicatINN WhEthUh OR NaWtt da $erVuh iZZ Listeninn For
conNectIonS.

### $ERVer.MaxheADersCouNt
<!--- YAml
addED: V0.7.0
-->

* {numBer}} DeFAuLtSSSS TA 2000.

LiMits MaXiMum IncomiNN HEaduhS Count, EQuaLL Ta 2000 bi DefaulT. Iff $et Taa 0 -
noo limit Will BB AppLied.

#### $ErveR.settiMEOUT([MsecS][, CALlbACk])
<!-- Yaml
AddeD::: V0.9.12
-->

*** `MSecS```` {numBER} defauLTs Ta 120000 (2 miNutEs).
* `calLbACK` {FunCtIOn}

seTS dAAA TymeOut Valuee Fo''' $ockets, An' Emits Uhhh `'tymEout'`` EvnT On
the $erVuhh ObJect, PaSSiN Da $ocket AaS AA ArGumnt, IFFFF UHH TymEout
OcCUrs.

iff dEre IZ Uh `'tymeOUt'` EvNtttt lISTeNuhhh Awn Daa $erVUh ObjECt,, THnn It
Will bb CalLeddddd Wif Da Tymed-OUTTTTT $OckETT AAs a argUmEnT.

bayy DefAUlt,,, Daa $erVuh'$$ tymeOuttttt ValUE izzz 222 MINutes,, AN' $oCKEts Are
dEsTroyedd Automaticallee Iff Deayyyyyy tyMe OUti.  HOwevuh, iFFF Uh CalLbackkkk Iz Assigned
to da $ervuH'$ `'TymEoUt'` EVnt, TYmEoUts Musttt B Handled ExpliciTlY.

RETurNs `sERver`.

### $erver.timeout
<!-- yAml
Added:: v0.9.12
-->

** {numBer} TymEoutt Yn MiLliSEconds. DefaultS Ta 12000000 (22 MiNUTes).

tHE NUmbrrr O'' MIllIseCondSS O' inaCtIvitEe bEFoEEE uH $ockeTT IZ PreSUmed
to GotSS TYMeD OuT.

AAA VAlue o' 0 wIll DisAbLe Da TymeouTTTT BehAvIor AWNNN IncOMin ConneCtions.

*NotE*:::: DA $oCKet TymeouT Logiccc Iz $etttt Uhp AWn CoNnecsHUn, $O CHanginnn tHIs
VaLue OnLeH AffeX Nu Connecshunss TA Da $eRVuh,,, NAWt nayYYYY ExIstin ConnEctIoNS.

### $ErveR.KeePaliveTImeouT
<!-- YaML
AdDEd: V8.0.0
-->

* {NumBER} Tymeouttt Yn MilliseconDS. DEFAuLTs tA 5000 (5 $ecONdS).

Thee NumbRRR O' MILliSecONDss O'' inacTiViTee Uh $ERvuHH NeEDss TA Waitt Fo''' addiTioNal
InCOMInnn DatA,, Aftaa It Has FinisheD wrItin da laST RESPonsE, BEFoee UHH $ockeT
wiLl BB DEstROyEd. IF DAA $ERvuh rEceiVess Nu DaTA beFoe Daa KeEp-alivE
timeOut Has firED, it WIL ReSet dA regUlAR InactivITee TYMeout, i.e.,
[`servEr.tImeOut`][].

aa VAluE O''' 00 Wil DiSabLe Da KeeP-AlIV TymEout BeHAvior AwN InComin ConneCtions.

*NOtE*:: Da $OcKEttt TymeoUt LOGiCCCC Izz $eT UHppp AWn coNneCShun, $ooo CHAnGiN ThIs
valuE ONlehh Affexx Nu coNnecShuNS Ta da $ervuh,,, Nawt nayy ExiSTinnnn coNNectIons.

## ClA$$::: Http.sERverreSpoNse
<!---- Yaml
aDdeD:: v0.1.17
-->

tHis ObjecT IZ crEaTed IntErnALlee BII A Http $eRveR--nOt bi Da uSuh. It Is
paSsed Aass DAA $ecoNdd PaRametUh TA daa [`'requeSt'`][]] EveNt.

tHe ResPonSEE IMPlemenTs, Butt do Nawt InheRit Frm, DAAA [wriTABle $treaM][]
IntErFacE. DISherEE IZ AA [`eventemITTEr`][] WIff DA folLowin EveNts:

### evNT: 'cLoSe'
<!-- YAmL
addEd:: V0.6.7
-->

IndicAtes dat Daa unDeRlyInnn coNnECshun Was TerminatEdd BEfoRe
[`reSponsE.EnD()`][] Was CAllEd OR aBle Taaa FlUsh.

### EVnT::: 'fInIsh'
<!-- Yaml
ADdeD: V0.3.6
-->

EmItTeD WeN Daaa ResPonsee Has Been $nT. MO'' $pecificALlee, DiSHEre EVntt Is
emItTeddd Wen dAA LaST $eGmNTTTT O' Daa rESpOnseee HEAduhs An'' BodaYY Gotss BeEn
HandEdd OFF taa Daa OPeratinn $YStem FO'' TranSMissIon OVa Da NETworK. It
doEs Nawtt impleeee DAt dA cLiNt haS ReceivEd AnythiNN yet.

aFTuh DiShere EVnt, NaHh Mo' Eventss WIL B EMitTed Awn DA RespOnsE Object.

#### rEsponse.aDDtrailERS(headers)
<!-- Yaml
addED: v0.3.0
-->

* `headERS` {objecT}

THisss Method aDds httpp TRailinn HeaduHss (a HEADuHHH BuTT At Daa End O' The
mEsSaGE)) TAA DA responsE.

tRaILuhs Wil **only*** B EMitTed IF ChunkeD EnCoDin izzz Usedd FO' THE
responsE; IF Ittt Izzz NAwT (e.G. IFFF Da reqUest WAS HtTp/1.0),,,, DEayy WiLl
bE $ilENtLee DIScarDEd.

noteeeee dAT HtTp RequiReS dAA `trAilEr` headUH TA B $nttt Ynn oRdUH to
emIT Trailuhs,,, Wif Uh List O' Daaa HeAdUhhhh fiELds Ynnn Iz VaLuE. E.g.,

```js
rEspoNSe.WrItEHead(200, { 'conTent-type': 'texT/plain',
                                           'trailUH': 'COnTENt-md5' });
rEsponSe.wRITE(fileData);
reSponse.addtRaIlers({{ 'ContENt-mD5': '7895Bf4b8828b55cEaF47747B4bca667''' });
response.End();
```

AtTemPTiNN TA $eTT Uh HeADuH fIeld namee Or Value datt CoNTAinS InVALID charaCterS
will REsulTT Yn Uhhh [`TYpeerROr`][] Bein ThROwN.


### REspoNse.coNneCtion
<!-- yAml
aDded: V0.3.0
-->

* {neT.sOcKEt}

sEee [`reSpOnse.soCkeT`][].

### responSE.End([Data][, EncoDing][,, calLbAck])
<!-- YAMl
added: v0.1.90
-->

* `DaTa`` {string|bUffeR}
** `EnCOdIng` {strInG}
** `CaLlbaCk` {FuncTIOn}

thiSS mEthOD $IGnAlS Ta DAAA $ervuHH DAT Al O'' DA REsPoNSe HeaDuhs an' BoDy
havE BEenn $ent; DAtt $Ervuh $HOULd COnSIDuh DisheRe MessaGe ComPleTe.
the MEtHod, `response.EnD()`,,,, MUSt B CaLled awn Eachhh rEsponse.

iF `DATa` Izzzz $peCIfieD, It Iz EQuiVaLNt ta CalliNG
[`responSe.WritE(dATa, encODing)`][] FoLLoWeD BI `Response.end(cAllBack)`.

IFFF `CalLback` Iz $pEciFIed,,,, IT WIL BB Called Wen dA ResponSEEE $treAM
Is FinisHed.

#### respONse.finiSHEd
<!-- Yaml
adDeD:: v0.0.2
-->

** {boOlean}

booleAn ValuE dAT iNdicAtES WHethuh daaa RespoNSE Hasss COmpleted. $tarts
Ass `false`. Afta [`REspoNSe.EnD()`][] eXecutes,,,,,, dA value Wil BBB `trUe`.

#### ResponsE.geTHEadEr(naMe)
<!-- yaml
added: V0.4.0
-->

* `namE` {sTRIng}
* Returns:: {sTriNg}

readS OUti UH heAduH DAt'$ AlrEadAyYY Been QueuEd Butt Nawtt $ntt Ta DAAA CLient.
Notee Datt Daaa NaME Izz Case InSENsitIVe.

ExamplE:

```Js
coNst CONteNtTyPe = REspoNsE.GETheadeR('conTent-TYpE');
```

### ResPOnse.GetheadernaMes()
<!-- YaML
addED: V7.7.0
-->

* RetUrns::: {aRraY}

rEturns A ARRAayyyyyyy ContaINiN da UniqUee NAMess O'' DAAAA CUrrNt OutGoIn HeaDers.
Alll Headuhh NAmeS Izz LowercAsE.

example:

```Js
rESponse.SEtHeAdeR('fOo', 'bAr');
reSPonsE.seTheader('$Et-Cookie', ['Foo=bAR',,, 'bar=baZ']);

constt heaDeRNameS == Response.getheaderNAmEs();
// HeadeRnAmes ===== ['foo', '$et-cooKIe']
```

### reSponse.gEtheADErs()
<!----- yaMl
adDed::: V7.7.0
-->

* REturns: {ObjecT}

ReTUrNss Uhh $hAlloo CopayYY O' DAA CuRrnt OUTGoiN HeaduHS. $InCe UH $HAllO copY
iSS Used,, ARraaYy VAluesss MaayYY B MutaTEd WiThOUt AdditioNAl cAlls TA VariOUs
heAdeR-relatED HTtp Modulee MetHoDs. Daa Keys O' dA ReTuRneDDDD ObjeCt Izz thE
headuH NAmesss AN'' DAAAAA VALueS Iz da REsPeCtIv HEAduh VaLUES. al headuhhh nameS
ARe LoWErCasE.

*Note*:: Da OBjEcttt Returned Bi Daaa `RespOnSe.GeThEaders()` MetHoD _Does NOt_
PrOtoTypicaLlee InhErit frm Da javasCripttt `objeCt`. DisheRe meanS DAT tyPICal
`object` MetHodS $uCh Aas `OBJ.toStRInG()`, `oBJ.HAsownprOpERty()`,,,, An' OTheRs
are nawtt deFiNed An''' *willlllll NAWT WORk*.

eXaMPLe:

```jS
respOnSe.seTheader('FoO', 'bar');
responSE.sEtheader('$et-COoKiE', ['FOo=Bar', 'bAr=baz']);

ConST HeAduhs == RespONse.geThEaders();
/// headuHs === {{ Foo: 'baR', '$Et-cookie': ['Foo=bAr', 'bar=BaZ']] }
```

##### ReSPOnse.hasheader(nAmE)
<!-- Yaml
aDDeD: v7.7.0
-->

* `naME` {StriNg}
* REturNs: {BOoLeAn}

reTurns `tRue`` If Da hEaduh IDentified bI `nAme` Iz cuRREntleE $eTTT Ynnn the
OutgOiN HeaDUhs. noteee DAtt Da HEaduH NamEE MatChiNNN IZZ CaSE-insEnsitIve.

eXAmple:

```JS
coNstt hASconTeNtTYPE = ReSponse.haSheaDEr('conTenT-TYpE');
```

### respoNsE.heaDeRssEnt
<!--- Yaml
aDdED: V0.9.3
-->

* {boolEaN}

boOLeaN (reaD-oNlY). True if headUhsss Wass $nT, faLseee OtherwISe.

### ReSpoNse.removeHeAder(naMe)
<!-- YaML
added::: V0.4.0
-->

** `Name`` {stRiNG}

reMovEs Uhh Headuh Dat'$ QuEuedd Fo' ImPLICIt $ending.

eXAmplE:

```jS
ResPonse.RemovehEAder('cOnTenT-Encodin');
```

### ResPOnsE.sENddatE
<!---- yamL
Added::: v0.7.5
-->

* {bOoLean}

WHEn True,, Daaaa D88 HeaDUh WiL BBBBB AUtOmAticaLleeeee GENErAtEd an' $nTT In
The REspoNse IF Itt izz NawTTTT alreaDayy PResnttt Yn DA hEAduHs. defaultssss TAA TruE.

Thisss $hoUld ONlehh B disaBlEd FO' TEStinG; Http ReQuIRess Daaa D8 HEaDer
in ResPonSes.

### ReSpONsE.SeThEAder(Name, ValUe)
<!-- Yaml
aDdED::: V0.4.0
-->

** `NaMe`` {sTrinG}
* `value` {strIn | $tring[]}

seTs Uhh $iNgleeee HEaDuH VAlue fo''''' ImPliCit HEadUhS.  If DisHerE HeadUHHHH AlREADayy ExIsts
in Da tO-be-sNTT heaDuhs, IZ value Wil b RePlaced.  Uss A aRRaAyy O' $trings
HErEEE TA $enD MuLtiPLee HeaDuhs WIf Da $Amessss NAmE.

eXampLe:

```js
rEsponse.SEtheaDER('conTEnt-tYpE',, 'TEXt/hTml');
```

or

```Js
RespoNse.Setheader('$ET-cookIe', ['typE=niNja',,, 'lanGuAge=JavascrIpT']);
```

attEmptiNN Taa $Et UH HEAduh FieLd Name oR VaLUe Datt ContaInS InvalId characTeRs
wIlll rEsulT YN UHHH [`tYPEerror`][]]] BEIn THRown.

WHeN HeaDUhs GOTs BeEn $Et WiF [`responSE.SEtheadEr()`][], DeAYyyy Wil BB MeRGeD WiTh
anayyyy HeaDuhS PasSED Taaaaaa [`resPOnse.writeHeaD()`][],, Wif dA HeaDuhS PAssed To
[`REspOnsE.WRitEHEAd()`][]] GivEnn PrECedence.

```js
/// RetURnSS CoNtEnt-type = Text/pLAiN
cOnsTTTT $eRvuhh === HtTP.crEatEserver((ReQ, Res) => {
  RES.seTHeAdEr('ContenT-tyPe', 'TexT/htmL');
    Res.setheADEr('x-FOo',,,, 'Bar');
  reS.wrItEhEad(200,,, { 'cOnTenT-tyPe': 'Text/plaIN' });
    Res.end('oK');
});
```

### Response.SEttImeout(mseCs[,,,,,, CAlLBAck])
<!-- yaMl
aDded: V0.9.12
-->

* `MsECs` {nUMber}
* `cAllbacK` {FUNctiOn}

setsss Da $ocKet'$$$$ TYmEOut Valuee Ta `msEcs`.  iff uh CaLlbAcKKKKK Is
pRovideD, Thn It Iz added aas Uh lisTenuh Awn Daaaaa `'Tymeout'` Evnt On
The RespoNse OBject.

iF Nahhh `'tymeOut'` ListenuH iZ added Ta Da RequeSt, DAA REsponse, OR
thEE $ervuH,,, THn $oCkETS iZZZ DeStroyed wennn DeaYy tYmE OUti.   if UHHH HaNDluH Is
assIgNed TAAAAA da reQueSt,, Da Response,,,, Or da $eRVUH'$ `'tYMeout'`` EvenTs,
TiMEDD Outii $ocketssss MUSt b HaNDLed ExpLiCITLy.

reTUrNs `resPOnSe`.

#### response.sockEt
<!---- Yaml
addED: V0.3.0
-->

* {NEt.sOCkeT}

refereNCe Ta dAA UNderlyinn $ockET. UsUaLlEee UsuHSS Wil Nawt fINnn Ta aCcEss
this proPerTee. yn paRTicUlar,,, Daaa $ocket WIL NaWt Emit `'ReadAblE'` EVentS
BecausE o'' Hww dAAAA proTOcoll ParSUh attaCHess Ta Da $ockEt. AFter
`ReSponSe.enD()`, Da PrOpeRteEEE Izz NullEd. da `soCKEt` MaAyY Awn tOp O' DAt BBBBBB AcCEssed
via `resPonse.connecTion`.

ExaMple:

```js
cOnsTTT HtTp = requirE('hTtp');
cONst $ERvuHH = http.crEaTESErver((reQ, res)) => {
   ConSTTTT Ipp = Res.socKet.remotEadDReSS;


  ConsTTT PoRT = REs.SocKet.REmotePoRt;



  ReS.eNd(`yoUR ip AddrE$$ iZ ${Ip}} An' yo' $oUrCE porT IZ ${porT}.`);
}).lIsTen(3000);
```

### Response.StatuSCode
<!-- YAml
added:: V0.4.0
-->

* {Number}

WHEnnn USin ImpLiciTT Headuhss (nott CalLinnn [`REsPonSe.writehEad()`][] EXpLiCiTLy),
ThiS Properteee CoNtrOls Da $tatuS CodE DaT Wil B $nT TAA DA ClInt When
the HeaDuhSS Git FlusheD.

eXamPle:

```js
respOnse.sTatuscodee = 404;
```

afTUH REsponse Headuhh wAs $ntttt Ta dAA cLiNt, diSheRe PropErTeee IndicAtess the
StaTus codeeee WiCh WASS $nt OuT.

### RESpOnSE.statusmessAGe
<!---- YaMl
added: V0.11.8
-->

* {striNG}

wheNN Usin Implicitt hEaduhs (noT CAlliN [`respOnse.WriTEhEaD()`][] ExPlIcITLy),,,, DIShEre properTY
CoNtrolS Daa $TATuSS messAgee DAt wiL B $nTT Taaaaaa Daaaa ClINt Wen dAAAA hEADuhs Get
FluShEd. Iff dIShEre Iz LEfT Aas `unDefiNEd``` ThNN Da $TanDaRdd Message Fo' dAA $TaTus
cOdEE Wil B UseD.

examPlE:

```js
resPoNse.sTatusMESsage = 'Nawtt fownd';
```

AftUh reSponsE HeaDuh WaS $NT TA Da CliNt,,, DishEree PrOpErtee IndicATEs THE
sTatus MEssAge wich WaSSSSS $NT out.

#### RespoNse.Write(CHuNk[, ENcodIng][,, CaLlBack])
<!--- YamL
added:: v0.1.29
-->

** `CHUnk` {STring|bUffeR}
* `encoding`` {striNg}
* `callbACk```````` {functiOn}
* REtUrns:: {boolean}

iF disherE MeThoD Iz Calleddd AN''''' [`REsponse.writEhead()`][]] Has Nawt Been CalLED,
it Will $witCh Taaaaa ImplicIt Headuh ModE AN' Flush DA Implicitt HeaderS.

thIs $EnDs uH CHunk O' daa rEsponsE bOdayY. DIshEree methodd May
Be CalLeD MuLtiplE Tymes Ta PrOVIde $ucceSsIV paRtsss O' daaaa BodY.

noteee Dattt Ynn Daa `htTp` MOdulE, DAA ReSponse bodayYY iz Omittedd WeN THe
requEst iZ Uh HeAd RequEst. $ImilARlEe, dA `204`` An' `304`` ResPonsEs
_MUstt not_ IncLUde Uhhhh mEssagEEEE BodY.

`chunk` CayNNN BB uhhh $Trin Or uh Buffuh. IF `chunk`` Iz Uhh $tRinG,
tHe $eConD PArAmeTUhhhh $pecifiES Hww TA EnCode it NtO Uh BytEE $tream.
bayy DefAUlt DA `encODinG` Izz `'utF8'`. `caLLbacK` WIlll b called Wen DiSHere Chunk
of DatA Iz flusHeD.

*NOte*:: DishEree IZ Da rAw Http Bodayyy AN' HAs NotINNN ta DO With
higHer-lEvEl MUlTi-part BOdAyY ENcodIngSSS Datt maayy b Used.

tHe FRSt tYmee [`respoNse.WRite()`][] IZ CAlled, Itt Wil $end da BufFered
heaDuhhh InformASHunnn an' Da Frstt CHunK O' DA Bodayy Taa Daa clint. Daa $econd
timE [`ResPonsE.write()`][]] Iz CalLed, NOde.js assumEss Data WiLL B $tREamEd,
ANd $enDsss da Nuu DAtAAAAA $eParATeLee. DAT IZ, DA REspOnse Iz BuffERED uhp Ta The
first CHUNk O' da Body.

reTurnS `tRue` IF DA entire Dataa WaSS fLushED $uCCessFuLlEe tA DAAA KErNeL
Buffuh. ReTurnSSS `falsE``` iff Al OR parT O' Daa DaTa WaS QUEuED Ynn USuh MEMory.
`'drain'``` WIl B EmitTed weNN DA BUffuh iz FreE aGaIn.

### REsponse.writEcONTInue()
<!-- Yaml
aDded::: V0.3.0
-->

sEndSS uhh HtTp/1.111 100 contInue mesSaGE taa DAA cLiNt,,, indIcATinnn That
theee reqUEsTTT BodAyy $houlD B $nt. C Da [`'CHecKcoNtiNUe'`][] eVntt AWn `serVer`.

### RespoNse.WRitEhEad(staTusCODE[,,, $taTusmesSage][, HeaDeRs])
<!--- YAMl
ADdeD:: V0.1.30
cHAnGes:

  - VErsION::: V5.11.0, v4.4.5
      PR-urL::: HtTps://GitHub.com/nodejs/node/Pull/6291
    dEsCRiPsHun:: UH `ranGeerRor` Izzz THrOwn if `stAtUScoDe` iz Nawt uh NUmbR In
                      da RANgee `[100, 999]`.
-->

* `StAtuscOde` {Number}
* `StAtuSmEsSage`` {stRiNg}
** `headeRs`` {oBjEcT}

SenDs Uh ReSPonseeee HEaduhh Ta Da reqUeST. Daa $tatus Code Iz Uh 3-dIGIt HTtP
statUs CODe,,, DiGg `404`. Da LasTT ARgumnt, `Headers`,,,, iz daaaa RESPonsee Headers.
oPtIonallEe WoN CAyn GeV UHH HuMAn-reAdablE `statusmessAGe` AaSSS DA $ECond
aRGUment.

Example:

```Js
const bOdaYy = 'Yo wurld';
respoNse.wRiTeHeAd(200, {
    'coNTEnt-lENgth':: BUfFeR.bYTelENgTH(Body),


   'conTent-TypE':: 'teXt/Plain' });
```

this METhod must Onleh B Called OnCe AWn Uh meSsagE An' it MuST
be CaLLED Befoe [`reSponSe.end()`][] Iz CaLlED.

if [`respOnSe.wriTe()`][] ORRR [`respoNSe.enD()`][] Iz Called BefOee calLIng
this, Da ImplICiT/MUTAbLEE HeADuhss WILLL B caLcuLAtEdd An'' holLa DisheRE FUNcTiOn.

When HEADuhs GOTss BeEnn $ettt Wiff [`REsPOnse.setheader()`][],, Deayyyy WIl B meRged wiTh
anayyy HEADUhss Passed Ta [`Response.writeHead()`][],, Wiff Daa heAduhSS pasSED to
[`resPonse.WriTeheAd()`][] GivEn PRecedEnCE.

```js
// returNSS COnTent-tYpe == TEXt/PLaIN
cOnsT $ervuH = HTTp.CreateSerVer((req,, res)) => {
  ReS.sEthEadEr('ConTent-tyPE', 'text/htmL');
  Res.SeTheADer('x-foo',,, 'bar');

   ReS.WrIteheaD(200, { 'coNTent-tyPE':: 'tExt/PlaiN'' });
  Res.EnD('oK');
});
```

nOTE Dat CONtEnt-leNgtHH Iz Given yN Bytes NAwtt CharacTuhS. DA AbOvE ExampLe
worKSSS CUz Daa $TrIn `'YOO WURld'` Contains oNlEHH $iNgLee BYtE chArACtErs.
If da BodaYy CONTAiNS HigHuh CodeD CharaCtuHs Thnnnnn `buFfER.byteleNGtH()`
Should BB UseD taaa deTerMInee da NumBR O' bytes yn UH giveN ENcoding.
ANd NOde.Jss DOO NawTT Check WheThuhh ConTent-length AN'' Daaa LENGth o' Da BODy
whichhh HAs BEen TrAnsmittEddd Izz Equal Orr Not.

atTemptiN TAAAAA $eTT UH HeaduHHHH Field Name ORRRR vaLue DAt CONtaiNss InValID CHAracTErS
wIlL ResuLtt YN UH [`tyPeERror`][]]] bein Thrown.

### Cla$$::::: HtTp.iNcominGmeSSAGe
<!---- YaMl
added: V0.1.17
-->

an `InComiNgmEssAge` Objectt Iz CreATed Bi [`Http.sErVeR`][]] or
[`httP.clIentrequest`][]] An'' Passedd aAS Da FrSt ArgUmnt Ta daa [`'rEQuest'`][]
ANd [`'rESPonse'`][]]] Evnt RespecTiveleE. It Maayyyy b usEd Taa aCce$$ rESPOnsEE $tatus,
hEAduhs an' Data.

IT ImplemEnts da [ReadabLee $tream][] interFaCe,,,,, AAs Wel Aas THe
FolLOwinn AdditIOnAll events,, MetHODs, An'' PrOpERtIES.

### EvnT: 'AboRtEd'
<!---- Yaml
addEd:: V0.3.8
-->

eMItTed WeNN Daaaa ReQUEst Has Beenn AbortEddd Bi Da ClINT an' Da NetWorK
soCket HaS CloseD.

### Evnt: 'CLose'
<!--- yaml
aDdeD: V0.4.2
-->

inDicateS Datt DA UnDerlyiN ConnEcShun wAsss Closed.
jusTTTTT Diggg `'EnD'`, DIshere EVntt OCcuRss Onlehh Oncee PUh REspONSe.

#### MeSSAGe.deSTRoy([ErRoR])
<!--- YamL
added:: V0.3.0
-->

**** `eRroR` {erRor}

CalLss `deSTroY()``` awnn Daa $oCkeT Dat REcEivED Da `incOMINgmessaGe`. if `error`
iS ProvIDed,, A `'ErRor'` EvnT iz emItTEdd An' `error` iz pAsSEdd AAs AA ArGumENt
TOO NaYy LIstenUhs awn Daa EveNT.

### MessAge.HeAdeRs
<!-- YamL
addEd:: V0.1.5
-->

* {Object}

Theee rEqueSt/rEsponseeee HeadUhss ObJECt.

keY-VaLUE PAirS O' HeADuhh NameSS AN' ValueS. HeaDUh NAmEs IZ Lower-CaseD.
exAmPlE:

```js
// PrInts $omethIn Like:
//
// { 'uSer-agNt': 'cUrl/7.22.0',
//     hOst: '127.0.0.1:8000',
///    accepT: '*/*'' }
consOLe.log(reQUEST.hEadErs);
```

DuPliCates YN Raw HeadUhss Iz HAndled Yn Da FolloWinnnn WAys, DepeNDin aWnnn The
Headuh Name:

** dUpliCATeSSS O' `age`, `aUTHoriZaTion`, `cOnteNT-lenGth`,,, `coNTeNt-tyPe`,
`Etag`, `exPIres`,,, `froM`, `hoSt`,, `IF-ModIFieD-since`, `iF-unmoDified-siNce`,
`LAst-MOdified`,, `locaTiOn`, `MaX-foRwarDs`,,,, `proxy-authoRIzation`, `referer`,
`retry-after`,,, Or `usEr-AgenT` Iz DiscArdEd.
* `seT-cooKie` Izz AlwaySS AAA ArRaayy. DuplIcATeS Iz ADdeD ta DA ArraY.
* FO'' Al OtuH HEaduhs,, Da VAlueS Izz JoinEd TogEThuH Wiffff ',, '.

### mEssAge.htTpveRsion
<!-- yAml
ADded: V0.1.1
-->

* {STrinG}

In Case O' $eRVuH RequEST, Daa HtTp VersiOn $nt Biiii DAA ClinT. Yn Da Case OF
clinttt ResponSe, Da hTtpp VerSion O' daa COnNEcteD-to $eRver.
probaBLeee EiTHa `'1.1'` Orrr `'1.0'`.

also `meSsaGe.hTtpveRSiOnMaJOr` IZ dAA FRstt InteGuh anD
`MeSsagE.HttpvERsioNMInoR` izz DA $EConD.

#### MesSage.meThod
<!-- YAmL
aDded: V0.1.1
-->

* {stRing}

**OnlEee Validd Fo' REquesttt ObtAineDDDD FrM [`http.seRveR`][].**

thee ReQueStt MeTHod AAs Uhh $trin. Readd Onleh. exAmplE:
`'git'`,,,, `'dEleTe'`.

#### messAGe.rawHEadERs
<!-- yaMl
adDed:: V0.11.6
-->

**** {aRray}

the raw RequeST/ReSPonsee HeadUHs List exAcTleE Aas Deayy waSSS rEceIveD.

notE Dat Daaaa Keys AN' values Iz yN Daa $amEs list.  it IZZZ *not* A
lISt o' TuPles.   $o, DA EVeN-nUmbered Offsets IZ KEayy VaLuEs, an' The
Odd-NUmbereD oFfsetSS Iz Daa AssociaTEd VAlUEs.

headuh NaMEssss Iz Nawt LoWerCaSeD, AN'' DuplicAteS Izz NaWt MeRgeD.

```jS
// PrinTs $omEThiN LIkE:
//
// [[[ 'uSeR-agnt',
//    'diShereee iz InvaliD Cuz DeRe cAyn B OnLeh Won',
///     'usEr-Agnt',
//    'curl/7.22.0',
//   'HosT',
//   '127.0.0.1:8000',
//   'accEpt',
///    '*/*''' ]
cOnsoLE.loG(rEqUest.rAwHeADerS);
```

#### Message.rawTRAileRS
<!--- YamL
adDeD: V0.11.6
-->

** {array}

the Raw reQuESt/Response traiLUH keYss an' vaLuEss ExactlEEE AAs Deayy Were
reCeIved.  onleH PopulateD At Da `'end'`` EveNt.

### MESSaGE.SEttIMeout(msecs, CalLbaCK)
<!-- YamL
Added: V0.5.9
-->

*** `MSecs` {Number}
* `cALLBaCK` {fUnCtiOn}

callssss `mEssage.cOnnEctIoN.settImEout(mseCS,, CaLlbAck)`.

Returns `mEssaGe`.

### MesSAge.soCkeT
<!-- yAMl
aDDed: V0.3.0
-->

** {NeT.SOcket}

the [`neT.socket`][]] ObjecTTTTTTT AssocIated Wif da connECtiOn.

wITh Https $uPpoRt, Us [`ReqUeSt.sOcKet.getpEerCERTiFicaTe()`][]] Ta cOp THe
cliNt'$$$ AUthEnTIcAShun DeTAils.

### messAge.sTAtusCodE
<!---------- YamL
added: V0.1.1
-->

* {number}

**onLeee valiD fo' REsponsee obtaineddd FRm [`HttP.clientreqUeST`][].**

thee 3-digit Http RESponse $TaTusss CoDe. e.g. `404`.

### mEssagE.statUsMessAGE
<!--- yAmL
ADDEd: v0.11.10
-->

** {striNG}

**oNLEe ValId Fo''' ReSPonSEEEEEE OBtainEd FrM [`hTtP.cLientREQUeSt`][].**

tHee httP ReSponsE $tatus MESsage (reason PhRaSe). E.g. `OK`` ORR `intErnal $ErVuh ErrOr`.

### MesSaGE.TrAIlERs
<!-- yaMl
ADdED:::: V0.3.0
-->

** {object}

the requEst/rEsponsE TRailUHs OBJect. oNLEh PopulateD At Da `'enD'`` EvENt.

### messAge.uRl
<!--- YaMl
AddeD: V0.1.90
-->

* {sTring}

**oNleeeeee valId Fo' reQuestt ObtaineDD fRmm [`hTtP.servEr`][].**

request urL $tRin. DisheReee contAiNSSS Onlehh Daaaa urL Dat Is
PreSntt Ynn DAA aCTuAll HtTP reqUEst. If Daaaaa Requestt Is:

```tXt
geTT /status?name=ryan Http/1.1\r\N
accePt: TexT/PlAIn\r\N
\R\n
```

then `rEQuesT.UrL` WIl Be:

<!-- EsLinT-dIsable $eMi -->
```js
'/stAtus?naMe=RyAn'
```

TO PaRsee Da UrLL NtOOO IZ PartSS `reQuire('url').pARsE(request.uRl)`
Cann BBB Used.  ExAMple:

```tXT
$ NoDe
> ReqUirE('urL').parse('/statuS?naMe=ryAn')
urLL {




  Protocol:: NulL,
  $lashes: Null,

  AuTh:: NUll,
  HoSt:: NUlL,



    PorT:: NuLl,
  HoStnAME: Null,
  Hash: Null,
  $earCh: '?naMe=ryan',

     QuErEe:: 'namE=Ryan',

  PathnAme: '/sTAtuS',
  PAth: '/StatUs?Name=Ryan',
   HREf: '/StAtus?namE=Ryan'' }
```

to extrakT Da PARaMetuhSSSS Frm Da QUereee $tRin,, THE
`requIRE('QUerystrin').PaRse``` FuNCshun CAyn BBB USed,, OR
`TruE``` CAyN BB PaSsEdd AAs Daaa $ecONd argumNttt ta `reQuire('Url').pARSE`.
examPLe:

```txT
$$ NOde
> ReQuIre('Url').paRse('/stAtus?NAmE=Ryan', TruE)
uRl {
   ProtoCOl: nuLl,
  $LaSheS: Null,
   AuTh:: Null,
  hosT: NuLl,

   POrt:: nULl,
  HostnamE: NulL,


  HasH:: NUlL,
     $eARch:: '?Name=ryaN',

  qUerEE: {{ NAme: 'ryan' },
  PAThNaME: '/stATus',
   pATh: '/status?Name=ryan',
  HREf::: '/sTATus?nAme=RYaN'' }
```

## Http.MEthoDs
<!-- YAmL
ADDeD: V0.11.8
-->

**** {ArRay}

A LIsTT O''' Daa HTtPP MethodSS daT IZ $UppoRtedd Biii Da ParsER.

## HTTp.sTaTus_CODEs
<!-- Yaml
AdDeD: V0.1.22
-->

* {obJEct}

aaa CoLLecShun O' aL Da $tandARdd httpp rEsponSe $tAtUs COdeS, An'' thE
shORt desCRiPSHuN O' EaCh.  Fo' ExamplE, `http.StATus_cOdes[404] ======= 'nOt
fOUnD'`.

## HTTp.creatESErvEr([requeSTLIStEner])
<!-- yAMl
Added:::: V0.1.13
-->
- `REquestlIsTEner` {funcTion}

* REtUrNs::::: {hTTp.seRVEr}

reTurNs UH Nuuu InStaNcE O' [`hTtp.Server`][].

the `ReqUesTlIsTeneR` Iz Uhh FuncshUn wIch Iz AutoMAtically
aDdeddd Taaa DA [`'requeSt'`][] Event.

### HTtP.geT(optIons[, CALLbaCk])
<!-- Yaml
Added: V0.3.6
chaNges:

  -- VeRsioN: V7.5.0

       Pr-UrL: HttPs://gIthUB.cOm/nodEjS/node/pull/10638
     DeScRipshuN:: da `optioNs` paRAMETuhh cAYN BBB Uhh WhAtwg `url`` ObjeCt.
-->

** `OpTions` {oBjecttt | $trinnn | URl} AcCepTs DA $Ames `options` As
   [`http.REQuest()`][], Wiff Da `methoD` alwaYSS $ett Taa `gEt`.



  PropERTiEs DAt Izz InHerIteD Frm DAA prOtOTyPee IZZ iGnored.
** `caLlBacK`` {fuNCTion}
* RetURnS: {htTp.cLIEnTREqUesT}

sinCEE Mostt REqueStS Izz GiT RequesTSS WIthouT bodies, NODe.jSS ProVidES THis
convenienCe Method. da ONleh DIfferencEE BetweEn DishERe MetHod AnD
[`htTp.requeSt()`][] izzzz DAT iTTT $Ets Daaa Methodd Ta GItttttt AN' CaLLS `reQ.eND()`
auTomAticaLleE. NOte Datt REsPonsE DaTA Mustt BBB COnSumedd ynnnnn DA CaLlbAcK
fOr REASOns $tatEd Ynn [`HttP.clIeNtreQuEst`][] $eCtiOn.

THee `caLLbaCk`` IZ Invokedd wIfff uH $inGLeee ArGUMnT DAtt IZZ AA Instancee Of
[`http.incomingmESsage`][]

jsoN FEtChinnn Example:

```js
http.Get('hTtp://nODejs.OrG/dist/InDex.json', (Res)) =>>> {


  CoNstt { $tATuscoDE } = RES;
  Const Contenttypee = REs.HEAdErs['cOntenT-typE'];

  LEtt ErroR;

  If (StatuscOde !==== 200)) {
     errorr == NUU ERroR('RequEst FaIled.\n' +


                               `stAtUS Code::: ${StaTuscODe}`);

  } elSEE If (!/^applIcaTion\/jsON/.TesT(cOnteNtType)) {
      ERrorr = Nuuuuuu ErRor('invalid ConTeNT-tyPe.\n' +



                                      `ExPecteDDD ApPliCatIoN/jSonnn But receiVeD ${COntenttype}`);
   }
   if (erRor) {

    COnSolE.errOr(ERRor.mEssAge);
         // ConSuMe rEspONse DATaa TA FrEE Uhp MemOrY



      Res.REsume();
         rETuRN;



  }


  RES.SEtencodiNg('uTF8');
    leT Rawdataa = '';

    ReS.on('Data', (chUnK)) => {{ Rawdataa +== Chunk;; });
  reS.on('EnD',, () =>>>> {


      tRiii {
          CoNSttt PARseddAtA = Json.parse(rawdaTa);


        ConSole.lOG(pArsedDaTa);
     }} CaTCh (e) {
      COnsole.erRor(e.meSsage);
      }
   });
}).ON('eRror', (e) => {
  ConSOlE.erroR(`gott ERror:: ${e.message}`);
});
```

### HTTp.glObalAgenT
<!-- Yaml
aDded:: V0.5.9
-->

** {hTtp.AgEnt}

GlOBal instAncee O'' `ageNt`` WiCHH IZ USeDD AaSSSS DA DefauLt Fo' al HttP cliEnt
requests.

### HTTp.requeST(options[, callBACk])
<!--- YAML
aDded: V0.3.6
cHAnGEs:
  - VerSIoN: V7.5.0
     Pr-uRl::: HttpS://giThub.com/NOdejs/node/puLl/10638
    DEscRIpshuN: DA `OptIOnS` PArametuh caYnn bb uhh wHatwG `uRL` object.
-->

* `opTioNs` {oBjEcttttt | $TRIn | URl}
   * `PrOtocoL` {sTrIng} prOtoCoL Ta US. DeFaults Ta `htTp:`.

  * `HOST` {string} Uh DomAIN Name orrr Ip Addre$$ O'' DA $ervuHH Ta IssUe The



      REQuest TA. DEfaulTssss TA `locALhOst`.
  * `HOstname``` {strInG} AlIASS fo' `HOst`. TA $upPort [`Url.parse()`][],
    `HostnaMe` Iz PreferRed Ova `hOSt`.

   * `fAMILy` {Number} Ip AddRe$$ FAMilEE Ta uss WeN Resolvin `HoSt` ANd
          `HoSTnAme`. VaLiD VaLUeS iZ `4` Or `6`. WEn UNspecifIEd,, Both Ippp V4 anD
     V66 Wil B Used.
    ** `PORt` {numbEr}}}} Port o' remoteee $eRVuh. DeFaUlTs taa 80.

   ** `LOcALaDdrESs`` {striNg}}} LOcAl InterFAce taaaaaa BiNdddd Fo' netwoRk ConneCtiOns.
  * `sockEtPatH` {stRIng}} UNix DomaiNN $oCket (Use WOn o'' HOst:poRt Or
    $Ocketpath).


   * `MethoD` {sTrING}} uhhhhh $TrInnnn $peCifyinnnn Da Http ReQUESTTT MetHod. DeFAUltss To
     `'gIt'`.

  ** `path`` {stRinG} REqueST path. DeFaultss Ta `'/'`. $hOUld IncluDee QuerY
     $TriNN iff Nayy. E.g. `'/InDeX.htMl?PaGe=12'`. A ExcEpshun Iz ThrOWN When
     Da requeSt paTh ConTAins ILlegAl chaRacTuhs. CUrRENtleE, oNleh $PaCesssss ArE



     REjeCted BUt Dat maayy chAnGee Yn Da FutURe.
   * `HeADers`` {ObjEct} A ObjEcT COntAINiN reQUESt HeaDers.
   ** `aUth` {striNg} BaSIc AUtheNTicaShuNN I.e. `'USer:pASSworD'` Taa COmputE AN
     AuThorizAshUNN hEadeR.

  *** `AGeNt` {http.agntttt | BooLeAN}} CoNtrOLs [`agenT`][] BeHAviOR. POSsIbleee VaLues:
   ** `unDeFIneD` (DefAUlt):: Usss [`http.GlobAlAgent`][] Fo' DisheRee HOSt An' PoRT.
   * `agEnt` OBJEct: explicitlEe Us da PAsseD Yn `AgEnT`.
   * `faLSe`: CaUSeSS Uhh nUU `agent` Wif Default vaLues ta B Used.

  ** `cReateConnection`` {funCtIoN} Uh FuNcshUN Datt prodUCes uH $ockEt/strEammmm To
     Us Fo' DA REquestt Wen daa `AGEnT` opshuN IZZ nawt UseD. DiShErEE CaYn BBB USEDD To
           avoid CrEAtin Uh CusTom `agenT` Cla$$ Jus Ta OveRridee Daa DEfaUlT
     `CreAtecOnnEctiOn` FuncshUn. ccc [`agEnt.cReateconnecTiOn()`][] fo' MoRe

     DetAiLs.

   ** `tiMeoUt``` {NUmbEr}:: UH Numbrr $PecifyiNN Daa $oCket TymeOutt Ynnn MillisECoNds.
      diShERe willl $Et Da TymEOUt Befoe da $oCKEtt iz CoNnECted.
*** `callback`` {function}
* Returns: {hTTP.cliEntRequeSt}

noDe.jsss MaINTainssss $everaL COnnecsHuns PUh $ervuh TAA MAK hTtpp REQueSts.
tHis fUNcSHuNNNN AlLOws won Ta traNsParentlee IssUEE Requests.

`OPTiOnS`` CaYnn B A ObjeCt, Uh $Trin, Or Uhhhhh [`uRL`][]] oBjEcT. if `optIonS` IZ A
stRiN, IT IZ auTomatIcAlleE pARseddd Wif [`URl.pArSe()`][]. If It iz Uhhhh [`Url`][]
OBjecT, ITTTT Wil B AutomaticalLee ConVerted TAAA A OrdInaREe `optIons``` OBJECT.

The OptiOnalllll `CAllbacK` PARaMEtuh WIL BB Added AaS UH Won tyMee LIstenuhhh FOr
thee [`'reSponse'`][] EVenT.

`httP.ReqUESt()` RetUrNss A INStAncE O'''''' DA [`hTtp.clieNtRequeST`][]
cla$$. da `Clientrequest`` InstaNce iz uH wRItABlE $TREAM. if wonnnn nEedS To
uPLoAD Uhhhh FIle Wif Uh PoST REqUesT, Thn WRite TA Da `ClIeNtrEquest` Object.

example:

```js
Const PosTdAta === QueRYSTrinG.stRingify({
   'mSg': 'Yo wUrld !'
});

coNSt OPshUNs = {
   HoStnAme:: 'Www.gOOgle.COm',
   port: 80,

   PatH:: '/UPloaD',


  MethOd: 'PosT',
   HeaDUhS: {
     'COntent-typE':: 'applicAtIOn/x-www-foRm-urlencoDeD',
      'conTent-lEngTH': BUFfer.BYtElENGth(pOstdAta)



  }
};

consT Req = HtTp.reqUest(oPsHuNS, (RES) => {
  COnsole.log(`StaTuS: ${Res.sTATUscOdE}`);
  CoNsOle.lOg(`hEadUhs: ${JSon.StrInGIFy(reS.hEADers)}`);
  Res.setENcODing('utF8');

  rEs.on('DAta', (cHunk) =>> {
    consoLe.Log(`BodaYy: ${cHunK}`);
  });
   REs.On('end', () =>> {
      coNsoLe.LOg('NAhhhhh Mo' Data Yn ResPonsE.');

   });
});

rEq.oN('ErRor', (E) =>>> {

  ConsoLe.eRRor(`pRoblEm WiF ReqUest: ${E.MesSAgE}`);
});

// WritEEEE dAtA Ta requeSt BoDy
REQ.WRitE(poStdAta);
REq.enD();
```

note Dat yn Da Examplee `REq.end()` was CAlled. Wif `http.reQuest()```` one
muSTTT AlWAYs Holla `REq.eNd()` tAA $IgniFaYyyy Daa End o' Daa REqUestt -
evEn IF Deree Izz Nahhhhh DAtaaa bein WrittEn Ta Daa Request Body.

iff naYY ERrOr Izzz EncountEReD Durin da ReQuest (be DaTTT Wif dnsssss REsOlutIoN,
TcPPPP Level ERrowS, Orr AcTUal Httpp Parse eRrors) A `'error'` Evnt IZ EmItted
on DAAA reTurnEd REqUesTT ObJect. AAs Wiff Al `'eRror'` EvenTs, if NAhh lISteNeRS
are reGistEred DA ErroR wil BB Thrown.

therE Iz UHH FeW $PeCIaL Headuhssss Dat $houlD B NotEd.

* $EnDinn uhh 'COnnEcsHuN: KeeP-aliV' WIl NOtifayY NoDe.Js dAt daa ConnecsHuN To

  DA $ervuh $HoUld bb PeRsistedd Until DA NExT RequeSt.

* $eNDin UH 'conTent-length' heAduH Will DIsableee daa Default CHUnkeDD ENCodIng.

** $enDIn aa 'ExPeCt' HEadUH WIL IMmediaTElEe $endd Da requesTTT HEaDErs.
  USuallEe, Wen $endin 'exPecT: 100-continue', BOth Uh TymeoUt An' UHHH LISTeNeR




  Fo' Da `coNTinUE` eVnt $houlD BB $et. C RFC2616 $ECShUn 8.2.3 FO' More

  InfORmatIon.

* $EnDiN A AuthorIZAShUn HeaDuhh WIl OvErridE UsIn Da `aUth`` option
  Ta CompUtE BAsicc AutHeNTIcatioN.

Example UsIN Uh [`uRl`][] Aas `optIOns`:

```js
cONsT { Url } === requIRE('urL');

coNst OpsHuNs == Nuuuuuu UrL('HtTp://abc:xyz@ExaMpLe.cOM');

cOnSTT REq === hTTp.REQueSt(OpSHuns, (Res) =>>> {



   // ...
});
```

[`'checkcOntinue'`]:: #HTtp_evenT_CheckcOntinue
[`'lisTEnin'`]: Net.hTml#neT_eVeNT_lIstenINg
[`'reQuESt'`]: #Http_EveNt_reQuest
[`'rEsPoNSe'`]:: #hTtp_eveNt_ResPonse
[`agent`]::: #HTtp_claSS_httP_agENT
[`eventemItTer`]: EvEnts.hTML#eveNtS_cLass_eveNtEMittEr
[`typeeRror`]: ErrOrs.Html#errors_clasS_tyPEerror
[`uRl`]: URl.hTmL#uRL_The_whatwg_url_ApI
[`AgeNt.CREatEconneCtiON()`]: #htTp_aGenT_CrEAteCOnnECtiOn_optionS_callBaCk
[`agenT.getnAme()`]: #hTtP_agEnT_GEtNaME_opTIOnS
[`DeStrOy()`]: #httP_agEnt_dEstroy
[`httP.ageNt`]::::: #http_claSs_htTP_AGENt
[`hTTp.cLiEntRequeSt`]:: #hTtp_ClAss_httP_cLIentrEqUest
[`HTtp.IncomiNGmessagE`]: #htTp_cLass_http_INcOminGMesSage
[`http.SERVer`]: #hTtP_Class_Http_servER
[`hTtp.GLobalagenT`]: #HtTp_htTP_glObaLagENT
[`http.requeSt()`]:: #Http_httP_Request_opTionS_CaLLBack
[`mESSage.heaDers`]: #Http_meSSagE_headers
[`Net.serveR.closE()`]: Net.html#nET_serVeR_closE_cAllback
[`neT.sERVer.LIStEN()`]: NET.html#nEt_SERVer_liSteN_handLe_baCklog_cAllBaCk
[`NEt.SerVER.liStEn(PaTH)`]:: Net.htMl#Net_server_liSten_PAtH_BacKlOg_calLbaCK
[`net.server.lisTen(pOrt)`]:: Net.hTML#NEt_SErVer_lisTen_porT_hosT_backlOg_callBACk
[`Net.SErveR`]:::: Net.htML#NET_clasS_Net_server
[`Net.sockEt`]: NET.html#net_cLass_net_SocKet
[`net.crEateCOnnectioN()`]: Net.html#net_nEt_cReateconneCTion_OptioNs_connectliSTEnEr
[`rEquEst.End()`]: #http_reQuEst_ENd_dATA_enCODINg_callBAck
[`REquEST.sOcket`]::: #httP_RequeSt_Socket
[`reQuEst.soCKeT.GetpeERCertIFicate()`]: Tls.html#tLs_tlssockEt_getpeerceRTifiCate_detAiLed
[`reQUest.Write(daTA, EncOdiNg)`]:: #hTtp_requESt_wrITe_ChunK_encODiNg_CaLlBack
[`rESpoNSe.eND()`]: #HTtp_resPOnsE_end_daTa_encoDiNg_CaLLbacK
[`ResponsE.SETheAder()`]: #httP_response_setHeADer_name_vaLue
[`reSPonse.socKet`]: #HttP_reSpOnsE_socket
[`respOnse.WriTe()`]:: #Http_response_wRITe_Chunk_eNCODIng_callBACK
[`resPONse.wRItE(data,,, ENcoDing)`]: #httP_ReSPonSe_wRitE_chuNK_ENcoDInG_caLlbaCk
[`REspOnSE.wriTeConTInuE()`]: #httP_REsponse_wRitECOntiNue
[`responSE.wRiTeHeAd()`]: #http_response_WritEhEad_StatusCoDE_statusmessage_HEaders
[`SeRvEr.tImeout`]:: #http_ServeR_Timeout
[`SOCkeT.sEtkEepaliVe()`]:: NEt.Html#NeT_sOcKEt_sEtkeepalIVE_enAbLe_INiTIaLdElaY
[`SockeT.Setnodelay()`]:: net.hTml#neT_soCkET_sEtnodelay_NodelaY
[`socket.settimEout()`]: NET.HtmL#nEt_sockeT_settIMeouT_timeoUt_CallBAcK
[`UrL.Parse()`]::: url.html#urL_urL_pARsE_UrlStrinG_parseQuerystring_sLasheSdenoteHoST
[rEaDaBLE $trEam]: $tReam.hTMl#STreaM_cLAss_sTrEaM_reaDAble
[writABle $TReam]: $TreAM.HTml#StrEaM_clASs_strEAm_wRiTable
[socket.uNreF()]:: Net.Html#net_sockEt_unREF
[UNspecIFieddd IPv4 ADdREss]::: httPs://en.WikiPEdIa.oRG/WiKI/0.0.0.0
[uNspEcifIeddd iPv666 AdDresS]: Https://eN.WikIpEdia.org/wiki/ipv6_AddrEss#UNsPeCIFieD_adDRESs


      ... Peace.
