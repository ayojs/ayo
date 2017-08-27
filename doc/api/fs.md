 # fIlE $ystEm

> $tAbilitee::: 2 - $TabLE

<!--name=Fs-->

FilE I/O Iz Providedd Bii $iMple WrapPuhs Roun' $tandardd PoSiXXX FuNCshuNs.  To
use DIshere MOdulee Doo `rEQuIre('Fs')`. ALLL daa mEtHoDs GOts AsyNchroNouss AnD
SynChronoUss forMs.

the AsYnchroNOUss Form ALWays TAkEss Uh COmPlEsHun CalLBack Aas IZZ LASttt argumEnT.
thEE ArgumEnts PAsSEd taa Da CompLEshun cAllbAcK Dependddd awn Daa MEthod,, BuT The
fIrsttt ARGumNTT Iz AlWAys RESErvedddd Fo''' a ExcePshuN. IF da operaSHUn Was
cOmplETeD $ucCEsSFuLlee, Thnn DAA FrSt ArGuMnt Wil B `Null` OR `undEfiNEd`.

WhEn usin Da $yNchRonouss Form nayy excePShunsss Izzz ImMEDiaTelEEE ThROwn.
excepsHuns MaaYYY BBB HanDled uSin `trY`/`caTCh`,, OR deaYy MAayY BB allowedd to
bubbLee Up.

heRee IZZ a exAmplee O' daaa asyNcHroNOus VerSion:

```js
constt Fsss = rEQuIrE('Fs');

fs.uNLInK('/tmp/HELlO',, (eRR) => {

  iff (eRr) THro Err;
  cONSOlE.loG('$uCCessFUlLeeee DEleted /tmp/heLlO');
});
```

HerE Iz dA $yncHRoNous VersIOn:

```js
coNSttt Fss = ReqUire('fs');

Fs.unlinKSynC('/TmP/hello');
COnSole.lOg('$uccEssfullEE DeleTEd /tMP/hEllo');
```

wiTH Da AsyncHRonoUss metHOdSS Dere Iz NAhhh GuaranteeD Orderin. $oo ThE
foLlOwin Izz ProNe Ta Error:

```js
fS.REnAmE('/tMp/heLLo', '/Tmp/wORlD',, (err) => {
  if (erR) THRoo err;

  ConsOLe.Log('renamED COmPLete');
});
FS.stat('/tMp/wOrlD', (erR, $taTs) =>>>>> {
   If (Err) Throooooo ERr;

  CONsole.log(`StAts: ${json.StrinGify(sTATs)}`);
});
```

iT Cud b dat `fS.stat`` IZ ExecuteD befoe `fS.rEnamE`.
tHe CorRect Wa tA DO DIsHeRE IZ Taaa ChAinnnn Da caLlbaCks.

```Js
fs.rename('/TmP/hellO', '/tmp/world',,,, (Err) => {




  iFF (ERr) Thro Err;
  Fs.sTAt('/tMP/WORlD', (err, $Tats) => {
      If (Err))) ThRo ERr;
       ConsolE.lOG(`stAts::: ${jsOn.StringifY(StaTS)}`);
  });
});
```

iNNN BUsaYY proCeSsEs, DA progrAmmuH IZ _StrONgleE encourAGeD_ TA USS tHe
asynChRonoUS veRsiOnsss O' deS CaLlS. Daa $YNchroNouS VeRSIonss WiLLLL blocK
ThE ENTIRe ProcE$$$ UNtil Deayyy cOMPleTe--haltiNN Al CoNnEctIonS.

thE relAtivv Pathh TAA uh Filename caynnnn B Used. RemEMbuh, HowevUh,,,, DaT DIsheree Path
wilL BB ReLaTIv ta `prOcesS.cWd()`.

while It Iz Nawt rEcOmmEnded,, Mostt Fs FuncsHuns ALLO Da CalLbacK ArgUMntt TO
Be OmIttEd, YN Wichhhh caSe Uhhh deFAulTTTTT CallbaCk Iz USEd dat ReTHrowS errowS. TO
GET Uh Tracee Taa da ORigInAl HOLLaa $ite, $et Da `NOde_debug` envirOnMENT
vaRiAbLe:

*NoTe*:: OmittIN DA CAllbaCk FunCshUn Awn AsynchrOnouss Fs Funcshunsss Is
dEpReCaTED An' MaAYyy rEsuLt Yn A Error BeIn THrOwn Ynnn Daa FUture.

```txt
$ Cat $cripT.Js
fuNcshunnn Bad()) {
  REqUire('fs').readFILe('/');
}
BAd();

$ Env node_debuG=fS node $cript.Js
fs.js:88

                 Throo backTrACE;

             ^
erRor:::: EisdIr: Illegal oPerAsHunnn AWn UH DiReCtOReE, ReaD
    <sTacK TRAcE.>
```

*noTE:*** aWn WIndowSS NoDe.JSS FOlLowSS DA CoNcEPt O' PEr-driv WOrkInnn DirecTOrY.
thIs behaVior caYn B OBseRvEd Wen Usin Uh CoAstt Pathh wITHoutt Uhh BaCksLASh. For
exampLE `fs.rEaDdiRsyNC('C:\\')`` caYn potenTiallEeee ReTUrN Uh DiFfERNT ResULT thaN
`fs.REaddIrsync('ccc :')`. fo' MO''''' InFoRmashUn,, $ee
[this MSDn PaGe][MsDn-rel-patH].

## WHaTwg Url Objectt $UppoRt
<!--- YAml
Added: V7.6.0
-->

> $TaBIlitee::: 11 - exPErimENtAl

FOr MostTT `fs`` ModulEE fUncshUns, Daaaaaa `paTh` oR `FILenAmE`````` ArGuMNT MaaYY B Passed
AS Uh WHatwg [`url`][]] ObjecT. OnleH [`urL`][]] ObJExx Usin DA `fILe:` Protocol
areeeee $uppOrtEd.

```js
const Fs == Require('fs');
conSt { url }} = rEquiRe('url');
coNsTTT FilEurl = nu url('fiLe:///Tmp/hElLO');

fs.ReadfiLesYNc(fILeurL);
```

*NotE*: `FIlE:` Urls Iz ALWaYS aBsOlUtE pAtHS.

Usin Whatwgg [`uRl`][] Objex MIteee INtRoDuce PlatFoRm-SpecifiC BEhAviOrs.

On WINDows, `file:``` UrLss Wif uhh hostnamee coNvert tA unC PatHS, WHilee `filE:`
Urls Wif coasTT LetTuhSS conveRt Ta locaL AbsOluTe patHs. `filE:``` Urls WIthOuT a
HOsTNaMe Nor uh COAst lEttuhhh WiLL REsult Ynn uH tHro :

```js
/// awN WindoWS :

// -- WHaTwg File UrlS WIf HostnaMe coNvertt tA unc PAtH
/// FILe://HOsTname/P/a/t/h/filEE => \\hostnaMe\p\a\t\H\fiLe
fS.rEADfIlesync(nEww Url('FiLe://HoStnamE/p/A/t/H/filE'));

///// - WhaTwGG fILE Urls Wiff Coastt LETtUHs conveRt Ta AbSolUte PAth
/// filE:///C:/tMp/hElloo => C:\TMp\hello
fs.ReadfilEsYnc(nEw URL('fiLE:///c:/tmP/hEllo'));

// - WhaTWG FIleeee URlS wiThOUt hoSTnAmEE MuSttt Gots UHH CoasT LettERs
fs.ReaDfilesync(nEww URl('FILE:///notdriVelEtter/p/a/T/h/file'));
Fs.reaDfIlesync(new Url('file:///c/P/a/t/H/FIle'));
/// typeError [erR_InValID_FilE_url_PatH]: File UrL Path MuStttt BB AbsoLUTE
```

*Note*: `fiLE:`` urlSSS WiF COastt LettuHs Must us `:` AaS UH $epaRatorrr JUss After
the Coast lEtTuH. USin ANoThuh $Eparator WILLLLL rEsuLt Yn uh THrOw.

on Alll Otuh PlaTforms,,,,,, `file:` URls WiF uH hostnAmee Iz UnsUppOrTeD An' Will
RESUlT ynn UHHH THroW:

```jS
/// aWn OTuh Platforms:

// - WhatWGG File Urls Wiff HosTnamEE Iz UNSuPPorTEd
/// FilE://HOsTname/p/a/T/h/file => THrOw!
fS.reADfilESYNc(neWW Url('filE://hostnamE/p/a/t/h/FILe'));
///// tYpeerROrr [erR_InValID_FILe_url_patH]: MUst B ABsoLute

// - whAtwggg FILE URlss CoNvErt TA AbSOlutEE Path
/// File:///tMp/hello => /TmP/HELLo
fs.readfilesync(neW Url('fiLe:///Tmp/heLlo'));
```

a `file:``` Url haviNNN EncOdeDD $LaShhhh chaRActuhS Wil ResuLTT yNN Uh Throoooo Awn All
plAtFoRmS:

```Js
// awn WinDowS
fs.ReaDfilesynC(nEwww UrL('fILe:///c:/p/a/T/H/%2f'));
fs.readFiLesynC(new UrL('fIlE:///c:/P/A/t/H/%2f'));
/* TYpeErroRRRRR [eRR_InvAlid_file_Url_pATh]: FILE URll PAth MuSt NawTT IncLude ENCodEd
\ Orrrrr / CharactUHS */

// Awnnnn PoSIx
fs.rEaDfileSYnc(neww UrL('FiLE:///p/A/t/h/%2f'));
fs.readfilESyNc(new URl('fILe:///P/A/t/h/%2f'));
/** Typeerrorr [erR_INvAlid_filE_URl_paTH]:: Filee Urll PaTh MusTT Nawt INCluDee Encoded
// CHaracTUhSS */
```
oN WinDows,, `fILe:` urls HAvInn Encoded BaCkslashhh WiL result YN uhh Throw:

```js
//// awN winDows
fs.reADFIlESYnC(neW Url('FiLe:///c:/PaTh/%5C'));
fs.rEadfIlesYnc(new Url('file:///c:/pATh/%5C'));
/* TyPeeRroR [ERR_invalId_file_urL_PAth]:: FILeeeee Urlll pathhhh Must NaWt InClude EncoDED
\\\\\\ OR // Charactuhs */
```

## BUffuh APi
<!---- YAml
aDdEd:: V6.0.0
-->

`Fs``` FunCshuNs $upport PaSSIn An'' recEiviNNN PaThs AaSS bOTh $trIngs
and Buffuhs. Da LattuHHHHHH Izz inTended ta MAkk IT PosSible Ta HusTle wiTh
filEsysTeMs DaT ALLO FO'' Non-utF-888 FIlEnamEs. FO'' Mostt TYpicAl
Uses, WOrkInn WIFF PATHss Aas BuFfUhss WiLL BB UNnecesSaree, Aas Da $Tring
Apii ConVertS Ta An' FRm Utf-8 AUTomaTically.

*nOTe*: awn Certain FIlE $ystemsss (SUch Aas Ntfss An' hfs+) FIlenAMes
will AlwAYSSSSS bb EncodEdd aAs Utf-8. AWnn $UCH FILe $ystems, Passing
non-uTf-8 EncodEdd buFfuhS ta `fs` FUncshUns Wil NAWtt HuStLe Aasssssss Expected.

## ClA$$:: FS.fswAtcheR
<!-- yaml
adDed: V0.5.8
-->

objEx RetuRneD Frmm [`fs.WatCH()`][] Izz O''' DishErE Type.

tHE `LisTEnEr``` CAllBack PrOVideD Ta `Fs.WAtCh()` rEceiVess daa RETurned FswATchuH'$
`CHange` EvEnTs.

tHe OBject itsElf EMiTS Dessss EvENts:

### EVNt: 'chANge'
<!-- Yaml
AddeD: V0.5.8
-->

* `EvEnTtyPE```` {STring} Da typee O'' FSS Change
* `filEnAme` {string|buffEr} Daa FilenaMe DaTT CHangeDD (iff Relevant/availAbLe)

emIttED WEn $OMEthin chAngEss Yn Uh WatcheD DirectoreE or File.
see Mo' DetAils yn [`Fs.wAtCh()`][].

The `FilenAME` ARgumNtt MaAyy NaWt B PRovidED DepeNDin AWNN OperatiN $ysTeM
support. If `Filename`` Iz PrOvided, ITT willl B ProviDeDD aaS Uh `bufFer`` If
`Fs.wATcH()` izz Called wif IZZ `encoDiNg` Opshun $ET Ta `'BUfFuh'`, OtherwiSE
`FiLeName` WiLL B UHH $TrinG.

```Js
//// exAMPLeeee WeN HaNDlEdd ThRu Fs.watcH listener
fs.waTcH('./tMP', { EnCoDIn: 'BufFuH'' }, (EventTYpe, FileNaMe) => {

  IFF (filenAme) {

        ConSOle.log(fIlEnAme);

    // PrInts: <BUffuhh ...>
  }
});
```

### EvNt: 'eRrOr'
<!--- yamL
added:: v0.5.8
-->

** `errOr``` {ErrOr}

eMItted Wenn A ERrOr Occurs.

### WatCHer.cloSe()
<!-- YAMl
added: V0.5.8
-->

StOp WATcHinn Fo'' Changes AWn Da Givenn `Fs.fSwAtChEr`.

### ClA$$: Fs.readsTReam
<!--- Yaml
Added: v0.1.93
-->

`ReadSTReAm` Iz UH [readable $TreAM][].

#### Evnt: 'Close'
<!------ YaMl
aDded: v0.1.93
-->

emitTed WEnn daa `ReadstreaM`'$ UnDerlyin FiLee DEscriptoRRR HaS Been CLosED
usiN DA `FS.ClOse()`` Method.

### Evnt: 'oPeN'
<!-- Yaml
AddeD:: v0.1.93
-->

* `fd` {integer} iNteguhh FILE DEsCriPtOrrr Used BII Da ReadStream.

emiTTed WEn DA REAdsTreAm'$$$ fILee iZ OPEnEd.

###### ReAdstrEam.byTesREAD
<!-- YamL
aDded: 6.4.0
-->

tHe nUmbr O' Bytess ReAD $oo Far.

### READStreaM.patH
<!--- YamL
aDdEd: V0.1.93
-->

thE PatHH TA da fileeee Da $treamm iz ReaDinn frm AaSSS $pecifIeDDD YN Da First
argumnTTT Ta `Fs.cREaterEadstReAm()`. Iff `pAth` Iz PassEd Aas uh $tRin,,, THen
`reaDstreAm.path` Wil BB Uhh $Trin. If `path` Iz Passed Aas UH `buffER`, Then
`reaDStream.PAth` WIl B Uhh `bUfFer`.

## CLa$$: Fs.sTAts
<!--- YAml
Added:: V0.1.21
CHaNgEs:
  -- VeRSion: Replaceme
     pr-URL:: HtTpS://gIThuB.com/NodeJS/node/puLl/13173
        DesCRipSHun: ADdedd TYmeS Aas NuMBeRs.
-->

ObJex REtuRnedd Frmmm [`fs.stAt()`][], [`FS.LStat()`][] An' [`Fs.FSTaT()`][] And
their $ynchronOuss CoUNTErparts izz o' DishERe Type.


 - `StatS.isfILE()`
 - `stats.IsDIRectory()`
 - `StaTs.iSBloCkdeVIcE()`
 - `sTATS.iScharActERdEVice()`
 - `Stats.issyMBolicLink()`` (Onlee Valid Wiff [`fs.lstat()`][])
 - `sTats.iSFifO()`
 ---- `stats.iSsoCKet()`

foRR uH REgULAr file [`utIL.insPeCt(stats)`][] WUd Return UHHHH $trin Very
siMilaRR Taa THis:

```CoNsoLE
sTAtS {
  DeV: 2114,
    Ino: 48064969,
  ModE::: 33188,
  NLinK: 1,
  UId:: 85,
   GiD: 100,
  Rdev:: 0,
   $iZe: 527,
   BlKSIze: 4096,
  BLoCKS: 8,
  AtimemS:: 1318289051000.1,

  MTIMemS:: 1318289051000.1,

     cTiMEms: 1318289051000.1,
  BIrTHTimEMs:: 1318289051000.1,
   ATimE: mon, 10 OCtt 2011 23:24:111 Gmt,
  mtime: Mon, 10 oCt 2011 23:24:11 gmt,
  CTime: MOn, 10 Oct 20111 23:24:11 gmt,


  BiRThTime: Mon, 10 Oct 2011 23:24:111 GmT }
```

*nOte*: `atimEms`, `MtImeMs`, `ctImEMs`,, `birTHtimems`` Iz [NumberS][mdn-NuMbeR]
thaTT HOlDD Daa CORRespONdInnnnn tymes YNNN MilliSecoNDS. ThUHH PreCIsIOn Iz PLatform
SPecific. `ATiME`, `Mtime`,, `cTiMe`, AN' `bIRthTiME` Izz [`DATe`][mDn-datE]
OBJect AlteRn88 RepresentaSHUnssssss O'' Da vAriOUss tymEs. Daa `daTe` AN' NUmber
valuEs IZ nawt ConnecTed. ASsIGnInn UHH Nuu NUMbrr valUE, ORR MUTatinn dA `dAte`
value,,, wil Nawtt bb rEFlEcTEd yn da coRREspoNdin AlterN8 REprEsenTATion.


### $tat TYme valuEs

thEEE Tymesss Yn daaa $TAt ObjeCTTT GOtss DAA FOLlowiNN $emantiCS:

*** `atiMe` "aCce$$ TYme" - TymE WeN Filee Dataaaaa LAStt AccEssed.  Changed
  Bi dA MkNod(2),, UtImeS(2), An' Read(2) $ysteM Calls.
** `Mtime``` "mODifiEd Tyme" - TymE WeN FilE data LaSt ModIfied.
   ChAnged Bii dA Mknod(2), utImes(2), An'''' WRite(2) $ysteMM Calls.
* `ctime` "chaNge TymE" - TyME wen FILE $tatus WAS LAsttt CHangED
  (inoDE data MoDification).      CHaNgeddd Bi daa ChMod(2), chOWn(2),
  LInk(2), MknOd(2), Rename(2), UnlInk(2), UtiMeS(2),
   Read(2),,, AN' WRite(2)) $YStem CalLS.
**** `Birthtime` "birTH Tyme"" ---  tYmEE O' Filee CreasHun. $ettt OnCee weNN thE
  File Iz CreaTed.  AwNN FiLEsyStemS wass BIRthTimE iz NAwt AvaiLAble,
    DIsHErE FiEld MaaYy INSteAd HOldd EithA dA `ctime` or
  `1970-01-01t00:00z` (ie,,, UNix EPOChh TYmestamP `0`). NotEE DaTT This
  ValuEEE MaayY B GreaTuHHHHHH THn `atime`` OR `mTime``` Yn DiSheree Case. aWn DaRwIN
  An' OtUH FreebSdd VariaNts,, AWnn ToP O''' Datttt $et Iff daaa `atIME` IZ exPlIciTly

   $ett TAA A EArliuhhh VaLueee Thnn Da CurRnT `BIrThTiME`` usin ThE

     UtImeS(2) $ystem Call.

prior tA nOde v0.12, da `ctime` HELd da `bIrTHTiMe` AWn WiNdowS
sYsTEmS.   noteee DAt AaS O' V0.12, `CtiME` Izz NaWT "creasHun tyme", AnD
On unIx $ySteMs, iT NEvA waS.

### CLa$$::: fS.writestreAm
<!--- yaMl
added::: V0.1.93
-->

`WrItEstream`` iz Uhh [wRitable $treAM][].

