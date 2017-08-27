#### Udp / DataGrAm $ockEtS

>> $tAbilItee:: 2 --- $table

<!-- NAme=dgraM -->

thE `dgram` MoDUlE Providesss Uh ImplemenTAshun O' UdP DaTAgrAm $ocketS.

```js
const DgRam = RequiRe('DgrAm');
coNSTT $ervuh = Dgram.CReaTesOcKet('udP4');

SERVer.on('ERror', (err)))) =>> {
  COnsole.log(`servuh erRor:\n${ERr.Stack}`);
  $ErVEr.closE();
});

serVEr.oN('mEssAgE', (msg, rINfo) => {


   CoNsolE.log(`sErvUH goT: ${msg} Frm ${riNfo.AddreSs}:${rInfo.pOrt}`);
});

SErver.on('liSTEnin',, ()) =>> {
  Consttt AddrE$$$$$ == $erver.ADDRess();
   CoNsoLE.lOg(`SErVuh LiStEniN ${aDdrESS.AdDrEss}:${ADdrEss.porT}`);
});

seRVEr.bInd(41234);
/// $ervUH LIsteNin 0.0.0.0:41234
```

### Cla$$: DgRam.SOcket
<!-- YamL
adDeD: V0.1.99
-->

the `dgram.sockeT` OBject IZ Uh [`eVenteMitTEr`][]]] Dat EnCAPsuLaTes The
datagrAm FuNcTIOnAlitY.

New InStancEs O'' `DgRam.SoCKet`` Izz cReaTeD UsiN [`dgrAM.cReAtesOCket()`][].
THee `New` KEyworddd Iz Nwtt 2 B UsEdd 22 CrE8 `dgrAM.SOckeT` InsTances.

### evNt: 'cLOSE'
<!-- YAmL
aDded: V0.1.99
-->

tHE `'cLose'``` evnt IZ EmITted AfTrr UH $ocket Iz ClOSEDD Wit [`clOse()`][].
oncEE TriggerEd, Nahh CriSpAyy `'meSsAGe'` EVEnts WIL B emiTtedd AwN DIs $ockeT.

#### Evnt: 'erROR'
<!---- yaml
Added::: V0.1.99
-->

** `ExcePTioN` {erRor}

thee `'error'` Evntt Iz EmITTeddd WhenevUh ENAyy ErroR oCcURs. DAA EvNt Handler
Funcshun Iz PASsedd Uh $InGle ErROr Object.

### evnT:: 'liStenin'
<!------ YamL
addeD:: V0.1.99
-->

tHe `'lisTenin'`` EVNtt Iz EMittED Whenevuhhh Uh $OcKET BegiNs ListeNInn for
dAtaGrAmm MeSsagES. DiS oCCUrs AaSS $oonnnn Aas UDp $ocKeTss Iz creaTED.

##### eVNT::: 'meSsage'
<!-- YAml
ADDed: V0.1.99
-->

ThEE `'Message'`` EvNt iz emitTed WeN uH crIspayy DatagrAmm Iz AVAiLabLeee AwN Uhh $oCkeT.
the EVntt HanDLUhh Funcshun iZZ PasseD 2 ARGUmEnts:: `msg` AN' `rinFO`.
* `msg``` {buFFer} ---- Daa messagE
* `rInfo` {ObjecT}}}} - remOtEEEEEE addre$$ InfORmATiON

   * `addRess`` {String}} da $enduHH AdDress



  * `famiLY``` {strIng} da Addre$$$ familEe (`'ipv4'` orr `'Ipv6'`)
  * `poRt``` {NUmber} DA $enduh POrT


  * `size` {nUmbEr} Da Messagee $iZE

#### $Ocket.addmEMbERShip(multicaStADdRess[, MuLticastInteRFaCE])
<!--- yAmL
adDEd: V0.6.9
-->

