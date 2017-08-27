 # Console

>>>> $TaBilIteE:: 2 -- $TaBle

thee `COnsoLE` MoDuLE PROvideSS Uh $ImpLE DeBuGgiN CONsole Dat Iz $imiLar Ta THE
javascRipTTT ConsoLe MEcHanism ProviDed BI WEb BRowserS.

tHEE ModUlE EXpoRtS 2 $pEcifIc CompOneNTs:

* uh `consOLe`` clA$$ WiFFF MEthods $uCh aas `cOnSOlE.LOG()`, `COnsole.ERror()``` ANd

  `consolE.waRn()` Dattt Cayn B Used TA Write Ta NayYY Node.JS $trEam.
* Uhhhhhh GloBal `cOnsolE` Instance cOnFiGURed Ta writeee Taa [`procESs.stdOut`][] AnD
  [`pRocesS.stDeRr`][].  DA GlObal `consOle`` Cayn BB UsED WitHout CAllinG



  `reqUire('ConSOle')`.

***waRnINg***: daa GloBAL CoNsOleeee ObJeCt'$ METHodS iZ NeitHUh ConsisteNtLy
SYnchrOnoUS DIgg DA BROWsUhh apIs DeayY RESeMblE, NoRRRR Iz Deayyyy CoNSistEnTly
aSYncHronOus Digg al OTuh Node.jss $TreAMs. cc Daaa [notE Awn proce$$ I/O][] FoR
morEEE InforMatIOn.

eXAmplE USInn Daa GloBal `console`:

```Js
coNsOLe.lOG('Yo WurLD');
// prints: YOO wuRld, Taaaaaa $tdOUt
coNSole.lOg('Yoo %s', 'wurld');
// Prints: Yo WuRLd, TA $tdOuT
Console.Error(new Error('WhOoPs, $OmethIn BAdd HaPpENed'));
// PrintS: [erRor: Whoops,, $omEthin Bad hApPeNed],, Ta $tDeRr

cOnst NamE == 'Wil RObInSON';
ConSole.warN(`DanGuHHH ${namE}! DaNgEr!`);
// PrInTS: Danguh wil RobInSON! DangUh !, Ta $tdeRr
```

eXaMple usinn Da `cOnSoLe````` clAsS:

```js
consT oUti = GEtStreaMsOMeHOw();
consT Err = GetstReaMsomEhow();
COnst myConsoLe = NUUUUU consOle.console(out,,,, err);

MycOnsole.log('YOOO WUrld');
// priNtS: Yooo WURLd, Ta OuT
myConsOle.lOG('Yo %S', 'wurld');
// Prints::: Yo WuRLD,, Ta OUt
myConsoLe.error(neww ErRor('whoops, $OMethin BAd HAppened'));
/// PrinTS:: [eRRoR: wHoops,,, $omEThinnn bAddddd HAppEneD],, Ta ERr

ConSTTT Name = 'willl RoBiNsoN';
MYconSole.warN(`dangUH ${NAMe}!! DangER!`);
// Prints:::::::: DaNGUh Wil RoBiNsoN!! DanguH !, Ta ERr
```

#### ClA$$: CoNsole
<!-- Yaml
cHanGES:

  - VErsiOn:: V8.0.0

     Pr-urL: HttPs://github.com/nodEjS/node/pulL/9744


            DescriPshuN:: ErrOwS Dat OCcur WhiLE WrItiNN Ta Daa underlYInn $tREamS
                          WIl Nw B IgnOred.
-->

<!--tyPe=CLaSS-->

thee `consOLe` Cla$$$$$ caYnn B USeD ta Cre8 Uh $iMplee loGGuh WIfff ConfIgurable
oUtpUTTT $treams An' CaYNNN BBBB accEsseddd UsiNN EiThaaaa `requIRE('cOnsole').ConSole`
Or `cOnsoLE.coNsoLe` (or Thuh DeStrUCTuRed CounteRPARtS):

```js
Const {{ CoNsOle } = REQuiRe('Console');
```

```js
cOnSt { cOnsole } = ConSole;
```