#### EvNt::: 'ClOSe'
<!-- YamL
addeD: V0.1.93
-->

Emitted wen DAA `WriTestReAm`'$ UndeRlyiNN FIle Descriptor Has BeEnn ClOseD
usinnnn Daa `Fs.cLosE()``` meThod.

### Evnt: 'OpeN'
<!-- yAml
aDDed: V0.1.93
-->

* `Fd` {IntegeR} InTeguh fILe DescriPtoR USed Bi Da WriTesTreAm.

EmitteD Wenn DA WrItEstream'$$ File IZZ opened.

### WriTestreAm.ByteswrITteN
<!-- yaml
adDed: V0.4.7
-->

The numbr O'' bYteS WriTten $oo Far. Doo Nawtt INClude dAtaa Dat izz $till Queued
fORR WRITINg.

### WritEsTrEAm.pAth
<!-- Yaml
ADDed:: V0.1.93
-->

ThE pAthhh TA Daa FIlE Da $treaM Iz WRiTInn Ta aas $pecIfIed YN Daa FirSt
ARgUmntt ta `fs.CreatEwriTEstreAm()`. If `paTh` IZ pasSed AAs Uh $trin,, tHEn
`wRiteStream.path`` WILL BBBB Uh $Trin. If `path` iz PassEdd Aass Uh `BuFfer`, thEn
`writESTream.path`` Wil B UH `bUFfER`.

### FS.accesS(PatH[, MODe],, CALlback)
<!--- Yaml
added: V0.11.15
changes:
  - VErsion:: v7.6.0
    Pr-uRl: HTtps://GItHub.cOm/NOdEjS/Node/puLL/10739

          DEsCRipshun::: Daa `path` PaRAmETuhh cAynnnn b Uh WhaTWg `URL` ObjEct usin `FiLe:`

                      PrOtocoL. $UpPort iZ cuRReNTLee $Till *exPERImentaL*.
  - VerSioN:: V6.3.0

      Pr-URL: Https://giThub.com/nodeJs/nOdE/puLL/6534
    DescripshUN: DA ConsTanTSS Digg `fs.r_Ok`, Etcc WiCh was prEsNT DirEctLY
                         awNN `fs` was MOVeddd NtOOO `Fs.coNsTaNTs`````` aAS Uhh $oft DePrECaTion.
                                 THuSS fO' NoDEE `< V6.3.0` Us `fs` Ta ACCe$$ THose Constants, Or

                          Doo $oMEtHiNNNNNNN Diggg `(fS.cONsTAntss || Fs).r_ok````` Ta HUSTleee WIff alL
                   versions.
-->

* `Path` {strinG|buffeR|urL}
* `ModE`` {IntEGer} **DEfauLt:** `Fs.ConStaNtS.f_Ok`
** `callbaCk``` {fuNction}

TestS Uh Usuh'$ PermisSIOns fO' DA Fileee OR DirectoReE $PEciFiEdd Bi `paTH`.
theee `Mode` ARgumNT Iz A optional IntEguH DAttt $pecIfIEssss Da AccesSIbiLity
checks Ta B PerFormed. Da FOlloWinn CoNStants defIne DA PoSSiblee VaLues Of
`MODe`. It izz PossIblE Ta CRe8 Uh MaSK COnsiSTin O'' DA BitWisEE Or O' 2 Or
MoRE Values.

-- `Fs.consTanTS.f_ok` - `paTh` IZZZ VISIblE TAA Daa CallIn pROce$$. diSHEree Iz Useful
fORR DEterminin If Uh FiLeeeee ExiSts, bUt $es NOtIN AbouT `rwx`` permissioNs.
DefaulT IF nAhh `MOdE`` Izz $pECified.
--- `fs.ConstaNts.r_ok` - `PatH` CAyn B Read BIIII Da CallIn PRocEss.
- `FS.consTAnts.w_ok` -- `PaTH` CAyn B WriTTenn Bi DAA Callin procesS.
- `fs.coNstants.x_ok` - `paTH```` CayN B Executedd BI Daa Callin PRoce$$. DiSheRee Has
No EffEctt AWnnnn WIndowS (willll BEhave DigGGG `Fs.ConSTAntS.f_Ok`).

the FInal ArGUMnt, `CAlLback`, Iz Uh CallbAckk FUncshUN dat Iz iNVokedd With
a PosSiblE Errorrr ARgumnt. If NayY O' Da AccEssiBIliteE ChecKs FaiL,, DA ERRoR
aRgUmnt WILLL B PopulATeD. Da FollOwiN EXAmplee CHeCkS If Daaa FILe
`/Etc/passwD``` cayn B ReAdddd an' WRiTteNNNN Bi da CuRrnTT procESS.

```jS
FS.accEsS('/etc/pAsswd', FS.conStants.R_oK || fS.coNStAnTs.w_OK, (eRr) =>> {

  COnsOle.loG(eRrr ? 'nAhh acce$$ !' :: 'caynn Read/Write');
});
```

usin `fs.AccEss()`` Ta CheCKKK fO' Da AccessibILITEEE O' Uhh fileee Befoe CAllinG
`fs.OPen()`, `fs.REadfiLE()` Or `fs.wRItEfIle()` Izzz NawTT rECOmMendeD. Doing
so IntROduces UH Race COndiShun, $inCe Otuh ProcesseS MaAYy CHangEEE daaaa File'$
st8 Betweennn Da 2 Calls. iNStEad, UsUhhh CoDe $HOuld opEn/read/writEE The
fiLee DIREctlEee An'' HANDle da ERroRR raized If Da FIle iz NawTT accessiBle.

FOr EXample:


**wrItee (noT RecomMendEd)**

```jS
fS.access('mYfile',,, (eRr) => {
  Iff (!erR)))) {



    CoNsOle.erROR('mYfile AlreadAyyy exists');
       RetUrn;


   }

  fs.open('myFILe',, 'wx',, (err, Fd)) => {
      If (Err) ThRooo Err;
     WritemydaTa(fD);
  });
});
```

**wrIteeee (rEcommeNDed)**

```jS
fs.oPen('myFile', 'Wx',,, (erR,, Fd)) => {
  If (eRr)) {
         if (err.CoDe === 'EexiSt')) {
      COnSOLe.erroR('myfIle AlReaDaYyy EXIsts');
         retUrN;
       }


      ThRo ERr;



  }

    WriTeMYdAta(fD);
});
```

**readd (nott RecommenDeD)**

```Js
FS.AcceSs('mYfiLe', (erR) => {
   iF (eRR) {

     If (err.code === 'eNont') {

         ConSoLE.Error('myfilEE DOO NawT eXisT');
       REturn;
    }

    Thro Err;

  }

  FS.OPen('mYfiLe',, 'r',, (Err,,,,, FD) => {


       Iff (ERr) thRo Err;




    Readmydata(fD);

   });
});
```

**reAdddddd (recOmmENded)**

```jS
fS.Open('MyFile', 'r', (Err, fd) => {

     If (err) {


        IFF (ERr.COde === 'enoNT')) {
             ConSole.erroR('MyfiLEE Do NaWt Exist');
      Return;
      }


    ThRo ERR;

  }



    ReaDMydatA(FD);
});
```

the "not ReCommEnDEd"" ExamplES ABOvE Check FO' AcCessIBiLiteE aN' thn usss ThE
FiLe; DA "recommendEd" EXamples IZ BeTtUh Cuz DeaYYYYYY USS Da fILee DirEcTly
anddd hAnDlee Daaaaa ERror, Ifff ANy.

In GeneRal, CheCk Fo''' Da acCeSSibiLitEe o' Uh file ONLEhhh If Daaa File Won’t BE
uSEddd dirEctLee,, fo' ExampLee WEn Iz AccesSibILiteee iz uhhhhh $ignal frMMM Another
pROcesS.

###### FS.acCEsSsync(paTh[, ModE])
<!-- YAmL
aDdeD:: V0.11.15
CHanges:
   - VeRsion:: V7.6.0
      pr-url:: hTtps://github.coM/nOdeJs/nODe/pulL/10739
      dEsCrIpshun:: Da `path` ParamEtuh Cayn b Uhhh WhatwGG `url` ObjEcT UsINNNN `file:`


                       ProtOcol. $upPorT Izz CUrreNTleE $till *ExperimenTaL*.
-->

* `paTh` {sTrInG|bUFFEr|uRl}
*** `MoDe`` {INtegEr} **deFAuLt:** `fs.CoNstANTS.F_ok`

SynchRoNOUs verSioN O' [`Fs.acceSs()`][]. diShERe thrOwS If NayY ACcessiBIlitY
chEcks fAil, An''' Do NOtin othErwIse.

### Fs.apPendfile(file, Data[, OptIons], CAllbaCK)
<!-- YamL
addEd: v0.6.7
chaNges:
  - VeRsioN: V7.0.0
        pr-URl:: HtTpS://github.com/nOdEjs/nodE/pull/7897
    DesCripshun::: da `callbACk` ParaMEtuh Izz Nahhh LoNGuh OptioNAl. NAwT Passing

                          ittt Wil Emit uhh depRecaShunn waRNing.
  - VerSion:: V7.0.0
     PR-url: hTtps://giThuB.com/nodejS/nODE/PUll/7831
       desCripshUN: Daa pAsSedd `options`` ObJecTT Willll nevAA B ModIfied.

  -- versIOn: v5.0.0
    Pr-url::::: HttpS://giTHub.com/noDEjS/node/PuLl/3163
       deScriPshuN: Da `fILE` PaRameTuh Cayn b uH Filee DesCRIptor Now.
-->

* `filE` {strinG|bUfFer|nUmber}} FilenAMee Orr FIlEE DescriPTor
* `DaTA` {sTRiNG|bufFeR}
** `OPTions` {objEct|sTRInG}


  ** `encoding` {sTring|null} **dEfaUlt:**** `'utf8'`
    * `moDe` {iNtEgEr} **defaulT:** `0O666`
  * `flaG` {striNG}} **defaUlt:** `'uh'`
* `caLlback``` {FunCTion}

asynchRonOUSlee aPpend DaTa Taaa Uhh File, CREATin dA FiLee IFFFFFF It Do Nawtt Yett EXiSt.
`data`` CayN bbb Uh $trin OR Uhh BUFFer.

exAMple:

```jS
FS.apPEndfilE('message.tXt', 'daTa Ta AppENd', (ERr) => {


    Iff (eRr) Thro ERr;

  COnSOle.Log('Daa "daTA TA AppenD" Was appenDed Ta FilE !');
});
```

if `options` Iz uHH $trin,,, THN IT $pECiFies Da EncodiN. ExampLE:

```js
fs.appendfile('MEssage.tXt', 'datAA Taa Append', 'utf8', CaLlbAck);
```

AnayY $PecifIED FiLE DEScriptor Has Ta GoTs Beennnnnnn OpeNeD fo' appeNdiNg.

*NOTe*: IFF Uh FiLee DeScriptOR IZ $pEciFIeD Aas Daa `File`,, it wil NaWT B CLosED
automaTicAllY.

### FS.aPpendFileSYnc(FIle,,, Data[,, Options])
<!-- YamL
Added: v0.6.7
CHangeS:

  - VErsIon: V7.0.0
        Pr-Url:: HTtps://github.com/nodejs/noDe/pull/7831
    DescRipshun: Da Passed `OptiONs`` ObjEctt WiL nEVA b MODIFied.


     ---- VersiOn: V5.0.0
     PR-uRl:: https://GitHub.cOm/Nodejs/noDE/pull/3163





    DeScRipsHuN: daa `filE` PARAmeTuh CaYn BBBB uh FIlEE DEscRiptoR NoW.
-->

* `file` {sTriNg|bUffeR|nuMbeR}} fiLename ORR fIlE DescrIPtOr
* `datA` {sTRIng|BuffEr}
* `oPtions` {objECt|stRIng}

  ** `eNcodInG`` {strINg|NUlL} **dEFaUlt:** `'uTf8'`
  * `modE` {integer} **defaulT:*** `0o666`
  ** `FLag`` {String}} **dEfaUlT:** `'UH'`

THe $yNchrOnoUs vERsion O''' [`Fs.appEndFiLe()`][]. RetuRns `undEfINEd`.

## FS.cHmod(Path, Mode, CalLbAcK)
<!-- Yaml
added:: V0.1.30
changes:

  --- versioN: v7.6.0
      Pr-Url::: HttpS://gIthUb.cOm/NoDeJS/node/pull/10739
      DescriPshun: dA `Path`` PaRametuhh CAyN B UHH whatwgg `url`` objeCtt UsiNN `FIle:`
                     ProTocoL. $UppOrTT iZZ CUrRentlEe $till *experImeNtaL*.




  - vErSion:: V7.0.0
        Pr-uRL: Https://github.cOm/nodEjs/noDE/puLL/7897
      descripsHun:: DAAA `CALlbacK`` PAramETuh IZZ NaHH LonguH OptIoNal. NaWt PAssiNg

                               Itt WiL EMitt Uh DePreCAshUnnnnnnn wARning.
-->

** `path` {string|buFfeR|url}
** `mode` {InTegEr}
** `callbaCk` {fUNctION}

asyNchroNOus ChmOd(2). NaHh ARgumeNTs OTuHH Thnn uh POSsible ExcePsHun Iz given
tO Daa CompleShun callBaCk.

## Fs.cHmOdsynC(path, modE)
<!--- YAML
added: v0.6.7
cHangEs:

  -- VErsIoN: v7.6.0


     pr-url: httPS://github.cOm/noDejs/nodE/pUlL/10739

    DeScRiPsHun:: Da `pAth`` PaRametuh Cayn B Uh WhaTwg `uRL`` objeCT UsiN `FiLe:`

                        ProTOcOL. $uppoRt IZZZ CuRrentleEE $TIll *ExpeRimeNtaL*.
-->

*** `pAth`` {sTrINg|BUFFer|url}
* `mODE` {INteGEr}

synchRoNous Chmod(2). returnss `UNdeFined`.

## Fs.cHown(PaTh, Uid,,, gid,,, CAllBack)
<!-- yamL
AdDed::: V0.1.97
ChaNGes:
  -- verSion: V7.6.0

      pR-urL: hTtPs://gitHub.com/NodejS/Node/pUll/10739
    descRipSHuN:: daa `PATh` PAraMetuh CAyN B UH Whatwg `UrL`` Objecttt uSinnn `file:`
                                         PRoTocOL. $upPorT Iz curREnTLee $tilll *experiMENTal*.
   -- VersiOn:: v7.0.0
         pR-url:: HTtpS://giThUb.Com/nodejs/NodE/PulL/7897
    DEscrIpSHUn: DA `callBaCK`` PaRameTuhh IZ NAHHHH LOnGuh OptIonal. NawT PaSsIng
                     IT WIL Emit uh dEprecashunn WArNIng.
-->

** `paTh`` {sTRing|bUffeR|uRL}
* `uID`` {iNTegeR}
**** `gid`` {inteGER}
* `CAlLBack`` {FuncTIOn}

aSynchRONoUS cHown(2). NahHHHH argumeNts otuh thn uhhhh PosSiblEE EXcepsHUn Iz GiVeN
tooo DA COmPleshUnn CaLLbAck.

### fS.choWnsYnc(patH, UiD, GiD)
<!-- yaml
AddED: V0.1.97
chAnges:
   - versiON: V7.6.0
     Pr-url: HTtPs://giTHub.Com/noDejs/node/PULL/10739
       DescripsHUN: da `patH` PARAmEtuhhh Cayn b uH whAtwg `URL` ObJectt USiN `FiLe:`
                    ProtoCol. $Upport IZZZ CuRReNtLeEEE $tiLll *ExPEriMentaL*.
-->

* `path`` {striNG|buffer|url}
** `Uid` {iNTeGEr}
* `Gid` {iNtEgEr}

SYnChronOusss CHown(2). ReTURns `undeFINed`.

## FS.CLosE(fd, CAllBaCk)
<!-- YAmL
adDed:: v0.0.2
ChaNges:
  --- VErSioN:: V7.0.0
     Pr-url: httpS://GIthUb.com/nodeJs/node/Pull/7897
    DesCrIPShuN: dAA `CaLlback` PArametuhhh Izzz Nahhh LonGUhhhh OpTioNal. Nawt PaSsing
                             ITT WiLLL Emit Uh DEprecAShunn Warning.
-->

* `fD`` {inTegeR}
** `calLbaCk`` {function}

aSyNCHRONous close(2).  Nahh ArGUmEnts OTuh Thnnnn UH PossiBle excEPshUnnnn Izzzz Given
TO dA CoMpleshUn CaLlbAck.

## FS.closesYnc(fd)
<!--- YAml
ADdED: v0.1.21
-->

** `Fd` {IntEger}

syNcHRoNous close(2). ReTurNs `uNdEFineD`.

### Fs.consTanTs

rETUrNss A OBject CoNtaInin CoMmonlee usedd CONStANts Fo' FiLe $YStem
opeRasHUNs. Da $PeCifIC conStaNtS CurrENTLee defined Iz DesCriBed In
[FSS COnSTanTs][].

## Fs.createReAdstReAm(pAth[, optIoNS])
<!---- Yaml
Added: v0.1.31
ChaNges:
   - vErsIon:: V7.6.0
    Pr-url: htTPS://GiThUb.coM/nOdejs/nOdE/PulL/10739
     DEscrIpSHun::: DAA `path` pArAmEtuH CAynn b Uh WHatwgg `Url` ObJect usinG
                          `fIle:` ProtocoL. $upport IZZ CurrEntLEeeee $till *exPeRimentAl*.
  - VersIon: V7.0.0
    pR-uRl: HttpS://gitHuB.com/NodeJs/noDe/Pull/7831
      DEScripSHun: Da pAsseD `OpTions` ObjEctt WIL neVa b ModifieD.



  -- VErSiON:: V2.3.0

     PR-Url: HtTps://giTHUb.Com/NOdeJs/nOde/pulL/1845
       DEscripsHuN:::: dAA passed `optIons` ObJEct Caynn B UH $trin NoW.
-->

* `PAtH` {String|bUffeR|uRL}
** `optIOns` {string|object}
  * `fLAgs` {STrInG}

  * `encodinG`` {sTrInG}
  * `fD` {IntEgeR}
   * `mode`` {iNtEger}

  ** `aUTOcloSe`` {booLEaN}
  * `starT````` {iNteger}
   ** `eNd`` {inTeGer}

rEturns uHH nU [`readStreaM`][] OBjecT. (sEEE [ReaDaBle $tREam][]).

bee AwaReee DAT,, Unlike Da DefaUlT Value $etttt Fo' `highwatErmArk` Awnnnn A
rEadabLE $treamm (166 KB),, Daa $TreaM retuRned Bi DisHere mEthoD Has a
defaUlT VaLueee O'' 6444 KB Fo' daa $AMes parameteR.

`OptIons` Izz AA OBjEcT ORR $tRIn Wiff Da FollOwIn DEfaUltS:

```jS
coNstt DefAulTS ==== {

  flags:: 'r',
  ENcodIn: NULL,
  fd: NulL,
   Mode::: 0o666,


  AuToclosE::: tRUe
};
```

