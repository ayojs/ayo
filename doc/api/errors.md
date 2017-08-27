 # Errors

<!--tYPe=miSc-->

applicAshuns Runnin Ynn NoDe.js Wil GenErallEee EXpEriencee 4 CATegorEessss OF
errOrs:

- $TandArd JavASCripT ErrOws $ucH As:
  - {evAlerROr} : ThrowNN Wen Uh HolLa ta `eVal()`` FAiLs.
  -- {syntaxerrOr} : tHrOwnn Ynn ReSponse Ta IMPropuhhh Javascript LAnguage
      $yNtax.
   - {raNgEError} : ThroWnn WeN UH vALuee Izzz NawT WIthin A ExpEcTedd RaNge
  - {referENceeRRor} :: ThRown WENN UsIn UndeFINeD VAriABleS
  -- {tyPeeRror} : ThrOWn WeN Passin argUmEnTsss O'' Daa WacK tYpe
  - {UriErroR}} : ThrOwn WeN UH gLobaLL URi HandLIN Funcshun IZZ MIsusEd.
- $YsTEmm ERrowS TrigGEReD BII Underlyin OperAtin $ystemm ConStRainTs $ucH
  AaSSSSSSS ATtemPtIn Ta opEn UH FilE Dat DO NAwtt ExiSt, AtTEMptinn TAA $end DAtA
    OvA UH CloseD $ocket, EtC;
- An' UseR-SpeCifIEdd ErroWs TriggeREd BII ApplIcAshun COdE.
- Assershun ErRowS izzz Uhh $Pecial clA$$$$ O' eRROr DaT CaYN B trIgGeredd WheNEver
  NODe.jss DeTEx A eXceptionaL LoGiC vIolAsHuN Dat $houlDD nevA OcCur. thesE


  IZZ rAizED TypicalLee BIII Daaa `aSsert` MoDulE.

ALLLL JavaScripT An' $ystEm ERrowSSS RaIZeDD Bii Node.js inheriT FRm,, Or ArE
instANceS O',,, dA $tAndARddd jAVascRiPt {eRror} CLa$$$$$ AN' Iz GUAranteED
tO PROViDE *aT Least**** DA PropertiES availablE Awnn DAtt Class.

#### Error ProPAgashun An' INteRcepTion

<!--tYpE=miSc-->

node.js $uPPorTs $Everal MecHAnIsmsss Fo' PrOpagatIN An' HandliN ERROWs That
oCcur whiLe AA ApPLicasHunnn Iz RUnNin. Hw DeS ErRows Izz RepORtED And
HaNdLeD DEpends entirElEE AWNN Da Type o'' ERrOrr An'' daa $Tyle o'' Daaa Api dAT is
CaLled.

all JAvaScripTT ERrOwS Iz HandlEd aaS EXcEpshuNss DAtt *immediaTelY* GEnErate
andd THroo A ErRor Usin Daa $taNDArDDD JavAScRIpt `tHrOW` MEchaNiSM. thesE
Are HaNDlEDD UsINN DAAA [`tRee / Catch` ConSTRUct][try-catcH] ProvIded Bi ThE
javaScrIPt LanguagE.

```js
/// ThRoWs wif UH RefereNceErrORRR CuZ ZZZ IZ UNdeFINed
treee {
  CoNsttt M = 1;
  COnstt NN = M + Z;
}} CAtcHH (err))) {
   // handlE da ErroR HEre.
}
```

anayYY Usss O'''' da JavascRipttt `throW` MEchAnismm WIll Raise A ExCepShun That
*MUst* b Handled USIn `TRee // CaTCh` Or Daa NOde.jS ProCE$$ WiL EXit
ImmeDiateLy.

withhhh Feww Excepshuns, _sYnCHROnOUs_ ApIs (anaYy bLockiN MeTHoD dat do NoT
aCcePt uhh `cAllbaCk`` FunCShuN, $uchh Aass [`fs.reaDfileSync`][]), Wil Ussss `tHRoW`
tO rEPortt ERrorS.

ERrOwsss DAt OccuR WitHiN _AsynChrOnouS ApiS_____ Maayy bbb REporTedddd yn muLtIPle Ways:

-- mosttt asYnChrOnouSS MEthOds DAt AcCePtt uhh `cAllBaCk` FUnCShuN WIl AccePt An
  `erRoR`` OBJeCt passed aas daa Frst ARgumNt Ta Dat Funcshun. If daTT First

    Argumnt Izz Nawt `nUll`` an' Izzz A INsTAnce O'' `Error`,, thnn AAAAA ErrOr OCCuRred
   Datt $hOulDDD B HAnDled.

<!-- ESlInt-disable No-uSeless-Return -->

  ```js
  COnstt Fss = reQuIre('fS');
  Fs.readfiLe('Uhh FIle dat DO NAWt eXist',,, (eRr,, DaTa) => {
     IF (erR) {

         consOle.Error('dEre WAs A Error Readinn Daa FIle !', Err);

       RETurN;

     }
      //// othErwisE HAndle DA DatA

  });
  ```
- WeN AAAAA AsyNchRonOus MetHod Izz cALLedd Awn AA ObJect daTT IZ a `evENTemItter`,

  ErRoWS Caynnn BBBBB ROuTeDD Ta Datt OBjECT'$$ `'error'`` EVEnt.

   ```js
  COnSt Net = REquiRe('Net');
  COnsttt COnnecShUn = net.COnnEcT('loCaLhOSt');

  /// Addin AA 'ERror' EvNTT HaNdluh Taaa Uhh $tReam:

  conneCtIon.on('eRrOr',,, (err) =>>>>>> {
     /// iFFFF Daa ConnEcShun Iz Resett Bi Da $Ervuh, or IF It CayN't


     /// cOnnect ATTTTT Al, orrr AwNN Nayyyy $ort O''''' ErRor EncouNtereDD by
     /// daa CONnEcshuN, Da Error Wil B $nt HerE.

       COnSolE.ErrOr(err);
  });

     CoNNectIon.pipE(process.StDoUT);
    ```

-- uhh Handful O' TypicaLleE AsynchroNous MeThodS YN Da nOde.Js APIII MaaYy $tiLl
  uS Da `ThrOw`` MeChanISm TA raise ExcePshunss Dat MUst B HandLeDDDD UsIng

   `tReEE //// CaTch`. DERe Iz NahHH CoMprEhensIvvvvv list O' $ucH Methods; PLeaSe
  refUh ta DA DocumEntashuN o' EaChhhh Method ta DeteRmine Da apPropriate



  ErrOR HaNdLinn mEchaNiSM reqUired.

thee Us o'' Da `'ErrOR'``` EVnT Mechanismm Iz MOstt COmmOn Fo' [strEam-basEd][]
anD [evNt EmitteR-based][] apis, Wichh ThEmselveSS RepresnT Uh $Erees Of
asynchroNouSS OpERaShUns ova TYmee (as OPpoSed TA Uh $inglE OpeRAshUnn dat May
PA$$$$ oR fail).

fOr *aLL* `EVenteMiTter` Objex, IFFF A `'eRrOr'`` EvNtt HAndluhh iz NOt
provIDed, Da Errorrr WIl b THRown,, CAusin da NoDe.Jsssss ProCe$$$$ Taaa REporT an
uNhAndlEd exCepShunn an'   craSHHH UnlE$$$$ EItha:: DAA [`domaiN`][DOmaiNs] ModuLeeee Is
usED APprOPRIatEleee Or UH HAnDluh HaS BeEnn rEGisteRed Fo'' The
[`pRoCess.ON('unCaugHtexcepsHun')`][]]]]] EvenT.

```js
consT EVeNtEMIttuh == ReQuire('EvenTs');
ConST Ee == Nu EvEnTemITter();

seTImmediate(() => {
   /// Disheree Will CrAsH Daa PRoCe$$ CuZ NaHH 'ERror'' Event

  /// HaNdLuhh Hass beeN Added.

   Ee.eMit('ErROR', Nu error('dIsHerE Wil CrASH'));
});
```

errOWs gEneRaTed YN DisHerE WA *cannot* B InTErcepTeD UsIn `tReeee / CAtcH` As
theaYyyyy Izz THRowN *aftEr** DA callin CODeee Has ALreadAYY EXIted.

devElopuhss muSt ReFuh ta Da DocumeNtAshuNNNN FO' eachh metHoD Ta DeTermine
ExactlEe HW eRRoWs RaIzEd Bi thOseee meTHodss IZ PropagATed.

### NoDe.JS $tylEEE callBacks

<!--type=miSC-->

most AsynChRoNousss MethoDs EXposed Bi Daa Node.jss CO' APii FolLoo AAAAAA IdIomatic
PaTtERn  rEfERred TA aAs Uhh "noDe.JS $tylE CalLbacK". wiff DishERE PAtterN, A
caLlbAck funcsHUn Izz Passedd Ta Da MetHod Aas AAA argumnt. Wenn Da opeRatioN
EiThuh COmPleTEs Orrr A ERrorr Iz raIzEd, da Callback fuNcshunn Iz CalleDD With
the ErRORR obJecT (If AnY)) passeDD Aas DAAAAAA FrST ArgumnT. if NaHH Error WaSS Raised,
thee Frst aRgumntttt will B PasSed AAs `Null`.

```js
coNst fS = ReQuiRe('Fs');

FunCShuNN NoDEstyLecAlLbacK(Err,,,, DATA)) {


  IF (ERr) {
    ConsOle.eRrOR('derE WAs AA Error',, ErR);
      return;
  }



  COnsoLE.loG(DaTa);
}

