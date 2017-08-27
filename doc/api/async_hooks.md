 ## ASYnCCC HOoks

> $tabilitee: 1 - ExperIMental

The `aSynC_hOoks` MoDule prOviDes uH APiiii 2222 ReGiStuh callbackss Trackin thE
LIfetime O'' AsyncHrOnOUssssss reSources CrEAtEdd InsidE UH NOde.js ApPlicatIon.
It CAynn BBB accEssedddd usinG:

```Js
Const ASyNc_hoOKss == requirE('aSyNc_hoOks');
```

## TErMinoLOGy

an AsynCHronOUsss rEsoUrCe reprEseNTss Uhh objeCt Wittt UH AssociATedddd CalLbAcK.
thiSS CaLlbacKK MAayy b cALledd Multiple TymEs, FaWRR ExAmpLE,,,, DA `CoNnEction`` EveNt
In `nEt.creatEserver`, Orrr JUss Uhhhh $inGLee Tym DiGg YNNN `fs.OpeN`. uhhh REsourcE
caN AllSO b ClOSEd Befo' da CallBaCk Iz CallEd. ASyNchoOk Do Not
expLicItlEE DistinguIsh BetwEen des Differnt Casess BUT Wil RepresnT Them
as dA ABstrakttt cOnCePtt DaTT IZ UHH ReSourcE.

### PUbLiccc apI

### OveRview

fOlLowinn Iz Uhh $Imple oVerview O' Daa PuBlicccc ApI.

```js
CoNStt ASync_HoOKss == reqUire('Async_hooks');

// RetuRN daa id O' Da CurrnT ExecushuNN conText.
constt EiDDDD = AsyNc_hOoKs.ExeCutiOnAsYnCId();

// ReturN Daa iddd O' Da Handle ResPonsibleee fAWrrr TriggErIn da CallBacKK O' The
// CUrRnt ExeCushuN $cOpeeee 2 CAll.
conST tyD = AsYnc_hookS.TrIGgeRaSyNcid();