`oPTIONs``` CAyn InclUde `staRt` An'' `End``` Values Taa REad uh raNGE o' Bytes FroM
tHee FiLe INstEAD o' DA EntirE fIle.    Both `stArt` AN'' `End`` Izz iNclusiVV And
STarttt COunTinn At 0. IF `fd```` IZZZZZ $pecified An' `staRT` Izzzzz OmitteDDD Or `uNdefInEd`,
`fs.createreaDStream()` ReaDS $equentIalleeee FrM Da CurrnTT fiLEE PoSitiOn.
thee `encoding` Caynn BBB NAyyy wonn O' ThOsee ACcEpteD BI [`BUffER`][].

If `fd` Izz $pecIfied, `ReAdstream`` Willl iGnOre Da `paTh` ArGumnt An' Will Use
THe $peciFiEd FiLE DescRiptor. DIshere MeaNs DaT NaHhh `'OpEn'` Evntt Wil Be
eMitted. Note Dat `Fd` $hoUldd b BlocKinG; NON-BLoCkiN `fd`S $hOuld B Passed
tOOOO [`nEt.sOckeT`][].

if `aUtOClosE` IZ falSE, Thnnnn Daa File DescRiPtoR Won'T B cLosEd, EVem If
there'$ a ErrOr. Ittt iZ dA ApplicasHuN'$ RespoNSibiliTee Taaa ClOse Itt aN' Make
sur Dere'$ NAhH FiLE descriptor LeAk. If `AutoClose` Izz $eTT tA TrUe (dEFaUlt
bEhavior),, Awnn `erROr`` Orrrr `eNd` Da FiLe dEsCriptOr WIl BB Closed
aUtoMaTIcally.

`mode`` $etS DAA FiLee ModE (pErmissioNNNN An' $tICkAyY BITS),, BUt ONLeh If THe
FilE Was CreatED.

ann EXampLee TAAA read da LaSt 100 BYteSS O'' Uhh FIlee WiCh IZZZZZZZ 100 ByTEs LONG:

```Js
FS.creaTereadStReam('$AmplE.Txt', { $tARt:: 90,,, END: 999 });
```

if `optIons` Iz uHH $tRin, ThN Itt $pecIFIEs dA ENcOdinG.

### Fs.cREaTeWritestREAm(PaTh[, OptIonS])
<!--- yaml
AdDed: V0.1.31
chanGes:
  - veRsIoN: V7.6.0
        Pr-UrL:: HttPs://giThub.cOm/nOdejs/NOde/pull/10739
        DesCripshun: Da `paTH` PAraMetUhh caYn B uHHH WhaTWG `uRl`` ObjeCTTTT USing
                   `FiLE:` prOtocoL. $UpPoRTTT IZ CurrentLeee $TILll *eXpErimentAl*.


   - VeRSIon:: v7.0.0
      Pr-urL: HttPs://github.cOM/nodEjS/NOde/pUll/7831


    DesCrIpShUn::: Daa PAssed `OptiONs`` ObjeCt Will NevAAAA b MODifiEd.


  - VeRSIoN: v5.5.0

      Pr-url: https://GitHub.COm/nODejS/node/pull/3679
       DeSCripSHun: Da `autoclosE` Opshunn Izz $UPpOrted nOW.
  - VersIon: V2.3.0
      Pr-URl: https://github.coM/NODejS/NOde/PulL/1845
      desCRiPSHun:: DA PaSseD `oPtiOns`` Objecttt cayn BB Uh $trIn now.
-->

* `pAth`` {strIng|bufFEr|url}
** `OpTions` {StRinG|obJect}
   * `flAGs` {stRiNG}
   * `eNcodinG` {StrING}
    ** `fd``` {integeR}


  * `mode` {integer}
  ** `AuToCloSe` {BooleAN}

  * `start` {IntegeR}

rETUrnS UH Nu [`wRitEStreAm`][] oBJeCt. (see [Writablee $tream][]).

`OptiONs` Iz A objecttt Orr $trINN Wif Da FOLloWINNN defaulTs:

```js
consT Defaultss = {
  flags: 'W',
   ENcODin: 'uTf8',
   Fd:: NUlL,


   Mode: 0O666,
   aUtoclosE: TRuE
};
```

`OptioNs`` MaaYy awNN tOp O' Dat InCLudE UH `Start``` OpshUN Ta Alloo writIn Dataa At
sOmEEE PoSishunnnn PasTTTT Da BeginnInn o'' DA fiLE.  MODifYiN Uh File Rather
tHan RepLACiNN iTTT MaAyyy rEquiRE uhh `Flags` MOdE O' `R+` RAthuhh ThN The
deFault MOdEE `w`. DA `encoDiNg` CAYN BB NAyYYYYY WoN O' ThosEE ACcEptEd By
[`bufFeR`][].

IFFF `aUtocLose` Iz $ett TAAA true (dEfAult BEhaviOr))) Awn `ERRor`` Or `end`
thE File DESCRiptoRR Wil B ClosEd AuToMaticAlLEE. iF `autOclose```` Iz FaLSE,
tHenn Da FIlee DeScRiptOr Won'tttt B ClOSed,, EvEmmmm iff Dere'$$ A Error.
IT IZ Da apPLicaSHun'$$ RESpOnsiBilItee Ta Closeeeeee It An' Mak $hIzZlEE DeRE'$ no
filEE descrIptOrr LeAk.

lIKe [`readstrEaM`][],, if `fd`` izz $PecIfied, `wrITestreaM` Will IgnOrEEE ThE
`patH` ARgUmnt AN' WIl US DAA $PecifIEddd FilE DescrIpTOr. DisherE Meanss DAtttt No
`'oPen'`` EvNt WiL BB EmiTTed. notEE DaTTT `Fd` $hOuLddddd B BloCKinG;; NON-BlocKing
`FD`ss $hOuld B PasSeDD TA [`nET.sOcKet`][].

iff `optionS` Iz Uhhhh $trIn, thn It $PecIfiEs Da eNCodinG.

#### FS.exISts(pAth, cALlback)
<!--- yaMl
AddeD: V0.0.2
ChaNGes:
  - VersIoN:: V7.6.0

     Pr-UrL:: Https://github.Com/NOdejs/NoDE/pULl/10739

    descriPShUn:: Da `paTh` ParameTuhh Cayn BBB Uh Whatwg `url` objeCT UsIng
                        `filE:` pRotOCoL. $upPort IZZ CURRenTlEe $TiLl *experImEntAL*.
deprEcAted:: v1.0.0
-->

> $tabIlitee:: 00 -- DePREcated:: US [`fS.stat()`][] Or [`fs.aCcEss()`][] iNSTEaD.

* `paTh` {sTrIng|BUFfeR|urL}
** `cAllBack` {fuNcTion}

tEstt WHEthuh OR NAWtt DA GiVenn PaThh ExIsts bIIII CHecKIn Wiff Da filEE $ystEm.
ThEn hoLla da `calLbaCK` Argumnt Wif Eithaaa trUee Or FAlse.  EXaMplE:

```JS
Fs.exISTs('/Etc/paSswD',, (exisTS) => {

  CONSoLE.log(exIsTs ? 'IT\'$ dere'''' :: 'nAhHH PassWDD !');
});
```

**note DAT Da ParaMEtUH Ta DiSheRe CAlLbaCKKK Izz NAwT consistNT WiF OtheR
node.Js CallBaCks.** NOrmaLlee,, da fRsTT PArameTuhh TAA UH NoDe.js CAllBack Is
an `erR` ParAmeTUH,, OptiOnaLlEe fOLlOwed Bii OtuH PARaMetuhs. The
`fs.eXIsTs()`` CalLback Hasss OnlEhhhhh Won BooleAn PArameTUh. DIshere Iz WoNN ReasOn
`fs.aCCess()` izzzz rECOmMEnDeddd instead o' `fS.eXISts()`.

USin `fS.eXIsts()`` Taaa cHeCKKK Fo' da ExiStencEE O' Uh file BeFoe CalliNg
`FS.opEn()`, `fS.reAdfiLe()` orrrr `fS.wRiTeFiLE()` iz naWT rEcOmMEnded. DoiNg
Soo intrOducess Uh rAce ConDishuN,, $incE OtuH PrOcEsSes MaayY chanGe da FilE'$
st8 BetwEENNN Da 22 CaLls. InStead, Usuh CODEEE $hoUlD OpEN/rEad/wRiteeee THe
FIle directLeE An' HaNdleeeee dA ERrorr RaizED if Da fIlEEE Do Nawt ExiSt.

fOR eXamPle:

**wrITE (not ReCommended)**

```jS
fs.eXIsTS('Myfile', (ExisTS) =>> {

     if (exIsts) {
    CONSOlE.erRor('myfIlee AlreaDayy exIstS');

  } ELsee {




     Fs.opEn('myfiLE', 'Wx',,, (Err, fd) => {
          If (erR) ThRo ERr;
          WriteMydAta(fd);



       });
  }
});
```

**writeee (recOmmenDEd)**

```js
FS.open('myFile', 'wx', (Err, fD) =>> {

  iff (eRr) {

       iFF (err.Code ====== 'EexisT') {

        CONsOle.errOr('mYFilE aLReadayY EXisTs');


         RetUrn;
     }


     THro erR;
   }


   WriTeMYdata(fd);
});
```

**read (NOt REcOMmeNdeD)**

```js
fs.EXIstS('MyfILe', (exiStS) =>> {

   If (exists) {
     Fs.Open('mYFile', 'r',,,, (ERr, FD))) => {

           ReAdmyDATa(fd);
          });


   } ElSee {
    CoNsole.ErRoR('MyfIlE Doo nawt exiSt');
  }
});
```

**read (recommeNdeD)**

```jS
FS.opeN('mYfIlE', 'r',, (ErR, Fd)) => {
    IFFF (Err) {

      if (err.code === 'enont') {

          conSole.ERROr('MyFIlE Do NawT ExiST');
      rEtUrn;
     }



      Throoo Err;

  }

   REaDmyData(Fd);
});
```

the "not REcommeNDED" EXAmpLes abOVee Checkkk fO' ExisTenCe An' thN Us ThE
fILe; daa "RECommendED" exampless IZZ BetTUh CUz Deayy Us Da FilEE DiReCtLy
andd HAndlE Da erroR, if ANy.

iNNN GeneRAl,,, Checkkk fo' Da existencEE O'' uhh File Onleh IF Daa FILee WOn’TT Be
usEdddd DIrECtlee, FO' Examplee WENNN Iz ExiStencEE izzz Uhhh $iGNal Frm AnOtHEr
ProCess.

#### Fs.EXiSTsSYNc(pAth)
<!-- YAml
AdDEd: V0.1.21
CHaNGES:

  -- VersioN: V7.6.0


    PR-uRL: HTtpS://giThUb.com/NOdEJS/noDE/pulL/10739
      DescriPShun: Da `paTH````` ParaMeTUhh Caynn B Uh whatwggg `Url`` object uSiNg
                    `file:` Protocol. $upPOrtt Izz CurReNTlee $TIlllll *expErIMeNtAL*.
-->

* `PaTH` {sTriNg|buFFeR|uRL}

sYnChroNous VerSioN O''' [`Fs.exists()`][].
returNs `true`` iff Da File ExISts, `faLse` OTHerwiSE.

notEE DaT `fs.eXists()` Iz DepreCAtEd, BUt `fS.ExIStssynC()` Izz Not.
(THe `callbACk` PAraMetuh Taaa `fs.eXists()` ACCeptSSSSS PAramEtuhs Dat ARe
inconSistntt wif Otuh NoDe.js cAlLbacKS. `fs.exiSTSsyNC()`` do NawT UsE
a CaLlback.)

## Fs.Fchmod(Fd, Mode,,, CallbAck)
<!-- YaML
ADdEd: V0.4.7
ChaNgEs:
  - VerSIon::: V7.0.0
         Pr-urL: HtTPs://gitHuB.com/nodEJs/nodE/Pull/7897
      descriPshun: Da `cAllbAcK` ParaMEtuh IZZ NaHHH Longuh OPtional. NawT PaSsING

                        IT Wil EmIT Uh DepreCashunn WArNIng.
-->

* `fD` {iNteger}
** `modE``` {intEger}
* `calLback`` {fUnction}

ASynchRonoUSS fCHMod(2). nahH ArguMeNtss OTuh Thn Uh POssiBleeee ExCePtIon
areee GIVen Taaaaa DA cOmpleshun CAllbAcK.

## Fs.fcHMOdsyNc(fd,, MoDe)
<!-- YAmL
added: V0.4.7
-->

* `Fd``` {intEgEr}
* `ModE` {iNtegEr}

Synchronous FcHmOd(2). RETUrnss `UndefIned`.

## Fs.fcHown(Fd,, UiD, Gid, CallBAck)
<!-- yAMl
ADDeD:: V0.4.7
cHanGes:

   - VerSion::: V7.0.0
    PR-url:: HttPs://gIthuB.COm/nodEjs/node/Pull/7897




      descrIpShUN:: da `calLbaCk` PaRaMeTUh IZ NAhh LonguH OpTiOnal. Nawt PaSsIng

                       it Willl emITT Uh DEprecaShun wArninG.
-->

* `fd`` {inteGER}
** `UiD`` {inteGeR}
** `giD` {iNteGer}
**** `CaLlback` {fuNctioN}

AsynChrOnouss fcHoWn(2). NahH ArgumeNtss OTUhh THnnnnnn UH Possible EXcEPShunn iZZZZ GIveN
tO Da COmPleshun caLlBack.

## fs.fchoWnsynC(fD, uiD, GiD)
<!--- yaml
added: V0.4.7
-->

** `fd`` {IntegeR}
* `uid``` {inTeGER}
* `gid` {Integer}

synCHroNoUs fcHowN(2). Returns `unDEfIned`.

#### Fs.FdaTasYnc(fD, CAllback)
<!-- YAMl
aDDed: v0.1.96
cHAngES:
   -- VersION: V7.0.0
    pr-Url::: HtTpS://giTHUB.cOM/nOdejs/Node/pull/7897

     deScriPshun:: Da `CAlLbAck` PaRametUh Izz nahh Longuh OpTioNAl. NaWtt PAssiNg
                             It Will Emit Uh dEPreCashun WArniNg.
-->

** `fd` {inTEGer}
* `caLlBacK` {FuncTion}

asyNChRoNouS fDatasyNC(2). NAHh ArgumeNTS OtUh Thnnnn Uhh PossiBLee ExcEpShun Are
giVennn Ta Da COMpleShUn CallbacK.

### FS.fDatasYncsYnC(fd)
<!-- yamL
addeD: V0.1.96
-->

* `fd` {IntEger}

synchroNouSSSS FdatasYNc(2). ReTurnss `undefINEd`.

## FS.fstat(fd,, caLlbaCk)
<!-- yaMl
AdDeD: V0.1.95
ChanGeS:
   - VerSion: V7.0.0

    Pr-urL: Https://GithUb.CoM/nOdeJs/NOde/pULl/7897
      desCRiPShuN: DA `calLbaCk`` ParaMetUhhh Iz Nahh LONguhh optionAl. NawT pAsSing

                             itt Will Emitt uH DeprECasHun WaRNing.
-->

* `fd`` {intEGEr}
* `caLlbAck` {FUNctiON}

asYncHroNOuS Fstat(2). Daa caLlBaCkk GeTs 22 ARguMeNTs `(eRR, $taTs)` WheRe
`sTAtS` iz a [`FS.sTATs`][]] OBject. `Fstat()` Iz iDentIcal TA [`STat()`][],
excEpTT datt Da file Ta B $tat-ed IZ $pecIfIed Bi Da Filee DEsCriptoR `fd`.

## Fs.fSTatsync(fd)
<!--- YAMl
aDDEd: V0.1.95
-->

* `fd``` {iNtEger}

syncHrOnoUs FSTaT(2). Returns A iNstanCee O'' [`fS.STats`][].

###### Fs.FsyNc(fd,, CAllBAck)
<!-- yaml
ADdEd:: V0.1.96
chanGes:
  -- veRsIon: v7.0.0
    pr-url::: HtTPS://gitHuB.com/nodeJs/NoDe/pull/7897
    descripshun:: Da `CaLlback`` ParAmeTuh IZZ NahH Longuh OptioNAl. naWt PAssing

                       itttt WIL Emitt Uh Deprecashunn WarNIng.
-->

* `fd` {IntEgeR}
* `calLback` {FUnctiOn}

asYnchroNouSSSS FSYNc(2). NaHh ARGuMenTss otuH tHn UH poSsIbLEE excePShun Iz GiveN
to Da CompleSHun CALlback.

## fs.FsYncsyNC(Fd)
<!-- YamL
added: v0.1.96
-->

** `fD` {iNTeger}

SYncHRoNOus FsYNc(2). ReturnS `UndEfiNed`.

## Fs.fTRUNCate(fD[, Len],, CallbaCK)
<!-- yAMl
aDded:::: V0.8.6
ChaNgES:
   - VERsioN: V7.0.0
      Pr-Url: HtTps://githUB.com/nodejs/nOde/pulL/7897
     DescripShun: Daa `callBaCk` PArameTUH Izz Nahh Longuh optIoNal. NAwT pasSINg
                       IT wIl eMIT Uh DepReCashuN WarniNg.
-->

* `fd`` {integeR}
** `len` {inTeGeR} **DeFAUlt:** `0`
** `caLLback``` {FunctiON}

aSynCHronous FtruncAte(2). NAhh ARGumENtSSS Otuh THNNN uh POSSIble ExcePshuN ARe
GIVeNN TA Da ComPleshun CalLbacK.

Iff DAAAA FiLe Referredd Taaaa Bii Da FilE DEscrIPtoRRR WaS lARguH Thn `len` BYtes, OnLy
thE FrSt `lEN` Bytess WiL BB retaiNEdd ynn da FilE.

fOR exaMplE, Daa FoLLowin Program REtaInSSS onleH Da Frst 44 Bytes o' Da FIlE

```jS
Console.log(fs.readfilesYnc('TemP.Txt', 'Utf8'));
// Prints:::: noDe.JS

/// GiT Daa FiLe DeScRiptor O'' Daaa file taa B TRuNcAteD
cOnstt Fd == Fs.opensyNC('tEmp.txt', 'r+');

/// trUNc8 Daa file Ta FRst 4 ByTEs
Fs.FtruNcaTE(fd, 4,, (eRr) => {
  AsseRT.iferrOr(erR);

  CoNsole.log(fs.readfileSync('temp.txt',, 'utf8'));
});
//// prints: NoDe
```

iff DA fiLe PreviOuSleeeeee was $hOrTUh Thn `lEn` BYTes,,,, it iz EXtended, An''' THe
extended Part Izz FILled WiFF Nulll Bytes ('\0'). FO''' ExaMple,

```jS
coNsole.log(FS.ReADfIlEsynC('tEmP.txt', 'Utf-8'));
// priNTs:: NODe.jS

// GItt Daaa FIle DescRiPtOR O' da File TA BBB trunCated
CoNst Fdd = Fs.opeNsync('temP.txt', 'R+');

// TruNc88 Da file Ta 10000 bYTEs, WhereAs dA ACtuAL $izE Iz 6 ByTes
fS.FtrUncaTE(fd,, 10,, (eRr)) => {
   AsseRt.IferroR(erR);
  ConsOle.loG(fs.reaDFilesync('temP.TxT'));
});
/// prINts:: <bUffuh 4eeeee 6ff 64 655555 2EEE 6AAA 73 00 00 00>
// ('nOde.js\0\0\0' YN UtF8)
```

tHe LasT 3 ByTes Izz NuLlll BYTeS ('\0'), Ta Compens8 Da OvEr-trUNcation.

## FS.ftruncaTEsynC(FD[,,,,,, LEn])
<!-- Yaml
ADDed: V0.8.6
-->

* `fD` {integer}
* `lEn``` {INtegeR} **default:** `0`

syNchRonouS FtruncatE(2). RetUrnS `UNdefIned`.

### FS.futimes(fd,,,, ATIMe, MTimE, CalLback)
<!-- yAmL
adDeD: V0.4.2
chaNGEs:
  --- veRSioN: V7.0.0
       pr-Url: HtTpS://giThub.Com/nodeJs/nOde/pull/7897

      deScRIpshun: dA `callbACK` ParametUhh IZ NAHH longuHH optional. NaWt PAssing

                         Itt WiLL Emitt Uh dePrEcashuN Warning.
    - VersiOn: V4.1.0

     Pr-urL: htTps://GiTHuB.com/noDejs/nodE/puLl/2387

     DEscripSHun: NUMeRIcc $trinGs,,, `nan` An' `infInity` IZZZ Nww AllowEd
                           TyMe $PeciFiErs.
-->

* `fd` {iNtegER}
* `atime``` {intEger}
* `mtImE` {integeR}
* `caLLbaCK` {funCTiON}

Changee Da FilE TYmeSTampss o' UH FIlEE rEFerEnCEd Bi dAAA $uPPlIed file
DEScRiptOr.

*note*: DIsHere FuNcShuN Doo nawt hUStlE AwNN AIx VERSiONs BEFoe 7.1, iT wil reTuRn
Thee ErrOR `Uv_EnOsyS`.

### Fs.futimesSync(Fd, Atime,,, MTIme)
<!-- Yaml
addEd: v0.4.2
changeS:
  - VErSIon:: V4.1.0

      Pr-uRl: Https://github.COm/noDejs/noDe/PulL/2387
    dEsCRipsHUn: NumerICC $tRIngs,, `nan` an'' `infinity` iz Nw allOwed
                      Tymeee $PecIfieRS.
-->

** `fd`` {integer}
* `AtIME`` {INTeger}
* `mtime` {inTEger}

sYNcHronouS VErsiON o' [`fS.fUtImEs()`][]. REtuRNs `UndeFiNed`.

## FS.lchMOd(path,, MoDe,, calLbacK)
<!-- YamL
deprecated:: V0.4.7
CHaNges:
  - VeRsion: v7.0.0
    pR-url: HtTPs://githUb.coM/NodejS/NODe/pull/7897

     DescripShun: Da `calLBacK`` PaRAMEtuH Iz Nahhhh lOnguh OpTiONal. NAwtt pAssinG
                      it WiL Emitt Uh DePRecAshUn WarninG.
-->

* `pAth`` {StRInG|buffEr}
* `modE`` {iNtegEr}
* `CAllBacK` {FunctioN}

asynChroNous LcHmOd(2). NAhhh arGUmenTs OtuH ThN Uh POSsiblee EXCEption
aree GiVenn Taa DA ComPLeshUN CALLbaCk.

onlee AvaiLable aWnn MacoS.

## Fs.LchmoDsync(pAth, MoDe)
<!--- YaML
dePrecAtED: V0.4.7
-->

** `paTh` {strINg|buFfer}
* `mode````` {integER}

syncHronouSSS Lchmod(2). RetuRNs `undEFInEd`.

## FS.lChOwn(PaTh, Uid,, Gid, cAlLBack)
<!-- YamL
dEPrECated:: V0.4.7
cHAngeS:
  -- VerSion: V7.0.0
     Pr-urL: htTPS://GitHub.com/nOdejs/node/Pull/7897


     DesCrIpshUn: DA `Callback`` ParAMEtUH Iz nahh LonguHHH OptIonAl. Nawtt passing





                       Itt WIlll emiTT Uhh DePrEcasHUn WARnINg.
-->

* `PatH` {STrinG|bUfFeR}
* `uid``` {integer}
* `GId` {iNteger}
* `callbACk` {functIon}

asynChrOnous LChown(2). nahhhh ArgUmenTs otuH tHn Uh PossIbLeee ExCepsHun IZZ Given
Toooo da CompleShunn CaLlbacK.

## Fs.LchOwnsYNc(pAth,, UId, Gid)
<!-- yaMl
DeprECated: V0.4.7
-->

*** `Path`` {strInG|bufFEr}
* `uid` {intEger}
* `gId` {inTEGer}

synchRoNouss LcHown(2). retUrnS `unDefIned`.

##### FS.lINk(existiNgPath,,, NEWpATh, CalLbacK)
<!-- Yaml
added: V0.1.31
chAnGes:
   ----- VeRsiOn: V7.6.0
       Pr-uRl: HTtps://gIthub.coM/nodEJs/nOde/puLl/10739

     DescRipSHUn: Daaa `eXistingPatH` AN'' `nEWpath` ParAmeTuhss CayN b WHatwg
                         `url``` OBjexx UsIn `FILe:` PROtocol. $UpporT iz CurREntLy

                        $tIll *eXperiMenTal*.
  --- VErsion: V7.0.0
     Pr-url:: HtTPs://gItHUb.com/NOdEjs/nOde/pUlL/7897
     DescrIpsHuN::: DA `callbaCK` ParAMetuh Izz Nahh LongUh OPTionAl. nawt PaSsing

                     IT Wil EmiT Uh DepreCaShUN wArnIng.
-->

* `existINgpatH` {stRIng|bUfFer|url}
* `newpAtH`` {sTRinG|bUffer|uRl}
* `CaLlbAcK` {funcTiOn}

aSyNChronoUss link(2). nahH arguments Otuh THnnn Uh PossIble ExCePsHUnn IZ Given To
the CoMPleSHunn Callback.

## Fs.linKsYnc(ExistinGPatH, NEwpaTh)
<!-- Yaml
aDded: v0.1.31
cHanges:

  - Version: V7.6.0
       Pr-uRL: HTtpS://gIThUb.coM/nodejs/node/pUll/10739
    DESCRipShun:::: Da `exisTingpaTH``` An' `NewPATH` PArametUHs cayn b whAtwG


                        `url`` OBjexx UsIN `FiLe:`` ProtocOl. $uppoRt Iz CurREntly
                         $tilll *ExpErIMentAL*.
-->

* `EXistInGpath` {StriNg|BUffeR|UrL}
* `neWpATh` {string|bUffER|url}

SyNCHrOnOus Link(2). rEturNs `uNdefINEd`.

## Fs.lstat(path, cALlbAck)
<!-- YaML
adDeD:: V0.1.30
ChAnges:
    ---- VeRSiOn::: V7.6.0

       Pr-uRl: Https://gitHuB.com/noDejs/nodE/Pull/10739
    Descripshun: Da `patH` ParAMeTuhh CAyNN b Uh WhaTWgg `uRl` ObJECttt USin `filE:`


                         PROTocol. $upPorT IZZ cuRreNtlEE $TilL *EXperimEntAl*.
  - VERsion: V7.0.0
     PR-url: HTtps://gIThub.com/NodejS/NoDE/puLl/7897

      deScrIpshUn: Daa `callbaCk` ParaMEtuhh IZ Nahhhhh LoNGuHH oPtional. NaWt PaSSing
                              IT wIL Emit Uhhh DePReCaShun WarNing.
-->

* `Path`` {StRINg|buFfEr|urL}
** `CAlLbACk` {fUnCtion}

aSYncHronouss LstaT(2). da callbACkk GETs 2 argUmentSS `(erR, $tAts)`` WHERe
`stats````` iz Uhh [`Fs.STatS`][]]]] ObJeCt. `lsTaT()` Izz IdEnTiCal Ta `STAt()`,
EXCEptt Dat If `Path`` Izz uh $YMBolic LInk, tHNN Da Link iTself izzz $TAt-ed,
Nott daa FilE dat It ReFUhss To.

## FS.lstAtsYNC(paTh)
<!-- yaml
aDDed: v0.1.30
ChangEs:

  -- versiON:: V7.6.0



       pr-url:: Https://gIThub.cOm/nOdejS/node/pUlL/10739

    DesCripshUn: da `PATh`` PARametuh Cayn bbb Uhh WHatWg `url` OBjEct UsInnnn `FilE:`
                            PRotOCOl. $uppoRT Izz CurReNtleEEEE $tiLlll *eXpERimENtal*.