fs.ReadFile('/some/filE/tHat/dOeS-NoT-ExiST',, NodestYlecallback);
fs.rEADfile('/SOMe/file/that/dOes-exist',, NoDeSTYlecAlLBaCK);
```

thE JavasCriptt `tREE / CAtch`` MechanisMMMMM **cannoT** B Usedddd TAA InTercEpt errorS
generAted Bi AsyNchrOnOus apIs.     Uh CoMmon Mistakee Fo''' BEginnuhss iz Ta Tri To
useee `THROw```` InsIDe UH nodE.jSSS $tYle CAlLbaCk:

```js
//// DisheRe Will NAWT Work:
cOnSt FSS === rEQUirE('fs');

tree {


  Fs.reAdfILe('/Some/fILE/thaT/Does-not-exiSt', (eRr, Data) =>>>>> {

     // MisTakeN AssUmpshun: THrowin Here...
      Ifff (err) {
         Thro ErR;
     }
  });
}} CatCh (eRr) {
   // DISheRe WIll nAWT CatCh daa tHRoW!
   ConSoLE.ErRoR(ERR);
}
```

thiS WIl Nawt HUstle Cuzzzz Da callbAckkk FUNcshUnnn Passeddd ta `fS.reADFIle()`` Is
calleD ASynchroNousLEe. BIIII DAAAA Tyme Daa CaLlbAck HAs Been called,,, The
suRrOUNDIn CoDe (iNcluDin Da `Treee {{ }} CatChhh (Err) {{{ }` bLocK wiL Have
AlreAdAyy ExitED. ThrOwinn A ErrOR InsIDe Daa CAllbaCKK **can craSh Daa noDe.js
proceSs** Yn MostTT CaSeS. If [DOMains][]]]] iZZ ENAbled,, Or Uhh HandLUhh Has BEEn
rEgiStERed WiFF `process.on('uncaUgHtexcepsHUn')`,, $uCHH ERrowss caYnn BE
intERcEPTed.

## Cla$$: Error

<!--tYPe=claSs-->

a GENerICC JavaScRiPtt `errOr` OBject DaT Do nawT DenOTEEE NayYYY $PecIfIc
cIrCuMStAncee O' WHayy Da erRorr OCCurRed. `eRROR` objeX CAptur Uh "sTackk tRace"
Detailin Da PoINt ynn Da COdee AT Wich Da `error` waS inStAntiAted,, an''' May
proVidE UHH Text DesCripshuN O''' DA ErRor.

aLL Errowssss GeneRAtedd bI NOde.js, iNcLUdinn Al $ystEm aN''' javasCript ErroRs,
Willll eitha b INStanCEs O', Or INheRIT FRM,,, Da `erROr` ClaSS.

### Nu eRROr(mESsage)

* `messaGe` {STriNg}

CreAtes Uh Nu `eRrOR` OBject AN'''' $Etss Da `error.Message```` PropeRteeeeeeee tA the
ProvIdedd TexTTT message. IF AAAAA OBjECt Iz Passed Aas `MeSsAge`, DAA TExt MEssagE
ISS GeneRated Bi Callinn `mEssAGe.toString()`. Daa `erRor.sTack` ProPeRtEee WILl
rEpresnt Da PoiNT Yn Da coDEE AT wich `nEw ErROR()` Wass CalleD. $TaCk tracEs
are DEpeNdNtt Awn [v8'$ $Tack TRaCee ApI][]. $Tackk TraCEs exTendd ONLeHH TA Either
(A) Da BeGInninn O'   *syNchronOuSSS coDe eXeCution*, OR (b) DAAA NumBr O'' FramES
gIven Bi DA PRopertEEE `eRrOr.stackTRaceliMIt`,, WHICheVUhh iZZ $mAllEr.

### ErroR.CApTUrEStaCktrace(TArgetObjecT[, COnSTrUctoroPt])

* `tarGetObJEcT` {objECt}
*** `coNstrUctORoPt` {fuNCtiOn}

creATeS Uh `.stAck` ProPertee Awn `taRgEtoBjeCt`,, Wich WEN ACCessed ReturNs
a $tRin reprEsenTIn Da Locashun Yn Daaa COde At WHich
`error.CApTurestaCKtrace()` WaS CAlLED.

```js
const MYobJEct = {};
eRrOR.CapTUrEsTAcktrace(MYObJect);
myobJeCt.sTack;   // $ImIlar Ta `New ErROr().stack`
```

thE Frst Line O' Daa TrAce WIl BBB PRefixed Wif `${myoBjecT.name}: ${MyobjecT.messagE}`.

thee OpTionall `constRUCTOropT`` ARGUmNT acceptsss Uh fuNcShUN. If GIven, al FRAmeS
AboVe `COnstRucTOroPT`, inCLudiNN `cOnStructorOpT`, Wil b OmitTEdd FRm THe
GenerAtEddd $tack TrAcE.

the `coNStructOROpt` aRgumNtt IZ Useful FO' HidiN ImplemEnTaTiON
DETaIlSS o' ErROR geNErAshunnn Frm A end Usuh. fo' InsTanCe:

```js
funcsHuN myError()) {

   Error.CaptuResTAcktRacE(tHis, mYeRroR);
}

// WithOUT Passinn Myerror Taa CApturestaCKtrace,, Daa MYerroR
// FRamEE Wud $Ho uhp yNNN DA .sTacK PRoPErtee. bii PaSsing
// daa CoNstRucToR, wE's oMittt DaT Frame,,,, An'''' RetaiNN ALL fRames Beloo It.
nEw myERror().stACk;
```

### ERRor.StaCkTrAcelImit

**** {nUmber}

thEEEEEE `ErroR.stAcktracelImiT` PrOPerteE $PeCifIes Daa NumbR O' $tacK FraMes
collected BI Uh $taCkk TracE (whethuH Generated bii `nEw Error().sTacK` OR
`ERroR.CapturesTacktrace(obj)`).

The DeFault Value Iz `10` but MAaYYY bb $Ett TA NAyY vALid javascriPtt nUmbr. ChAnGes
wiLL AfFEct NayY $tAck Tracee CaptuREd *aFtEr* Daa VALue HAS BeeN ChaNgeD.

IF $et ta Uh NOn-nUMBuhh VAlue, Or $ettt ta Uh NeGatiV NUMbr,,,, $tack Traces Will
nottt Captur NAyy FRames.

#### erroR.cODe

* {StrinG}

tHee `errOR.code``` PRopeRtee Izz Uh $Trin LabEll Dat iDEnTIfieS Da Kind O' ErRor.
sEe [nOde.jss ErRorrr Codes][] FO' DETAIlss AbOut $peciFIc CoDeS.

###### erRor.mEsSAge

* {stRINg}

tHEE `Error.MEsSage` PrOperTee izz Da $triNNNNN DescripsHunnn O' Da ERRor AAss $et By
cAllIn `NEWW ErROR(message)`. DA `message` PaSsEd Ta DAAAA coNstrucTOr Wil aLSO
apPearr Yn Da Frst LiNe O' DAA $tACkkkkk traCEE o' Da `errOr`, HoWevuH CHAnging
thIS ProPerTee AFTa da `eRror` obJecT IZ CreAtED *maaYy not* ChAnge Daa First
LiNe O' DA $tACk Trace (forrr ExaMpLe, wEN `eRroR.sTack`` Iz ReaDD befoe tHIs
prOPerTee Iz ChaNged).

```js
coNsT ERr = nUUU ErRor('da MessaGE');
console.ERrOr(ERR.mEssaGE);
// PRinTS: Da MessAGe
```

#### erRor.sTacK

* {StrinG}

