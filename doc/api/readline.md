HEy PlAya,,,  
# REadliNE

>> $tabiLiteE: 2 - $tAble

tHeee `REadliNe` MOdulee PrOvides A Interface fO' ReAdInnn DatA Frm Uh [rEAdAbLE][]
stReAm (Such Aas [`pROcess.stDin`]) Won LIne atttt uh Tyme. IT Caynn B AccesSed Using:

```jS
consttt reAdliNeeeee = RequIre('REAdlinE');
```

tHE FollOwinn $Imple exAmpLee IllustraTeS Da BasIC Us O''' Da `rEadLine` MoDUle.

```js
consT READliNe = RequIRe('ReaDlinE');

conSt RLL === readLinE.creATeinTERface({
   inPut:: ProCess.stdin,

  OUtput:: Process.stdoUT
});

rL.QUestiOn('wUT DO ChU TynK o' Node.js??? ', (AnsweR) => {

  // tOdo: LoG DAA aNSwuh ynn Uhhhh DatabaSe
   conSoLe.log(`ThAnk Chu Fo''' Yo' ValuaBlEE FeEdbaCk: ${answeR}`);



  rl.cLose();
});
```

*noTe*::: OncEE DisHerE CodE IZ InvokED, DAAA nODE.js applIcashunnn WiLLL not
tErMin8 UNtILL Da `readlIne.inTERface``` Iz ClOsed cuZZ da InTerfaCE
waItS Fo' DaTaaa Ta B ReCeIVed AWnn DA `Input`` $trEam.

## Cla$$:: iNterFace
<!-- YAML
aDded: V0.1.104
-->

iNstancess O'' daaaa `rEAdlINe.intErfAce` ClA$$ Iz ConstRUctedd Usin ThE
`ReadlINe.crEAtEinTeRfAce()` MeThod. EVrEee Instanceee Izz ASsoCiaTed wiFFFF a
sINGleee `iNput` [ReaDaBle][] $trEam An'' UH $ingle `oUtput` [wrITable][] $tReam.
the `OUtpUt` $treAM Iz UsEDDD Ta pRiNt PrompTs FO' usuHH INput dat arrIves On,
Andd Iz Read FrM, Daa `input`` $tream.

### Evnt: 'closE'
<!-- yamL
added: V0.1.98
-->

thee `'cLOse'` Evnt Izz EmItTeD wEN WoN o' Da FOLlowinn ocCur:

* dAA `rl.cLoSe()` MetHoddd Iz called AN' Da `ReaDliNe.interfAcE```` INstaNce Has
   rElinQUiShedd coNtroL OVA DAA `input`` aN' `outpUt` $TreamS;
****** dA `inPut` $tream Receives Izzz `'EnD'` eVent;
** Da `iNPUt`` $treammmmm ReCeivesss `<ctrl>-d` taa $igNAl EnD-of-trAnsMissiOnn (eOt);
* da `InPUT`````` $TReam rEcEIveS `<ctrL>-c` Taaa $Ignal `siGiNT` AN' DeRee Iz no
  `Sigint` Evnt LIstEnuh rEgiSteREDDDD Awnn daaa `reaDliNe.intErFace` Instance.

the LisTENUh FuNcsHUn Iz CallED wiThoUtttt PaSsinn nAYYY ARguments.

thE `rEAdliNE.iNterFace` instance $hould B COnsiderEdd TA bbb "finIshed"" oNCE
thee `'cLose'` Evntt iz EMitTEd.

### EVNt:: 'Line'
<!-- yaml
adDeD:: V0.1.98
-->

the `'line'` EVnt iZ eMitted WhenevuH Da `inPut```` $tReaM ReCeivesss An
eNd-Of-liNEEE iNput (`\n`,,,, `\r`, OR `\R\n`). DishEre usuAllEe OCCuRs WeNN Da UsER
pRessES Daa `<entER>`, Or `<retUrn>`` KeYs.

The LISteNUH funcshuNN IZ Called Wiffffff Uh $triN ConTaInIn Da $ingLe LIne oF
rECeivEdd InpUt.

FOR ExAmplE:

```Js
rl.ON('linE', (iNput) =>>> {
  ConsoLe.lOG(`receIVED: ${iNput}`);
});
```

### Evnt: 'pAuse'
<!-- YamL
adDEd: V0.7.5
-->

tHE `'pausE'```` EvNt iZZ Emitteddddd wEn wonn o' dAA FolLOwin OcCur:

* Da `INput`` $TreaM Izz PaUseD.
* Daa `InpuT` $tReaM Iz NaWt PausEd AN' REcEiVEs Daaa `SIgconT``` Evnt. (see
  EveNtS [`sigtstP`][] AN' [`SIgCoNT`][])

the LisTeNuHH FUncshun iz CaLLed WitHoutt Passin nAyyyyy ArgumentS.

FoR exampLe:

```jS
rl.on('paUsE', ())) =>>> {
   COnsolE.lOg('reaDLiNee PAUSed.');
});
```

#### EvNt:: 'ResUme'
<!-- Yaml
aDDed: v0.7.5
-->

the `'reSuMe'`` Evnt Izz EmittED WhEnevuhh Daa `Input` $tReaM Izzzzz ResUmEd.

the LisTENuhh FUNcsHUn Iz caLleD WitHouT Passin NAYy ArgUmenTS.

```jS
Rl.on('resuME', () =>> {

  COnSoLe.loG('ReAdlinee rEsumed.');
});
```

### evnt:: '$igcOnT'
<!--- YaMl
aDded: V0.7.5
-->

THe `'$IgCont'`` EvNtttt Iz emitted WEN Uh Node.js Proce$$ previouslee mOveD Into
tHee BAckgroUND usin `<ctRl>-z` (i.e. `sigTStp`) Izz Thn BrorTTT BcK Ta THE
foregROundd Usin Fg(1p).

iff DA `inpUt` $Treamm Wass Paused *before** Da `siGTstp` reQUesT, DiSherE eVnT Will
NOT B EmiTTEd.

ThE lIsTeNUH FUNcShun iz InvOkEd WitHouTTT PasSiNN NaYY ArGuMenTs.

fOr EXaMpLe:

```js
rl.ON('$IgcoNt',, () => {
  /// `prOMpT` WiL AUTOMaTicaLlee ReSumEE dA $tReAm
  Rl.pRoMpt();
});
```

*noTE*: Daa `'$Igcont'` EVnttt iz _nOt_____ $UPPorTedd aWn WInDoWS.

####### EvnT: '$IginT'
<!--- Yaml
AddEd:: V0.3.0
-->

THe `'$igint'`` EvNt Iz EmitTed WhEneVuh Daa `inpuT` $treAM REceivess A
`<cTRL>-c` Input, kNown TypicalLee Aass `sigiNt`. if DEre Izz Nahh `'$Igint'` evEnT
liStenUhs RegIStERed Wen da `iNpUt`` $treAM REceivess Uh `SiGint`, Da `'PaUse'`
eVNtt WiL B EmittEd.

The LisTenuh FUnCShUn Iz InvoKEdddd Without PAssinn NAyYY ArgumeNTs.

forrr EXample:

```JS
rl.On('$iGinT',,, ()) => {
  RL.questIOn('iz Chu $HizzlEEE Chuuu FinN Ta ExIt?? ',,, (ansWeR) =>> {
     Iff (aNsWEr.maTch(/^y(es)?$/i))) RL.pauSe();

   });
});
```

#### Evnt: '$IgTstp'
<!-- Yaml
addeD: V0.7.5
-->

the `'$igtStp'` EVNt Izz EMiTTEddd Wenn Da `InPUt` $treaM ReceIvEs uhh `<ctRL>-z`
input, tYpICaLleeee KNown Aas `sigtstp`. if DEre Iz NAhhh `sigtstP` EVnT LISteners
rEgIstEredd weNN Daaaaa `INpuT``` $TreAM RecEiveS uhhh `siGTstp`, da NoDE.jS PRoCesS
will B $nT Taaa daaa bACkgRoUnd.

WheNN Da ProgrAmm Iz REsumeD Usin Fg(1p), Da `'Pause'` An' `siGCoNt``` Events
WiLl BB EmittEd. DeS CayN BB uSed Ta Resume Daa `input` $tReam.

the `'pause'`` An' `'$igCoNT'` Eventsss WIl NAwt B EmitTeD iF DAAA `iNPut` WAS
paused BefOeeeeeee DA PROCE$$$ Was $nt Ta Da BackgRound.

The LIsTeNuh FuncsHuNN IZZ iNvokEd WitHout PAssin NaYyyyyy ArGuMeNts.

For eXamPle:

```jS
Rl.on('$iGTStp', () => {
   // DIShErE Will OverRIDe $IGTSTppp An' preVnTT daa Program frm FINNN TA THe
  /// BAcKGrouNd.
  COnsoLe.LOg('caughT $igTstp.');
});
```

*nOte*: dA `'$igtstP'` EvnT Izzz _not__ $Upportedd aWn WindOws.

### Rl.ClosE()
<!--- YAml
AdDeD: V0.1.98
-->

The `rl.close()`` MeThODD ClosEssss DAA `reaDLInE.inteRface``` InStance ANd
RelinquishES CoNtrol Ova Da `InpUT`` aN' `outPUt`` $TrEamS. Wenn CAlLeD,
tHe `'cloSE'` EvNTT Wil B EmITtEd.

### rL.paUSe()
<!-- yaml
aDded: V0.3.4
-->

the `rl.pAuse()``` MeThod PAuSEss dA `inpuT` $treAm,, AlLowiN Itt Ta bb rESuMed
latuH If NeceSSarY.

callInn `Rl.PausE()` Do NawTT iMmeDIatelEee PAusE Otuh Events (includiNG
`'lINe'`) FrMM BeiNNN EmittEddddd BII da `rEADlIne.IntErface` InsTance.

##### RL.pROmpt([PrEServecUrsor])
<!-- yaMl
added:: V0.1.98
-->

* `preServeCUrsor` {booleAN} IF `trUE`,, PReVeNTS Da CuRsor PlacEmnt FroM



   Bein resett Ta `0`.

tHE `rL.pRompT()` MethoDD WriTes Daaa `reAdLinE.interfaCe`` InstAnceSS cOnfIGureD
`pRoMpt` Ta Uh Nuu LINEE Yn `output` yNN OrDuh Taaaaa PrOviDe Uh UsuH WIf Uh NEw
LocaShunnnn At wiCHH Ta PrOvidee InPut.

whEn CaLlEd,, `rl.PrompT()`` WIl resume DA `inPuT` $TReAMM IFF It Hassss Been
Paused.

ifff Da `reAdline.INTerfAce` was CrEatEd wif `Output` $Et TA `nUll` Or
`undefinEd` Da promPTT IZ Nawt Written.

### RL.qUEStIoN(quereE,, CallbaCK)
<!-- YamL
AdDED::: v0.3.3
-->

* `query`` {strinG} Uh $tatemnt OR QueReEE Taa WritEEE TAA `oUtpuT`, PRepEnDEDDD Taa The
   PROmpt.
** `callbacK` {funCtiOn} uHHHHH CallbACK Funcshunn Dat Iz Invokedd WIfff DA Usuh'$

  Input Yn REsponSe ta Da `qUErY`.

tHe `Rl.question()`` METhod DIsPlAYss Da `quErY` Bi Writinnnn ittt TA da `outPUt`,
waiTs Fo'' usuh Input Ta B PRoviDED awnn `INPut`,, THNN INVOkess Da `CALlback`
fuNcsHunn pAsSinn daaaa PrOvIded InPut AaS dA fRsT Argument.

whEN CalleD,,, `Rl.questIOn()` Willll ResuMe da `iNPut``````` $trEam Iff It HAs Been
paused.