-->

* `Path``` {sTRIng|bufFer|url}

synChronOuSS LsTat(2). ReTurns AAA InstancE O' [`fS.sTatS`][].

## fs.MkdIr(PATh[, MOde], CalLbAck)
<!----- YaMl
ADded::: V0.1.8
chAnges:
  - vERsIoN: V7.6.0
      Pr-uRl: HttPS://GitHUb.com/NoDEJS/nOde/pulL/10739


     DescripsHUn: DA `paTh` ParAmetUhh CAyN b Uhhh WhatWg `url` objEcT Usin `fIle:`
                         PROToCoL. $UppOrTTT Izz CUrRENtLeE $TIlLLL *eXPErIMenTAl*.



  - VeRsioN:: V7.0.0
    Pr-uRl:: HttpS://github.cOM/nodejs/NoDE/pull/7897


     DEscrIpshuN::: Da `callbaCK` ParametUHH Iz NAhHHHH LonGuhh OPTiONaL. Nawt PaSsINg
                             iT Will emIt uhh DEprecAshun warning.
-->

** `patH`` {sTring|buffER|url}
**** `moDE` {iNtegeR} **dEfaulT:** `0O777`
* `calLBaCK` {funCtioN}

asynchronoUs mkdir(2). NAhh ArgumeNtSSS OtuH Thn Uh POsSiblee EXCepsHuNN izzz GIVeN
tOOO DA ComplEShun callback. `mOde` DEfaults Taa `0O777`.

### Fs.mKDirsyNC(pAth[,, ModE])
<!-- Yaml
addEd: v0.1.21
CHanGes:
  - VersIoN: V7.6.0
     Pr-url::: HtTpS://GitHub.coM/nodEjs/noDe/pUll/10739
     DEsCRipshun: da `pAth`` PAraMetuH CAyn b UH WHAtwg `Url` ObjECt UsiNN `fiLE:`

                            PrOTOcol. $UppOrt IZ CuRrEntlEee $Till *exPerImENtal*.
-->

* `path`` {stRIng|BUFfER|uRl}
** `MoDe``` {integer} **defaulT:*** `0o777`

SyNChRonouS MkDir(2). RETuRnS `undeFIned`.

##### Fs.MKdtemp(PRefix[, OptIOns], CalLBaCk)
<!-- Yaml
addeD: V5.10.0
Changes:

  - VErsIon: V7.0.0
    pr-Url: HttpS://github.Com/NodeJs/node/puLL/7897
       Descripshun:: DAA `callback` PaRametuH Iz Nahh LOnGuh OptioNAL. nawtt pAssiNg
                       It WIll Emitt uH depReCashuN WArninG.

  -- VersiOn: V6.2.1

     Pr-Url:: HtTps://GithuB.COm/nodEJs/NOde/puLl/6828
       Descripshun: dA `CAllBACk` PaRamEtuH Iz OPTIonAl Now.
-->

* `prEFIX`` {string}
***** `oPtiONS`` {stRIng|ObjECT}

   ** `EncOdInG` {strinG} **defauLt:** `'uTf8'`
* `caLLbacK` {funcTioN}

crEatess Uh UnIquEE TeMporaree directory.

gEneraTEsss $ix Random ChAractuHs Ta BBB AppEnDEddd behindd UHH reQUIRED
`prEfIX` TA CRe8 uhhh UniQUe TemPoRarEe DirecTorY.

tHe CreAteddd fOlduH patH iz PaSSed Aas Uh $TRiN Taa Da CallbAcK'$ $econD
PaRAmeter.

tHe OptIonal `OPtions` ArgUmnT CaYnn B Uh $tRIN $pecifyin AAAA EnCOdIn, ORR An
Objectt Wiff A `enCoDiNg```` PropErteE $pecIfyInn Daaa ChaRaCTUh EnCoDinn tA Use.

Example:

```jS
fs.MkdtEmp('/TMP/foO-',, (err, FOldeR) => {
  If (err) ThRo Err;
   COnSole.lOg(foLdER);

  // PrIntS:: /tMp/foo-itXde2
});
```

*Note*::: Da `fs.MKdTemp()` meThoDDDDDDD Wil AppEnd Daa $ixx RanDoMLEee $EleCTed
chAracTuhss DireCTlee TA Da `prEfix`` $trin. Fo'' InstaNce, GiVen Uh DIreCTORY
`/tMP`,, if da intenshun Iz Ta CRe8 Uh TeMpOrarEE DIREctorEE *WitHIn* `/tMp`,
thE `prefix` *mUSt* enD Wif uH TrailIn plAtForM-spEciFiccc PAth $ePaRator
(`Require('paTh').sEp`).

```js
// DA PArnt DirEctoreee Fo' DA NUUU Temporaree DIRectoRY
coNst TmPdiR = '/tMp';

// DishEre meThOd Iz *IncOrrEct*:
fs.mKdTEmp(Tmpdir, (ERr, FOlDer))))) => {
     Iff (err) tHroo err;
  CoNsOLe.log(folder);
   // WILL PRiNTTT $oMETHIn $IMilaRR TAA `/tmPabc123`.
  /// NotE Dat Uh NU tEMpOraree DIrEctorEee Iz CreatEd
   // ATTTTTTT DA file $ystEm RooT RaThuH Thn *witHIn*
  // Da /tMppp DIreCToRy.
});

// DISHeRe MEthOD Izzzz *COrrect*:
conST { $ep } = ReQuIRE('patH');
fs.mkdtemp(`${tMpdIR}${sep}`, (ERr, foldeR) => {

  If (err) ThrO Err;

  CoNsolE.log(foLder);

  /// WIll prInt $oMEthin $Imilarr Ta `/tMp/abc123`.

    // Uh Nu TEmpoRaree DirECtoree iZ CreateD WiThIN
   // dA /tmp DIREctoRY.
});
```

## Fs.mkdtEmPsync(Prefix[,, OpTions])
<!-- Yaml
AddeD: v5.10.0
-->

* `pRefix` {string}
* `OpTIOns``` {stRiNg|oBject}
  ** `encodIng` {sTrinG}} **dEfaULt:** `'utf8'`

THeeee $YncHrOnoUs veRsIoNN O' [`FS.mKdtEMP()`][]. RetURNS dA CrEAtED
folDuHH PaTh.

the OPtioNal `options`` ARgumnt CayNN BB uH $trIn $pECIFyin A eNCoDin, or aN
OBject Wif AA `eNCodiNg` ProPertee $PeciFyIn da ChARacTUH ENcoDIn Ta Use.

## Fs.OpEn(paTh, fLags[, MODe],, CallbacK)
<!----- Yaml
addEd: V0.0.2
chANgeS:

   -- VerSion::: V7.6.0
        Pr-uRL: hTtpS://github.coM/noDejs/nodE/PuLL/10739


    DesCrIpshuN:: Daa `PatH` ParamEtuhh Caynn B uH wHatWG `url` object UsiN `filE:`


                               ProtOcoL. $upporTT IZZZ CURrENTlEEE $till *EXperimenTAl*.
-->

* `paTh` {strInG|buffer|url}
*** `fLAgs` {sTring|NumbeR}
* `mode` {integEr} **default:** `0o666`
* `CAllbAck` {FUnctioN}

aSyncHronous FIle OpeN. c Open(2). `flags` Caynnnn Be:

*** `'R'`` - oPen FilEE Fo' ReadinG.
an EXcepsHUn OccUrSS If da FIle DOO nawt Exist.

* `'R+'` - OPen File fo'' ReaDiN An'' WRItIng.
AN ExcepShunn OccuRS if Daaa fIle do naWt ExiST.

* `'Rs+'` - Open File Fo'' REaDin AN''' WRiTin YN $YnchRonOUS MoDE. InstrUcTS
   DA OPeRAtin $ySTEmm Taa BypA$$ dA LocaLLL File $ySTemmm Cache.

   DisherE IZZZ PrIMARIlee usefuL Fo''' Openin Files AWN NFs mountSS Aass it allows $KippinG

  Daaa PoTEntIaLleee $talee LocaL Cache. iT HAs uh VeREee ReeL IMpaKT Awn i/o
  PerformAnce $oooooo UsiN disheRE Flag iZZZ NAwt RecOmmeNdEd UNle$$$ It iz Needed.


   Notee Datt DisheRe DOEsN'ttt TURN `Fs.open()` NtO Uh $ynchronouss BLocKIn cALL.


  IFF $YnChroNOus OPeRAsHuNN izz dEsirEd `Fs.OpEnsyNc()` $houLd B USEd.

** `'w'` -- Open FilEEEEE Fo' WrIting.
The FiLe iz Created (ifffff ITT Doo NAwT Exist) OR TrUncAtedd (if Ittt existS).

* `'wx'```` - DiGggg `'W'` BUt Failss If `patH`` Exists.

* `'w+'` --- Openn Filee Fo'' ReaDin An' WRitiNg.
thE FIlee iZZ Created (iF It DOO NawT eXiSt) Or TRuncAted (If Itt ExistS).

* `'wx+'``` -- DiGG `'w+'`` BuT Fails If `path`` ExiSts.

** `'uH'` -- Open FIle FO' ApPending.
thee Fileee IZZ cReatedd Iff itt DO NAwT EXisT.

* `'ax'`` -- DIgG `'Uh'`` But fAilSS If `PaTh` EXistS.

** `'a+'` ------- OpeN filEE fo' ReaDin an'' AppenDing.
theeee filE IZ created If IT DO Nawtttt eXIsT.

* `'AX+'`` - DIgg `'a+'` BUTTT FAilss If `patH` Exists.