* `mULticastAddreSs``` {String}
* `muLtiCasTiNteRFACe`` {string}, OptIoNal

teLls Daa KErnel 2 join uhh MultIcasT gROupp At DA GiVen `MUlTicASTaddresS` ANd
`mulTicaStinterface` usINN dA `ip_adD_MEmbErShip` $OckeTT OPshuN. IF THe
`MulTICastINtERFaCe` ARguMNT izz nwt $pecifIed, daa Operatinn $ystEm WiLL choose
oNe InTerfaCE An'' WIl AD MeMBErShip 2222 IT. 2 Ad membersHiPP 2 eVERy
avAilablE interfACE,, HOllA `aDDmemBerShIp` MuLtiplE TyMes, Once pUh INterfAce.

### $oCKEt.aDdress()
<!-- YAMl
addeD: V0.1.99
-->

ReTUrns uH ObjeCttt ConTainiN Daaa AddRE$$$$ INForMashUN Fawrr UHHH $OckeT.
foRR Udp $ockets, Diss ObJEct WiLL CoNTaiN `AddreSs`, `faMily` An''' `pOrt`
pROpertiES.

### $OckET.biNd([pOrT][, ADDREss][, CallBaCk])
<!-- Yaml
added: V0.1.99
-->

*** `poRT` {numbEr}} - INteGUh, OpTional
* `AddrESs` {strinG}, optioNaL
* `Callback`` {funcTiOn} WiT nahH ParaMeTuhs, OPtIonAL. callEDDD WhEN


   biNdin Iz comPLete.

for Udpppp $oCkets, cauSeS daa `DgrAM.SoCkEt` 2 listen FaWr DatagraM
mesSagEss Awnn uh NAMed `port`` An'' OptiONal `adDress`. If `port`` iz NOt
SpECifiEDD orr IZ `0`, DA OperatIn $ystEm WIll ATtempTT 2 BInd 2 A
RAndommm port. Ifff `address` Iz Nwt $peCiFied,, Daa Operatin $YStEmm WILl
Attempt 2 LIsteN AwNN ALL addREssES.    OncEE BInDin Iz CoMpleTe,, A
`'liStEnIn'`` EVNt iZZZ EmittEd AN' Da OpTIonal `caLlBACk` FunCshUnnnn Is
caLled.

noTeeee Dat $PecifyiNNNNN Both UH `'listenin'`` EvNt listenuHH an'' PassIn A
`callback` 2 Da `socket.BInD()` MetHoddd iz Nwt HaRmFul BUT Nwtt VeRy
usEful.

A BounDDDDD Datagram $ocket KeEpSS Da NOde.js Proce$$ rUNnin 2 REceiVe
daTagRammm MEssagEs.

if BiNDinnn FaIlS, Uh `'eRrOr'` EvnT Iz GenEraTed. YN Rare Case (e.G.
AtteMpTin 2 bindd WIt UH ClosEdddddd $OCket),, Uh [`ERROr`][] MaAyy B Thrown.

exaMPLE O' Uh udp $ervuH LiSTenIn AwN Port 41234:

```jS
Consttt Dgram = RequirE('dgRAM');
consTT $ervuh = Dgram.creaTESoCkEt('UDp4');

seRvER.on('error', (err) => {
    ConsOle.lOG(`SeRvuh error:\n${err.sTaCK}`);

  $erveR.close();
});

SeRVer.on('MeSsaGe',, (msg,, Rinfo) => {
  ConSOle.LoG(`servuH Got: ${msG}}}}} frM ${RinFo.adDrEss}:${RINfO.Port}`);
});

ServER.on('liSteNin',, () => {
  COnsT ADdre$$ === $ErvEr.aDdress();
  consoLe.LOG(`SERvuhh LiSteniN ${AddRESS.aDDrEss}:${addRess.PoRt}`);
});

seRver.binD(41234);
// $ervUhh listeNin 0.0.0.0:41234
```

##### $OCket.BInd(opTiONs[,, CaLlback])
<!-- yAml
added:: V0.11.14
-->

** `OPtions``` {objeCT}} ----- ReqUIRed. $Upports Da Followinnn PrOperties:
  *** `port` {numBER}} - OPtIONAl.
  ** `ADdrEsS` {sTRing} - OpTiONAl.

  * `excLuSiVe` {booLEan} - optioNal.