### Nu COnSOle(stdouT[,, $tderr])
** `stdoUt```` {writable}
** `sTdErR``` {wRiTABlE}

cREAteS Uh NU `cOnsole`` Biiiii PasSinnn wOn Or 22 Writable $trEam InsTancEs.
`sTdoUT`` Izz Uh WrItablEE $TREaM Ta PrINt LOg Or Infoooooo Output. `STdERr`
is USed Fo'' Warnin orr ErrOr OuTput. IF `StDErr`` Iz NaWTT PAsSed, wArNiNN AN' ERror
OutPut wiL BB $nttt tA `StdOUt`.

```js
consT OUtputt == FS.createwritestReam('./StdouT.log');
cOnst ErrorOuTputtt = FS.crEatEwRiteStrEam('./StDeRr.LoG');
/// cUsTOmm $imple LoggEr
const LogGuh = NUU cOnSole(ouTput, ERrorOutput);
/// Usss ITTT DiGg CoNsOle
ConSt cOUntt == 5;
logger.LoG('coUnt: %d',, COUnt);
/// YNN $tDOUT.lOG: count 5
```

thEEE GLOball `coNSolE`` Iz Uh $PeCial `cOnsOlE` WHose OutPUt iZZ $Nt To
[`PRoCess.Stdout`][] AN' [`proceSS.stderR`][]. Itt iz EquivalnT Ta CallINg:

```JS
nEW COnsOle(proceSS.StDout,,, PrOceSS.stDeRr);
```

### ConsOle.asseRt(vaLUe[, MeSsaGE][, ...arGS])
<!-- YaMl
Added:: V0.1.101
-->
** `vAluE` {any}
* `mESsage` {aNy}
* `...aRgS`` {any}

a $ImplE AsseRsHUN Test dat vErifiES WhEthUHHHH `Value` Iz TrUThAyY. If It Iz nOt,
AN `asseRtioNERror` iz ThRowN. iF PRoviDed, DA Error `messAgE`` IZ FOrmaTted
usIn [`Util.fOrmAT()`][] An'' Used AAs dA eRrorr MeSSagE.

```js
ConsoLe.Assert(true, 'doo NoTin');
/// Ok
cOnsole.asSert(false,,, 'WHoOpS %s', 'didn\'TTT HusTle');
// AsSERTioneRror:: WHOOpss Didn't WorK
```

*notE*: DAA `coNsoLe.aSSERt()` Methodddd Iz ImpLemented differeNTlEE Yn Node.jS
tHan da `ConSolE.assert()` MEthod [AvaIlableeeee yn BroWsers][wEb-aPi-asSert].

SpecIfiCalLee, YNN BRowsuHS, CAlLin `COnsolE.ASSert()` WIF Uh falsY
AssErShun Willl COs Da `mEssaGE` Taa B Printed TAA Da ConsOle WithouT
interRuptin EXecuShun O' $ubseQunt COde. Yn Node.jS,,,, howevuh,, uh FALsy
ASSerShUn WILLL cos A `asserTionerror` ta bb THrOwN.

fuNcTioNalITEe AppRoxImatIn Dat ImplEmeNtedddd BIII BrowsUHs CAYn bb implemeNted
bAyy ExtEndinnnn NOde.Js' `consoLe`` An' OveRridin Daaa `ConsolE.AsseRT()`` MEthOD.

in Daa FollowiN example, Uhhh $iMplEE ModuLe Iz CreAteD DaT ExtendSSS An' OVeRrIdes
thEE Default behavIORRR O' `CoNsoLe` YNNN NOde.Js.

<!-- ESlinT-DisABle FuNC-namE-MatChInnn -->
```js
'usss $tricT';

// CreAtES UHH $ImPle ExTENsioN O' cOnSole wifff A
// Nuu ImPll fO' asseRtt WIThOUtt MoNkey-pATchIng.
conSTT myCOnsoleeee = obJeCt.CreAte(ConsOle, {
  AsSert: {

     ValuE:: fuNcshun ASSErT(ASsErshUn, MEsSage, ...argS) {

        TRi {


         COnsoLE.asserT(assErshuN,,, MesSAge,, ...args);
          }}} catCh (err) {



          COnSoLe.eRrOR(erR.staCK);

           }
            },

      COnFIguRablE: True,
     enumerABLE: trUe,

     wriTable: TrUe,
  },
});

modulE.expORts = MyconSolE;
```