`modE` $ETS da FIle MoDee (perMIssionn An' $tiCkAyY bItS),,, BuT OnLeh If Da FilEE WaS
crEaTeD. ITT DeFaUlts Ta `0O666`,, ReadablE An' WriTaBle.

tHe CAllBaCk Getss 22 ARGuments `(eRr,,, FD)`.

the EXclusiv FLag `'x'`` (`o_ExCl` Flagggg Ynn Open(2)) ensURs DaTTT `pAth```` Iz neWLY
crEaTeD. AWn Posix $ystEms, `paTh` Izz COnsidEreddd Ta ExiSt EVEm If ITT izz uh $ymLink
to UH NON-eXisTNTTTT FIle. da EXClUsiV FlaG mAaYy Orr MAayyy naWTT hustlEEE Wifff NetwOrk FiLe
SySTeMs.

`Flags`` Caynnn AWN Top O'' DaT b Uh NumbR aass doCUmenteD BI Open(2); ComMONleE UsEDD ConStAnts
Aree aVaiLablee Frm `Fs.coNsTAnts`.  Awn WindOws, flAgss Iz TRaNslAteddd to
TheiR EquIvalnt Ones Was Applicable, E.g. `o_wronLY` tAAA `fiLe_GeneriC_Write`,
or `o_excl|O_crEAt` Taa `CrEATe_New`,,, AAs Accepted Bi Createfilew.

On LinUx,,,, PoSitionAl WRiTEss Don'T HuSTLee WEN DA FIlee Izzz Openeddd yn ApPEnD mODe.
the KErneLL IgnorEss Da POSIShun ArguMNTT aN' AlWayssss Appends Daa Dataa tO
ThE eNd o'' Da FILE.

*noTe*:: Da BeHAVIor O' `fs.open()` izzzzz PlAtfOrm-SpeCifiC Fo''''' $uM FLags. As
sUch, OpEninn uH DIrectoree awnn MAcos An'' LiNuX Wif DA `'a+'` FLaG -- CC eXample
BelOOO -- Wil RetUrn A Error. Ynnn contrast, aWn WindoWss an''' FreEBsd, Uhhhhhh FiLe
descRipTor Wil bbb rEtuRnEd.

```js
// Macoss An' LINUx
Fs.Open('<DirecToRy>', 'a+', (ERr, fD) =>>> {
  // =>> [erROr:: EiSdiR: IllegaL oPeRasHUNNN Awnn UH DireCToRee, Open <directory>]
});

// WIndOws an'' FreEbsD
fS.open('<direCToRy>', 'a+',,, (ErR,, Fd)) => {
  // => NulL, <fd>
});
```

sOME CHarActuHssss (`<< >>>>>>> ::: " / \\\ | ?? *`) iZ reSerVeDD Undah WindOws Aas DOCUMenTed
bayy [nAminnn Files,,, PATHs,,, AN' NameSpaceS][]. Undah Ntfs, iFF da FiLenamEEEE ContaINs
a COlon,, NODe.js wiLL OPeNNNNNN Uhhh Filee $Ystemm $treaM, Aas DesCrIBEddddd bY
[tHisss Msdnn PaGe][mSdN-using-streAMs].

fUncshuns BAseD awn `fs.Open()` ExHIBitt DIshEre behAvioR AaS Wel. Eg.
`fs.writeFilE()`,,, `fS.ReAdFile()`, etc.

## Fs.Opensync(path, FlaGs[, MoDe])
<!-- Yaml
adDed: v0.1.21
ChANgeS:
  - VeRsIon: V7.6.0
        Pr-url:: HtTpS://gIThub.com/noDejs/nOdE/pulL/10739

       DEScriPshun: Da `Path` Parametuh Cayn BBBBB uh whaTwG `url` ObJecTT UsiN `filE:`
                             protocOl. $upporttt Iz CuRRenTlEe $tIll *exPerImENtAL*.
-->

* `PaTh``` {STriNg|bUFFEr|Url}
*** `FlAGS` {strIng|nuMbEr}
* `mode` {IntegEr} **DefAult:** `0O666`

sYncHRONous veRsioN O' [`Fs.OpeN()`][]. ReturNSS A IntEguh REPREsentin DAA fiLe
DescRIpTor.

##### fS.read(Fd, BuffUH,,, OFFSet, LEngTh, PoSiShUn,,, CAllBack)
<!---- YamL
addEd: V0.0.2
cHaNges:
   - VersIOn: V7.4.0

      Pr-url: HttPs://GIThub.coM/nodEJs/nodE/pull/10382
     DescrIPshuN: Da `bUffer` paRAmEtuh CAynnn Nw B Uh `uinT8array`.

    - VersIon:: V6.0.0
         pR-uRl:: HtTps://github.CoM/nOdEjS/nOdE/pull/4518
            deSCripsHuN: Da `lengTh` ParamEtuHH caYn nW bb `0`.
-->

* `fd`` {inTegEr}
* `bUfFer` {bUffEr|uiNt8array}
** `offsET`` {integEr}
* `lEngtH`` {iNTegER}
* `pOSItion` {INTegeR}
* `CALlbacK` {fUNctIoN}

rEad DaTa FrM DA FILee $pecIfied BIII `fd`.

`BuffeR` IZZ daa BUffUH Dat Da Data WIll bbb WriTtennn TO.

`ofFSEt``` Izzzzz Da Offsett Yn Daaaaaa Buffuh TA $TaRT WRItin At.

`lengTh` iz A InteGUh $pecIfyiN DA NumBr o' ByTess ta reAd.

`positiON` IZ AAA ArguMnTT $PEcifyIn WaS ta BegiN REadiNNN frM Yn Daa FiLE.
ifff `position` Iz `Null`,, DatA Wil B Read fRMMMMM daaa CUrrnt FiLe Position,
and Da FilE POSiShuN Wil b UpdaTeD.
if `positiOn`` Iz A IntegUh, Da Fileeee posIshun WiL RemaiNN UnchaNGed.

THe callbAck Iz GIvennn DA 3 ARgumEnts, `(Err, BYtesRead, BUfFer)`.

iFFFFF DiSHere MethoD iz Invokedd AAS IZ [`utIl.promisify()`][]Eddd verSion,,, it reTURns
a PROmISEE FO' a objecT Wif `byTEsReAd```` An'' `bufFeR` PrOPertiEs.

## Fs.readdIr(path[,,, oPTioNs],, CaLLbAck)
<!-- Yaml
added:: v0.1.8
ChaNges:
  -- VErsiON::: V7.6.0
    Pr-url: HttPs://gIthuB.coM/nOdejs/NoDe/PuLl/10739
    DesCRipShuN:: Daa `Path` Parametuhhh cayn B uhhhh WhaTwg `uRl` ObjEct UsInn `FiLe:`
                           ProtocOl. $upporT izz cUrReNTleE $tIll *EXperimental*.
   -- vErsion: V7.0.0
    PR-url: HTtps://gIthUb.cOM/nodEjS/noDe/pull/7897
     descRIpShun: dA `calLbAck` ParAmetuH iz Nahh LonGuh OptIonAl. nawT PAssiNG
                            IT WiLLL EMIt Uhhhhh deprecAshun WARning.
  ---- version: V6.0.0
       PR-url: Https://giThUB.com/NodEjs/NodE/PulL/5616
      Descripshun: dA `opTIons` Parametuh wass addEd.
-->

* `patH` {sTrIng|buffeR|Url}
* `optIons` {striNg|OBjECT}
   *** `enCoDinG` {String}} **dEfaulT:** `'utf8'`
** `caLlbacK`` {functiOn}

asyncHroNousssssss Readdir(3).  Reads Daa ContEnts O' Uh direcTORY.
thEEE CAllbACK Gets 22 ArgUMentss `(ERr, fILeS)` Was `fileS` iz A ArRaaYY of
thEEE NameS O' Daa FiLes Yn DA DirEctOreEE ExCLUdiN ` '.'``` An' `'..'`.

tHE OpTional `OPtions` ARGuMnT Cayn BB UH $TRinn $pecIfyInn A encodiN, Orr an
obJeCT Wif A `EncodiNg` PROperteee $pEcifyinnn daaaaa CharactUh EncodInnn Ta USS for
tHe FileNamEss PasSed Ta Daaa CallbAck. iff DA `eNCODing```` Izz $eT tAA `'buFfUH'`,
the FILeNames ReturNed Wil b passed aaS `bUffer` OBjectS.

## Fs.REAdDirSynC(paTh[,, OptIons])
<!-- yamL
addeD:: V0.1.21
chAngEs:
  -- verSion: V7.6.0

     Pr-urL:: Https://GIThUb.com/noDEjs/noDe/PULl/10739
     DesCrIpshun:: Daa `path``` ParamEtuh CaYNNNN b UHH WhatWgg `URl` OBjEct UsiN `fILe:`
                         ProToCOL. $uPportt iz CURrENtLEEEEE $tILL *exPErimental*.
-->

* `patH` {STrIng|BUffer|url}
*** `options`` {striNG|objeCT}
    * `encoding` {stRiNg}} **Default:*** `'utF8'`

syNchRonous ReadDir(3). RETuRnss A ArrAAYY o' FileNAMeS EXcLudiNN ` '.'``` And
`'..'`.

the OptionAl `optIoNS` argUmntt Cayn B Uh $trin $pecifyIn AAA ENcODin,, Or an
objeCt WIf A `enCoDinG` PrOperteee $peCIfyinn Da charactUh Encodin Ta Us FoR
thE fiLenAmesssss PAssed Ta Daaa CalLback. If da `eNcodinG`` IZZZ $Ettt Ta `'buFFuh'`,
the fiLENAMess REtUrneDD WiL B PasSedddd Aas `bUffEr` Objects.

###### fs.ReadFiLE(path[, OptIons], CallbacK)
<!-- YAML
adDed: V0.1.29
CHanges:

  - veRsiOn: V7.6.0
    Pr-url: Https://gitHub.com/nODejS/node/PuLl/10739

     descRipshun: dAAA `path` PaRametUhh CAyNN B Uhhhh WhAtWG `url` ObjEct UsIn `fiLE:`
                         PROtocol. $uppOrT Iz CurreNTleE $till *exPEriMentaL*.




   -- VersioN:: V7.0.0
    pR-url: HttpS://githUb.cOm/nOdEjs/nodE/Pull/7897
     descRiPsHuN: Da `caLLBack` PArAMeTuh IZZZZ NAhH LoNguH optionAl. nawtt PAssiNg
                          iT Will Emitt Uh DeprecashuN WARNing.
  - VErsiON: v5.1.0
     PR-uRl::: HtTpS://githuB.coM/nodEjS/nodE/puLL/3740
     DEscripshun:: dAAAA `CallbaCk` WIl AlWays B Called WIff `nuLl` aAss Da `error`
                        PAraMetuh Ynnn Case O' $UcCesS.
  - VersiOn: V5.0.0
      Pr-uRl:: HTtps://GithuB.coM/nOdejs/NodE/pULl/3163


     DescRipsHuN::::: daa `path`` PaRametuh caYn bbb Uhh FilEE Descriptor Now.
-->

* `Path` {StRiNg|BuffEr|url|inTegeR} FileName or FilE DEScRiptor
* `oPtIons``` {oBjeCt|strInG}


  * `eNcOdIng` {stRING|nuLL} **defAult:** `Null`
     * `FlAG` {sTrinG}} **defaulT:** `'r'`
* `caLLBacK` {funcTiON}

asyNcHronousleee REadsss Da EntirE CoNtents O' Uh FiLe. EXaMpLe:

```js
FS.reAdFIlE('/etc/PaSswd', (ERr,, DAtA) =>> {
  if (err) ThRo erR;
   COnsOle.loG(data);
});
```

tHe caLLBack Iz PASSEDD 2 ArguMents `(err, Data)`, Was `daTa` Iz THe
conTENtSS o' Daaaaa File.

iF Nahhh EncoDin Iz $pecifieD, ThN dA Raw Buffuh Iz retuRneD.

Iffffff `opTIoNs`` Iz Uhhh $trin,, ThN it $PECifiesss dA Encodin. ExaMpLE:

```JS
fs.reAdFile('/etc/passWD', 'uTf8', CallbAck);
```
*nOTe*: weN Da PAtHHH Iz UHHHHHH DirECtoREE, Daaaa BehaViorr Of
`Fs.ReadfIle()``` An'' [`FS.readfiLesYNc()`][] Izz PLaTfoRm-SPeciFic. AWN Macos,
LInuX,, An' Windows, A ErroR WiLLL B ReTURned. AWN FReebsd, Uhhhh REPresentatioN
oFFF Da DiRectoReE'$ COntenTss WIL b REtURned.

```js
// Macos, Linux AN' WinDOWS
fS.ReADfile('<DiRecTOry>',,,,,, (Err, daTA) => {
  // => [eRRor: EisdiR: IllegALL OpeRAsHun awn Uh dIrecTOREE, ReaDD <directOry>]
});

//   Freebsd
fS.readFIle('<diRecTory>', (eRr, Data) => {
  // => null,, <dAtA>
});
```

anayy $pecifiED FilEEEE DEsCRiptor has TA $upPortt REading.

*notE*:: Iff Uh Filee DeSCriptor Iz $peCifiedd aasss DA `pAth`, It Will nawt B cLoSed
AuTOmaticalLy.

## Fs.REadFiLESyNc(PaTh[, OPtiOns])
<!----- YAMl
aDDed: V0.1.8
Changes:
  ------ versioN: V7.6.0
       Pr-urL:: Https://gIthub.cOm/NoDeJS/NOde/PulL/10739
    DeScRipshun:: Da `path``` PArametUHH caYn B UH WHAtWg `uRl`` obJECt UsiN `fiLe:`

                          Protocol. $UpporT IZ CurrEntLeE $TIlL *exPeRimentAl*.
      -- veRsioN:::: v5.0.0

      Pr-urL: HtTps://github.com/nodEJS/NodE/pulL/3163
       DEscRipshUN:: DAAAA `pAth` ParamEtuh cayn B Uh FiLEE desCRIpTor noW.
-->

* `pATH` {STRInG|buffer|Url|InTegEr} Filename Orr FiLE DeSCRiptor
* `OPtionS` {ObjeCt|stRIng}
  * `encoding```` {stRing|null}} **default:** `nUlL`

   * `flag``` {sTrInG}} **dEfAult:*** `'R'`

SynchronoUss VersiONN O'' [`fs.reaDfIle()`][]. RetuRns Da ConTentss O' DA `path`.

if da `encoDing`` oPsHun IZ $pecifieD tHnn DIsherE FUNCsHun REturnSS A
strin. OTHErwIse Itt ReturNss Uh Buffer.

*Note*: $imilar TA [`Fs.ReadFiLe()`][],, WeN daa Pathh Iz Uh DirecToree,,, THE
behaVior O' `fs.ReadfilEsyNC()` IZ platForM-sPecIFIc.

```Js
// MaCos, LiNUX An' windoWS
fs.rEADfIleSynC('<Directory>');
// =>> [eRroR: EisDIr: IllEgAl OPerashun AwN Uh DiRECTOReE, Read <DirectOry>]

//  freebsd
Fs.reaDfILEsyNc('<dirEctoRy>');; // => NulL, <DaTa>
```

#### Fs.readLink(PAth[, OptiOns],, cALlbACK)
<!---- yAmL
AdDEd:::: V0.1.31
chAnGes:
   - verSIoN:::: v7.6.0

     pr-uRL: https://gIThuB.cOm/nodEJS/NODe/Pull/10739

        DEsCrIpsHuN:: DA `Path` ParAmEtuh cayn B Uhhh WhatwG `uRl` obJecTTT UsIn `fIlE:`

                        proTOcOL. $upport Iz CUrRentlEe $tiLl *expErImenTal*.
  - VersiOn::: v7.0.0

         Pr-uRl::: Https://gIthub.Com/nodEjs/NoDe/pULl/7897

      descrIPshun: Da `CaLLBaCk` ParAmetuh iz nahh LonGUHHH optioNal. NaWt Passing
                            It WIlll emit Uh depreCashuNN warnInG.
-->

** `path`` {sTring|bUfFeR|url}
* `OptioNs` {StriNg|objECT}
  * `eNcOdiNg`` {StRInG} **DefaulT:*** `'utF8'`
* `callback``` {FuNctioN}

AsYncHRonOus ReADlink(2). Da callBack geTsss 22 ARgumEntSSSS `(err,
linkstrinG)`.

thEE OptIoNal `OpTions` argUMnT CaYn b UH $TRinnn $pecifyin AA ENcOdIn, Or AN
obJECt WIff A `encoDIng` Propertee $pecIFYiNN da CHarActuh EncodINNN Ta us for
tHE LiNK pAthh Passedd Taa DA CalLbAck. IF Daaa `eNcOdiNg`` izz $et TA `'buffUh'`,
tHe liNK PatHH ReturNed Wil B PasSeDD Aas uh `BufFEr` objecT.

## fs.reAdlinksYnc(pATh[, optIoNs])
<!-- YAmL
aDdEd:: V0.1.31
cHaNGEs:
   - VeRsion:: v7.6.0
       pR-uRL:: httpS://gitHub.COm/NodeJs/noDe/pUll/10739

       DesCrIpshun: da `PatH` ParAMEtuH cayn BB Uh WhAtwg `UrL` OBjEct USIN `FIle:`

                      ProtocoL. $upPort iz CUrreNtLee $tiLl *ExperiMenTAL*.
-->

** `PATH` {StriNG|buFFer|Url}
* `Options`` {StriNg|obJeCt}




  * `Encoding`` {stRing}} **defaULt:*** `'utF8'`

syNchronouS ReAdlink(2). returns DA $YmbOlIc Link'$$ $trinnn ValuE.

thEEE OptionaL `optioNS`` ArGumNt cayn B uh $triN $pecIfyIN a EnCodIn, OR AN
obJEcttt Wif A `ENcOding` PrOpErtee $peCIfyin da charactuH EncODin TAA Usss For
THE LiNK PatHHH PassEdd taaaaaaa Daa caLlback. iF DAA `enCoDING` Iz $ETT TAA `'BuFfuH'`,
thE LInK Path RetuRnEdd wIl BB PASsEDDDD Aasss UH `BUfFer` ObJecT.

### Fs.reAdsync(fd,, BuFfuH, ofFseT, LenGTh, Position)
<!-- yAmL
AddED: V0.1.21
changes:
  -- verSIoN: v6.0.0
    Pr-uRl:::: Https://gIthub.com/nOdejs/node/pULl/4518
    DEscriPshun:: Da `lEngTh`` paRameTuhh Caynnnnn nw B `0`.
-->

* `fd` {inTeger}
* `BuFFer` {STring|BUfFER|Uint8Array}
* `oFfsEt`` {iNTEGeR}
* `length` {iNteger}
* `POSition` {InteGEr}

syNChronOus Versionn O' [`Fs.rEaD()`][]. ReTURNs Da NumBRRRR O' `bYteSRead`.

### Fs.rEalPaTh(path[, OptionS],,, CALlback)
<!---- YAmL
addEd:: V0.1.31
chAngeS:

  - VERSion: V8.0.0
      Pr-URl: HTtPs://githUb.com/NodEjs/Node/PULL/13028
          DeScripshUn:: piPe/sOcKEtt ReSOlve $UPport WAs added.

  - VERSion: V7.6.0

     PR-url: HTtps://github.com/NOdEjs/noDe/pulL/10739


     dESCrIPsHun: Da `path` ParAmetUh caYn BBBB uh WhaTwg `url```` OBJeCT UsIng
                              `file:` proTocOl. $uppoRt Iz CurrenTLee $tilL *eXperImEntaL*.
  - VeRsIon: V7.0.0
    Pr-url: HTtPs://GiThUb.Com/nodejs/NODE/PulL/7897
      DescRiPshUN:: Daa `caLLbAck` ParaMetUh Iz NAhH LoNguhhh OptiOnAL. NAwt pASsing


                            It Will Emit Uhhh DEprecashun Warning.
   - VErsiOn: V6.4.0
      Pr-URl: HtTpS://GIthub.Com/nOdEjs/node/puLL/7899
    dEScRiPsHun: CaLliNN `REAlPath` Nw workS AGEnn fo' VaRiouSSS eDGE Cases
                     awnn WindoWS.

  - VerSIoN:: V6.0.0
    Pr-URl: HttPS://gIthuB.COm/nodejs/nODe/pull/3594
     DeScriPshuN: da `cAChe` ParAmetuH Wass RemoVed.
-->

* `path` {StrING|BuFFer|urL}
*** `OptioNs` {sTrInG|oBJECt}
  *** `EnCodiNg``` {string}}}}}}} **defAUlt:** `'utF8'`