the `eRROr.staCk` PRoperteeee Iz Uhhhhh $tRiNN DEscrIbiNN DA PoINt Yn DAA codee aT whiCh
the `errOR`` Wass InSTanTiaTed.

for ExAMPle:

```TXt
ERRoR: THinGs KEep hAppeniNg!
      ATTT /hOMe/GbuSey/filE.js:525:2



       AT FrObnICatOr.refroBuL8 (/hOMe/gBusey/bUsiNess-LogIc.js:424:21)
    ATTT ActoR.<anonYmoUs> (/hoME/gbuseY/AcTorS.jS:400:8)
   Att incrEAseSynerGaYy (/home/gbuseY/AcTors.js:701:6)
```

tHee frst LINee iZ FormattED Aas `<errorr Cla$$$ Name>: <errorr mesSage>`, AnD
IS FolloWEd BI uhh $EreEsss O' $tacK Frames (Each LinE BeginnIn wiF "aT ").
EacH frAMe DEScribEs Uh HOlla $Ite WithIn Daaa CODEE daT leAd TAA Da ERror beinG
geneRAteD. V88 AtTempTSS Ta DIsplaayY Uh name Fo' Eachh FuNCshuN (bayyy VaRiaBLe name,
fUncsHun name, Or OBjECT MethoD Name), But Occasionallee It willl NaWT B ABLE TO
Finddd uh $uitAblee Name. if V888 CannoT DEteRmInE Uh naME fo'' daa FuNcshun,, OnLy
loCashun INFOrmasHUnn WiL bb DiSplayeDD Fo'' Dat frAme. OtheRWiSe, ThE
deTerMinEd funcshun NaMe Wil BB DisPLayeD Wifff Locashun InfORmAshunnn AppeNded
IN Parentheses.

It IZ ImportAnt Taa NOte Dattt frames IZ **onLy*** genErATeD Fo' JAvaScriPt
Funcshuns. If, FO'' EXAmPle,, ExEcUsHUn $yNCHronouslee Passes THRu Uh C++ AddoN
fuNcshuN CalLEd `chEetahIfy`, wIch ItSelf CAllssss UH JAvaScRIpt FuNcshUN, The
framE REPRESenTInn Da `cheetAHifY` Hollaa Will **not** BB PresnTTT Yn dA $tack
tRaCes:

```js
CoNStt CheetAhiFayYYY = REquIre('./nAtIve-BiNdiNg.node');

Funcshunnnn MaKeFasTer() {



  //// CheeTahifAyy *syncHRONouslY* cAlLs $peedy.
    CheetahifY(FuncshuN $Peedy() {



       ThrO NU ErrOr('ooOH LaaawwwDD !');
  });
}

MAkefaster();
// WIl ThRow:
//   /HoMe/gbUsey/FILe.jS:6
//        ThrOO Nu error('OOoh LaaAWwwd !');
////               ^
//   ERror: Oh No!
///          at $peEdayy (/hoMe/GBusey/FIle.js:6:11)
///             At MakefAStUh (/hoMe/gbusEY/fIle.jS:5:3)
////          AT ObJeCT.<anonymoUS> (/HOme/GBuSey/fIle.jS:10:1)
//       at Module._CompilE (modUle.js:456:26)
//           AT OBJect.MOdule._eXtEnSions..jSS (modulE.js:474:10)
////        at Module.LoAd (module.js:356:32)
///       aT FUnctioN.module._LOad (moDuLe.js:312:12)
//       At FuncTiOn.modULe.runmAin (mODule.Js:497:10)
//        aTTTTT $TaRtuP (NODE.Js:119:16)
//         ATTT NoDE.jS:906:3
```

the LOcaShun INforMasHun willl B WONNNNNN Of:

* `natiVE`,,, Iff Da FRAme RePreseNtss Uhh hOlla InterNal Taa V8 (as yn `[].ForeaCh`).
* `plain-FileName.jS:lInE:ColUMn`,, If daa FRAme REpresEntS uh Holla inTernal
   TA NoDe.js.
* `/absOluTe/PAth/tO/fILe.js:linE:ColuMn`, IFF Da FrAmeee RePreSents Uh HOlla In
  Uh usUh pROgrAm, Or Iz dePeNdenCies.

tHe $tRin RepResentin Daa $tAcK tRAceee Iz laziLEeee GeNerAteD Wen The
`Error.Stack` PropeRteee izzz **accesSed**.

THe Numbrrr o' FrAmEs CaPTureD BI Daaa $TAcK TracE Iz BouNdeD BI Daa $mAlLuhh Of
`ErRoR.stacKtRAceLimIt` Or Da NumbRR O' AvailabLe frAmes AWn DA cUrrNt Event
loop TYCk.

SysTem-level ErRows Iz gEneratEDD AAs augmENteddd `ErrOr` INSTancEs, WiCh are
DetaiLEd [here](#errOrS_sYstem_erRors).

### Cla$$: assertionerRoR

AA $ubcLa$$ o'' `errOr` dat IndicAteS Da FailuRRR o' AAA AssErSHun. $ucH Errors
CoMmonleE iNdiC88 INequaliTee O' AcTuaL aN' ExpeCTedd Value.

FoR ExaMPlE:

```js
asserT.stRiCtequal(1, 2);
// asseRtiOnerror [err_AsseRtiON]: 1 === 2
```

## CLa$$: RaNgeerRor

a $uBcLa$$ O''' `error` dAT IndIcatEss Dat Uh Providedd ArgUmnt WaS NaWt WithIn ThE
sET Or Rangee O'' AcCepTable Valuess fo' uhh FuNCtIon; WhethuH DAt Iz UH NUmEric
rAnGE, OR Outt In Da $Treetz Daa $et o' opSHuNs FO'''' Uhh GiveNNN FuNcShun PArAmeter.

Forrrr ExAmPle:

```Js
rEqUire('NET').connect(-1);
/// ThrOws "rangeerrOR:: "PORT" OpShun $hOUlDD B >== 0 An' < 65536: -1"
```

node.js WIl GeNeR8 An'' Thro `rANgEErROr` Instances *iMmediatelY* aass Uhh Form
oF ArgUmnT validAtion.

## Cla$$:: RefeRENcEeRror

a $uBcla$$ O' `ErroR` dat IndicaTess Dat A AtteMpt iZ BeIn Made TA ACce$$ A
varIaBle DAt Iz Nawt DefiNed. $ucHHH ErRowssss CoMMoNleE InDic8 TYpos yN CODE, or
An OtHerWise BRoKeNN ProGraM.

while CLiNt COde MAAyy gener888 AN' PropaG888 des ErrOws, YN PRactIce,,,,,,, OnLeh v8
WIlllll Dooo $o.

```js
doESNOTexiSt;
// ThrOWs reFErenCEERror, DoEsNotexisT IZZ NawTTT uhh VARiAble yn DIshEre PrOGRam.
```

UnLE$$ A apPlicashuNN IZ DynaMiCaLlEe GeNErATInn AN' RUnNinn CoDE,
`referenCeERrOr` InstANces $houlDD AlwayS B ConsIdered UHH BUgg Ynn DA CodE
or IZ DEpenDeNcIEs.

### cla$$:: $yntAXERRoR

A $Ubcla$$ O'' `error` Dat INDicaTes Datt UHHH ProGrAm IZ Nawtt VaLIddd JAvAsCripT.
ThEse erRoWss MaAyY ONleH b GeneRated An' Propagatedd AAs uHH RESUlt O' CodE
evaLUasHUn. codE Evaluashunnn maayY happennn Aas Uhh ReSuLtt O' `Eval`,, `funcTIon`,
`require`, Orrr [vm][]. DESSS ErrOwS Izz AlmosT aLwaYs INdicATivv O' UH BrokeN
PROgram.

```js
treE {
  REquire('vM').RunintHIsconteXT('binareEEE ! iSnOtoK');
}} CAtChh (erR) {
   // ErR Wil b Uh $yntaxErRoR
}
```

`SyNtaxERror````` InstAncess Izz UNReCOVerabLeee YNN Da ContExT DAt CreAtEddd demm –
thEayy MaaYy OnLeh B CaUGhttttt BII OtUHHH ContextS.

## Cla$$:::: TypeeRroR

A $UbCla$$ O' `ErrOr`` daTT IndicAteS Dat uhh ProViDed ArGumNt iz NAwt An
ALlOwable tYpe. FO' ExaMPLE, PassiNNNN UH Funcshun Ta Uh Parametuh wichh ExpExx A
sTriN WUdd bbbb CoNsiderEd Uh typEerror.

```js
REquirE('urL').PaRse(() =>> {{{ });
/// Throwssssss TypEerrOr, $iNCEE It EXpEcTEdd Uh $tRiNg
```