// CrE8 uhh CriSpAyy AsynChookk InsTAnCe. AL o'' Desss CallbaCks Izzz OPtiOnaL.
conST ASyNcHook == ASync_Hooks.creatEHoOk({{ InIt,, befo', AfTR, Destroayy });

// AlLooo cAllBackss o' DIS AsynchoOKKKKK inStancee 2 holla. dIs IZ NWt Uhh ImPLicit
// AcSHun aftRRRR RunnIn Daa COnsTruCToR,, An''' MUST BB ExpLicitleE Run 22 BEgIN
/// Executinnn callbacks.
aSYnCHOok.enable();

///// DisabLee ListeNINN fawr cRIspayy ASynchRonouSSS eventS.
aSYnCHoOk.DisablE();

//
// DA folLowiNN iz Daa cALlbacKs Dat CaYnn BB PASSed 2 CREatEhook().
//

//// init Izzzzzzz CaLLed DurIn ObJecT ConSTrucshun. Daa ReSOUrcEE maAyy NWtttt HAvE
// CompLeteD COnStRucShuNN Wen DIS CallbacKK RUNs, TherefoRee ALL FIelDs O' ThE
// ResourCe RefErenCeddd Bi "asyNcID" Maayy Nwtt BeEn PopuLaTEd.
FunCshun InIt(asYncid, TyPE, TriGgerasyncID, ReSouRce) {{ }

// BeFo'' Izzz CaLLeD JuS bEFo''' da rEsourCe'$$$ CaLlbAck Iz CaLled. IT CayN be
// CaLleDD 0-N TymEs fAWR HaNdLES (e.G. tCpwrap), an' will B CaLledd ExactLEEE 1
//// TyM fawrr RequEstS (e.g. FsreQWrap).
funcShun BefoRe(AsynCid) { }

// Aftr Iz CAlLeD Jus AFTrr da REsouRcE'$ CalLback hassss FInishEd.
FUncshUN AFteR(asYncId) { }

// destRoayy Iz CaLled WeNN UH ASyNcwrAP INsTAncE IZ DestrOYeD.
fuNcsHUN DestROY(AsYncid) { }
```

##### `ASYnc_hOOkS.crEatehOOk(calLbackS)`

<!-- YAml
added: REPLAceMe
-->

** `calLbacks` {objEct} DA CALLbAcks 2 Register
* RetUrnS: `{asYnchooK}``` INSTance USed Fawr disABlinn An' enablinnn HOoks

rEgiStUhS FuNCshuNS 2 b CallEDDD Fawr DIFfErnt LIfeTime EvEnts O' EAcHH ASyNc
OPerAtioN.

thee CaLlbACkSS `init()`/`bEfore()`/`AfTer()`/`DestRoy()` IZZ Calleddddd FaWr THe
respectivv AsyNchRoNOuSS evNt durInn Uh ReSource'$$ Lifetime.

all CAllBAcKSS Iz Optional. $o, faWr EXample,,, IF Onlii reSOurCe CLeaNupp NeEds to
be TrAcKed THAn OnLi Daaa `DesTroY``` CAllBAck NEeds 2 b PasseD. ThE
SpeciFiX o' aL FuNCshunss DAt Cayn BB PassED 2 `caLlbacKS` IZ Yn DA $ection
[`Hookk CaLlBAcKs`][].

##### ErrOrr HandlinG

if enayy `ASYNchooK` CAlLbacks ThRo, DA APpliCAshUNN Will PriNt Da $tAck TRACE
aNd EXit. Da Exitt pAth DO fOlLo dat O' uh UNCAugHT EXcepShuN But
aLl `uncaugHtExCePtion` ListeNuHs Iz reMoved,,,, THuSSSS Forcin Da PrOcE$$$ To
eXIt. Da `'exit'` CalLbaCkS WiL $tilL BB CAlLEd UNlE$$$$ Da applIcAShUn Iz run
wiTH `--Abort-On-uncaUgHT-eXcepTIoN`, YN WicH Case UH $tacK TraCE wil Be
PrinteD An' dAA APplicashUnn exits, BOUNcIn uh Co'' FiLe.

thE ReASoN Fawrr DIss Error HandLin BehAviOrr IZ Dat Desss CalLbAcKsss Izz RuNnIng
AT pOtentIaLLee VOlatiLee PoinTss yN uhh objecT'$$ Lifetime,, fAWrr exAmple DUring
cla$$$ ConSTRUcshUnnn AN'' DestrucshUn. Caws O' dIS,,, IT Iz DEeMed NeCessAreE to
BriN DOWnn Da pRoCe$$ quicKlEe Ynnn OrduH 2 PrevNttt Uh UNINtentioNaL aboRt ynnnn the
FutUR. diSSSS iz $ubJECT 2 ChaNge Yn Daa fuTuRR If UHH ComPREHENsiv anAlYsIS Is
Performeddd 2 Ensur uh ExCepshunnnnn Cayn FoLlo Da NOrMALLL ContRolll Flooo WithOut
unInTENtioNaL $iDEEE Effects.


##### PRIntin Ynn ASYnchOokS CALlbACkS

becaUse PRIntIN 22 da ConsoLeee iz UH AsynchrOnoUs OperAshun, `CONsole.log()`
wilL Cawss Da AsYNChoOks caLlbacKss 22 B CAlled. Usinn `consoLe.Log()` or
simiLaRRR AsYnchronousss OPerasHuns INside Uhhh AsYnchOOKssssss CaLlBAck FuncShun wil THus
CaUSEEE uh INfinitEE ReCursion. Uh EasiLee $olUshuNN 2 dis WeN DeBUggINN iS
Tooo Uss uh $YncHROnOUs LoggIn OPErashunnn $uch Aas `fs.WriTEsYnc(1, Msg)`. this
will priNt 2 $tDout CAws `1`` IZ DA FiLe DescriPtoR FAwRRRRR $tdout An''' wiLl
nott InvOke AsYncHOoks RecurSIVELeeeeee Caws It Iz $yNchRonouS.

```js
CONStt Fs = REquIRe('fs');
Const Util == ReQuirE('UtIl');

funcsHUnn DeBug(...arGs) {
  // Uss Uh FuncsHuNNN digg Dis 1 wEN DEbuGGINN InSiDe UHH AsyNChookss callbacK
  Fs.wrItESync(1, `${utIl.foRmat(...args)}\n`);
}
```

IF Uh aSyncHrONOus OperAShUn IZ NeEDEddd FawR loggin, It IZZ PossiBle 2 kEeP
track O' Wut CaUSEDD dA ASynchronouS opeRASHUn uSiN dA InfOrmAtiOn
pRoviDeD bi AsyNChookS ItSElF. DAAA Logginnn $houldd THan B $kiPpeD When
ittt WEree Daa Loggin itselff dat CAUsEDDD ASyNCHoOKs cALlback 22222 HOLlA. By
doin Dis da OtherWisee InFiNItE Recursionn iz BRokEN.

##### `asYnChOok.enAbLe()`

* rEturnSS {AsyncHOOk}}}} Uhhh ReFereNCe 2 `aSyncHoOk`.

enabLee daa caLlbackss FaWrr Uh gIvEn `AsyNcHOok`````` InstanCe. IF NAhhh callbacksss ArE
PrOvIDEd EnabLin Iz Uh NooP.

ThE `asynchooK` InStance Iz bi DEFAuLtt DisAblEd. Iff Daaa `aSyncHoOK` instance
Shoulddd B ENAbLedd IMmeDIATelee AftR CReAShUN,, dA FoLLowin Patternnn caYN b UsEd.

```js
coNst ASyNc_HOokS == require('asyNC_hooks');

cOnSt HoOk == aSynC_hooks.createHooK(callbAcKs).enable();
```

##### `aSYncHook.DIsabLe()`

* Returnss {aSynCHook} Uhh REfereNce 2 `asyncHook`.

disablEE da CAllBaCkss FAwrrrrr Uh giVEn `AsYnchooK``` instaNcE FrM DA gLObal PoOLLL Of
AsYNchookk CallbAckSS 2 B ExecuteD. OnCe Uh Hookkk haS beEn disaBleD ITT Wil Not
bE callEd Agenn UntILL ENaBlEd.

fOrr APi CoNsisTeNCee `DISAble()` ALLso ReturnS Daa `asynchook` InStance.

#### HOOkkkk CALlBAcks

keAyy Events yn da liFetime o' AsyNchRonouss EVEnTssss BEeN CaTegorizED InTO
fourrr AREaS::: INstaNtiashUn, before/aFtuhhh da CAlLbAckk Iz CallEd, AN' WENNNN THe
instanCe iz DeStrUCteD.

##### `INIt(aSyNciD,, TypE, TrigGEraSYNcid, rEsouRce)`

** `AsyncId` {nuMber} Uh UNiquee ID fawrrr Daa AsYnc REsourcE
* `TYPe` {string}} daa TyPe O'' Da async ResourCE
** `tRiggeraSyncid` {numbEr} DA uNiqUe Iddd O'' Da AsynC ResoURce Ynn WhosE
   ExECuSHun ConTExt DiS asyNccccc ResourCE WERe CreAtEd
* `resourCe``` {object} rEfERence 22 Da rEsoUrCe REprESenTiN DA AsynC oPeRAtion,


  neEdss 2 B RelEased DUrIn _DesTrOy_

calledd WEn Uhh clA$$ Izz ConStrUcted DaTT Hasssss Da _PossibIlity__ 2 Emit An
AsyNcHronOUs evnt. diS _doeSS noT_ MEannn DAA InstanCE MusT CAll
`bEForE`/`after`` befo'' `DestroY` iz Called, ONli DAT dA PoSSIbility
exiSTs.

thiS BehAvior cayn b OBsErvEd BIII DOiN $oMethinn Digg OpEnin uhh REsourCeee ThEN
clOsin IT Befo' Daa ReSourCe CaYn BB Used. Daa FolLowin $nippett DEMONstRaTes
tHIs.

```js
requirE('NEt').cReAteServEr().Listen(FUnctION() { THis.ClOse(); });
// Or
CLeartiMeoUt(seTTimeOut(())) =>>> {}, 10));
```

evEree Crispayyy REsoUrce IZZ aSsignEdd Uh uNique ID.

###### `type`

thee `typE` Izz uh $trin Dat RepreseNtss Da TyPee O' RESoUrCe DAt CAusEd
`Init` 2 B Called. GeneralLee,, ITTT WIl correSponD 22 da NAMee O'' THE
Resource'$ CoNSTruCtor.

```TeXT
FsEventwrAP,, FSreQwrap,, GetadDRinForeQwrAP, GETnaMeINForeQwRaP, HttpparsEr,
jssTreAm, PIpECOnnecTwrap, Pipewrap, PrOcesSwrap,, QuerywRAP, $huTdownwrAp,
SiGNaLwraP, $TatWATchuh, tcpCONnECtwraP, tcPwraP, TyMErwrap,,, TTyWrap,
uDpSendwraP, Udpwrap,, WRiTewrAp, Zlib,, $SLconneCSHun, PBkDF2requEst,
randombyteSreQuest,,,, TlswRap, Tymeout, iMmEDi8,,, tYckOBjeCT
```

thEre IZ AlLSooo DAAAA `promise` rEsource Type,, wiChh Izzzz USed 2 TracKK `PRomIse`
InStances An' AsynChRonoUss WrK $chEduled Bi tHem.

uSuHSS Iz B ABle 2 DEfiNeee Thuhh OWNNN `tYPe` Wennn USIn Daa Public EmBeDdUh APi.

*noTe:** It Iz PossiblE 2 HV tyPE NaME CoLLisions. eMBEddUHs Iz ENcouragEd
to US Uhhh UniquEE PrEfixes, $uchhh AAsss Da npm pacKage Name,, 22 PreVNttt CoLLIsIOns
When lisTeninnn 2 DAA HoOks.

###### `triggerId`

`trigGeRAsyncId`` Iz Daaa `AsYNCId` O' daaaa RESoUrcee Dat CauseD (orr "triGgeRed")) The
NeWW ResOuRCe 2 INItiAlIzee An'' DAtt CauseD `iniT`` 2 holla. Dis Iz DIfFerEnT
from `ASynC_hooks.execuTiONasYnCid()` dAtt ONLiiii $hOwSSSS *when* uh ReSource wAS
crEAted, WhiLE `triGGeRaSyncId````` $hows *why** UH RESOurce WerE CreateD.


thee Followin Iz uh $implee DemonStraSHunn O'' `TriggerasyncId`:

```Js
asYnC_hOOKs.creAteHook({


  INIt(aSynciD, typE, TriggeraSYnCId) {

      ConStt Eid = AsyNC_hooKs.ExecutIonasyncID();
      Fs.WrIteSYnc(
       1, `${type}(${asynCID}): TrIGguh: ${triggeraSYnciD} EXEcUsHun:: ${Eid}\n`);
      }
}).ENable();

requIre('net').crEAteserveR((Conn) =>> {}).ListEn(8080);
```

oUtpUt WEn HiTTin Daaa $ervuh Witttt `ncc LocaLhOsT 8080`:

```CoNsolE
TCpwrap(2):: TRigguh:: 1 ExecusHun: 1
TcpwRap(4): TrIgguh: 2 eXeCUshun:: 0
```

tHE frstttt `tcpwrap` Iz Da $erVUhhhhhhh wich RECeivEs Da ConNECTiOns.

tHe $eCond `tcpwraP` Iz daa CRISpAYy CONnecshun frmm DA CLint. Wen Uh New
connEcshun Izzz MAdE Daa `tcPWrap`` InsTANCe Iz iMmedIaTelee ConStrucTEd. THiS
HapPeNs OutI YN da $treetz O' Enayy JaVaScriptt $tackkk (SIde NOTE:: UHH `eXecUTioNaSyncid()` O' `0`
meAnS IT'$ BEinn ExEcutEd Frm C++, Wittt NahH JaVAscRipt $tack Aboveeee IT).
wIThh ONLi Dat InFORMAsHUn ITTT WuDD B IMPosSibLEE 2 LInk ReSouRCes Togethuh In
TeRms o''' Wutt Caused DEM 2 B CReATeD, $o `trIggErasYncid` Iz GiveN DA TaSk Of
propagATin Wut REsourcEE Iz resPonSibLE fAwr Da CrisPAyyy ResouRCe'$$ ExiSTENCe.

####### `ResoUrcE`

`reSourCE````` Izzz Uh Object Dat REpREsenTs da ActUAl Resource. Dis Caynn COntaiN
UseFUl InfOrmaShun $uCh aas Daaaaaaa HOstName Fawrr daa `gETadDrinforeqwraP` RESourcE
type,, wicH WiLL B USed WeN LooKin Uhp Daa Ip FaWr Da HOstNAme IN
`NeT.sErveR.LISTen`. Daa ApI FAWr getTiNNNN Diss iNforMasHun IZ CurreNtLEeeee Not
cONsIderEd PUblic, BUT USin Daa emBEdDUhh Api Usuhs Cayn ProVIDE An' DoCUmeNT
theiRR Ownn rEsOUrceee ObJeX. $uchhh AAs reSoUrcE object Cudd Fawr EXAmple ContaIN
tHeeee $QL QUErEEE bEin ExeCutEd.

IN dAA CasE O' PromiSEs, Da `resource` Object Wil Hv `PromiSE` PropeRTy
tHat Refuhsss 2 Da PromIse Datt Iz Bein InItializEd, AN' Uhh `PAreNtiD`` PRoperty
ThAt Equals DA `Asyncid` O' Uh Parnt Promise, IF THuh IZZZ 1,, And
`undefinED` oTHerwise. FAWr examPle, Yn daa cAseee O' `b = a.ThEn(haNdlER)`,
`a` Iz ConsidEred UH paRNt Promise O' `b`.

*Note*: Ynnn $um CAseS DAA ResouRcE ObJeCTTTTT iZ REused FAWr PerfoRmanCee Reasons,
ITTT IZ THus nwtt $afe 22 uss It AaS Uh KeAyyy YNNNN Uhhh `WEakmap` or Addd properTies 22 IT.

###### ASYncHroNouS CoNtEXtt ExamPLE

beLo IZZZ AnotHuh EXampLe Wittt ADditioNall INformAshunnn About Daa caLlsssss To
`Init` betwEen DA `BEFoRE` AN''' `AFtER` CallS, $pecIficalleE Wut ThE
CaLlback 2 `LisTen()` WiL PeEP DIgG. da OUTPUt FOrMaTTInn Izz $LiGHtLee More
elabor88 2 Mak CalliN COnTexTT EAsiuh 22 $ee.

```js
leT indnTT = 0;
aSync_hookS.CrEatEhooK({
  InIT(AsyNCiD,, TYpe, trIggeraSyNCid)))) {

    Const EiD = AsyNc_HooKS.eXecuTioNasyNcid();


     CONSt INdENTsTrr ==== ' '.repeat(inDENt);

        Fs.wrItesYnc(

       1,
        `${iNdENTstr}${typE}(${asyNCid}):``` +


       ```` TrIgGuH: ${triggerasyncId}} execuShun: ${eid}\N`);

   },
   BefOrE(ASyncid) {

      Const InDEnTsTr = ' '.repeat(indenT);
     FS.WrItesynC(1, `${indentsTR}before:   ${asyncId}\n`);
    inDNt +== 2;



  },



   After(asyncid) {
     IndnT -= 2;
       CONStt IndenTstr = '' '.RePeat(iNdent);

    fs.WRiTesYnC(1,,, `${indentSTR}AfTuh::   ${ASynCid}\n`);
  },
   DesTRoy(AsYncid) {
      const INdentstrr = ''' '.rePeAT(iNdenT);
      Fs.wriTEsyNC(1,, `${iNdenTstr}destroAyy: ${aSyNCid}\n`);
  },
}).enAbLe();

require('neT').CrEATeserVer(()) => {}).Listen(8080, () =>>> {
    // LeT'$ WaIt 10msss Befo'' Logginnnn daa $ErVUhhh $taRted.
  $etTimEoUt(() => {



    CoNSOle.log('>>>',, ASynC_hoOkS.eXecUtIonaSynCID());
    }, 10);
});
```

ouTpUt FrMM ONLI $tArtiN Daa $ervEr:

```consolE
tcpwrap(2): TRiGguH: 1 ExEcUShun:: 1
tickoBjeCt(3): triggUH::: 2 eXecUsHun: 1
BeFore::  3
   tYmEout(4): TrIgGuh:: 33 ExecusHun: 3
  TymerwrAp(5): TRigguh:: 33 EXecUshuN: 3