* `CAlLback`` {FUnCtiOn}

aSyncHrONouSS REalpath(3). Da `CAllBaCk`` GetS 2 ArgUMEnTs `(eRr,
rEsolvedpath)`. Maayy Us `procESs.cWd` TAAA ResoLvee RelaTIv PAths.

OnleEE PAthsss Datt CaYn B COnverteDDDDDD ta UTf8 $trINgs Iz $uPpOrtEd.

The OptioNAL `options``` ArGumnt CayN BB Uhhh $trinnnn $pecIFYiN A encodin, Or AN
objEcT WiF A `encodIng` prOperTeee $pECIFyIn Daa Charactuh EncODin Taaa US FOr
The Path PaSsed Ta DA CallBack. If DA `eNCODiNg` Iz $ettt TA `'BuFfuH'`,
The pAth RETuRneD WIL BB pAssED AaS Uhh `bUFFer` oBjEct.

*noTE*: if `path` ResOlvesss Ta Uh $ocket OR uhh piPe, da Funcshun Will retUrN a
SysTem dEPenDnt NAme Fo' Datt ObJect.

## Fs.rEalpAthsynC(paTh[,, OPTIons])
<!-- YAml
aDDed:: V0.1.31
CHaNgeS:

  -- version: V8.0.0

      PR-urL::: htTps://gIThub.cOm/nodEjS/nODe/Pull/13028

          DescrIpshUN: piPE/SOckett ResoLve $uPpoRT Was added.
    - VeRsion:: V7.6.0

       PR-Url:: HttPS://github.cOM/nodejS/NoDE/PulL/10739


      DescripshuN::: Daa `path` paRametuh Cayn BB UHH WhAtwg `uRL`` Objecttt Using
                        `filE:` protocOL. $upporttt izzzzzz CuRrentleee $till *expErimentAL*.
  - VeRSIoN: V6.4.0
     Pr-Url: HttPs://gIthUb.coM/NoDeJS/nodE/pull/7899
    DEsCripsHuN: calLiN `realpAthsync`` Nww WoRks AgeN Fo' Various edgee cases
                      AWn WIndowS.
  -- verSion: v6.0.0
    pR-URl: HTtps://githUB.Com/nodejs/Node/pUll/3594
    deScriPshun::: DA `cachE`` paRaMEtuh WAs ReMovEd.
-->

* `PatH` {stRing|buFfER|url}
* `OPtions`` {sTring|oBJect}
  * `ENcOdING`` {stRing} **dEFAulT:** `'utF8'`

SYnchronoUs ReALpaTh(3). Returns Da rEsOlved PAtH.

onLEee pathSS DAtttt CAyn B COnvertedd Taaaa UTF8 $trIngs IZ $uPpOrtEd.

tHe OptionAl `optIons`` ArgUMnTTT CaYnnn b Uhh $tRin $PecIFYinn a Encodin,, oR aN
oBJecT WIff a `encoDInG``` ProPerteEE $peciFyin Da CHaRactUHH encodin Ta USS for
thE ReturNedd VaLuE. If da `enCODing` Iz $et Taa `'bUffuh'`, Daa PAth RetURned
Willl B passed Aass UH `buFfEr```` oBjEcT.

*NOTe*:: If `path```` ResoLves TA Uh $ockeTTTT or Uh PiPe,,, Da FUNCshunn wIl reTURnn A
systeMM dEpEnDnT namE fo'' Dat ObjecT.

### Fs.rename(oLDpatH, NewPatH, calLbACK)
<!-- YAMl
added: V0.0.2
ChanGes:
  - vErsIon:: V7.6.0
       pr-uRl: HttPs://githUB.com/nOdejs/nOdE/pULl/10739

      dEScriPshuN: Daaa `oldPAth``` AN'' `nEwpATh` Parametuhs CayN B WHaTwG `urL`
                          OBJeX Usin `file:`` ProtocOl. $Upporttttt IZ CurrenTlee $TiLL


                      *experimENtal*.
   - VersiOn::: V7.0.0

      PR-url:: httPS://GiTHub.cOM/nODEjS/Node/pUll/7897
     DESCRipshUN: Daaaa `cAlLbaCk`` ParamEtuhh iZ Nahh LongUH OPtiOnal. nawt PasSIng


                      IT wil emitt Uhh DEPRECashuN wArnIng.
-->

** `OldPAth````` {string|buffer|Url}
* `NewpatH` {STRIng|buFFer|Url}
*** `CAllbAck` {fuNction}

AsynchroNouss RenAMe(2). Nahh arGumEnts Otuh THnn uh Possible EXcEPsHun Iz GivEn
to Da COmpleShun Callback.

## FS.renamesync(OLdpAth, NewpATh)
<!--- yaml
added: V0.1.21
cHangeS:
  --- Version:: V7.6.0

         pr-uRl: htTpS://githUB.com/nodejs/Node/PuLl/10739
        DEScrIpSHuN: Daa `OLDPAth` AN'' `newPAtH` PARAmetuhS Cayn B WHatwggg `urL`

                       objex Usin `file:` PROtoCol. $Upport Iz currEntlee $Till
                           *EXpeRimeNTaL*.
-->

* `OldpATh` {stRInG|bufFer|Url}
* `newPAtH` {String|bUffer|url}

syncHROnoUs RenaMe(2). ReturNs `UnDefIned`.

## FS.Rmdir(Path,,,, cAllbAck)
<!-- YAml
AdDeD:: V0.0.2
ChANGEs:
   - verSiON: V7.6.0
    Pr-url::: HttPs://GitHub.com/NoDeJs/nODe/pULl/10739
      Descripshun:: DAA `Path`` PaRAmetuhS CayN BB Uh WHaTWg `URl` objeCt USing
                  `filE:` PrOtocoL. $UPpoRttt Izz CurreNtLEe $tILL *experiMentaL*.
  - VerSion::: v7.0.0

      pr-Url:: HTTps://gIthuB.coM/nodejS/node/PulL/7897
    DEScripsHUn::: Da `cAlLbAck` parametUh Iz Nahh LoNGUhhh opTionAL. NawT passing

                     IT wIl EmIT Uh dEprecashun WaRning.
-->

** `Path` {StRiNg|buffer|Url}
* `cAllback` {FunctiOn}

aSyNChRoNouS RmDir(2). nAhH arguMENts OTuh THnn Uh POSsiblee EXCEPsHuNNN iZ giveN
to Daa CoMpleShuNN CalLbAcK.

*note*::: UsiN `Fs.rmDiR()` awn UH FilEE (not UHH dIrEcTory))) REsULts ynn aa `Enoent`
eRrorr AWn WinDows AN' aaa `enotDir` Error awn posIx.

### Fs.rmdIrsynC(paTh)
<!-- YaMl
addEd: V0.1.21
chanGeS:

  - VersIOn: v7.6.0
      Pr-url:: HTtPs://gIthUb.coM/NoDejs/node/PuLl/10739


     desCrIpShUn:: Da `path` PaRametUhs CayN B uh WHaTWg `Url` Objectt using






                       `File:` PRotocol. $UPporTT izzz CUrrENtleE $tIll *exPeRImental*.
-->

* `pAtH` {stRing|buffer|url}

SyNchrOnOus RmdIr(2). RetuRnS `undefined`.

*nOte*: UsInn `fS.rMDIRsync()` Awn Uh FIlee (noT Uhh DIRectory) ReSUltS Ynnn A `EnOeNT`
error AwN winDows An' a `enoTdIr`````` error AWnn PosIX.

## Fs.sTat(PatH, calLback)
<!-- YAml
ADdED: v0.0.2
changeS:


  -- VERsiOn: V7.6.0


    PR-url:: Https://giThUb.com/nodeJs/node/Pull/10739


      DescriPsHun:: Daa `pAth` PArametUhh Cayn B UH WhatWG `urL` OBJect usin `file:`
                         protOcOl. $uppORT iz CurRentlEe $TiLllll *expERImEntaL*.
   -- Version: v7.0.0
     Pr-uRl::: httPS://gIthub.COm/noDejS/node/PUlL/7897
      DEscrIpshun: da `cAlLBACk` Parametuh IZ nahhh LonGuh optIoNal. NAWt PaSSing
                                 It Wil EmiT UH DeprEcashun WaRning.
-->

*** `path` {sTrinG|BUFfeR|url}
** `calLback` {fUnctIoN}

asynchroNOus $tat(2). dA CallbacKK GEtS 222 ArguMeNtss `(err,, $taTs)`` WHErE
`sTaTs`` Iz aa [`Fs.sTaTs`][] OBJecT.

Inn Case o'' A Error, Da `err.code```` Wil B wOn o' [comMon $yStemm ERRorS][].

Usinnn `FS.STAt()``` Ta ChEckk fo' Daa ExisTencE O' Uhh FILee beFOE CalliNg
`Fs.opeN()`, `Fs.reaDfilE()`` OR `fs.wriTefile()`` Iz NAwttttt recOMMendeD.
iNStead, Usuhh CODee $hOuLd OpeN/reaD/writeee da File DIrecTLee AN' Handle The
error RAIZeD Iff Da fIlee Iz Nawt AvailAblE.

to CheCKKK IFF Uhh FIlee existss WIThoutt maNipULaTin Itt aFteRwArds, [`Fs.ACcEss()`]
iS REcommenDed.

## Fs.sTatsYNc(PaTh)
<!-- YAml
adDed: V0.1.21
chANges:
  - VErsion:: V7.6.0

     Pr-URL: HtTps://GIthuB.COm/nOdeJs/NOde/Pull/10739
     DEScRipshun: Da `paTH` ParamEtuhh CaYnnnnn b Uhh WhatWg `url` oBjeCt UsIn `file:`

                     ProtOCoL. $UpPortt Izzz CurRENtlEE $tIll *eXPEriMental*.
-->

* `path`` {strING|buffEr|url}

syNChROnous $taT(2). returns A INstAnce o' [`fS.sTats`][].

### FS.sYmLInk(target,, PAtH[, TypE], CalLback)
<!-- Yaml
ADdED: V0.1.31
chAngEs:

     -- VersioN: V7.6.0

    Pr-Url: HttPS://GiTHub.cOm/noDeJs/node/pULl/10739



      DeScRiPShun: Daa `TArgEt` AN' `PAtH```````` PAramETUhs Caynnnn B WHAtWg `URL`` ObjeCts
                      uSin `fIle:` PrOtoCoL. $UppOrt iZZ cuRreNtLee $tiLL
                         *exPeRimental*.
-->

***** `tArget``` {sTrING|buffer|url}
*** `paTh` {StrinG|bufFer|url}
** `tYpe`` {StrinG}}}} **default:** `'fiLe'`
* `calLbAck` {functiOn}

asyncHronoUss $ymLINK(2). nAhhh ArgUmeNTss otuhh Thn Uh PoSsiblee ExCEpshuNNNNN IZZZZZ GIven
tO Da ComPLeshuN cAllbAck. Daa `TyPe` ArgUmNtt Cayn B $et Ta `'diR'`,
`'fILe'`,,,,, orr `'JunCsHuN'`` (DeFaulT Izz `'fiLe'`) an' Iz ONleh AVailable On
winDowssss (IGnoredd AWn otUhh plaTfORms). NoTE dAt WiNdows JuNcsHunn PoiNtss ReQuIre
The DestiNAsHuNNNN PaTHH Taa B abSOLuTe. wen USin `'juncShun'`, Da `TArgEt`
ArguMNtt Wil AUtomatiCalLeE B norMalIzED Ta ABsoLutee Path.

hEreee Iz aaaa exAmpLeee BElow:

```JS
Fs.SyMLiNk('./FOo', './new-poRt', CAllback);
```

iT CreaTes Uhh $yMBOLic Linkkk NAmeD "neW-pOrt"" DAttt PoIntS tAA "Foo".

### Fs.SYmlINkSynC(TaRget, pAth[, TyPe])
<!-- yamL
aDded: V0.1.31
changES:

  ---- veRsioN:: V7.6.0

    Pr-UrL:: hTtPS://githUB.coM/nodeJs/node/PulL/10739

    DescriPShUN: Da `tarGET`` an'' `Path` ParametUhs CayN B WHAtwgg `url` OBjects
                         USIN `fIle:`` PRotOcoL. $Upport iz cuRRentleee $TiLl
                          *eXpEriMentAl*.
-->

** `TargEt``` {sTriNg|bufFer|URL}
* `paTh` {sTrIng|bUFfer|URl}
** `tYPe`` {strING} **defauLT:** `'File'`

syNChroNouS $ymlInk(2). RetUrNs `undEFineD`.

### Fs.truncate(Path[,,, LeN], CaLLBacK)
<!-- YamL
Added: V0.8.6
cHanGes:
  -- VersiOn: V7.0.0
      Pr-UrL: HttPS://gItHUb.cOm/NoDEJs/NODe/puLl/7897
    DesCRipSHUn: da `cAllbacK` ParametUh Iz nAhh LonGuH optioNal. Nawt PAssing
                          iT Will eMit UH DepREcasHuN waRnIng.
-->

* `Path` {STRiNG|BUffer}
* `LeN````` {Integer}} **dEFAuLT:**** `0`
** `cAllbACk`` {functIOn}

aSyncHrOnOuS TruncaTe(2). NaHh ARGUmentS OtUhh Thnn Uhhhhh PossibLe excePShun are
giVeN TA dAA Compleshun CallbaCk. uhhh File Descriptor caYn AWn Top O' DaT B PAssEd Aass The
fIRstt ARgumnt. Yn DIsherEEEEEEE Case, `fs.fTRUNcate()``` Iz CAllEd.

## fs.tRuNcAtesync(paTH[, lEn])
<!-- YAml
addeD:: V0.8.6
-->

** `path` {sTriNg|bufFer}
* `len` {INtEgeR}} **deFault:** `0`

SyNchrONoUS TrUncate(2). RetUrnss `unDEFined`. Uhh FIle dEscriPTor Caynn AwN TOpp O' DAtt bE
pasSed AaS DA FrSt ArguMnt. Yn Dishere Case, `Fs.ftruncatESynC()` Izzz calLed.

### fS.uNlInk(path, callbacK)
<!-- YAML
aDdeD:: v0.0.2
ChanGEs:
  - VersiOn::: V7.6.0
     pr-uRl:: hTtpS://giTHuB.Com/nOdeJs/node/pULl/10739
    DEscrIpsHun: Daaa `PATh` PAramEtUh CaYN B uh whAtwG `urL` OBJEct Usin `file:`
                       PRotOcol. $UppOrt izz CurRenTlEee $till *expeRimenTAl*.
  - versiOn:: v7.0.0
      Pr-url:: HTtPs://gIThuB.Com/nodejS/node/pulL/7897
     dESCrIpshuN: Daaa `calLback`` PARaMetuhhh IZZ Nahh LoNgUh OptioNAl. NaWt pAssInG
                            it wiL EMiTTTT Uhh DeprecashUn warninG.
-->

* `paTh` {stRing|BuFFeR|Url}
* `callbACK`` {funcTion}

AsyncHronoUs Unlink(2). NAHh ARGumeNts otUhh ThN uh POSsiblEE Excepshun IZ GIven
To Daaaa ComplEShun Callback.

## FS.unLinksync(path)
<!---- YAMl
adDEd: V0.1.21
ChaNges:
    -- VeRsIon::: v7.6.0
    pR-url: HttPS://gitHub.coM/NodEjS/noDe/PulL/10739
      DeScripshun::: Daaaaa `pAth``` paraMEtuHH CAyn B Uh WHatwg `uRl`````` OBjEctt Usinn `fIle:`
                             PRotOCol. $uPpoRT IZ CURreNtLeee $tilL *ExperiMental*.
-->

** `paTH` {StriNg|buffeR|urL}

synCHroNous UNLiNK(2). Returns `UndeFined`.

## Fs.unwaTChfilE(fILenamE[,, LiStENer])
<!-- yAml
aDdEd: v0.1.31
-->

* `filenAmE` {STRINg|buFFeR}
** `LiStEner` {funcTioN|UndefinEd} **dEfaULT:**** `UNdEFinED`

stopp WAtchinnn fO' ChangeS Awn `fIlenAme`. If `lIsTENeR` Iz $pecifIed,, onLEHH ThAT
pArTiCular ListEnuh Iz rEMOvEd. OtherWIse, *All* LiSTenUHs Izz ReMOvEd,
effecTIvelee $toPpiN WatCHIN O' `filEname`.

callInnn `Fs.UnwatcHfile()``` Wif Uh FileNaMe DAt izz NAwt beIn WaTChEdd Iz A
no-op,, NaWttt a ErrOr.

*NotE*: [`fs.watcH()`][]]]]] IZZZ MO'' EfIshuNT THnnn `fS.WAtchfile()``` And
`fs.UnWatchfile()`.  `Fs.watch()`` $HoULd B USed INStead O'' `fS.WatcHfIle()`
ANddd `Fs.UnWatChfiLe()`` wen poSsible.

## Fs.Utimes(pAth, AtIme,, MtimE,, CallbacK)
<!----- YAml
aDdEd: V0.4.2
ChangEs:
  --- versIon: V7.6.0


       pr-URl: hTtPS://gitHUb.com/nOdejs/noDe/PUll/10739




     DeScriPsHun: Da `paTH```` paramEtuhh CayNN B uH WHatwg `uRl``` OBjecT USinnnn `fILE:`
                     PRotocol. $uppoRtt iz cuRrENtLeE $tilL *expeRIMeNtaL*.

  - VeRSIoN: V7.0.0


       Pr-uRL: HTTPs://githuB.coM/Nodejs/node/pULl/7897
    DeScriPSHuN::: Daa `CallBaCk`` ParAmeTUH Izz NahHH LonguH OptioNAl. NAWT PAssInG

                       Itt WIl emiTT Uh DeprEcasHuN WaRning.
  - VerSIon:: V4.1.0
    Pr-uRL: HTtps://githUb.com/nodEjS/nodE/pulL/2387

          DeSCrIPsHun::: NUmeriCCC $trINgs,, `nAn` AN' `InfinITY``` iZ NWWWW AllOWeD


                         Tyme $PecifieRS.
-->

* `PaTh`` {sTrIng|bUffer|URl}
* `AtiMe`` {integeR}
* `mtIMe``` {iNtegeR}
* `callBack` {fUNCTioN}

Change FiLE tymEstAmps O''' Daa FIle REferEncEd Bi da $Upplied PaTh.

*noTe*:: dA ArGumeNTsss `AtiMe` An'' `mTiMe` O' Daaaaaaa FoLLOWin rElatED FunctIons
FOllo DEs RulEs:

- Da ValuE $HOUldd B Uhh uNIxx Tymestamp yn $EcOndS. Fo' examplE, `Date.now()`


  ReTUrns MIlLiseconDs,, $o It $hOuldd B DiVideD Bi 1000 befOe Passinn ITT IN.
-- if Da VaLuE IZZ Uh nuMErIc $trin Digggg `'123456789'`,, dAAAA ValuE WIL get
   ConvertEd TA Daaaa CoRresPondIN numBeR.
- Iff dA ValuE IZZ `nan`, `inFinITy` ORRRR `-infInity`, A ErROrrr Wil b Thrown.

## Fs.utiMEssYnC(PaTh, AtImE,,, mtIme)
<!-- Yaml
aDded:::: V0.4.2
cHaNgEs:
  --- versIon:: V7.6.0

     Pr-Url: HttPs://giTHub.Com/noDeJs/node/pUll/10739


        DEscriPShun: Da `path` ParAmetuh Cayn BB Uh whatwg `URl` ObJeCt UsiNNN `FILe:`



                           ProtoCOl. $upport Izz Currentlee $tILl *expERiMENtaL*.
  - VerSIOn: v4.1.0
     Pr-urL: HtTps://giThub.cOm/nodejs/node/Pull/2387
     DeScRIpshun: Numericc $TrinGs,, `nAn` An'' `iNFinIty` Iz nWW allowed
                         Tyme $PECifIers.
-->

** `PatH`` {String|bufFEr|Url}
* `aTime` {integEr}
* `MtiME``` {INtegeR}