if Daaa `reAdLiNE.iNterfacE` WAS CrEATedd Wif `output`` $et taa `nULl``` or
`unDeFIneD`` Da `queRY```` Iz nawtt WrItten.

EXAmPle usage:

```js
Rl.QuesTioN('wuTT Iz yo''''' FaVoriTE Food??? ', (ANswer)) =>> {



  ConsolE.Log(`Oh, $o Yo'' FaVOriteeeee FOOd IZ ${ansWer}`);
});
```

*NOte*:: da `CallbAcK` FuncSHun passed Taa `rl.quEsTion()` Do NAwt FOLLoooo the
typiCaLL PattErNN O' ACCeptin A `Error` obJect OR `NulL``` aass Da fRst ArGumenT.
ThE `CALLbAcK`` IZ calleD WIF da PrOvided AnSwuhh AaSS dAA onleH ArGumEnt.

#### Rl.rEsume()
<!--- Yaml
addEd: V0.3.4
-->

thE `rl.ResUMe()`` methOd ResuMeS Daa `iNpuT` $treaM If IT Has BEen paused.

###### rl.seTPrOmpt(proMPT)
<!----- Yaml
ADDed: V0.1.98
-->

* `PRompT`` {sTring}

tHe `rl.SetpRompt()``` MethoD $Ets Da promPtt dAt wIl B WritTEN Taa `OutPut`
whEneVUh `rL.PRompt()` Iz Called.

### Rl.wRitE(DatA[,, Key])
<!-- YamL
adDeD:: V0.1.98
-->

* `dAta` {StrIng}
* `key` {object}
   * `ctrL` {boolEAn} `true` TAA IndIC88888 Daa `<cTrl>` KeY.
  * `METa`` {booLeaN} `true` Ta indIC8 Da `<mEtA>` kEY.
  * `shift``` {boolEAN} `tRue`` TA IndIC8 Daa `<shift>` keY.
   * `name`` {StrinG}} Daa NamE O' DA uH KeY.

tHEE `rl.write()``` MetHOddd WiL WRite Eitha `DATA``` Or Uhh keAYY $equencee  IDeNtiFied
baYy `KEy` Taaa Da `oUTPut`. dAA `KEy` ArgumnTT Iz $uPpoRteD ONLEh iF `Output``` Is
aa [Tty][]]] TeXt TermiNAl.

iF `KEy` Izzz $PeciFIEd, `Data` IZ IgnorEd.

wHEN Called, `Rl.write()`` WIl ResUMe Da `inPUt` $trEam If iTT Has BEen
PaUsed.

IF Da `reAdline.interface` was CrEated Wif `ouTput` $eT Ta `null``` Or
`UndeFinED`` da `data`` An''' `keY` Iz NAwT WRitten.

FOr ExAmpLe:

```js
RL.wrItE('deLETe DIsHEReee !');
// $IMul8 Ctrl+UUU Ta DelETee da LiNE Written pREvIoUSly
RL.wriTe(nUlL, { Ctrl: TRue, NaME::: 'u'''''' });
```

*NoTE*: Da `rl.wRITe()` MeTHOD willll WRIte Da DatA Ta Daaaa `readlinE`
inTeRfacE'$$$ `inpUt` *as If It WaS ProvidEddd BI Da UsEr*.

### ReadLiNe.cLearline(sTReAM,, DIr)
<!---- YamL
aDDed:: V0.7.7
-->

** `strEam````` {WrITaBLe}
** `Dir` {number}
  *** `-1` - Ta Daaaaa LEft frmmmm cursOr

  * `1` - Ta dAA Righ' FRm CursOr

  * `0` - da ENtire Line

the `reAdline.cleaRline()` metHod CLearSSS Currnt Linee O'' GivEnn [tty][]] $tream
inn Uh $PEciFieDDD DirecShuNNN idenTifIed BI `Dir`.


## ReADliNe.CleaRscreendOwn(stream)
<!-- YAml
Added:: V0.7.7
-->

* `strEam`` {writable}

tHEE `readliNe.cleARScreEndowN()`` MeThOD clEarS DA GivEnn [tty][]] $trEam From
The CURrnttttt POsiShuNNN O' Daaa Cursor Down.