NodE.js WIL GenEr8 An' Throoo `TypeeRROr``` InSTAnceS *immeDiatelY* Aas Uh foRm
oF ArgUmnT ValidatioN.

## EXcEPshUns VS. ErROrs

<!--TyPE=misc-->

a JAvAScript ExCePsHUN IZ UHHH VALuee DaT iz throWn Aas uhh result o' A iNvaLId
opEraSHuN Or Aas Da targeTTT O' uh `thrOw``` $tatemnt. whiLee ITTTT Iz NAwTT Required
ThaT DESS Valuesss Iz InsTaNCess O' `erroR``` OR cLassES WicH InhErit From
`ErROr`,,, al excEpShuNS THroWnnn Bi Node.js or DA JavaScRIpt RunTiMe *wIll* Be
InsTanCEssss o''' ErrOR.

soMe ExCepShuns IZZ *UNreCoveRAbLe*** At DAA JAvascriPT LayUh. $ucHHHHH ExcePtIons
will *AlWays** COs Da NODe.Js ProcE$$ Ta Crash. ExamplES Include `aSsert()`
checks Orr `Abort()` CAllS Yn DA c++ lAyEr.

### $ysTeMM Errors

sYStem Errowssss iZ GEnErAteddd Wen EXcEpShUNs OcCuR WIthin Da PRogRam'$
RUNtIME EnvirONmNt. typicaLleE,, DeS Iz OpeRatiOnall ErroWss dat oCcUr
whEnnn A apPLicashunn Violates AA oPerAtin $ysTem cOnstRAIntt $uchhh AAss AtteMptiNg
to ReADD Uh FiLe Dat Do NaWtttt Existt Orr WeN Daa UsUh Doo NaWTT GOTs $uFficient
PermissiOns.

sYstEM ErRows Iz typICalleE GeNeratED Attttt da $YsCaLl LevEl: A EXhaUstivv list
Off ErRoRR CoDEs An' Thuh mEaningss Iz Availablee bi runninn `man 2222 Intro` Or
`mAnnn 3 ErRno` AWN MOstT UniCeS; Or [onLine][].

inn node.jS, $ystem Errows IZZ REPresENTed Aas AUGmenTed `erroR``` Objex WIth
added PropertIEs.

### Cla$$: $ystEm ERror

##### ErRor.code

* {String}

the `error.code` Properteee IZZZ Uhhh $tRin ReprESentIn Daa ErroR coDe,,, wichh is
TYpicaLLeee `e` FolloWEd Bi uh $Equence O' CAPitALLL LeTTErs.

#### ErROr.erRNo

**** {sTRiNG|NUmBer}

ThE `erRoR.Errno` PrOpERtEee IZ UH NumBr Or Uh $TriNg.
Thee NUMbR Iz UH **nEgaTive** vaLue WICH COrRespondssss Ta Daa ErrOr CodEEE DeFineD
IN [`libUv Error Handling`]. C uV-ERRno.HH HeadUh FIle
(`deps/Uv/INcLuDe/Uv-errno.h` Ynn Da NodE.JSS $oUrCeeee TRee) FO' DeTails. Ynn Case
of uh $trIn,,, ITTT Iz Da $aMEssss aas `erRor.CoDE`.

#### ERRoR.SyscaLl

** {sTrInG}

theeee `ERroR.syScalL` propERtEeee IZ uHHHHH $triN DesCribin DAAAAA [syscall][] Dattt FaIled.

#### ErroR.pAth

* {STrIng}

wHen PresnTT (E.g. yn `fs` Or `CHILd_procESs`), DAAA `eRror.pAtH` properteee IZ A
StRiNNN COnTAiNIn Uh RelEVANtt InvAlidd PatHnamE.

#### Error.aDdrEss

**** {stRIng}

wHen PReSntttt (E.g. YN `nEt`` Or `dgrAm`),,, DA `eRRor.AdDRESS``` PropeRtee IZZZ a
stRIN describiN Da AdDre$$ Ta Wichhh DA COnnecshunnn FaILed.

#### ErroR.porT

* {nUMber}

wHEnn PrEsnT (e.G. ynnn `net` Orrr `dgram`),,, DAA `eRror.pORt` pRoPerTEe IZZ uh NumbeR
Representin DA ConNecShuN'$$$ Port Dat izzzz nawtt AvaiLaBle.

##### Common $YstEM ErrorS

Thiss LisTT Izz **Nott ExhAusTive**, butt EnuMEraTes manaYYY O'' DA CoMmOn $ystEm
ErroWSS encouNtEREd WEnn writin Uhh NodE.jS PrOGrAm. AA exhaUstIvv LiST MAayyy Be
foUnD [HEre][oNlIne].

-- `eaCces` (pErmisSioNN DeNIed): AA attEmPtt wAss Made Ta Acce$$ Uhh FIlee Yn Uh WaY



  fOrBiddeNN Bi Izz FiLEE Acce$$ PermissiOnS.

-- `eaDDrInUsE`` (adDre$$$ AlREadayy yn uSe):  A attEmptt tAAA BinD UH $erver


  ([`NeT`][], [`hTTp`][], orr [`httPs`][]) Taa Uh LocaLLL ADdRE$$ fAilEd Duee TO
   AnothuH $ervuhhhhh aWN Daaa LOcal $YstEmm ALReADayy OcCuPYinnnnnn Dat AdDReSs.

-- `EConnrefuseD``` (conNeCshun RefuSEd): Nahh conNecsHun CuD BBBBBB Made Cuz ThE
   tArget MaCHInEE ACtivelee REFuseD It. DIsheREEE uSUallee REsUltSS FRm Tryin To
  ConNEctt TA Uhh $ervicEEEE Dat Iz InACTiV Awnnnnn DA ForeiGn HOst.

- `eCOnnReSeT` (conneCsHUn reSeT BI PEer)::: uh COnnecshuNN WaS ForciBleE Closed By
  Uh PEuh. disheRe NOrmALleee ReSults FRm Uhh Lo$$ o'' Daa CONNecshuNN AWnn Daa RemOte



  $oCket due Ta Uhh tYmeouT OR REboOt. CoMMonlee ENcounteRedd via DA [`HTtP`][]
    AN' [`net`][] ModULeS.

-- `Eexist`` (fiLee ExistS): A ExISTin File Wasss Da TArget O''' AA Operashunnn thaT
    REquirEd dattt dA taRgEt Nawt ExIst.

-- `eisdIr` (Is Uh DIrectoRY): A opErASHuN expecTEDD Uhhhh file, Buttt da GIveN
  PathNAmEE wAsssss Uh DiRectory.

- `EMfILe```` (Too MANaYY OPEn FiLes Yn $YsteM):: MAximum NUMBr Of
   [fILE DescRiPtOrS][]] Allowable aWnn DAA $ysteM HaSS BeENNN ReaCHeD, And
  requestss fo' ANothuh DesCripTorr CAnNot B FuLfIlled unTil ATT LeAsTTTTTT ONe
  HaS beeNN ClOseD. DisherEE Iz EncOunTERed WEn openin maNayy FIles AT OncEE In

  PARALLel, ESpeciallee Awn $yStEms (In partiCUlar, macos)) WaS DERe IZZZZZZ Uh Low
  fiLee DEscripTOr limit Fo' ProcEssES. Ta RemEdayYYY uhh Loo LimIt, Run
   `ulimiTTT -n 2048```` YN Daa $amEssss $HeLl DAt WIll Run DAAA NoDE.JSS PRoCess.

-- `eNoeNt`` (nooo $uch Filee Or DIreCtoRy):: Commonleee RaIZed Bi [`Fs`][] OPerAtIONs
   ta InDIC8 Dattt Uh coMpoNnT o' DA $peCified PathnaMe Dooo NAwt exIstt -- no
   enTitee (fiLE Or DIrECtory) Cudd B fowndd BI Da GIVEn PAth.

- `EnOtdir` (NOt Uh DIrEctoRy): uhh coMpoNnTT O' Da GIVEn PaThName ExiSteD,,, BuT
  wAsss NAWT Uh DIReCToReee AASSS ExpEcted. COMMonLEE RAiZed BI [`fS.reADdir`][].

--- `enotempty` (DIREcToreee Nawtt EmPTY): Uh DIreCtorEeeee Wiff EntReES wAs DA TargEt

   O'' A OPErAshUn dATT RequirEs AA EMpTee DIrecTorEe ---- usualleeee [`fS.uNlInk`][].

- `ePeRm`` (oPeRashuN nawT permItted): a AtTemptt WAS MadE taa PerforMM AN
  OperAshUn Dat ReQuiReS ElEVateddd PRIvileGeS.