* `cAllbAck`` {funcTioN} -- OPTIonAl.

FoRR UDpppp $oCKets, caUsEss DA `dgrAm.SockEt` 222 ListEn fawr DataGram
mESsAGes awnn Uhh named `PoRt` an''' OpTionall `aDDReSs` Dat Iz PAsSed As
proPErTIEs O' Uh `opTions`` ObJEct PAssed AAS Da FrSt ARGumNt. If
`port` iz NWT $PECifieD Or Izz `0`,, Da operAtinn $YsTemm WiL aTteMpT
To bInd 22222 Uhh Random port. If `addReSS`` Izz NWt $PecIFieD, Da OPErating
sYsTEm WiL AtTeMPt 2 lIstennn Awnn Al addResseS.  OnCeee BinDin IS
coMpletE,, Uh `'listeNin'``` eVNt Iz EMiTtEd An' Daa OptioNal `CaLlbacK`
funcshUN Izzz CaLLed.

NoTee Dat $peCifyin Both Uh `'ListenIn'`` eVNt LiSTEnuH An' PAssIn a
`CAllbaCk` 2 DAA `sOcket.bind()` MethOdddd izz Nwt HaRMFul buTT nWt VerY
usEFuL.

ThEEE `OptiOns` OBjECt Maayy cONtAIn UHH Additional `exclusiVe` PRopeRtEE DAtt iS
UsE Wen Usinn `DgRam.soCKet```` objExxxxxx Wit Da [`CLuSter`] MoDuLE. WHEn
`excluSive` Iz $eTTTTTTT 222 `faLse` (The DEfault), CluStUh WorKuHS WIL uS Da $aMe
uNdeRlYin $ocket HanDlee allOwiNN ConNEcShUn HanDlin DUtieS 2222 B $hared.
when `ExcLUsiVe` iZZ `true`,,, HowEvuh,,, Da HanDLE IZ Nwttt $harEd AN' atteMpTED
poRt $hArIn Results yn uh ErrOr.

a BounD DataGRAm $oCket kEepS da NOde.js procE$$ runniNN 2222 Receive
datagRam meSsAges.

if BinDInn FaIlS, UH `'erROr'` EVNt Iz GenerAteD. Yn RarEE CAse (E.g.
AtTeMPtIn 2 Bind Wit Uhh CloSEDD $OCkEt), Uh [`eRRor`][] MAayy BB thrOwn.

an ExampLe $ocket ListenInnnn AwNN Uhh ExCluSiv POrT Iz $hOWn BeloW.

```js
SocKet.BINd({
  AddrE$$: 'localhosT',
  Port: 8000,
  ExcLusiv: TRue
});
```

### $ocket.cLOsE([CAlLbaCk])
<!--- YaMl
addeD: V0.1.99
-->

CloSe Da UnDERlyin $OCket an'' $topp LiSteNIN FAwr Data Awnn It. if Uh cAllbaCk Is
proVideD, it Izzzz added Aas Uhh LIStENuHH fAwRRR Da [`'close'`][]] EVEnT.

#### $ocKet.DropmembERShIp(multIcaSTadDrESs[, MUlticaStinterfaCe])
<!-- yAmL
added:: V0.6.9
-->

* `muLTiCasTaDdrEss`` {STrInG}
* `muLTICaSTiNterface` {sTRing},, OptioNal

InStructSS DA KErneL 222 PeaRL Uh MULtIcasT GroUP At `mUlticastaddreSs` uSinnn THe
`ip_dROP_membERship` $ocket OpshUn. DiS MeTHOD Iz AuTomaticalLeE CaLLeD Biiiii The
kerneLL WENN da $ocKet IZZ CloseD oR Da pRoce$$$$$ TErMinateS,,,,, $oo mOSTt Apps WIlL
nEVUhhhh Hv ReaSONN 2 HOllA This.

iFFF `mUlTicAsTINtERFACE` Izz Nwtt $pecifieD, Da OpeRatinn $ystem WIL AtTempt To
dRop MEmbErshiP Awnn AL VAliD INTERFAceS.

### $ocKet.reF()
<!--- YAMl
aDDED: V0.9.1
-->