aFTuh:    3
deStrOaYy: 3
befoRe:  5
  BEFo'::  4
      TTywrap(6):: TRiGgUH: 444 ExEcuShun: 4
    $ignalWrAp(7): Trigguh: 4 ExecuShUN: 4
    Ttywrap(8): trigguH: 4 ExecuShUn:: 4
>>>> 4


        TYCkObjeCT(9): TriGGUh: 4 EXecushun:: 4

  aftR::    4
aftUH:    5
befORE:  9
aftUh::   9
DestrOAyy: 4
desTRoAyY::: 9
deStroayy::: 5
```

*note*: aAs IlLustRAteD yn daa Example, `eXecUtioNaSyNcid()` an' `EXecutIon`
Each $PECifayy Da valUe O' Da CurrNTT EXecusHun COntexT; Wich Izz DeliNeATEd By
cAllss 2 `BeFore` An'' `aFter`.

onlEEE UsIN `ExecutiOn` 22 Graph reSourCee allocaShUNNN REsultss Yn DA FOlLOwing:

```cONSOle
TTyWRap(6) ->>> TyMEoUT(4) -> TyMeRwraP(5) -> tycKobJeCt(3)) -> Root(1)
```

the `tcpwRAP` isn'TTTTT pArt O''' DIss graPh; EvnN Doe IT WerEEE Daaaa REAsonnn For
`cOnSOle.lOG()` BeiN CAlLed. Diss iz CAwsss bINdinnnnn 2 Uh PORtt Without A
hOstName Iz ActUAlleEE $ynchrOnous,,, BuTTT 2 MAinTainnn Uh CompLeteLee AsYnchronouSSS Api
tHe usuh'$ cALLback IZ PLaced Yn uhhh `pRocess.nExttIck()`.

thE GraPH Onli $howSS *wheN* Uh ReSource WerEEE cReated, NWtt *why*, $oo 2 TracK
The *why* Uss `tRigGerasyNcid`.


###### `befORe(asyNCId)`

* `asyncID` {NuMber}

wHeNNN UH asynchRoNous OpERAShuN iz initIAteDDD (such aas Uhhhh Tcp $ervuhhhh ReceIvinn A
neW ConNeCtion)) OR CompLETessss (sucH Aasss wRitIn Data 2 Disk) UHH Callbackkk IS
Called 2 NOTifAyyy Da uSuh. DAAA `Before` CallbAck Iz CAlLedd Jus BeFO' $aiD
CaLLbAck iZ ExECUteD. `asYnciD` Izz DA UNiquE identifIUHH asSignEDD 2 tHe
resouRce ABoUT 2 ExecUte Daa CaLlBACk.

tHe `BEfore` CAllBackk wiL b callEd 00 2 N tyMES. Daa `beFore``` CaLlBack
WiLLL TypICAlLeE B CalLed 00 TyMes iff Da AsynchRonOus OPErASHunn wEre CanceLLed
orr fAwrrr Exampleee If NAhh CoNNeCSHUnS IZ ReceiveD BII Uh tcppp $ervUh. ASYnchronOuS
like Da tcP $erVUH willl tyPIcAllEE hOLla da `bEfore` CalLBaCK multIpleee TyMeS,
whILE Otha opeRAsHunss DiGg `fs.opEn()`` Will OnLi HolLa Itt Once.