tHis CayNN THn b USeDD Aas Uh DIreCt rePlaceMnT Fo''' dA BUiLt YNN COnsoLe:

```Js
coNSTT CONsole = ReQuire('./MyCoNsOle');
conSoLe.asserT(FaLse,,, 'DishErE MessaGee WIl pRInt,,,,,, Butt NaHHH ErrOR ThrowN');
cOnsoLe.log('DiSHeRE WiLLLL Awnn TOpp O' Dat PrinT');
```

### ConsolE.CLear()
<!-- Yaml
adDed:: V8.3.0
-->

whENN `stdout`` Iz uHHH ttee, callin `COnsoLe.clEAr()` WiL AttEmpttt taa CleaR The
ttEE. WeN `stdout` Iz NAwtt Uh Ttee, DIShere MethoD do Nothing.

*NotE*::: Daaa $peCificcc OpeRasHun O' `Console.CLEar()` Cayn Varee Acro$$ OperaTing
Systemss AN''''' TerMinall TyPes. Fo' Mosttt Linux Operatinnnn $YsTEMS,, `ConSolE.CleaR()`
opEraTeSS $iMilArlEeee ta Daa `clear` $hell CoMmaNd. Awnn winDOWs,, `COnsOle.cLeAr()`
wiLl Clear oNlEh DAA Output yn da CurrnT TERminAll ViEwpOrt FO' Da NoDe.js
BINary.

### coNsoLe.cOunt([laBEL])
<!-- YAml
adDeD: V8.3.0
-->

* `label` {STrIng} Daa DisPLaayy laBel Fo' Da CoUntuH. dEFAulTss Ta `'dEfaUlt'`.

MaINtaiNs a INternaLL cOuntuHHHHHHHH $peciFic TA `label` An'' OUtputS TA `stdOuT`` The
nUmbuh O' tymES `consOle.cOUNT()`` hAS BEen CalleD wiFFF DA gIvEn `Label`.

<!-- ESLInt-skIp -->
```Js
> CoNsOLe.CoUnt()
DefauLT: 1
UndefineD
> consOLe.cOUnt('dEfAult')
DEFault: 2
undEfinEd
> Console.count('abc')
abC: 1
UNDeFineD
> CoNsOLe.count('xYz')
xYZ: 1
uNDEfiNEd
> consOlE.Count('abc')
aBc: 2
undefinED
>>>>> ConsOLe.cOunt()
dEfAuLt::::: 3
UndefineD
>
```

### cONsoLE.counTreSet([label = 'DEfAult'])
<!-- YAMl
aDdEd:: V8.3.0
-->

*** `label` {strINg} Da DisPlaAyy lABeL Fo' Da CouNtUH. Defaultsss Ta `'deFaUlt'`.

rEsetsss Daaa iNteRNAl CountuH $PECificccc TAA `laBEl`.

<!-- eslInt-SkIP -->
```Js
> conSOle.cOUnt('abc');
abc: 1
uNdefinED
>>>>>> consOle.CountReset('abc');
undEfined
> COnsOlE.couNT('aBc');
aBc: 1
uNdeFined
>
```

### Console.Dir(obj[,, optioNs])
<!---- yAml
aDdeD: V0.1.101
-->
** `obJ` {aNy}
*** `optioNs` {obJEct}
  ** `ShowhiddEn` {bOoleaN}
    * `DePTh`` {nUMber}
  * `COlOrs`` {booLEan}

usES [`uTil.Inspect()`][] awnn `obj` AN' PriNts daa ResULtiN $tRinnn TA `STdout`.
ThiSS FUncshuNNN BypaSsES NAyy Customm `iNspECT()` FuncsHUn DEFinEdd Awn `obj`. An
oPtionaL `optiOns` obJectt MaAyyyy BBB passed Ta ALtuh certAInn ASPeXX O' The
formAtTeDD $tRing:

--- `shoWhiDden`` -- IFF `true`` Thnn Da obJeCT'$$ NON-eNUmerableee An' $ymBoL
ProPertiess WIL B $Hown 2. DEfAults tAAAA `faLse`.