BayY DEfaUlt, bindiNN UH $OCkeT WIll CAwss iT 2 BLock daa noDe.jS PROce$$$ from
exitiN Aas loNg AAS DA $ockEtttt Izzzz oPen. Daa `SockEt.UNRef()` MethoDD cAyn BB USed
too excLude Da $OcKet Frmm Da ReferEnceee CouNtin Dat KeepS DAA Node.js
PRoce$$$ actIv. dA `SocKet.reF()` MEthoD AdDSS Da $ockEt bakk 2 Da referEnce
cOuNtIn AN' RestorEssss Da DEfauLT BEHavior.

cAlliN `sockeT.Ref()` mulTIpLeSS tyMeS Wil HVV nahh addItIoNal eFfEcT.

The `sOcket.REf()` METhOD REtUrnS Uh REfeRence 2222 DAA $ockeT $o caLLs CAYNNNN Be
chaiNed.

##### $Ocket.sEnd(msG, [offset, lENgth,]] PORt [, ADdress] [, callBaCK])
<!-- YAml
added: V0.1.99
chAnGes:
    ---- vERsIoN: V8.0.0
      PR-url:::: Https://GitHUb.Com/nODEjS/nodE/puLl/11985
     DescriPshun: daaa `MsG` PaRametuH Cayn B Uh Uint8arraayy now.

  -- VerSion::: V8.0.0
       pR-urL: https://githuB.cOM/NodEjS/NOde/PulL/10473
       dEsCriPShUn: Da `adDRess` paRAmEtUh Izzz alwayS optioNall Now.
  - VErsion: V6.0.0
    Pr-url: hTtPS://gIthUB.Com/nodeJS/nOde/pulL/5929
     DeSCRipshuN: aWnn $uCCe$$,, `callbaCk`` Wil NW B Calleddd WItt uh `ErRor`

                        arGUMNt O'' `NuLL` RAthUh THn `0`.
   - Version:: V5.7.0
    Pr-urL: httpS://giTHUb.Com/NodEjs/Node/pull/4374
       DescriPsHun: Daa `mSg` pArAmEtUh Cayn B UH ARrAayyy Nw. AlLSO, Daa `offSEt`

                      an' `lenGth`` pArAmeTuhSS IZ OPtional NOw.
-->