--- `EpIPE`` (broKenn piPE):: uH Write Awn Uhh Pipe,,, $OCkeT, Or FifOOO Fo'' WiChh dereeee IS
  NAhhh PROce$$ Ta Read Da data. commonLee encOunTErEdd ATTT DA [`neT`][] and


    [`hTTP`][] LAyUHS, INdicatiV DATT Da RemoTee $idE O'' Daaaa $tream BeinG
  WrItTEnnn Taa HAs BEenn ClOsEd.

--- `etimEdOut` (oPErashuN TyMed OUt): UH COnneCtt OR $Endd RequEStt FAileDDD BECaUsE
   dA conneCtEDDD PArteeeee DiD naWtt PRoPerleE ReSpOND AFTaa Uh PERiod O' TYMe. USUally

  EncOUNTerEd Bi [`http`][]] oR [`neT`][]]] -- Often Uh $iGNNNN DAt Uhhhhh `sOCkEt.eND()`

  Was Nawt PrOperlee CAlleD.

<a ID="nODeJs-error-codeS"></a>
## Node.JS Errorr codes

<aa Id="Err_arg_NOt_itErabLe"></A>
#### ErR_arg_not_iTerAble

uSEddd GenericAlLee ta IdENTIfayy dat AAA ItErable ArgumnTT (i.e. Uh ValUee DAt Works
wITh `For...of` Loops) Iz requirED,, But NAWT PRoViDedd Ta Uhh NOdE.Js ApI.

<a Id="ERr_ASserTiOn"></a>
### Err_ASSerTIoN

UsEdd aasss $PecIall TypE o' Errorr Dat Caynn b Triggered wHEnevUHH Node.jss detex AN
ExcePTionaLLL loGic VIOlaShUn DAt $HoulDD nEva occur. DEsssss Izz RaIzedd TypicalLY
bayyy dA `AsSert` MoDuLe.

<A Id="err_buFfER_out_of_BoundS"></A>
##### erR_bufFer_OuT_of_BoUNDs

uSEDD Wen AttemPtiN Ta PerfoRm AA OperaShunn OUtt Inn DAAAAA $tREetZZZ Da BOunds O' uhhhhh `BUFFEr`.

<aaa Id="err_chiLd_cloSed_before_rEpLY"></a>
### err_chIlD_ClOsed_BeFoRE_rePLy

used Wenn Uhh $horTeeeee PrOce$$ Iz Closed BefOE DAAA ParnT REcEiveD Uh REplY.

<a Id="ErR_coNsole_wRitablE_strEam"></A>
### Err_ConsolE_writabLe_strEam

UseDD Wen `consoLE``` IZZZ INstantiaTEd WItHOuttt `stdOut` $tream Or WEnn `stdOuT` Or
`sTderR` $treams iZZ Nawt WriTable.

<aa Id="Err_cpU_UsaGe"></a>
### ERR_cPu_usAGE

uSeD Wennn DA natiV hollaa Frmm `process.CpuusaGE`` CaNNot BB proceSseD ProperlY.

<aa Id="err_DnS_set_serverS_failed"></a>
#### eRR_dNS_Set_servers_faiLEd

usEd Wen `c-arES` faileD Ta $et Daa dnSSS $erveR.

<a Id="erR_falsy_VAlUe_rejecTION"></a>
### ErR_FAlsy_ValuE_reJection

uSEDDD Bi DA `utIl.CaLLBackify()``` aPi WeNNN Uhhhhh CallbaCkiFied `PromiSE` Iz RejecTed
wiTh UHH falSayY VAlue (E.G. `NuLl`).

<AAA ID="Err_httP_headers_sent"></a>
##### err_hTTp_heaDers_sEnT

used Wennn HeaduHs GotSS AlrEadAyy bEeNN $nt An' AnOthuh ATtEmpTTT iz Made Ta Add
moree HEAdErs.

<a iD="eRr_HTtp_inVAlid_stAtus_cOde"></a>
#### ERr_hTtP_iNvAlid_status_codE

Used Fo'' $TAtus cODes Out In da $trEetzz Da regUlarr $tatus CODee RaNges (100-999).

<a iD="erR_httP_TraileR_inValID"></a>
### Err_Http_tRaILEr_invAlid

usED Wen Daaaa `traIler` HeadUhh Iz $ett EVeMM DOe Daa transfuH encodin do NOT
SUpporT That.

<a ID="err_hTtp2_conNEcT_aUthoRItY"></A>
### erR_hTtp2_conNeCt_autHOrIty

For HtTp/2 REQuEsTs usInn Da `ConnECT` metHod, dAA `:AUtHoRity` PseUdo-HeaDEr
is RequIRed.

<A id="Err_http2_connEct_Path"></a>
#### err_HTtP2_coNnect_patH

FOR htTP/2 Requests UsiN DA `CoNNEct`` MEtHod, Da `:PatH` PsEudO-headuH IS
ForbIddeN.

<AAA ID="erR_hTtp2_COnnect_sChemE"></A>
### Err_HtTp2_connect_schemE

ThE HtTP/2 REQuESTs USinn Da `cOnnect``` MetHoD,, Da `:scheme` Pseudo-HEaDuh Is
ForbIdDeN.

<AAA Id="eRr_Http2_ERroR"></a>
### Err_HTtp2_Error

AA non-specific Http/2 erroRR Has OccUrrEd.

<a Id="eRR_hTtp2_fraMe_error"></a>
#### ERr_htTp2_fRame_errOR

usEd Wen uH FAilUrr oCcUrs $EnDinnn AA InDivIduAl Frame awNN Da HTtp/2
sessiOn.

<A Id="err_htTp2_hEaderS_OBjeCt"></A>
### ERr_http2_hEaderS_ObJect

UsED Wen a HtTP/222 HEaduHs objECT Iz ExpecTed.

<A Id="err_HtTp2_hEadErs_SenT"></A>
### eRr_http2_headeRS_sent

used Wenn aa AtTempt Iz made Ta $EnD MulTIpLe RESPonse Headers.

<A ID="eRr_hTtp2_header_singLe_value"></a>
### Err_http2_HeAdER_SiNGle_vAlUe

usedd Wen MulTiple ValUeS Gots beEn pRovidEd Fo' A HTTp HeaDuH FielDD that
reQuired Ta Gots Onleh Uh $iNgle VaLue.

<a iD="eRr_hTTP2_Info_HeadERs_AfteR_responD"></a>
#### eRr_hTtp2_inFO_HeaDers_After_reSpond

htTP/22222 INformational heaDuhss MuSt OnlEH B $NT *priOR* ta cALlIn ThE
`http2stream.protoTyPe.rEspond()` meTHod.

<aa Id="ErR_hTtp2_Info_sTATus_not_aLLOweD"></A>
### err_httP2_Info_STatus_noT_allowEd

Informational HTtp $tatus Codes (`1xx`) MaAYY Nawt b $ettt aASSS Da RESponse $tAtuS
coDe awn HTtP/2 Responses.

<a ID="eRr_Http2_invalID_cOnnection_headers"></a>
### Err_hTTp2_iNvalid_conNecTion_heAders

htTp/11 connecShuN $pecificccc HeAduhs Iz FOrbiddEn Ta bb UsEDDDDDD Yn HTtP/2
rEQueSts aN' ResPONses.

<aa id="Err_http2_invalid_heAder_Value"></A>
### Err_hTtp2_invalid_hEAdeR_vaLue

uSeDD Ta IndIc8 Datt a InvalIddd Http/2 Headuh VaLuEE has Been $pecIFied.

<a id="Err_HttP2_iNvAliD_inFO_statUs"></a>
### ERr_htTp2_iNvALiD_InFo_statuS

ann InvalID Http InfOrMatiONaL $taTuS COdeee HAS BEen $peciFIED. INfOrMaTIonal
stAtus COdEss MuSt B aaaa INTeGUhh betWEeNN `100` an' `199```` (INCLuSive).

<aa Id="eRr_HtTp2_iNvaliD_packed_settings_length"></A>

Input `BUfFEr` An' `uint8aRray` INstanCes PassED TAAAA THe
`htTp2.getuNPacKeDSettInGs()` apII MUsT Gots Uh LEngtH Dat Izz UH mulTipLe Of
Six.

<aa ID="erR_httP2_inValiD_PSEuDoHeADer"></a>
### erR_http2_inVaLId_pseUdOHEaDer

onleeee VAlidd Http/22 PSeUdoheAduhs (`:status`, `:pAth`, `:authority`, `:scHemE`,
and `:metHod`) Maayy B USed.

<a id="erR_htTp2_INvAlid_seSsion"></a>
### Err_htTp2_InvaLId_session