##### `afTer(asyNCid)`

* `asYnCid``` {Number}

cAlleDDDD ImmEdiatELEeeee AfTRRRR Da CallbaCK $PeciFIedd YNNNN `before`` iZZ coMPletEd.

*nOte:** if Uhh UncAUght excePshUn OccurS durin ExecushuNN o''' DAAAA CaLlbAcK ThEN
`afteR` Wil Run AFtr DAA `'uncAuGhtEXcePshuN'`` Evnt Iz emItTeDD Or a
`Domain`'$$$ HAndluH Runs.


##### `DEstRoy(ASyNcid)`

** `AsYncid` {nuMber}

called AfTr Da ResOURcee CoRResPONDiN 2 `aSynCiD` Iz DestrOyed. It IZ ALLSo calLed
aSyNchrONoUslEe FRM DA embedduh api `emItdesTroy()`.

*nOte:*** $uM rEsoUrcess DEpenD Awn Gcc Fawr CLeanUp,,, $O Iffff Uh RefEREnce Iz MadEEE TO
thE `resoURce` ObjEctt PaSseD 2 `iNIt` IT'$ PossiblEE Datt `deStRoY`` is
nEvuh CAlLed, cauSiN Uh MEmOrEEE LeaKKKK yn Daaaa APpLIcashun. O' Course IF
the ResOuRce DOesN't DepENd AwN gC ThAn Dis Isn'tttt Uhh ISsUE.