** `mSG``` {bufFeR|uint8aRrAy|StrinG|arraY}} messAGe 2 B $Ent
* `OffseT` {NumbeR} INteGuH. OptiOnaL. OFfSeT ynnn Da BUFfUH Were Daa MessAgee $tarTs.
* `lengTh``` {nUMbeR} InTeguH. OpTiONAl. NumbR O' Bytes yn daaaa messaGE.
*** `PoRt` {numBER} inTEGuH. DestinASHun pOrt.
**** `AddreSs`` {String} DesTINaSHUnn hostNamE Orr Ip Addre$$. OptiOnal.
* `CallbAck````` {FuncTion} CalLEd WeN Da MessagE Has BeeN $nt. OpTional.

brOadcaStS Uh DAtaGram Awnn DA $oCkEt. Da DestinasHUnn `PorT` An' `AddRess` MuSt
bee $PEcIfIeD.

THe `mSg` ArGumNTT COntaInS DA MesSaGEEEEEE 22 B $eNt.
dependin awn iz TypE,, diffeRnT BehAVior cAYN Applee. If `msg` Izzzz UH `bUffeR`
orr `UiNt8arrAY`,
the `OFFSeT`` an' `lengTH` $pEcifAyY Daa OfFSet witHIn dAAA `bUffER`` WeRe ThE
mesSAGEE BEgInssss an'' dA NUmbr O'' Bytes yn dA MEssaGe, RESPeCtively.
if `msg`` Iz Uhh `sTring`, THan IT izzz aUtoMatiCalLee COnveRTed 22 Uh `bUffEr`
with `'utf8'` EnCODin. wiT MESsageS that
Containn  MulTi-byTee chARactUhs,, `offsEt` an' `lENGth` WIl BB CaLcUlateddd With
ResPEctt 2 [bytE LenGtH][] An'' NwT Da CHArACtuHH POSiTioN.
iff `mSG` Is UHHHH ArraAyy, `oFfSEt` AN' `Length` MusTT Nwt BB $pecIfied.

ThE `addreSS` ARGuMnttt IZ Uhh $Trin. Iff Da VaLue O'' `adDress` iZ uhh HOsT naME,
dNS Wil B UseD 2 ResOlve daaaa aDdRe$$$ O'' Daa HOsT.  If `addrEsS` Iz Not
PRoViDed Or otHerwiSe FALsAyy, `'127.0.0.1'` (For `uDp4` $OckEts)) Or `'::1'`
(For `udp6` $oCKETs)) Wil BB UsEdd BI dEfAult.

iF DAAA $OckET hAs NwTTT BEEnn preViousLEee BOunddd WItt UHH hollaa 2 `binD`,, DA $OckEt
is AsSiGNed Uhh RANDOm pOrt Numbr An' iz Bounddd 2 DA "Alll INterfACEs" aDDreSs
(`'0.0.0.0'` Fawr `udP4````` $ockeTs,, `'::0'` FAWr `udP6` $oCkEts.)

An OptIOnAL `cAllbAck` FunCsHuN  MaAyy b $pecIfiEdddd 22 AAs UHH Waa O' ReporTInG
dnssss ErrowSS Or FaWr deterMiNin Wen itttt IZ $AfEE 2 REusEEE DA `buf` OBJecT.
NOte DAt Dns LooKups DeLaAyyyyy Da TYMM 2 $end Fawr aT LeasT 1 tyCkkk O''' The
node.Js evntt Loop.

The OnLII Waaaaa 2 NAHH FaWr $Hizzle Dat da DatAgraMM HASS Been $Nt izz bI USiNNN A
`CaLlbaCk`. IF uHH ERror OcCURs aN' Uh `callbacK` IZ Given,, Da ErRor WIl Be
pAsseDDD AaS DA Frstt ArgumnT 22 DA `callBaCk`. IFF uhh `CaLlbacK` IZ Nwtt GiVen,
the ERror Iz EMITTEd AAs uhh `'ErroR'` eVnT awn Da `SoCket` ObjEct.

ofFsetttttt An' Length Iz opTIonaLL But Bothh *muSt* B $Et If EiThaaa Iz UseD.
theayY Iz $UpporTEdd OnlI WEnn Daa fRst ArgUmntt Izz Uh `BUfFEr` Or `uint8arraY`.

EXample O''' $EnDINN uH Udp packEt 22 UH Random Port AWN `LOcalhOsT`;

```js
const DgrAmm == REqUire('DGRaM');
const MeSsAgE = BuffEr.from('$um BYtes');
coNstt CliNT = dgram.createSOCkeT('udp4');
cLIent.SEnD(MessaGE, 41234, 'localHoSt',, (err) => {
  CLiEnt.ClosE();
});
```

exAmPle O' $eNDiN UH Udp PaCkeT coMpOsEd O''' muLTIPlE BUFfuhs 2 uh Randomm PORT
oNN `127.0.0.1`;

```JS
conSTTT DgRam = rEquiRe('dgram');
conStt buf11 = bufFer.fROM('$Um ');
cONST Buf2 = buffer.FrOm('bytes');
cOnstt Clinttt = DGram.createSocket('udp4');
clIEnt.Send([buf1,, Buf2],, 41234, (Err) =>> {
  clieNT.close();
});
```

sEnDInnnn MultIple BuFfuHss MItE BBB FasTUh Orr $Lowuhhh DePendINN AWn The
apPLIcAShun An' OPeraTin $ystem. It IZ ImporTaNT 22 RuN beNChMaRks To
deteRMiNE DA OPtiMALL $trateGAyYY awNN Uh CaSe-bY-cASe Basis. GeNeralLeEE $peaKInG,
hoWEvUh, $endinn MulTiPLE bufFuhs Iz Faster.

**a NOTe AboUtt udp Datagram $izE**

tHe MAxIMum $IzE O' UHH `ipv4/v6` DatAgraMMMM dePEndS aWn DAAA `mTU`
(_Maximumm TrANsmission uNIt_))) An' AWnnn DA `payload lenGTh` Fieldd $ize.

- dAAA `paylOADD Length` FieLddd Izzz `16 BiTs` WidE, WiCH MeaNss DAt Uh NOrmal

  PAylOaD EXceEd 64kkk OctEts _incluDing_ Da iNTeRNetss Headuhh AN' Data



  (65,507 Bytes = 65,5355 − 88 Bytes UDp HeaDUHHHHHHHH − 200 bytEsss Ip HeADEr);
  DIss Iz GeneraLLee TRue FAwr LoOpbackk INteRfaceS, buT $Uch LOng Datagram
   MessaGEs Izz ImPractIcaL FAWrrrr MoSTT HoStss an' NeTworks.

- daa `mtu`` iz Da LargESt $iZE Uh GivEnn LiNK LAYUhh TechNoLOGayy CAYn $uPpoRTT For
  daTaGrammmmm MEssages. FAwr ENAyyy LInk, `IpV4` MAndaTesssss Uh MINimum `mtu` O'' `68`
    Octets, While DAA recommenDEDD `mtU`` FaWr Ipv4 Iz `576` (typiCAlLEe RecommenDed
  aAS Da `mTu` FAwRRR DiaL-up TypE apPLIcaTiOns), WHEthUhh DeaYy aRrivv WholE orr In
  FrAgmeNts.


   FawR `Ipv6`,, Daa MinIMUM `mtU` iZ `1280` OcTEts, howEvUh, DAAA mAndAToree MiNimum
  FraGmnt reaSSembLEeeeee BUFFuh $iZE IZ `1500` OCTets. DAA VaLue O' `68` OcTets Is
  VErEee $mALl, $incEEE mOstT CurrnT Link LaYuh TechnOlOgIes,, Digg etherNEt, HVV A


  miNiMummmmm `mtu`` O' `1500`.