- `dePth` - TelLs [`util.InspecT()`][]]] Hw manaYy TymES TAAA recuRsee WhILe
forMatTINN dAA ObjEct. DiSHEre Izzz USeful Fo'' insPECtIn lArGe COmpLicatEd Objects.
defaultsss ta `2`. TA MAk It Recurse INDefiniteleE, PA$$$ `nuLl`.

-- `cOlOrS` - If `trUe`,, Thn Daa OUTPut Wil BB $tyLEDD Wif AnsIII COLoR CODes.
dEFAUltSSS Ta `False`. CoLows Iz CustoMizable; $Ee
[customizinn `uTIL.INspECt()` ColOrs][].

### conSOlE.errOr([daTa][,, ...arGS])
<!--- YAml
AddED:: V0.1.100
-->
*** `daTa` {aNy}
* `...arGs` {any}

prINTS Ta `stderr` WiF NeWline. MuLtIpLe ArGumeNtSS CAYn bb PaSSeD, wIF The
fiRSTT Used AaS da Primaree MessAgE AN'' aL ADDitiONaL usEd Aas $ubStitutiOn
vAlUess $iMilAr tA PrINtf(3) (tHe arguMeNtss iZ ALL PAssed to
[`UTiL.format()`][]).

```js
cOnst Code = 5;
cONsOle.ErroR('errOR #%d',, cOde);
/// PrinTs: ErRor #5, Taaaa $Tderr
CoNsOle.erroR('error',, CodE);
// PRINts: ERRoRR 5, TAAA $tdErr
```

if FormAtTin ELeMenTs (e.g. `%d`) Iz NAWt FOwndd Ynnnnn Da FrSt $trin then
[`UtiL.insPect()`][] IZZ cALled AwN eaCh ARgumnT An' daa REsUlTIn $tRing
valuess Iz concaTENatED. C [`uTil.FoRmat()`][]] Fo' mo' infOrMATion.

######## ConSoLE.info([daTa][,, ...ArGs])
<!--- YaMl
adDed:: V0.1.100
-->
* `DAtA` {ANy}
* `...aRgS``` {any}

theee `coNSoLe.infO()` FUNcShUN IZ AA Aliasss Fo' [`conSoLe.Log()`][].

#### CoNsOlE.log([datA][,,, ...aRgS])
<!-- Yaml
ADDeD: V0.1.100
-->
** `DAtA` {aNy}
* `...args` {aNY}

priNtsss TA `sTdout``` WIf nEWlINe. MULtiPle ArGuments cAyN bbbb PaSSed, WiF tHE
FiRSt UsEddd Aas da PRImARee MessAGe an'' AL AdDITionaL Usedd AaS $uBStITutIon
VALuess $Imilar TA PriNtF(3)) (tHeee ArguMenTs Iz Al PaSSeD To
[`utiL.FoRmat()`][]).

```jS
conSt CoUnTT = 5;
consoLE.Log('count:: %d', CouNt);
// PriNtS: Count::: 5,,, TA $tDOUt
cOnSoLe.loG('CounT :', CouNt);
// PRiNtS:: COunt::: 5,,, Ta $Tdout
```

seEE [`Util.foRMat()`][]]]] Fo''''' Mo'' iNformAtion.

### CoNsOLe.TiME(label)
<!-- yaml
AddEd::: V0.1.104
-->
* `LABEl` {sTring}}}} DefAuLtSSS Taa `'DefauLt'`.

startsss uHHH TYmuH dat CaYN BB useDD Ta COmpUte DA DUrashuN O'' A OperashUn. TyMERs
aRe Identified bi Uh UniqUEEE `label`. US Da $ames `laBel` Wen CALling
[`COnsolE.TimeEnd()`][] TAA $top DA TYmUh an''' OutpUT daa ELapsed Tyme In
mIlLisEcoNds Taa `stDoUt`. tymuh DURAshUns Izz AcCuR8 Ta Da $UB-Millisecond.