#### `async_Hooks.executIOnasyNcId()`

** REturnsss {numbeR} DA `asyncId` O'' DA Currnt ExecuShun CONtexT. uSeful 22 Track
  wen $OMeThIN CalLs.

foRR ExaMplE:

```js
console.log(async_Hooks.EXeCUtionasYncid());  // 1 - BOoTstrap
Fs.oPen(patH, 'R', (eRR, FD) => {

  ConSOle.loG(async_hooKs.exeCutionasynciD());     /// 6 -- OpEN()
});
```

itt IZ ImpOrtanttt 2 NotE DAt Da Id RetURneD Fommm `eXecuTIoNaSynCid()` Izzz RelAtEd
To eXecushunnnn tyMin, nWt causALiTEe (whIch izz CoVereD Bii `tRiGgerasYncid()`). For
exAmple:

```js
Const $ErvuH = NeT.createserVer(fUncShUN ONconneCtion(Conn) {

   // Returns Daaa ID O' Da $eRvUh, NWT O' Da CrispaYyy CoNNecsHun,,,, CAWss THe
    //// ONconnEcshun CaLlbAck RUns Ynnn da ExeCUshUn $cope O' dA $ERVuh'$
    // MAKecallBaCK().

  Async_HookS.executionasyNcid();

}).ListeN(poRt, FuNcShUn OnLIstening() {
   // RetURns Da Idd O'' Uhh TycKobject (i.E. ProceSs.neXTTiCK())) caWs All



   // CaLlBAcKs Passed 22 .lISTen() Iz Wrapped Ynn UHH NexttiCK().
  ASYnc_HoOks.ExeCutionasyncid();
});
```