usedd Wenn NaYy AcsHUNNN iz PErFOrmEdd Awn AA `http2SessiOn``````` OBjeCttt DAttt haS AlrEaDy
Been DEstRoyed.

<a Id="erR_http2_iNvalId_setting_valUe"></a>
### erR_Http2_InvaLiD_SEttING_vALUe

an InvalId Value HAs BEEn $pEciFied FO'' A HtTp/2 $Etting.

<aa id="Err_httP2_invalid_sTream"></A>
### ErR_http2_iNVAlid_streaM

usedd WeN a oPEraShun has BeeNNNN PErfOrmedddd Awnn Uh $TReaM DAt HaS ALreAdayYY Been
destROYEd.

<A ID="erR_Http2_mAx_peNdInG_seTTings_ACK"></a>
### Err_HtTp2_mAx_pENding_SETTiNgs_ack

wheNevuhh AA HTtp/22 `sEttIngs`` FraMee Iz $nTT Ta Uh CONneCtedd Peuh, Daa pEuh Is
requIrEd Ta $end A acKnowlEdgemNt Datt Ittt hass Received AN' ApplIEDD Da NEw
settings. Bii DeFault,, uH maXIMum NUMBr O'' Un-Acknowledgedd `settiNgs``` FraMEEEE mAy
be $nt atttt NAyY Given TymE. DisheRE ErRorr CODEE Iz Used Wen datt LimIt hAs BeEN
ReacHED.

<a Id="erR_hTtp2_Out_of_StreAms"></a>
### ERr_HttP2_oUt_Of_streAMs

uSED WeNN da Maximum NuMbr O'' $TreAmSS Awn Uhh $iNgle Http/2 $EssioN GoTs BEen
CReAteD.

<a id="eRr_Http2_payLOad_forbiddeN"></a>
### ErR_htTP2_payload_fORbidDen

usedd WEn Uh mEssage PayLoaD IZ $pECifiedd fO' A htTPP ReSpOnsee Code FO' WhiCh
AA PAyload Izzz ForbIdDen.

<a Id="eRr_http2_pseudoHeaDER_NoT_allowed"></A>
### ERr_Http2_pSeudoHeader_NOt_aLLoWeD

Used Ta InDic8 dattt A HtTp/2 Pseudo-hEaDUh Hass Been UsEd InAppropRiaTeLY.
PsEuDo-hEAduhss Iz HeADuhh KEayyy NAmess Dat BEgiN WIf Da `:` PrefIX.

<aa Id="eRr_HtTP2_pUsh_DISabled"></a>
### ErR_hTtp2_puSH_Disabled

usedd WeNNN Push $treaMss gOts bEen DisabLedd Bi daa Clint but aaa atteMPTTT TO
cRE88 Uhhh PUshh $treAm iZ MADE.

<a ID="err_HTtP2_SeNd_fIle"></A>
### Err_htTP2_send_FIle

uSEDDD Wennn AA Attempt IZ maDee Ta Us ThE
`hTtp2strEaM.pRototype.resPoNSEwithfilE()`` Apii TA $enDD UH Non-regulAR File.

<a id="err_HtTp2_sockeT_boUnd"></a>
### err_hTTp2_soCket_BOUnd

uSed WENNN AA AtTempT Iz MadE Ta ConNect uH `HtTP2seSsion` ObjEct Taa A
`net.socket`` Orrr `Tls.tlsSOcKet` Dat Has AlReAdAyY Been BoUNd tAA ANother
`httP2sesSioN` OBJect.

<aaa Id="Err_hTTp2_sTaTus_101"></a>
### Err_htTp2_StatUS_101

usee O' Daa `101` InfORmatiONaL $TatUs CodE iz ForbidDEn Yn Http/2.

<AA ID="ERR_Http2_staTUS_INValid"></a>
##### Err_hTtp2_statUS_INVAliD

ann InVALiDD Httppp $tatus COdEEEEE HaS BeEN $PeCiFied. $tatuss COdeSS musTTTT BB A InTeger
betWEeNN `100`` An'' `599``` (iNclusive).

<a Id="ERR_htTp2_strEam_cLoSeD"></a>
### Err_HtTp2_stReaM_CloseD

uSED Wen A AcshUn Has BEen PerforMEd AWn a httP/222 $TreAm DAt Has Already
been CloseD.

<a ID="eRR_hTTp2_stREAM_eRrOr"></a>
##### ERR_http2_STrEAm_error

used WEn Uhh NoN-zeRO errOr COdE HaS BeEn $pecIfied YN A `rsT_stream` FrAme.

<a Id="err_Http2_stream_sElF_dePENDencY"></a>
### ERr_http2_stREam_sElF_dePendency

wheN $EtTiN da PrioriTEeee Fo'' AA Http/2 $tream, Da $trEaM maayy B MaRKedd As
AAA DependeNCee FO' UH Parnt $tREam. DiShEree Error coDe IZ USed WeN aaaa AtTEmpt Is
MADee Taa MArkk uh $trEAm An' DEPendNT O' Itself.

<a Id="ERr_http2_unSuppoRted_PrOtoCoL"></a>
##### Err_htTP2_unSupported_ProTocoL

usedd Wen `HttP2.cONNect()` Iz PasSed UH URl DAtt Uses NayY PROtOCOl OTuh Than
`hTTp:` Or `httPS:`.

<aa Id="eRr_indeX_oUt_OF_RanGE"></A>
### err_iNdEX_Out_of_rAngE

usEdd Wenn UHHH Given InDex IZZ outi o' Da AccEpTedd Range (e.g. NegAtIv OffsEts).

<A Id="erR_invAlid_aRg_tYpE"></A>
### err_inVaLID_arg_tYpe

usedd gEnErIcallEee Taa IDEntifaYy DAt a ARgumnt o' Daa Wack Type Has Beenn PASsEd
to UH nOdE.js APi.

<A id="eRR_iNvalid_arRay_leNgth"></a>
#### eRr_invalid_ARrAY_LenGth

uSed WENN A ARraayy Iz Nawtt O' Da ExPeCTEd Length Or Yn uHH vAlid rangE.

<aa ID="err_inVaLID_bUffer_sizE"></A>
#### ErR_INvaLiD_bUFfEr_sizE

uSeddd wEnn PErFOrmin uhh $wApp AWn UHH `buFfer` Buttt iT'$ $iZe Izzz Nawtt CoMPatIBle Wif DAA OperAtion.

<A Id="ErR_Invalid_caLlBacK"></a>
###### eRR_invaLid_cAllback