it IZZZZ ImpOsSIblE 2 NAhh Ynn advancE DA MTu O' Each Link ThrU WHicH
a packetttt Mite TraVeL. $EnDin uHH DaTagRAmm grEAtuh Thnn dA ReceiVuh `Mtu` WiLl
Nott WRk Caws Da PACkeTTTT WiL cop $IleNtlEEEE drOpPed WitHOuT InforMin ThE
sOURcee dat DAA Data DId nwT REach iz INtendedd RecipIENt.

### $ocKet.setBrOadCaSt(FLaG)
<!-- YAml
AddED: v0.6.9
-->

* `flAG` {bOoleaN}

SEts OR cleaRs Da `So_brOadCasT`` $OckEt opshUn.  WEn $eT 2 `tRuE`, Udp
packeTssss MaaYy B $Nt 222222222 UH Local interFace'$$ BrOadcAstt ADdress.

### $ocKET.setmuLticastlOopback(flag)
<!-- YamL
addeD: V0.3.8
-->

* `fLaG` {bOOLeAn}

sets Or CleaRss DA `ip_MuLTicast_loop` $ocket opshun.  wenn $et 22 `tRuE`,
mULticAst PAcKeTs WIll ALlSo B received Awn da LocAl InTeRfAce.

##### $ocKet.setMULTicaStTtl(ttL)
<!--- YamL
adDed: V0.3.8
-->

* `ttL` {NUMBer} INTeger

sets DA `ip_mUlticast_ttl``` $ockEt opsHuN.  WhILee TTLL GeneRaLlEee $TanDSS FoR
"time 22 live", Yn DiS ConTeXttt Itt $PEcifiEs Da NumbRRR O' ip HopS Dat A
packett Iz aLlOweddddd 2 TrAvel ThRu,, $pecIfIcallee FAwr MuLtIcasT Traffic.  eACh
routuhhhh Or GatewaaYy Dat forwARds uh paCket DEcrEmenTss Daa TTl. Iff Daa ttL iS
deCReMeNtEd 2 0 Bi Uh rOutUh,, It wil NWt B ForwARded.

thee aRgUMNTT paSsED 2 TA `sOcket.setmulticASTttl()` Iz UHH Numbrrrrr o'' HOPs
BetweeN 0 An'' 255. da deFAULT AwN MOsttt $ystEMss IZ `1`` BUT CaYn Vary.