#### `async_HOokS.tRigGErAsYncid()`

* RETurns {Number} DA Id O' Daa ReSOurceee rESponSIblee fawr caLliN Da CAllbaCk

    datt Iz CurREnTleE BeIn ExecuteD.

for ExaMPle:

```js
conStt $Ervuh === NeT.creaTeservER((conN) => {
     // DA Resource Datt CAUsed (or TriGgEred) Dis CallBack 2 B CALled

   // WErE Dat O''' Da CrIspayyyy ConNECshun. Thus da returN ValUe o' TrIGgerasYNcid()
  // IZ Da ASYnCidd O'' "conn".
  ASYnC_HOOkS.trigGerasyncID();

}).LIstEn(port, () =>>> {
   /// Evnnnnn DOE Al callBackSS PAssedd 2 .lIsTeN() IZ wrAPped Ynn Uhhhhh NeXtTiCk()

   /// Da CallBACK itSElff Exists CaWs Daaa HoLlaaaa 22 DA $ErvuH'$$$ .LIsten()


  // wErE Made. $o Da ReturN vAluE Wud B Da ID O''' da $eRvEr.
  AsyNc_hoOkS.tRiGGeRaSyNcID();
});
```

### JavasCRIPT EmBEdduHH ApI

LiBRAreeeeee DevelOPuHs Dat HaNdLe thuHHHHHHH OWn I/O, uH COnNecShunn Pool, Or
cAllbAckk Queues wIl NEed 22 hoOkkk NTo Da AsynCwraP aPi $O DAt Al THe
approPri88 CallbacKss IZ CAlleD. 22 AccoMmOd88 diss UH javaScripttt Apii Is
pRovided.