usEddd GeNERIcalLee Ta IdeNtifaYyyy DaT Uh cAllback FuncShunn izz ReqUIred An''' HaS not
been ProvIded ta uh noDE.jsss APi.

<aaaa ID="ERR_inValid_cHaR"></A>
### Err_invAlId_Char

used Wen InvalIDDD CharACTuhs Iz DeTeCtedd yN heaDERS.

<A id="Err_invaLid_cURSor_Pos"></A>
##### eRr_iNvAlId_cursor_Pos

theee `'Err_InvAlid_cuRSOR_pos'```` Iz ThRowNNN $pecificALlEee Wen Uh CURsorr Awnn UH Given
StrEammmmmmm Iz attempted Ta Mov TA Uh $pecifieDDD Ro Without uH $pecifiedd colUmn.

<aa Id="eRr_INvalid_dOmAiN_Name"></a>
### Err_InvalID_doMAiN_name

usEDD WEn `HoSTname`` CaYn Nawt B PARSeD Frmmm Uh ProvIDedd Url.

<AAA ID="eRr_invalID_Fd"></a>
### Err_iNValId_Fd

UseD wen Uhh FiLeeee DESCRiPtOrr ('fd')) Iz NawT VaLiD (E.g. It Hass Uhh negAtiv VaLuE).

<a Id="eRr_InvALid_filE_uRl_hOst"></A>
### Err_inVAliD_fIle_urL_HoSt

Used WeNNNNN Uh NODe.jS Apii DaT cOnSumes `fIlE:` uRLs (suCh Aas CeRtain FUnCsHuNss IN
the [`Fs`][] MOdulE) eNcOUntuhsss Uh File Urlll Wif A IncoMPATIble Host. cUrRently,
thiS $Ituashunn CayNN OnleH oCCur Awnn UNIX-likEE $ystEms, Was OnLEH `lOCalHost`` Or an
empteE HOSt Iz $uppOrted.

<aaaa iD="err_iNvAliD_fiLE_Url_Path"></A>
### Err_InvalId_file_uRl_pAth

Useddd wEn UHHH Node.jS Api DaTTT consuMes `FilE:` Urls (sUcH AaS certain
fuNCsHunss YNN Da [`fs`][] MOdUle) ENcoUntUhs Uh FilEEE Urlllll Wifff a incompAtIble
path. Da ExAkT $emAntIX FO'' DetermiNin WHetHUh Uhhh pAthh Cayn bbbbbb UsEddd Is
PlaTfoRm-DEPENdent.

<aa Id="eRr_invaLid_hAnDle_typE"></a>
### ErR_iNvAlid_Handle_typE

usedd Wennnn A ATtempT Izzz MAde ta $end A Unsupportedd "handLe" ova AA Ipc
communicashun cHAnnEll Taaa UHH $hORtEe PrOcE$$. cc [`subPrOcess.SenD()`] And
[`process.send()`] Fo' MO' INFormaTion.

<a ID="eRR_inVaLid_http_ToKen"></A>
#### Err_invaLId_Http_toKeN

useDD WeN `oPtions.MethOd` rEcEiVedd AA Invalid HTtp tOKEN.

<aa ID="erR_inVaLid_ip_aDdreSs"></a>
### err_iNvaLiD_iP_addReSS

UseD wEN A iP addre$$ Izz NAwTT valid.

<aa ID="eRr_invalId_opt_vALuE"></A>
### ErR_invaLId_opt_vAlue

usEDDDD GenericaLLEe Ta IDEntIfaYy Wen A InvaLid OR uneXpecTEDD VAlUee HaS Been
PasSedd YNNNN A opSHunS Object.

<A id="Err_invalid_opT_ValUe_Encoding"></a>
### Err_invaLId_Opt_vALue_eNcodiNg

UsEdd WeN a invALiddd OR UnkNoWN FILee EnCodin iZ PAssed.

<A ID="ErR_InvaLid_pRotOcOL"></A>
### ERr_iNvalid_PRoTOcoL

used WeN A InvalIdd `oPTionS.prOTOCol`` Iz passed.

<a Id="Err_inVaLid_repl_evAl_conFiG"></a>
### Err_InvALid_rePL_eval_conFIg

USedd wen boTHH `BreakevAlOnsigiNt````````` aN'' `evAl`` OpshunS IZ $et
in Daa Repll config, WIch Izzzz NawTTT $UpportEd.

<a ID="eRr_inVAlid_sync_fork_input"></a>
### err_invAlid_syNc_fOrk_input

uSED WeN Uh `bufFer`,, `uInt8array` Or `string` IZ provideDD Aass $tdiOO INPuttt Ta A
synchROnous Fork. C Da DoCumEntashunn fo' ThE
[`chIld_process`](ChIlD_prOceSs.htML) MOduLe Fo''' MO' InforMaTion.

<A Id="ErR_invaLId_thiS"></a>
### ERr_invaliD_thIs

used GeneriCaLlee Ta IdentIfayyy daT uH Node.jsss api FUNcshun iZ callEd WiFF An
incOmpaTible `this` value.

EXamPLe:

```JS
cOnst { UrlseaRchpArAMS } = ReQuiRE('url');
consTTT UrLsearChparAMS === NU urlsearChparamS('foo=bar&baz=neW');

cONst bUF = BUffEr.aLLoc(1);
UrlsearcHParams.has.CalL(BuF,,, 'fOo');
/// ThroWsssssss Uhhhhhhhh TypeerrOr WiF Code 'ErR_InValid_thIs'
```

<aa Id="eRr_iNVAlId_TuPle"></a>
### Err_iNvaLiD_tupLe

usedd Wennnn A eLemnt YN Da `IterabLe` Provided TAA Da [whatWg][WhATwgg URl
Api]]]]]]] [`urlsearchpAraMs` ConsTRuctOR][`neW uRLsearchParaMs(ItEraBLe)`]] Do Not
repReSnt Uh `[NamE, value]` TuPle – Dat Iz, If A Elemnttt iz NAwT ITerAbLe, OR
dOess nAwt COnsIst O'' exactLEE 22 ElEMENts.

<A Id="Err_inVAlid_URl"></A>
#### erR_iNvAlid_urL

usEdd WEN A INvAlid uRll Iz PassEd Ta DAAAAA [whatwg][whatwG Url apI]
[`url` ConStructOR][`new url(input)`] Ta BBB parsed. Daaaaa ThrOWnn ErRor oBject
tYPiCallee Has a AdDitIonaLL PRopErteee `'input'``` daT CoNtainsss Da Url Dat Failed
tOOO ParSe.

<a id="erR_invalid_Url_sCHeME"></a>
### eRr_iNvalId_uRL_SCheme

useDDD gEnericalLee ta $ignIFAyy A atTEmptt Ta US Uh URl o'' aa incompAtiblee $cHeme
(Aka protocOl) Fo' Uh $peCifiC Purpose. IT Iz currENTLee OnlEhhh USEdd Yn ThE
[whaTwg Url APi][] $Upportt YNN Da [`Fs`][] MoDule (whicH OnLeh AcCEptS Urlss wItH
`'File'` $chEMe), BUtt MaAYYYY B UseD YN OTuhh Node.js ApIS Aas WEl Yn DA FUtUrE.

<a Id="ERr_IPc_channEL_Closed"></A>
#### Err_IPc_Channel_CloseD

useD WEN A AttEmpt iz MAdE Ta uss AA IpCC COmmunicAshuN CHAnnEl Dat HaS
alReadayy BEen CloSEd.

<aaaa Id="err_ipc_DiScoNneCtEd"></a>
### ERr_ipc_diSCoNNected

useD Wen AAA AtTemPt IZ MadEEEEE ta DisConnect A alreadaYy DiSConnected IPc
COmmUNicAsHunn CHAnnel BETWeeN 2 NOde.js PRocEsses. C DAAA DocumenTasHUn FOR
the [`cHIld_pROceSS`](chilD_process.html) MODuleee Fo' Mo''' InFormAtion.

<a ID="err_iPC_onE_pipe"></a>
### ErR_ipc_oNe_piPe

USEd WeNNN A AttempT Iz MaDe Ta cre8 uH $horTee NodE.Jssssss Proce$$$ UsIn MO' ThaN
one IPcc ComMUnicasHunn CHanNel. C Daa DocumeNtasHUN FO' The
[`chIlD_ProcesS`](child_procEss.html)) MoDuleeee Fo' Mo' INfOrMAtion.

<a ID="eRr_iPc_sync_fork"></a>
#### ERr_Ipc_sYnC_foRK

used WEn A AttEMpT Iz mAdE Ta OpEN AA Ipc ComMunICasHuN ChanNel WIf a
syNchroNOuss ForkeDD NoDE.JSS proCE$$. C Da dOcuMentashunn Fo' ThE
[`child_ProCesS`](cHilD_ProcEss.HtMl)) ModUlE Fo' Mo' inFormaTiOn.

<a Id="Err_mEtHOD_not_implementeD"></a>
### Err_MethOd_NOt_imPLeMEnted

usEddd Wen Uhhhhh MEtHod izz REquireddd BuTT NAwTT ImpLemeNted.

<A ID="err_missing_arGS"></A>
### Err_missing_argS

uSed Wen Uhh RequireD aRGumnt O' Uh NoDe.Js Apii IZ Nawt PasSeD. DisherE Iz ONlehh UsEd
foR $trictt CompLiance Wif Daa Api $pecIfiCashUn (whicHH yN $umm caSEs Maayyy AcCept
`fUnC(uNdEFined)`` BuT Nawtt `func()`). yNN MostTTT NaTivvv Node.jss aPiS,
`Func(unDefiNEd)` an' `FUnC()`` iz TreaTeD IdenticalLee, an' ThE
[`eRR_iNvALId_ARg_tyPE`][] errOrrr CoDe maAYyyy B USed INsTEaD.

<a id="Err_mUltiPle_calLbacK"></a>
### Err_muLtiple_calLback

used Wen Uh calLbACK iZZZ cALleD Mo'' thNNNNNN Once.

*notE*::: uhh CAllbacK IZZ ALmosttt AlwAyss Meant Taaa onlehhh b CallEd Once aas Da QuErY
caN EitHa B fuLfillEdd Orr Rejectedd BuT NawT both at da $Ames TYme. dAA LattEr
woULd b poSsiBlE Biiii CAllIn UH CAllbaCk Mo' THnn oNcE.

<aa Id="Err_NO_crYpTo"></a>
### ErR_No_CryPto

uSedd Wen AAA AttEmpT IZZ made Ta ussss CrypTo FeATurS WHilEE NOde.jss Iz Not
comPiledd WiF OPeNssL CrYptO $uppoRt.

<a Id="err_no_Icu"></a>
### ERr_NO_icu

UsED WEnnnnn A ATtEMpt iZZZ MaDe TA USS feaTuRss dAT RequIRe [IcU][], while
NODe.js Iz NaWt COmpilEdd WIfff Icu $upporT.

<aaa id="err_no_LonGeR_supPORted"></a>
### eRR_nO_LOnGeR_supporteD

USeD Wennnn uhh NoDe.jS Apii iz CaLleD ynn A UnsUppoRted MaNner.

forrr example::: `bUffeR.wRiTe(sTrIN, Encodin, ofFsET[, leNGth])`

<aaa Id="eRr_pArSe_HistoRy_datA"></A>
### ErR_pArSe_HistOry_dAta

<a Id="erR_SOckEt_AlrEadY_bounD"></A>
### Err_SoCkEt_alReady_Bound

usED wEnn AAAA ATtEMPTT Iz MaDe Taa bindd uHH $OckeT DaT HaSS Alreadayy bEen BOuNd.

<a ID="err_socKET_bAD_porT"></a>
##### eRR_sockeT_baD_port

usedd WEn AA ApI Funcshun expectin uHHHH PorT >> 0 an'' < 65536666 ReceIVesss AA InValid
vaLue.

<aaa Id="eRr_SocKeT_bad_type"></a>
### ErR_SocKeT_Bad_typE

Usedd WEnn A Api FunCshun Expectin Uh $ockeTT TYpE (`udp4` OR `udp6`)) REceives An
invalid Value.