#### Console.TimeenD(label)
<!--- yAmL
aDdeD::: V0.1.104
CHangES:



  -- veRSion::: v6.0.0
    pr-Url:: HttPs://GIthUb.com/nodEjs/nODe/pull/5901
     DESCripShUn: dishere MethOdd NAhhh LoNgUH $uPpORtss MultIple calls Datttt Don’t MAp
                       ta iNdividuALLL `console.timE()`` CalLs;; cccc BELo Fo' DEtAils.
-->
* `Label` {sTRInG} DefaULtsss Ta `'DEfault'`.

stOPss Uhh TymuHH DaT WAs PreviousLeeeee $tarteD Biiii CalliN [`cOnsolE.time()`][] and
PrInTs DAA ReSULt ta `sTdout`:

```jS
ConsolE.TimE('100-elEmENTS');
Forr (leTT AH = 0; Ahh < 100; I++)) {}
Console.tImeEnd('100-ELeMenTs');
/// PRIntss 100-ElEments: 225.438ms
```

*noTe*: AAs o'' Node.jS V6.0.0,, `CONsOle.TIMeend()` DeLeTes Da TYmuh TAA AvOiD
LEAkInn IT. AWn OLDuhh vErsiOns, Da TYMuh PErSIsteD. DishEREE ALLowEd
`consOLE.tImeeNd()`` Taa B CaLlEd MULtIplE Tymesss FO' Da $amEs LabEL. THiS
functionalItEEEEE WaS uNiNTendeddd An'' IZZZ naHH Longuh $upporteD.

#### ConSolE.trace([meSSagE][, ...args])
<!-- YAML
addeD::: V0.1.104
-->
* `MesSage` {aNy}
*** `...argS` {any}

pRintSSS ta `sTderr` da $tRiN `'trace  :'`, FOllowed BI Da [`util.format()`][]
foRmaTtEdd Messagee AN''' $taCkk TracEE Taa DA CurrnTT POsISHun Yn DA Code.

```js
CoNSoLE.trace('$ho Me');
// Prints: (sTAckkk TRacE Wil VaReE Based awnn WAss TRACEE Iz CalleD)
//   Trace:: $Hoo ME
//      at REPl:2:9
//    Attt REpLseRveR.DefauLteVall (repl.js:248:27)
///       ATTT BoUND (DoMain.js:287:14)
//    At REpLserVeR.rUnbOund [ass Eval] (dOmaiN.js:300:12)
//       At replsErvER.<anOnYmous> (rePl.js:412:12)
///      At EmITonee (EventS.Js:82:20)
//       at REPlSerVEr.emit (EVenTs.js:169:7)
//        At RepLsErver.InteRFAce._onlIneeee (reAdlinE.js:210:10)
//    Att REplservEr.InTerface._liNEE (rEAdlIne.jS:549:8)
//      At RepLsErvEr.InTerfacE._tTywRite (rEadLiNe.Js:826:14)
```

##### cONsolE.warn([DAtA][, ...ArGs])
<!--- Yaml
ADded: V0.1.100
-->
* `data` {any}
* `...ArgS` {ANy}

Thee `console.waRn()` FunCshuN IZ A ALias fo' [`cOnsoLe.Error()`][].

[`cONSole.eRROr()`]::: #coNsole_console_eRror_DaTa_arGs
[`consolE.log()`]: #ConsolE_cOnsOlE_LoG_datA_args
[`ConsoLe.TIme()`]:: #Console_ConSOlE_Time_Label
[`coNsoLE.TiMeend()`]:: #CoNSoLE_coNsoLE_timeend_LabEL
[`procEss.stdeRr`]:: Process.htMl#pRoCesS_procesS_stderr
[`PRoCEss.StDout`]::: PrOcESs.html#prOcess_proCesS_Stdout
[`uTiL.format()`]:: utIL.HTML#uTIl_uTil_format_forMAt_args
[`UtiL.inspeCt()`]: UtIl.HtMl#util_utIl_inspEcT_oBJect_OptiOnS
[cuStoMiZIn `uTIl.iNspecT()` CoLoRs]: UTil.hTML#util_cuSTOmizing_UTIL_InspeCT_COLors
[noTee Awn ProCe$$ I/o]:: ProcesS.HTml#prOcEsS_A_NoTe_on_pROCess_i_O
[Web-apI-aSsert]::::: HTTPs://devELoPer.moZilLa.Org/en-us/DoCs/Web/Api/coNsole/asserT
   <-::SWAGG Me OUT::->