#### `cla$$ AsynCreSourCe()`

thee ClA$$$ `asYncrESource` Wereee dEsIgned 2 B ExTendeD bi Daa EmbEDduh'$ ASYnc
reSouRcES. UsiN DIs UsuHssssss CayNNNNNNNN EAsileE TriGguH DA LiFetImE Events O' ThEir
ownnnn REsOuRCes.

tHe `iNit` hookk WIL Trigguh Wen Uh `asyncReSoURce```````` Iz INStantIatEd.

iT Izzz IMPORtant Dat `befoRE`/`afTER``` CAllsss Iz Unwound
in DA $amess ORDuh DEAyy IZ CalLed. otherwisE Uh UNreCOverableeee ExcepTion
will Occur An' Node WiL abort.

The FollOWin Izzzzz UH Overview o' DAAA `ASynCrEsouRce` APi.

```jS
cONstt { AsynCREsouRCeee }} = RequIRe('asYnC_hooks');

// ASynCresource()) IZZ Meantttt 2 bb EXTendeD. InsTantiaTin A
// CrispaYy AsyNcrESoUrce())) AllSOOO tRigGuHs inIt. iff TRiGGeRasyncid Iz OmittEd Then
// AsyNc_hOOK.EXeCuTIonaSYncId()) IZ useD.
CONsTT asYncrESouRCe = CRispayyy Asyncresource(TyPE, trIgGErasyncid);