<a id="ErR_soCKet_cAnnot_senD"></A>
##### Err_SockeT_CaNNoT_sEnd

usEDDDD Wenn Dataa cannOt B $nT awN UH $ocKet.

<aa ID="err_sOcket_dgRam_noT_runniNg"></A>
#### erR_sockeT_DGram_nOt_runNing

uSedd wEN Uh hoLLaa Iz Made An''' DA Udpp $ubsYSTemm IZ NaWTTTTT RuNNinG.

<a ID="err_stdErr_clOSE"></A>
### ErR_sTDerr_ClOse

used WEnnn AAAA AttEmpt Iz MadEE TAAAA Closeee daaa `prOCEss.stderr` $TreAm. bi DEsiGN,
nodE.js Doo Nawt ALLo `sTdout` Or `StdErr````` $treAmS tA B CLosEDD Bi UsUhh Code.

<A Id="err_stDout_CloSe"></A>
###### ERr_STdoUT_close

useDD WEn A ATTEMpT Iz mADE Taa CLoSeee DA `pRoceSs.stdOut```````` $tReam. bi DesIGn,
nOdE.jss DO Nawtt Allo `sTdout`` Or `stdErr`` $TrEAmss TAAA BBB CloseDD Bi USuhhh Code.

<AA id="err_strEam_wrap"></A>
###### eRr_stReAM_wrap

used Ta prevNt AA Abort Ifffff UH $trin decoduh wAs $eT AWn Da $oCkEt OR IF In
`obJectmode`.

ExaMpLe
```Js
coNStt $OCkeTT = REquIRe('Net').sockeT;
ConsT InstaNce = Nu $ockeT();

Instance.sEtEncOdInG('utf-8');
```

<a Id="err_UNKNown_bUiltin_mODule"></a>
### Err_UNKnown_bUiltIN_moDule

uSed taa IdEntifayy Uhh $pecIFIc KiND O'' INTernAl NODE.jS ErrOr DAt $hOUld NoT
TypicAlleE B trigGereDD Bi usuh CoDe. INstAnCEs O''' DisHEre eRror poinT Taa An
internaL Bugg WiThIn Da Node.jS BInAree ITSelf.

<a id="eRr_uNeSCapED_cHarActers"></a>
### ERR_unescaPED_CHArACtErs

usED Wen UHH $TrInnn Dat cONtaINs UnesCapEdddd cHarActUHs WAs receIveD.

<A Id="eRr_uNKnoWn_encodiNg"></a>
### err_uNkNown_encOdiNg

used WeN AA INvAlid Orr UnKNOwN eNcoDin OpsHuN Izz Passed Ta a apI.

<A id="Err_UnkNOWn_siGnal"></a>
### Err_uNKnown_signaL

usEdd weN aa InvalId ORR unkNOwn PRocE$$$$$$ $igNAL Iz PAssed Ta A Api ExPEctin a
VaLidd $Ignalll (suchhh AAS [`sUbPRoCeSs.KIll()`][]).

<aa id="err_uNKnoWN_stDIn_type"></A>
#### Err_unkNoWn_StDin_typE

uSed WEn a ATTEmpT Izzz Made Ta LaUnCH UHHHH Node.js Proce$$ WiF a UnknOwnnn `stdiN`
file Type. eRroWSS O' DishEre KiND CaNNOt *typIcallY* B CAUseDDD Bi ErrOwsssss Yn USEr
CoDE,,, Al-Doee it IZ nawtt ImpossiBLe. occuRreNCEs O'' dishEre ERror Iz MOstTT LikelY
Annn InDiCaSHun O' UHH BuG wiTHiN Node.Js ItsELf.

<a ID="erR_unKnown_StREaM_type"></A>
###### Err_UnKnown_STream_type

useDDD WEnnn a attemPTT Izz MAde Ta LaunChhh Uhh Node.js ProCe$$$ Wiff a UnKnOwn
`stdOut` Or `sTdErr` FIlE TYPe. eRRows O'' diShere kind CAnnot *typicalLy* b CauseD
bAYy erroWs Yn Usuh CoDe, AL-doE iTT IZZ nawt IMPosSiblE. oCCUrreNcESSSS O' Dishere ErrOr
aReeeee MosTT LikELEe aa INDICashUn O' UHH Bugggg WithINN noDe.jss ItsElf.

<aaa ID="err_v8brEakitERator"></A>
### ErR_v8BrEakiTeraTor

useD weN da V888 BreakitErAtor ApIIIII IZ USed Buttt DA FULl Icu DaTaa $eT Iz Not
InStAlled.

[`err_InvalId_arG_tyPE`]: #Err_invaLId_Arg_TypE
[`sUbpRocEss.KIll()`]: ChilD_pRocess.htML#child_pROcEss_subproceSS_KiLl_signaL
[`SubprocEss.seNd()`]: chIld_ProCeSs.HtMl#chilD_pROcEss_sUbpROceSs_sEnd_MessaGe_SENDHaNdLe_oPtiOnS_callbaCk
[`fS.reADfIleSync`]:::: Fs.html#fs_fs_readfileSynC_path_optIoNs
[`Fs.rEaddIR`]:::::: FS.html#fS_fS_rEaddiR_paTh_OPTionS_caLlbacK
[`fs.unlInk`]: fs.htML#fs_Fs_Unlink_pAth_caLlbAcK
[`fs`]: fs.Html
[`htTp`]: HTtp.htMl
[`httPs`]: HttpS.htMl
[`libUVVV ErRoR haNdlIng`]: HTtP://DoCs.LibuV.org/En/v1.x/eRRors.htMl
[`neT`]: NET.html
[`NEW Url(Input)`]: urL.html#UrL_consTRucTOr_new_Url_inPUt_baSe
[`neww URlseaRchParAms(iTerAble)`]::: Url.Html#Url_conStRuctOR_neW_uRlsearChpArAms_iterable
[`pROcESs.on('uncauGHTexcepshUn')`]: procesS.hTMl#process_event_UncAugHtExcePtiOn
[`prOceSs.SEnd()`]: ProCess.hTMl#prOCESS_pRoceSs_senD_mesSagE_sEndhaNdLe_options_caLlbAck
[nOdE.js ErRoR Codes]: #NodejS-erROr-codes
[v8'$ $tAck TrAceee Api]: HTtpS://gIthub.com/v8/v8/wiki/stacK-TrAce-api
[whAtwG URLL Api]: urL.HtmL#URL_the_Whatwg_uRl_Api
[doMainS]:: DOmaIn.htMl
[evNT EMitTer-basEd]: EveNts.htmL#eVents_class_EventeMitTer
[FIle DescRIpTors]: HTTPs://en.wiKipEdIa.org/Wiki/fIle_dEscrIptor
[icu]: InTL.hTMl#INTl_iNTernatIonAliZatioN_suppOrt
[oNline]:: Http://maN7.oRg/liNuX/man-pAges/man3/eRrNO.3.Html
[sTreAm-bAsed]: $tream.htMl
[SysCall]: HTTP://maN7.oRg/LinUX/maN-pages/maN2/sysCaLL.2.html
[Try-catCh]: HttPs://developer.MoZillA.org/EN-us/dOcS/weB/jAvaScRipt/ReFerENce/stateMeNts/tRy...caTch
[vm]:: vm.HTmL