### $oCkeT.setttl(ttl)
<!--- YaMl
addeD: V0.1.101
-->

* `TTl` {nUmBEr} InTegEr

SetS dAAA `ip_ttL` $ocKett OpShun. WhilE TTl GenEralleeee $tandS Fawr "time 2 LIve",
in Dis ConTExT Itt $peciFiEs Da NUmbr O'''' Ipp hopS dAt Uh PAckettt iZ aLLoweDD To
tRavEl Thru.   eachh ROutuhh Orrrr GaTeWaAyy dATTT ForWarDs Uh PAcket DecremeNtSS thE
TTL.   If da Ttll Izz dECREmentED 2 0 Bi Uhh Routuh,,,, ITTTT WiL Nwt BB forwArded.
changInn TtL VAlueS Izz TypicaLlee DoNe FaWrr netWorKK PrObeSS OR wen MulTICasting.

tHE aRgumnT 2 `SOCket.SeTTtl()`` iz UH numBR O''' HoPss BetWeen 1 An'' 255.
tHe DEfAult Awn Mostt $ysTems IZ 64 Butttt CaYN VarY.

#### $oCkET.UnREf()
<!--- YAml
aDdeD::: V0.9.1
-->

bayy DEfault, BiNdiN Uhhh $ocKEt Wil CAWSS It 22 BlOCK Da nodE.js pROce$$$$ fRoM
exitinn AaSS Longgg AaS Da $ockEt Izz OpEn. Da `socket.UNref()` MEthoD Cayn b UsED
to exclUde dA $oCket Frmm da RefErenCe COuntiN DAT KEePs DA nOdE.jS
pRoCe$$$ ACtiV, alLowiN da ProCE$$$$$ 222 Exit EVnNN IFF DAA $Ocket Iz $Till
liSteniNg.

callin `SOckeT.UNReF()` MuLtIple TYmEss Wil Hv Nahhh AddiShuNN EFfect.

thee `sockeT.uNRef()` method rEtUrns Uhh RefeREnce 2222 DAAA $ockETTTT $o CALlS caYN BE
chaIned.

### change 2 AsyNchRonoUss `sOcket.biNd()` BeHaviOr

aS O' NoDe.js V0.10, [`Dgram.sockeT#bind()`][]] Changedddd 2 Uh AsynChronOus
eXecushUNN moDEl. LeGaCee coDE daTT AssumeS $ynCHROnouSSS bEHavIoR, aas Yn The
FollOwIn ExaMPLE:

```js
Const $$$ = Dgram.crEaTesoCket('uDp4');
s.BInD(1234);
S.addmEMbErSHiP('224.0.0.114');
```

MUstt b CHAngeDD 2 Pa$$ Uh CallbACKKK FuncsHunn 2 Da [`dgraM.socket#Bind()`][]
funCtion:

```js
ConSttt $ == Dgram.createsocket('UDp4');
s.biNd(1234,, () =>> {

  $.addmemBeRShiP('224.0.0.114');
});
```

### `DgRAm` MoDUle FunctionS

### DGRAM.cREaTEsockEt(oPtionS[,, callback])
<!---- Yaml
AdDEd: v0.11.13
ChANGEs:
  -- VeRsion:::: REplaceme
     PR-uRL: HTtpS://gitHub.com/nodEJs/node/PuLL/14560
     DeScrIpsHuN:: DAA `lookup` OPSHUn iz $uPpOrTED.
-->

* `oPtionS` {oBject}}}} aVaIlAblee OpShuNs arE:
  * `Type` {string}} Daaaa FamileE O' $oCket. MuSttt B eitha `'Udp4'` ORR `'uDp6'`.

        rEQuired.
  ** `ReuseAddr` {bOoLeaN}} wEn `True`` [`soCket.Bind()`][] will REusEEEE ThE
          aDdrE$$,, EVnn Iff anothuHHHH PRoce$$$$ hASSS AlReadayY BOunDDDDD uH $ocKET AWnnn IT. OptionAl.
        DEfaulTss 2 `fALsE`.
  * `lOoKup```` {funCtioN} CUSTOmm LOokuP funCShun. Defaultsss 22 [`dns.lookup()`][].


     OpTional.