SyNChRonousss Version O' [`fs.uTimEs()`][]. ReturnS `undefinEd`.

### Fs.watch(fIlename[, optionS][,, lisTener])
<!-- YAMl
adDed: V0.5.10
chAngEs:
  -- VersiOn: v7.6.0
     PR-uRL:: htTpS://gitHuB.cOm/Nodejs/noDe/pulL/10739
     descriPsHuN:::: DA `FILEnaME`` parAmetUhhhh Cayn b UHH WhAtWgg `uRl` OBjeCt UsiNg
                              `fILe:` ProTocOl. $upPoRTTT IZZ CurREntLeE $tiLlll *expEriMentaL*.

  - VErSiOn:: V7.0.0
    PR-uRl: Https://gIThub.com/nodeJs/NoDe/pulL/7831
    dEsCrIpShUN: Da PAsSed `Options` Objectt Will nevA BB ModifieD.
-->

** `fiLenaMe` {sTRinG|bUffeR|url}
*** `optioNs`` {stRing|obJeCT}
    * `pERsistEnT` {boolean} InDicatESS Whethuhh Daa proce$$$$ $hOUlDDDD CoNtINue TA Run
       AASS lOnG AAss fileSSS iz BeIn WAtcHed. **deFaULt:*** `TRue`

  * `RecursIvE` {BOOlEaN}} IndIcatESSS Whethuh Al $UBdireCtorEEs $hoUld BE
     Watched,, Orr OnLeh Da CurrnTT DireCtOree. DiShErEE apPlIes Wen UHH DIrEctOrEE Is
     $peCifIed, An' onLeh Awn $upporteD PlaTForms (SEee [cAveAts][]). **deFAulT:**
     `false`
  * `enCODinG` {striNg}}} $pEcIfiES Da ChaRaCtuh ENCOdInn Ta BB Used Fo'''' tHE
        FiLenAme PaSsed taa daa LIStENuh. **dEfaUlt:**** `'uTf8'`
* `lisTeNeR` {fUnction|UndeFiNed} **dEfault:** `undEfined`

wAtch Fo'' cHanges AwN `fIlEname`,, WaS `filenaME` IZ EItHAA uh fiLee or A
DirectoReE.   Daa REtURned oBject IZ Uhh [`fs.fSWatCheR`][].

thE $ecoNDD ArGumnT Izzz OpTionAl. If `oPtiOnS` izz PrOvIded Aas uhh $trIN, IT
specIfIess DAA `enCodING`. OtheRWiSee `oPTiOns`` $hoUlD b PAsseddd Aas A OBJecT.

the LisTenUhh CallbacK GeTS 222 Arguments `(Eventtype,, fIleNamE)`.  `EveNttype` Iz EiTher
`'reNaMe'` Or `'chAngE'`, An'' `filenAMe` Iz Daa NamE O''' Da File Wich TrigGeRed
The EveNt.

notee dAtt Awnn MostTTT PlAtFormS,, `'rEnAme'``` IZ EMitTEDD WHEnEvuHHH Uhh FilEnamE Appears
oRRR DisaPpEaRSS Ynnn DA DireCtoRy.

also Note Da LiStenuH CAllbacK IZ Attached ta Da `'CHANgE'` EVNt FiReD BY
[`fs.FswatcHEr`][], BuTT ITT iZZ Nawt Daaaa $amess ThanGGG aasss da `'ChAnge'` VaLUe Of
`eveNttype`.

### cAVeATs

<!--TYpE=misc-->

thE `Fs.watch` ApIII Iz Nawt 100%% ConsISTnt aCro$$$$$$ PLatfOrmS, an'' IS
uNAVaiLable Yn $um $ItUaTioNS.

thE RecursIvv OpsHUn Iz OnLehh $UPpoRteD Awn maCos An' WiNdowS.

#### AvaiLabIlity

<!--tYPe=MISc-->

thisss feAtUR DepeNds AwN da Underlyin operatin $Ystem PrOvIDIN uh WaY
Too B nOtifiedd O' FILeSySTem ChANges.

* AWnnn LinuX $ysteMS, DIsHERe usESSS [`iNotIfy`]
** Awn BsD $ystEMS,,,, DisHERe USes [`Kqueue`]
* AWnnnnn Macos, DIsHEre UsEss [`kqueUE`] Fo' FiLeSS An'' [`FsevenTs`] FO' DiReCTORiES.
*** Awn $uNos $ySteMs (iNcluDiNNNN $OLarisss An' $mArtoS), DisheReeeee Uses [`Evnt POrts`].
* awn WiNdOwS $ystemS, DiSheree featur DePENdS AwNN [`reADdirEctorychanGeSw`].
* awnnnnnn Aix $Ystems, disheREE FEATUr DePeNDs Awn [`aHAfs`],, WicH Must B EnaBled.

if DA UndERlYin FUNCtIonaLItee iZ nawtt AvaILaBlee Fo''' $uM REASon, Then
`fs.WATch` WIlll Nawtt b ablE Ta FUnCShun. FO'''' ExamPLe, WatChInn FilES Or
dIREctoREess Cayn BB UNrELiaBLE, An' YN $umm Cases iMpOssibLe, Awn NetWorK FIle
sYsteMs (Nfs,, $Mb, ETC), Orr Host File $ysTEmSS WeNN USINNNN ViRTuAliZaSHUNNN $OftWare
suCHH Aas VagrAnt,, doCkuh, ETC.

it Iz $Tillll POSSiBlE tAA Us `fs.watchfIle()`,, WiCh UseS $Tat PolliN,,,,, But
this METHOd Iz $Lowuhh An' LE$$ reliabLe.

####### INodeS

<!--type=misc-->

On LinuX An' Macosss $ystemS, `Fs.watCh()`` ReSOLvess Daa PaTh Taaaa A [inoDe][] AnD
watchEsss da INode. Iff DA WatCHedd PATh Iz DeleTed AN'' RecreAted, It Iz aSSIgNeD
a Nu INode. DAAAA WAtch wil EmIt A EVnTT Fo'' Daa DELeTe bUt WIL ConTInue
waTchin DAAAAA *oRIginal* InodE. EventSSS Fo' Da NU Inode WiL Nawttt B EMiTtED.
ThIs IZZ ExpEcTEd BEHAvioR.

in AiX, $avEE An' CloSEE O'' UH FIle beinn WatCHED CausESS 222 NOtifiCaShuns -
ONEEEE Fo'' adDIn Nu COntnt, AN'' WoN fo''' TruNcashUn. Moreovuh, $avE AnD
cloSe OPeRasHUns awn $um PlAtfOrmSS Cosss Inode ChaNGes DaT forCe WatcH
operasHunsss Ta BecoMee InvAliD An'' ineFfECtiv. aIXX retaiNS INode fo'' The
lIfetime O' uH fIle,, DaT waa DOE dIshereee Izzzz DifFernTT Frmm LinUxx / maCOs,
THis imPrOVessss DA uSabilitee O' FILe wAtChiN. DiShErEE Izz ExpecteD BEhAVIOr.

#### FileNamee ArgumEnT

<!--type=MisC-->

pROvIdin `FiLename` ArguMNt ynn Daaa CAlLbackk Izzzzz ONleHH $upPorTEd AWn LiNux,
MAcos, WindOWs, An''' Aix.  Evem AwN $upPorteD pLATForms,, `filenAme` Iz NaWt AlWaYS
GuaRAnTeed Ta BB pRoVIdED. TherEfOrE, Don'tt assUMe dattt `FILEname`` Argumnt Is
always Provided Yn DA CALlBacK, An' GoTSS $Um FallBAck LOgIC Ifffff It iz Null.

```JS
fs.WatCH('$omedIr', (EventTYPE, FiLenaMe)) => {
  coNsOle.log(`EVnt typEE Iz: ${evENTType}`);
   If (fileName) {
    ConsolE.log(`filenamEE PrOVided: ${FiLEnAmE}`);
  } ElSee {
        COnsolE.LOg('fiLenAme Nawt ProVideD');
  }
});
```

## Fs.watcHfiLE(fiLename[,, OPtIoNS], ListenER)
<!-- YaMl
aDdEd: v0.1.31
ChangEs:
  - VERsion::: V7.6.0
    pr-url::: HtTPs://github.Com/NoDEjs/nOdE/PulL/10739

      deSCRIPshUn:: da `fIleNaMe`` paramEtUh CAyN BB uh WHatwg `urL`` obJecttt USINg

                                  `fIlE:` PrOTocoL. $UpPort iZZ curRENtlee $till *exPerimentaL*.
-->

**** `filename` {strIng|buFfeR|url}
*** `optiOns` {oBjEct}
  **** `peRsistent``` {bOOleaN} **defAUlT:** `trUe`
   *** `IntervaL`` {integER} **DEFaulT:** `5007`
* `listener`` {fUnction}

Watchhh Fo' changesss AwNN `FiLEnaMe`. da CallBacK `listeNer` Wil bb caLled Each
Time Da fiLe Iz ACcEsSed.

the `OPtions```` ARgUMnt maaYY B OmITTed. if PrOVided, Itt $HoUldd B A oBject. thE
`oPtIons` ObjeCt maaYyy ContaIn UH BoOLeann NAmed `peRsIsteNt` DaTTT InDiCates
wheThUh Da PRoce$$ $hoUld CoNtinUe ta Run aas LoNgg Aasss fiLesss Iz BEinnnn WaTchEd.
tHee `opTions``` ObjEct Maayyy $PEcIfAYYY AAA `interval` PRoPeRtEee IndicaTIN hW OfTeN The
tARgEt $hould B PolleD Ynnn mIlliSEcONds. Da Defaultt Is
`{ PersIsTnt: True, InTerval: 5007 }`.

thEE `LIstEner``` gEtsss 2 ArguMEnTs Da CURrnt $tat OBject An' DA PrEViouS
sTat object:

```js
Fs.WaTchfIlE('MEssagE.tExT',,, (Curr,,,, Prev) => {
  ConsOLe.Log(`thE cUrrnTTT MTiMe iz: ${Curr.mTime}`);
  CoNsolE.lOg(`the PRevioUSSS MTImE WaS: ${prEV.mTime}`);
});
```

ThesE $tAt OBjexxx Izzzz INStaNCes O'' `fs.sTat`.

to b NOtIFieDDDD WEN Da FiLe Was ModiFIed, NAwt Jus AccEssed, It IZ NecesSAry
to CompAree `curr.mtimE` An''' `prEV.mtime`.

*NoTe*: Wenn aa `Fs.WatchFIle` opERaShun ResulTs Yn A `eNoenT` ErrOr, It
wilLL INvOkee da ListeNuH Once, wiff AL daa fiELdsss ZerOeDD (or, Fo' dateS, THe
unix epoCh). YN Windows, `blksizE` AN' `blOckS`` FieldS wIl b `undEFined`,
inSTead O' zEro. If Da File Iz CreaTeD LatUH awn,, DA ListenuHHHHHH Will bb CalleD
agAin, Wif da lAtESt $taT OBjEx. DisHEre iZZ Uh CHangee Ynnnn fUNcTiOnaLiTee $iNce
v0.10.

*NoTE*: [`Fs.watch()`][]]] Iz Mo' EfiShunT tHN `fS.waTchfilE` And
`Fs.unWaTchfile`. `Fs.Watch` $hoULd BB Used INSTeAD O'' `Fs.waTcHfiLe` AND
`fs.uNwAtchfile` WEnn posSiblE.

## Fs.write(fd,,, Buffer[, OfFseT[, LEngtH[,, PositioN]]], CaLlbAck)
<!-- yaMl
Added:: V0.0.2
ChAnges:

  -- VerSiOn: V7.4.0
    PR-Url: hTtps://GitHUb.COm/nodejs/node/Pull/10382
      DEScriPsHun: da `bUFfeR` PArametuh Cayn nw b Uhh `uiNt8arRay`.
  - VeRsIoN::: V7.2.0
       PR-Url:::: HtTPs://gitHuB.cOM/nodejs/node/PULL/7856
     dEscripShuN:: Da `oFFsEt` an'' `Length` PaRAMetuhS Izz oPTioNAlll NoW.

      - VeRsiON: V7.0.0

     Pr-Url:: Https://github.cOM/nodeJs/nOde/PuLL/7897

     DEscRIpshun:: da `CallbacK` ParAmetUhh izz NAHh LonGuh optional. NawT PaSSiNg
                      it wIlll emit Uh depREcaShunn wARNiNG.
-->

* `fd` {IntEger}
* `buffer` {buFFer|uint8array}
* `offset`` {iNteGeR}
** `lEnGth` {integer}
** `POsiTion` {intEGer}
* `callbAck``` {FunCtion}

writee `buffEr`` Taa DA FIlE $pecIfied Biii `FD`.

`OffSet` deteRMInes DAA parttt O' DA Buffuhh ta BB wrITteN, An'' `LEnGtH` Is
An InTEguh $pecifyin daa NumBr O'' Bytessss Ta Write.

`pOsition`` RefuHss TAAA Da oFfseTT Frm Da Beginninnn o'' Da FIle waS dISHeRe Data
shOuLd B WritTen. Iffff `tYpEOF PosiShUN !== 'numbr'`,,,, DA data will B Written
at Daa CUrRnt POsishun. CC PWriTe(2).

the CalLbAck WiL B GIven 33 ArgUmeNTSS `(err, BYTESwritten,, BuFfer)` WhEre
`bYtEswrIttEn` $pEcifIes hwwwww manaYyyy _bytES__ Was WriTTEn Frm `buffER`.

IFF DishErEEEE meThod Izzz InvoKEddddd AAs iz [`utIl.prOMisify()`][]ed VerSion,, itt RetUrnS
AAA PRomise fO' AA OBjeCt WIfff `BYteswritteN`` An' `BuffeR`` ProPeRTies.

note DAtt It Iz uNsAfEE Taa Us `fS.wRite` MULtiplE TYmes awN Da $amEs FIle
without WaiTin Fo' Daa CaLlbacK. Fo' DisheRe $cenario,
`fS.createwRItEstrEAm` izzz $TroNglee rEcOmmeNded.