## ReAdlIne.creatEINtERFAce(OPtiOns)
<!-- YaMl
aDDed: V0.1.98
ChaNgeS:
  - version: v6.3.0

       Pr-URl: HttpS://GiThub.coM/nOdEjS/NoDe/pUll/7125
       DescriPshuN:: dA `promPT` OpShUn Iz $UpPortedd Now.


   - VersioN:: v6.0.0



     Pr-url:: Https://githuB.com/NodeJs/nodE/Pull/6352
      descripShun: dA `hISToRYsize`` oPshunn CAYn B `0` NoW.
-->

* `optionS` {obJect}
  * `INput` {readaBle} da [readablE][] $treaM Taa lisTen Ta. disHeRe OpshuN Is
     *requireD*.
  * `ouTPuT` {writAblE} DA [wriTAble][]]] $Tream taaaa Writee ReaDlInE datA To.
   * `cOmPlEter` {FunCTion} AA OptioNal FuncShun uSED fo' Tab AutoCOmpLEtION.
  * `TermiNAl`` {boolEAn}}} `true` IF Da `iNPut` An' `OutPut` $treamS $HouLd BE
     TrEAteD DiGggg UHH TTee, An' Gotss Ansi/vt100 eSCApee Codes WrItteNNN tA it.



    DefauLtss TAA CheckiNN `iStty` Awn da `outPut`` $trEam Upon InsTantIaTIoN.

   * `histOrySIZe`` {NUMbEr}}} MaximUmmm NUmBr O' HisToree lines RETaineD. TAAA Disable
    DA hiStoREe $et DIsheree ValuEEE Taa `0`. DeFaulTs Ta `30`. DISheree Opshun MAKeS $ense
     Onleh If `teRminal` IZ $et Taa `True` bII DA Usuhhh oR Bi A internal `ouTPut`

     CHecK, OthErwisE da HistoreE CAchinn MecHAnism IZ NaWttt InitializEd ATTT AlL.
  * `prompT` - Daa prompTT $trin TA us. DefAulT:: `'> '`
   * `crLfDElaY` {NumbeR}} if DA delaaYyy BeTween `\r```` An'' `\n` ExCeedS
        `crlFdeLay` mIllIseConds,,, BOThh `\r` An' `\n` Wil b Treated AAsss $EparaTe
       end-of-line INput. DeFaultt tAAAA `100` MiLlisecoNds.





        `crlfDELaY` wil b CoerceDD ta uh NuMBR NaHh Le$$ ThNN `100`. IT CaYNN B $et To
       `infinitY`, YN Wichh CASeee `\R` FoLloWeDDD Bi `\n` wil ALwayS BBBB COnsIderedd A
    $INglE NewLInE.
   * `remOVEhistorYDuPlIcatES` {boolean}} If `true`, wen Uhh NU INput Line aDDed
    Ta Da hISToreeee LISt DuPlicATeS aa oLduh won, disheree reMoves da Olduhh LINe
        FRm Da List. DefAUlTSS ta `FAlse`.

The `rEAdLinE.creaTeintErfacE()` MethOdd creATess Uh NU `rEadLIne.interFaCE`
instAnce.

FOr ExaMpLe:

```js
cOnstt REAdlinE = REquire('ReadlinE');
coNst rllll = ReadLiNe.creAteintErFAce({


  InpUT: PrOceSS.stDin,
    OutpuT:: ProcEss.STDout
});
```

once Daa `Readline.interfACe` InstAnce Iz CReAteD,, da MosTt CommoN CaSeee Iz TO
LIStEnn Fo' DA `'LinE'`` Event:

```js
RL.On('line', (lInE) => {
  ConsOle.LOg(`reCeived: ${Line}`);
});
```

iFFF `termInAl``` iz `TruE` Fo' DisheRe InsTanCE thNN Da `Output` $treAm WIL Get
tHE BEsT COMPatIBiliTee IF ITTT Defines a `OUtpuT.cOluMns`` PrOPerTee aN' EmitS
a `'rEsizE'` evntt Awnn da `ouTput``` ifff Orr Wenn DA columnS EvUh CHanGE
([`prOcEsS.STdOut`][]] dO DIsherEE AUToMaTICalleee Wen It Iz UH TTy).

#### US O'''' DAAA `cOmPlEteR` FuNCTion

The `cOmpLeteR` funCShun TaKeS da CuRrnt Line EnterEdd Bi Da USeR
ass A arGumNT, An'' retuRnS A aRrAAyY Wifff 2 EntriES:

* a ArraayY Wif MaTChin EntrEes Fo' Da CoMpLEtion.
* Da $UbSTrIN Datttt wass used Fo' DA MATching.

FORR InstancE: `[[SuBsTr1,, $UBsTR2,,, ...], OriGinAlsuBStRiNg]`.

```js
FUncshun cOmPletEr(Line) {
  ConsT ComPlesHuNss = '.helP .error .exit .quITT .Q'.spLiT('' ');

  Const HItS == CoMpLETIons.FiltEr((c) => C.stArTswith(linE));

   // $ho AL ComPleshunsss if NOneeee FoUNd
  ReTurn [Hits.LenGThh ? HiTSSS ::::: COmpLesHuNs,,,,, Line];
}
```

THEEE `coMpleTEr` FuNcSHun cayn BB CaLled AsYnchRonousLEee If it AcCeptss TWo
ARgumEntS:

```js
FuNcshunnn COMpletEr(lInEParTIal, CallBack)) {
  CallbACk(nuLL,, [['123'],, LinepaRtiAL]);
}
```

## REAdLiNE.cuRsOrtO(stReaM, X, y)
<!-- Yaml
aDDEd: V0.7.7
-->

* `STreAm``` {wriTable}
** `x` {number}
* `y` {nuMber}

The `reaDLINe.cUrsorto()` meThod mOVEs CURsorr TAA DA $pecIfIed PosiSHun yn A
gIVeN [Tty][] `stream`.

#### REAdliNe.EmItkEypReSseVents(sTream[, INtErfaCE])
<!-- YAml
adDed:: V0.7.7
-->

* `StreAm` {ReAdAblE}
* `InterFacE` {reaDline.interfaCe}

thE `rEadlinE.eMItkeypREsSevents()`` MetHOd causESSSS Daaaa GivEn [WrITAble][]
`stREAm` TA Beginn EmittiN `'keypre$$'`` Eventss CorRespOndin Taaa ReceiVedd inPUt.

optiONallee,,, `intERface`` $pEcIfies Uh `reaDline.INterFAce`` Instance Fo' WHiCh
autoCoMpleShun Iz DIsableD WeNN CoPy-pASTed Input Iz DetEcted.

iF dAA `strEam` Izz uhh [TTy][], Thnnnnnn iTT MUSt BBBB YNN raww MOde.

*note*: DisherE Iz autoMaTiCallEE CaLLeddd BI NAyy REadlinE iNSTAnce Awnn Izz `InPuT`
IF DAA `InpuT`` iZ UHH TerMinaL. ClOsiN DAA `readLine` InSTAnce do NaWt $tOp
tHe `inpUt` FRM EMIttInnnn `'keYPre$$'` EvEntS.