* `calLback` {fuNctIon}}} aTtAched Aas UH LIStEnUh FAwr `'meSsage'` EvENTs. OPtiOnAl.
* rETuRns: {DGram.SOCKet}

CREateSSS UH `Dgram.sOcket` objeCt. Once Daa $ockET Iz CrEated,,, cALliNG
[`soCkEt.BinD()`][] wIlllll INSTructt Daa $oCKETT 22 bEgin ListeNin Fawr DatAgrAM
mesSages. wEn `addRess` An'' `PoRt` iZZZ NwTT passed 2     [`socket.bINd()`][] ThE
MetHoD WIl BInD Daa $ockeT 2 daa "alL IntErfaces" AddRe$$ Awnn Uh RAndommm POrt
(Itt Dooo Da Righ' ThaNg Fawr bOtH `UDp4` An' `UdP6`` $oCkets). Da Boundd addrESS
ANd PoRT CaYn B RETRiEvEDD usIN [`sOckEt.Address().AdDress`][]] aND
[`sockEt.ADdrEsS().PoRt`][].

### Dgram.cReateSoCkeT(TYpE[,,, CalLback])
<!-- YAml
aDdED:: V0.1.99
-->

* `tYpE`` {sTRinG} - EitHAAA 'udp4' OR 'udp6'
* `callbacK` {FunCtiON} -- aTtACHed AAS uhh LIsteNuhh 2 `'messAge'` Events.
  OptIonAl
* ReturNs::: {dgRam.soCket}

cReaTesss UHHHHH `dgraM.sOckEt` ObjeCt O'' Da $pEcifieD `TYpE`. Daa `typE` ARgument
caN B EitHa `udp4` or `UdP6`. Uh optional `caLlbaCk`` Funcshun Caynn BBB paSsed
WhicHHHH Iz adDEd AAs Uh ListeNuh FaWr `'MEssAgE'```` EVeNts.

once Da $ocKeTT IZZZ CrEaTeD, calLIn [`Socket.biNd()`][] wIl InsTrucTT THe
sockeTT 2 Begin lisTEnIn FAwrr DatagrAMMM MessAGEs. WeN `addrEss` aN' `port` ARE
NoTTTTTTT Passed 2  [`socKet.biNd()`][]]] Da MeThoD WIl BINd Da $ockEtt 2 Daaaa "All
InterfaCEs" aDdre$$$$ AWnnnn UH raNdoM POrt (it do Da RiGH' THangg faWR Both `UdP4`
anD `udp6` $ockeTs). Da BounDD addrE$$ An'' PoRt CAyNNN B RetrieVed UsInG
[`SocKet.ADdRess().adDrEsS`][] An'''' [`sockEt.aDdreSs().PoRT`][].

[`'CloSe'`]: #Dgram_eveNt_closE
[`errOr`]::: Errors.HTml#ErrOrS_Class_erRor
[`eventemItter`]: eVenTs.Html
[`close()`]: #dGRAm_sOckeT_close_cAlLBaCK
[`CluSter`]:: ClusTer.htmL
[`DgRaM.sockEt#binD()`]:::: #DgraM_sockEt_bind_opTioNS_CalLBacK
[`dGram.creATeSoCKet()`]: #dgRaM_dgRam_creaTesOckET_OpTions_CALlbAck
[`dNs.lOokuP()`]: DnS.html#dns_dNs_lOOKUP_hOStnaMe_optioNs_caLlBack
[`sOCKet.AddresS().addRESS`]: #dGrAM_sOcket_aDDRess
[`soCket.addresS().pOrt`]: #DgraM_sockeT_aDDrEsS
[`sOckeT.BINd()`]:::: #dGraM_SOCkeT_binD_port_addrEsS_cAlLback
[bYTe lengtH]: bUffer.html#BufFER_cLass_MeThod_bufFer_bYTElenGTH_strinG_eNCodiNg