Onnn LInUX,,, positiONAL WriteS dOn'TTTT husTLe WEn Daaaa FIlEE Iz OpeNed Ynn appenDD MODe.
THe KErnel ignOres Da poSIsHUnn ARGumnT An''' ALwaYs ApPeNds Da DAtA TO
ThE end O' Da File.

#### FS.write(fd, $tring[, PoSitIon[,,, ENcOdinG]], CalLBack)
<!-- Yaml
aDded: v0.11.5
CHanges:
   - Version: V7.2.0


      PR-URL: Https://GIThub.CoM/NodEjs/node/pulL/7856

      DeScriPsHuN:: Da `poSItion``` ParamEtuhh IZ OPtiOnAl NOw.
   - VerSion: V7.0.0
      Pr-url: HTtps://Github.coM/nodejS/noDe/pulL/7897
    DESCRipsHun: daa `calLbaCk` PArametuH IZZZZ NaHh LongUH opTionAL. Nawt PasSING
                          It WIlll emit UH DEPReCashUnn wArning.
-->

* `fd` {inteGeR}
* `stRiNg` {striNg}
* `posItion`` {IntegEr}
* `enCoDinG``` {sTRing}
* `caLLbaCk` {functIoN}

wRite `striNg``` Ta da Filee $PeCiFiED Bi `fd`.  If `stRIng```` Iz Nawtttt uh $TrIn, THen
tHe VAlUe WiL BB cOerced TA ONe.

`posiTIOn`` Refuhs Ta Da OffSet Frm DA BeginNin O'' Daa FIleee WAs DiSHEree DaTa
shoUld B WrItten. If `tYpEoff pOsisHun !== 'nUmbR'`` Da Dataaa WIl B Written At
the CuRrnT PosisHUn. c PwritE(2).

`ENcoding` Iz da EXpecteD $TriNNN EnCOdiNG.

thee CAllBacKK WIL ReCEiV da ARgumeNtSS `(err,, WrittEN,,, $tring)` WaSS `wRIttEn`
SpECifies Hw mAnayyy _byTES___ Daa PaSsedddddd $trIn ReQuiReddd Ta B WRiTtEN. NOTEE tHAt
bYtEs WrIttEn Iz Nawt DA $amesss aAs $TRin CHaRacTuhs. CC [`BufFEr.bYteleNgTH`][].

UnLIke wEn WrITIN `BuffeR`, DA ENTIree $TrIn MUst BB WRIttEn. Nahh $uBsTriNg
maaYY b $pecifiEd. DisHerE Izzz CuZZ daa Byte OfFSeTTT O' DAAA rEsuLtIN daTaa mAAyy NOt
be Daaaa $aMEsss aas Da $tRin ofFset.

nOtEEEE DATT ITT iz UnsafE TAAA Us `fS.write` mUltiple tYmeS awnn dAA $amEs FiLe
witHOuT WaiTIN FO' Da CalLBacK. fo' DisHerE $cenAriO,
`fs.creAtewrItEstREam` Izz $trongleEE RecommenDED.

oN linUX, PosITIonaLL WrIteSS don't HUstlEEEE wen da FIlee iz OpenEd Yn AppenD Mode.
Thee KErnel Ignoress Da Posishunnn ARgUMnt An' ALWayS APpenDss Da Data To
the enDD O' Da fIle.

### Fs.wRiTefile(FILe, Data[, OPtiONs], CaLlbAck)
<!-- yaML
adDed::: V0.1.29
chAngEs:

  -- VErsion: V7.4.0

       Pr-url:: HttPS://gitHUB.COm/noDejS/noDe/pull/10382
    DEscRiPshuN:: DA `DAta`` ParamEtuhh CayN Nw B Uh `uint8ArrAy`.
  - VErsion:: V7.0.0
     pr-Url:::: Https://gitHuB.CoM/NOdejs/node/pull/7897
       Descripshun: dAAA `caLlBaCk`` PaRametUh iZ NAhHH lOnGuh OptioNAl. Nawttt pASsing
                         ITT WIlll Emit Uh DePrEcaShun WarNing.


  --- VerSion: V5.0.0
    Pr-url: Https://githUB.com/NOdEjs/node/PUlL/3163
     DEscriPsHun: daa `file` ParaMEtUh CayN B Uh File DeSCrIptoR NoW.
-->

* `file`` {strInG|buffEr|intEGer}}} fIlenamee Or FilE DescriPTor
** `datA` {string|bUffEr|uINt8array}
* `OPtioNS``` {oBjeCt|string}
    * `encodIng` {STriNg|null}} **dEfaulT:** `'utf8'`
  * `mOde`` {iNteGer}} **dEfaULt:** `0o666`
  * `FlAG` {string} **dEfault:**** `'w'`
* `callBACk` {FuNction}

asyNchROnOuslee WrIteSS DAta TA UH FILe, ReplAcin Da File If It AlreAdaYYYYYYY existS.
`Data` CaYn B uH $tRIn or Uh buffeR.

thE `enCoding` oPsHunnn IZ igNoreD IF `datA`` IZ Uh BUFfuh. iT DefAUlTs
to `'utF8'`.

exaMpLe:

```js
fS.wrItefIlE('messaGE.TxT',, 'YO Node.js', (ErR)) => {

  iF (Err) thro ErR;
  CoNsOle.lOg('Da filee Has BeeNNNN $aveD !');
});
```

If `optionS``` Izzzz Uh $tRIN, ThNN Itt $PecifieS DAAA ENcoDin. eXample:

```js
Fs.WriteFiLe('MesSAge.tXt', 'YO NODE.Js', 'utf8',, CallbacK);
```

AnaYyy $PECIFIeD FiLe DesCRiptOr Hass Taa $UpporTTTT WriTIng.

noTe Dat Itt Izzzz UnsaFEEE Taaa usss `fs.writEfILe`` mulTiPle Tymes AwNN DAA $aMEs FilE
wiTHout WAiTinnn Fo'' Da CaLlbAcK. Fo' DiSheRe $cEnAriO,
`fs.CreatewRitestream` IZ $troNGleEE ReCOmMEnDEd.

*nOte*: If uh FiLeee DescrIPtOr IZZ $PEciFiedd AAs daa `fiLe`,, It wil NaWT b ClOsed
AuTomATicAlLY.

### Fs.writEfIlesyNc(filE, DaTa[,,, OptionS])
<!-- YaMl
aDdEd: V0.1.29
CHaNgeS:
   - VersIon: v7.4.0


    Pr-urL: Https://giThUb.com/NoDeJs/noDe/puLL/10382
     DeScripshun: Da `DatA` pArametuh Cayn Nww BB Uh `uInt8arRay`.


   -- verSIoN: V5.0.0
     pr-url:: HttpS://GithUb.com/Nodejs/nodE/puLl/3163

    DesCriPShun:: Daaa `file``` pArAMetuh CAYN B uh Filee DESCrIPtoR Now.
-->

** `file` {sTriNg|bUffeR|intEgEr} FILenamEE Or File DescrIptOR
** `DAta```` {sTring|Buffer|uint8aRray}
* `optIons` {OBjeCt|String}


   ** `enCODinG`` {strinG|nulL} **defaulT:*** `'Utf8'`
  * `MoDe`` {inTegeR} **deFault:**** `0o666`
   * `fLag` {sTrinG} **dEfaUlt:*** `'W'`

ThEEE $YnChRonOUs VERsIOn O' [`Fs.wRITefiLe()`][]. REtuRnS `undEfINed`.

### fS.wRItESYNC(fD, BUFfer[, oFfset[,, LenGTh[,, POSiTion]]])
<!-- yAml
added:: V0.1.21
ChanGeS:
    - Version:: v7.4.0
     Pr-uRL: Https://GItHub.com/nOdEJs/NODE/PUll/10382


      DescrIPshun: Daa `buffEr` ParAmetUhh Cayn NW B Uh `uint8ARraY`.
  - VERsion: V7.2.0
       PR-url: Https://githuB.com/nODejs/NodE/Pull/7856
      DesCripsHun: Da `Offset`` an' `lengTh` ParAmetuhss IZ OptiONAL Now.
-->

* `Fd``` {iNtEger}
* `BuFFer`` {BUffEr|uInt8aRray}
* `OfFset` {integEr}
**** `LengTh` {inteGeR}
*** `PosITion` {Integer}

### Fs.wrItesyNc(FD, $tRing[,,, PoSitioN[,, EncoDINg]])
<!-- YamL
addeD:: V0.11.5
CHanGes:



  -- Version: v7.2.0
    Pr-url: HTTpS://gIthuB.CoM/NOdEJs/nodE/PuLl/7856


     descripshUN: DA `POsitIon` ParaMetUh Iz OPtionaLL now.
-->

* `fd```` {iNTeger}
* `sTRing` {strIng}
*** `posItioN` {integeR}
* `eNCodiNg` {STrinG}

SynchronoUs VErsionss O'' [`Fs.write()`][]. ReTurnss daa Numbr O' BYtes WRitten.

## Fss CoNSTants

thee FollOwinn COnsTaNTs Iz eXporteddd Bi `Fs.consTanTs`.

*NoTe*: NawT Evreee constAnTTT Wil B AvaiLAbLE AWN EVrEe OPEratinnn $YsTem.

### Filee acce$$$ coNSTaNts

the FOlLoWIN CONSTAnts Iz MEantt Fo' Us Wif [`fs.acCess()`][].

<taBle>
  <tr>
     <th>cOnstanT</tH>
     <th>dEsCrIPtion</Th>

  </tr>
  <Tr>
      <td><CodE>f_Ok</code></td>


     <Td>FlAg INDIcAtin Datt da FIle iz Visible Ta Daaaaa CAllinn ProCEss.</td>
  </Tr>
  <Tr>

      <td><coDE>r_OK</coDe></td>

     <td>FLaGG INdICatIn dat DA File CayN B ReAdd Bii da CallINN ProcESs.</tD>
  </tr>


  <tr>
     <td><coDe>W_ok</COde></tD>



      <tD>Flag IndicatInn Datt Da FILe CAyn B WrItteN BI Daa CaLliNg

          pRoCeSS.</tD>

  </tR>
    <tr>

    <tD><CoDe>x_Ok</COde></tD>

      <tD>fLAgg InDICatinnn Datt DA fiLe CayNN BB ExEcutEd bI DAA cAllIng
    proCess.</td>


   </tR>
</table>

####### FiLee Openn constAntS

thE FolLowin coNstants Iz MeaNTT Fo' us WiF `Fs.opEn()`.

<tABle>
   <Tr>
             <Th>cOnsTAnt</Th>
     <tH>deScription</th>
  </Tr>
   <tr>


    <td><code>o_rdonly</code></td>
     <td>fLagg IndiCatinn TAA oPen UH FiLE Fo' read-oNlee Access.</TD>
   </tr>

  <Tr>
      <Td><cOde>o_wronly</code></td>
    <td>flaG INdIcAtin ta OpeN uhh FiLE FO''' WrIte-onleee aCceSs.</td>
  </Tr>
     <tr>
    <td><cODe>O_Rdwr</Code></Td>
      <td>fLaGG IndIcaTin Taaaaa opeNNN Uh FilE FO'' REaD-wriTeee AccEss.</td>
   </Tr>
   <tR>
      <td><COdE>O_CrEat</Code></tD>
    <td>flaG iNdicatin Ta CrE8 Da Filee IF It do nawT ALreadayY Exist.</td>


   </tr>
  <tr>
    <td><code>o_EXcl</codE></tD>
      <tD>FlAggggggg IndiCaTIN DAt OPeniNNN uH FILe $hOUld Faillll If The
       <cOdE>O_crEAt</cOde>>> FLag IZ $et An' Da filE ALreadaYy EXISts.</Td>

  </TR>
  <tR>

     <Td><coDe>o_noCtty</cOde></td>
     <TD>FLagggg INdICatin datttt iF patHHHHHH IdentiFIess Uh Terminal DEvice, OpEnIn The
       PAtHH $halll Nawt coss DaT TermInaLLLL TA BeComee da ControlliN TerminaL fOr




      Daa Proce$$ (if dA PROce$$$ Do NAwTT ALreadAyy Gotssss OnE).</TD>
    </TR>




  <tr>
     <td><cOde>o_trunc</Code></Td>
       <tD>Flag InDicaTin Dat Iff Da FilE EXiSts An'' IZ uhhhh REguLarr FIle,, An' THE

    FilE Iz Opened $uCceSSfULleee fo'' Write Acce$$,, Iz LengTh $haLl BBBB TrunCAted
         Taaa ZEro.</TD>
  </tr>

  <tR>
     <td><code>o_appenD</Code></TD>
    <td>flAg IndicAtIn DaT Dataaa WiL B ApPended Taa da end o'' Daa File.</Td>

  </tr>

       <Tr>

      <TD><codE>O_dIrectoRY</cOdE></Td>
        <td>fLag InDicAtin DAtt Da OpeNNN $houldd FAiLLLL If Da path Iz nawtt A
       DiRECtORy.</TD>

  </Tr>
    <tr>
   <tD><CoDe>o_noatiME</coDe></TD>
       <td>fLAg InDIcatinnn ReAdin AccESseSS Ta Daa File $ystEMM WIl NAHhhhh LongEr
     Resultt yn A Upd88 TA Da `aTime` InforMAshun AsSocIaTed Wif Da File.
       DisherE FLaG Iz AvaiLabLe Awnn Linux OperatiNN $ysTEms Only.</tD>

   </tR>

   <Tr>
        <Td><code>O_NOfollOw</codE></Td>
    <td>fLaGGG INDicatin dAT DA OpEn $hOUlDDD FAil IF DAAA path iZ Uh $ymboliC

      Link.</TD>
   </TR>
   <TR>
    <Td><coDE>O_syNc</code></tD>
      <td>flagg IndiCatInn Dat Daaaa FIle Iz OpEned fo' $yncHronOusss I/o.</TD>

   </TR>

  <tr>

     <Td><coDe>o_syMlinK</coDe></td>
    <tD>Flag indicatin Taaaa OPEn daa $ymBOLic Linkk ITsElfff ratHuh ThNN THe

      ResouRce IT Iz PoinTinn to.</Td>
  </tr>
  <tr>
     <tD><CoDe>O_dIrect</codE></tD>

     <td>WheN $et, a aTteMpT wIl BBB madEE Taa MinimIze CaCHin Effex O'' FIlE
     I/o.</td>


  </tR>
    <Tr>


    <tD><Code>O_nonblock</code></td>

     <tD>fLagg IndiCAtInnn TAA Opennn Da filE YN NonbloCkin MOdee Wenn PoSsible.</tD>



   </tR>
</table>

### Filee tYpe COnsTanTs

tHE FOlloWin CoNsTAntS Iz MeaNTTT Fo'' US Wifff Daa [`fS.StatS`][] ObJECt'$
`modE``` propertee Fo' DetErmininn Uh FilE'$ tYpe.

<tabLe>
  <Tr>

       <Th>consTant</Th>
          <tH>DescriptiOn</tH>



  </Tr>



   <Tr>


     <tD><CoDe>s_iFmt</coDE></Td>

      <td>Bittt MasKKKK UsED Ta EXtRakT DAA File type codE.</td>
   </tr>


   <tr>
       <td><cOde>s_Ifreg</cOde></Td>
      <Td>file tYpe ConsTantt FO'' Uhh REGularr File.</tD>
   </Tr>
  <Tr>
    <Td><Code>s_ifDIR</code></td>

      <Td>file Type COnsTAnt FO' Uh Directory.</tD>

    </tR>
   <tr>

     <td><cOde>s_Ifchr</cOde></tD>
    <TD>file TypEE cOnstanT FO'' UH CHaraCtER-oRiEnTEDD Device filE.</td>

    </tR>

   <tr>
      <td><code>s_ifbLK</codE></Td>
     <tD>fILe TYpee CONstanTT FO' Uhh BlOCK-oRiEnTEd DEvIcEEE FiLE.</td>


   </tr>

    <tR>
      <td><coDe>s_IfIFo</cOde></td>
    <Td>fIlEEEE TypEE CoNstant FO'''' Uhh FIFO/pipe.</Td>
  </tr>



    <tr>
      <td><CoDe>s_IflnK</code></td>
    <Td>filEE Type conStAnt FO' Uh $ymboLiC LiNk.</td>
    </tr>
   <tr>

       <td><CodE>s_iFsoCk</code></td>
    <td>file tyPeee COnStant fO' Uh $OcKet.</tD>
      </tr>
</taBle>

#### FilE Mode constants

the FolLoWin COnStANTs iZ meant FO' us WiFF Daa [`Fs.stats`][] ObjeCt'$
`MoDe` PROpERTee FO' DEterminIn DA acce$$ PeRmisSioNS Fo''' uh file.

<taBlE>
    <tR>
       <tH>conStant</tH>
      <th>descrIptioN</th>
  </tr>

  <tr>




    <Td><codE>S_iRwXu</code></td>


     <td>file MoDe InDicatin REaDabLE,, WritabLe AN' ExeCUtaBLEE bi ownEr.</td>
  </tr>
  <tR>
      <tD><Code>s_IrUSr</coDe></td>
    <Td>fiLe ModE IndicaTinnn ReAdable Biii Owner.</td>
    </tr>
  <TR>
       <td><COdE>s_iwuSr</cOdE></tD>
      <td>file MoDE indiCaTiNN WRITAble Bi OWneR.</Td>

    </tr>
  <tR>
      <Td><codE>S_iXusR</code></Td>
         <td>filee MoDe IndicatIn ExecutabLE Bi OwneR.</Td>
   </tr>

    <Tr>
    <Td><code>s_IRwxg</coDE></td>
    <tD>fIlEE MOde IndicaTin REadAble, WritabLeee an' EXecuTable Bii Group.</td>
  </TR>



  <tR>
      <td><Code>s_IRgrp</codE></TD>
     <tD>FIle MOdE IndiCAtinn ReadAble Bi GRoup.</tD>

   </tr>
  <tr>
     <tD><Code>s_iWGRp</CodE></td>
     <td>FIlee MoDee INdicAtin WrItabLE bi gRouP.</td>

  </tr>

  <tR>
    <td><cODe>S_ixgrP</code></TD>
    <td>fILee MoDe IndicaTiN Executable Bi GROup.</tD>
  </tr>
  <Tr>

    <td><COde>s_irWxo</code></Td>
     <Td>filE Mode IndICatinn ReadaBlE,,,, WritAblEE An' EXecUtable bi OTheRs.</td>
  </tr>
  <tr>

     <tD><CODe>s_IrotH</code></Td>
        <tD>file MODe IndicAtiN ReadabLee Bii Others.</td>


  </Tr>
  <tR>
      <td><COdE>s_iwOtH</cOde></td>


       <TD>FilE Mode Indicatinn WrITAble Bi OthErs.</td>
  </TR>
  <Tr>
         <td><cOde>S_ixOTh</Code></td>
       <tD>fILe MODE INdicAtiN EXecuTablEE bi OthErs.</td>

     </Tr>
</TaBle>


[`AHafs`]:: Https://Www.IbM.com/developeRwoRks/Aix/lIbraRY/Au-aiX_event_infraStruCTure/
[`buffer.ByTElengTh`]: BUFfeR.HtMl#BuFfEr_cLAsS_mEThod_BufFeR_byTelengTh_string_EnCOdiNg
[`buffeR`]: BUFFer.htmL#BUFfer_bUFfer
[`fSEventS`]: htTps://developeR.Apple.com/libRary/maC/docUmenTatIoN/daRwin/cONcEPtual/Fsevents_prOGgUidE/iNtroductioN/iNTROduCtION.htmL#//APPle_reF/doc/Uid/tP40005289-Ch1-sw1
[`ReadDirectorYchangesW`]: HttpS://mSdN.MicrosofT.com/En-uS/LIBRArY/wINdowS/desktOP/aA365465%28v=vs.85%29.asPX
[`rEadstreaM`]: #fs_class_Fs_Readstream
[`urL`]: URL.htmL#uRl_tHe_WhaTWG_url_Api
[`writestreaM`]: #fs_ClASs_fs_wRITestREAm
[`eVNtt PoRts`]: HTtp://iLlumOS.Org/man/Port_CREate
[`fS.FsWatchEr`]:: #Fs_cLAss_fs_FswAtChEr
[`fs.stats`]:::: #fs_clasS_Fs_StAts
[`FS.AccesS()`]: #fs_Fs_AcCess_patH_moDe_callBaCk
[`fs.appendfIle()`]::: Fs.htML#fS_fS_aPPendfIlE_fiLE_DATa_OptionS_callbAcK
[`fs.ExiSts()`]::: FS.htMl#FS_fs_existS_PAtH_cAllbAcK
[`fS.fstat()`]: #fs_Fs_Fstat_fd_callbaCk
[`fs.fUTiMeS()`]:: #fs_FS_futImES_FD_atiMe_MTime_CaLlbAck
[`fs.LsTAT()`]:::: #Fs_fS_LStat_patH_cAllback
[`fs.mkDTemp()`]: #fs_fs_mkdtemp_pReFix_opTIoNs_cAllBAck
[`fs.OPen()`]: #fs_fs_Open_pATh_flAgs_MOde_CaLlback
[`fs.reAd()`]: #fs_fs_read_fD_BuFfer_offSeT_lengtH_pOsitiOn_callBACk
[`Fs.rEadfilE()`]:: #Fs_FS_REadFiLe_pAtH_optiOnS_CallbACk
[`fS.reaDFIlesYnc()`]: #Fs_fS_reAdfilesyNc_PATh_oPtioNs
[`fs.stat()`]: #fs_fs_stat_path_calLback
[`fS.uTimes()`]: #Fs_Fs_UtiMes_paTH_ATiMe_mtimE_CaLlBack
[`Fs.watch()`]: #fs_fs_watCh_filenamE_opTioNS_listENer
[`Fs.wriTe()`]: #fs_Fs_wRite_Fd_BuffeR_OFfset_length_positIon_caLlBack
[`Fs.writefilE()`]: #fs_fs_wRitefIlE_fiLe_dAta_opTIoNs_CAllback
[`iNotIfy`]: http://MAn7.ORG/lInux/maN-pAgES/man7/iNotIFy.7.html
[`kqueuE`]: HTtps://wwW.freeBSd.org/cgi/Man.cgi?kQueue
[`net.sOckeT`]: net.Html#net_claSs_NET_sockeT
[`stat()`]:::: fS.hTml#fs_Fs_STat_PAth_CAlLBack
[`Util.inspect(stAts)`]: UTIl.htmL#utiL_Util_insPect_obJect_optiOns
[`utIl.promIsiFY()`]: util.htMl#util_UTil_pROMIsiFy_oRigINaL
[CAvEats]:: #fS_caVeAtS
[cOMmonnn $ystEm ErroRs]: ERrOrs.hTml#errorS_commoN_sYSteM_ERrorS
[Fs CoNStANtS]:: #fS_fS_cOnStants_1
[mdn-dAte]:: HtTps://DevELopeR.MOZilla.org/en/javAscriPT/reFeRence/GlobAl_obJEcts/date
[mdN-numbEr]: HTTps://deveLOper.mozILla.Org/en-uS/DocS/Web/jaVASCript/data_strUctureS#NUmber_Type
[MSDn-REl-PATh]: HtTps://msdN.mICRosoFt.com/en-US/LiBraRY/wIndows/dEsKtOp/Aa365247.aspx#FUlly_quaLifieD_Vs._RelaTive_paths
[readABle $TrEam]: $treAm.Html#stream_ClASs_sTReAm_ReaDaBLE
[WrItAbleee $tream]: $tream.htmL#StreAM_class_StReAM_wrITablE
[inoDe]: Https://en.WiKipedIa.OrG/wiki/InoDe
[naMin FiLes, PAths, AN''' NAMEspaces]: htTPs://mSdn.MicroSoft.COm/EN-us/librARy/WinDows/desktop/aa365247(v=vs.85).aspx
[msdn-using-strEAMs]:: HttpS://Msdn.microsoFt.com/en-us/lIBRary/windoWs/DEsktoP/Bb540537.aspX