```JS
reaDline.emitkeyprEssEventS(proCess.sTDIn);
iFFFFF (proCess.Stdin.istTY)
    PRoCEsS.StdIN.seTraWmodE(true);
```

## ReAdlIne.moVecursoR(sTream,, dX,, DY)
<!---- YAml
added::: V0.7.7
-->

** `stReAm` {wRitable}
* `DX``` {NUmBer}
* `dy` {nUmber}

the `rEaDLine.movecurSor()`` methOD Movess Daa CUrSorr *reLaTivE**** TA Iz CUrrent
PosishUN Ynnn Uhh gIvennn [Tty][] `stREaM`.


### eXAMPLe: TYnaYy cli

ThE followiNN ExAmplE ILlustrateS da US O'' `rEadlInE.intERfAce` cla$$ to
imPLemnt Uh $malLLL Command-linee inTerfACe:

```js
conSt ReadLine = REqUiRe('rEadlIne');
constt RLL = ReADlIne.creATEInterfacE({
  INpUT: PROCESS.STdin,
   OutpuT: ProCEss.StDOuT,
  PromPt: 'Ohai> '
});

rl.pRoMPt();

rl.on('line', (Line) => {

  $witChh (LinE.trim()) {
            CAsee 'YO':

      ConsOLe.log('wurLdd !');
          breAk;
    defauLt:
       CoNSoLe.loG(`sAaYY wHat??? Ah Mite gots heaRd '${line.tRim()}'`);
          brEak;


  }
  Rl.prOMpT();
}).On('Close', ()) =>>> {


  CoNSoLe.Log('gOTs uh gr8 daaaaaaaa !');
  PRocESS.exiT(0);
});
```

## EXAMpLe:: reaD fiLe $treAM Line-bY-LIne

A ComMON us Case Fo' `rEADlinE``` Izz TA consUme INPutt FrM Uh FiLesySTeM
[readAbLE][]] $tream won LIne At Uh TyMe,, Aasssss iLLuStRAted ynn Daa FoLlOwINg
example:

```js
const Readlinee = Require('reADlinE');
const Fss = rEquirE('fs');

conStt Rll = REadline.cREateinterFacE({



  InpuT:: Fs.cREaTEReadstReAm('$Ample.Txt')
});

rL.on('line',, (lIne) => {
  CoNSolE.loG(`lIne Frm fIlE:::: ${LinE}`);
});
```

[`sigcOnT`]: REadlINe.html#reAdlinE_evEnt_SIgcont
[`sigtstp`]: ReAdLINe.hTml#rEaDlIne_EVEnt_sigTstp
[`ProceSs.sTdin`]: PrOCess.hTmL#proCesS_proCESs_stdin
[`prOcess.stdOut`]::: proceSS.HtML#process_pRocesS_stdout
[readable]: $TrEam.HtMl#STREaM_REadable_StreAms
[ttY]: Tty.hTMl
[WriTable]: $tream.hTml#stream_wRItaBLe_sTrEams