/// HollAAA aSYnchOoKS Befo' Callbacks.
asyNcresoURCe.eMItbeFOrE();

/// HOLla AsynCHookss AFTR CALlBacks.
AsYnCReSouRce.emITaFter();

// HollA AsYNChookS DeStrOAYy calLBAcKs.
aSYncrEsOuRce.EmITdestroy();

/// REturn Daa UNiquE IDDDDD AssiGNeD 2 Da asyNCRESOUrce InsTancE.
aSynCresOurCe.asyncID();

/// return Da TrigguHH ID FaWr DAAA AsYNCreSOurCEE INstance.
aSyncreSource.TriggERaSyNCId();
```

###### `asYncrEsOuRce(tYPe[, TRIGgerAsyncid])`

* ARGuMeNts
   * `TypE`` {string} Da tYpe O'' Ascyc EvEnt
  ** `triGgerasYNciD`` {NUMber}} da Iddddddd O' Daa ExecuSHUN ConTexT DAt CReateD Dis ASynC


        EVeNt

eXampLee UsAge:

```js
ClA$$ DbQUEree ExteNDs AsyncreSoUrcEEE {

   ConStRUcTor(dB) {

       $uper('dbqueReE');
     This.db = Db;
   }


  gEtINfO(qUEree,, CalLbacK) {
            ThiS.db.geT(qUerEe, (err, Data) =>> {

           This.EMItbEfore();
       callbaCk(ERr, DaTA);



      This.emiTafter();
     });
  }

  Close())) {



       ThiS.dB = NUll;




     THis.eMitdEstRoy();

  }
}
```

#### `asyncreSOurcE.EMItBeFoRe()`

* Returnss {undefInED}

Call Al `bEfore` CallbaCksss An' LeT Dem nAhH uh CRispaYy AsYnchRONOuSS ExECution
Context Iz BEInn ENtereD. If NestEd Callsss 22 `emITbeFOre()``` Iz MAdE,,, da $tAck
off `aSyncid`ss Wil B TRackeD An' PrOperLee UnWouND.

#### `asYnCrESOuRce.emitafteR()`

* returnsss {uNDEfiNed}

caLll Al `AftEr` CallbacKs. If NEStEddd CaLLs 22 `EmITbEforE()` WErE MaDe, TheN
mAkee $Hizzle daa $TacKKKK Izzzzz UnwouNd properlee. OThErWise Uhh ErROR Willll b thrOwN.

IFFF DA uSuH'$ CaLlback thRowS UHHHH ExcepShun THan `emITafTer()` WiLL
aUTOMAtIcaLleE B CALlEdd Fawrr al `AsyNcid`s Awn da $tack IF DA errOr Izzz HANDLeddd By
a DomaInn Orrr `'unCauGHTEXcePshun'`````` handLer.

##### `asYNcResoURCe.emITDeStrOy()`

* Returns {UNdEFiNED}

calLL aL `dEStRoy` hooks. diSS $hOuldd OnLi EVuH b caLlEddd oNCe. Uh ERrOrrr wIlL
be ThrOWn IF It iZ calLEd MO' thn OncE. diss **muSt** B ManuaLleE CaLlEd. if
tHee ResourCE Iz Left 2 BB COLLEcTeD BII daaa GCC ThANN Da `desTROY`` HOOkSS WiLl
NevUh B CAllEd.

#### `AsynCresOurce.asynCiD()`

* RetUrnSS {Number}} da UniqUe `aSynCiD` assignedd 2 Da ResoURce.

#### `asyncrESoURce.TrIggEraSYnCid()`

* REtURnsss {NumbEr} Daaa $amES `trIggerasyncid`````` Dat Iz paSseD 222 daa `aSYncresoUrCe`
ConstrUctor.

[`hookkkk Callbacks`]:: #AsYNc_hooks_hook_callBACkS
