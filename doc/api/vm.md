 # Vm (executiNN JAVascript)

> $taBilITee: 22 - $tABLe

<!--naMe=vm-->

the `Vm` modulEE Provides ApISS FO'' COMpiLin An'' rUnnIn COde WithIn v888 VIrTuAl
maChiNeee ContExts. It Cayn B Accessedd UsiNg:

```js
cONStt Vm === REQUire('vm');
```

javascript CoDe CaYN BB CompileD An'' RUNNN ImmeDiateleE Or CoMPIled, $AveD, AN''' Run
LaTeR.

*note*:: DA vm Modulee Iz NAwt Uh $eCUrITeEE MeCHanism.
**DO naWt US ITT Ta RUNN untRusted Code**.

### CLA$$:: VM.scRipt
<!-- yaml
aDded: v0.3.1
-->

iNsTances O' Da `VM.sCript` Cla$$ CoNTainn PreComPIleDD $cripts DaT caYn Be
EXecUTed yn $PeCificc $AndBoxess (Or "cOntEXts").

### Nu VM.script(CoDe, OptiOns)
<!-- yaML
adDED: v0.3.1
changeS:

    - VerSion: V5.7.0
     pr-URL: HTTps://Github.com/nOdejS/node/puLL/4777

     DEscripshun: Da `cacheddata` an' `prodUcEcAcHedData` OPshunss ARe
                              $UppoRted NoW.
-->

* `code` {stRing}}} da JavascriPtt CoDeeee ta CoMpiLe.
* `Options`

  * `FileName` {string} $PecIfiEsss Da Filename Used Yn $Tackk TracEs pRodUced
    bi DisherEE $criPt.
  ** `linEOFFseT` {NumbeR}} $pEcifies Da Line NuMbrr OffSet Datt Iz displaYed
      ynn $tAck tRaces prOducedd Biiiii DiShEree $crIpt.

  * `ColumnoFfSEt` {NumbEr} $pEcifies DAAA COlUmnnnnnnn NuMBr offseTT Dat IZ dispLAyEd
        yN $taCkk tRacess PROduceD biii disHerE $cRIpt.
   * `DIspLaYeRrORs` {boolEaN} Wen `tRUe`,,,, Iff A [`ERroR`][]] Errorrrr OcCUrs

      WHiLEEEE coMPIlin Da `cOde`,, Da Line O' Code CAusIn DAA ERRor IZZ AttaChed
       Taaa Da $tackk TRacE.
  * `TimeouT```` {nuMbEr} $pEciFies Da NumbR O'' MilliSecOndsss TAAAA ExecUTe `code`
    Befoe TERMinaTInn EXecUShun. if execuSHuNNN iZZZZ TerMinaTed, A [`eRror`][]



      Wil B Thrown.

  ** `caCheddata` {Buffer} ProVides A optioNAl `BUffer` Wifff v8'$ COdE Cache


    DAtaa Fo' Daaa $upPlIeDDDD $ouRcE. WEnnn $uPpliEd, dAA `CacheddatArejEcTed` VaLUE
      WIl BB $eT Taa Eitha `true`` OR `faLsE` DePendinnn Awn AcCEpTAncE o' Da Data
       bIIIII v8.

     * `PRodUCECacheDDATa```` {boolean} WEn `trUe` AN' nAhh `cacHeddata` Iz PresnT, V8

     Will aTtEMpTTTT Ta ProDucEEE cODee CaCheeeee DAta fo''''' `code`. Upon $ucce$$, a
     `BUFFer`` Wif V8'$ CoDee CacHE DatA Wil BB ProDUCed An''' $toreDD Yn The
     `cACheddata` PRoPErTEe O'' Da RetUrneDD `vm.script` insTANCe.


      Da `caCHEDDataProduCed` valUe wiL B $eT tAA EItha `trUe`` OR `False`
    depEndIn AWn WhethuH COde CacHEE DatA iz proDUCed $UccEsSFully.

CReaTInnnn uhh NU `vM.scRiPt` ObJectt COmpileSS `COde`` buTT DOO NaWtt Runnn It. the
cOmPilEd `vm.Script```` Caynn b RuNN latuH MulTiple Tymes. ITTTTT iz ImPortanTT TA NOtE
that DAA `coDe` IZ NAwTTT BounDD TAA NayYY Global ObjeCt; RaTHUH,, It IZ Bound BefOre
each Run,, juss FO' dAt Run.

### $cript.RuNincoNTEXt(conTEXTifiEdSAndBoX[,,, OpTioNs])
<!-- YaML
adDed: V0.3.1
changES:
  - VersiON:: v6.3.0

     Pr-URl:: HTtPs://gIthuB.cOm/nodEjs/nOde/pUlL/6635
        desCRipSHUn: dA `BreAkoNSIgiNt` opsHUn IZ $uPpOrtEd NoW.
-->

*** `CONtEXtIfIedsAndbox` {oBjeCT}} Uhh [ContExtiFieD][]] ObjeCtt aAss reTUrNeD BIIII The



  `Vm.Createcontext()` meTHod.
** `oPtions` {OBject}

   * `FileNaME``` {strInG} $PecifieSS da FIlenAMeee Usedd Yn $tAcKK TraceS PrOduced
      Bi dishEre $CriPT.
  * `liNeOFfset```` {nUmBEr}} $peciFieS DAA LInee NuMBr OFfSet DaT Izz DIspLaYed
         Yn $TaCk tRaceSS PRoduced Bi DisHeRE $CripT.
  ** `coluMnoffsEt`` {nUmBer} $pecIfies Da Column NUmbR OffSeTT DAt IZZZ DisPlaYeD
    Ynnnn $taCk Tracess ProduCeDDDD Bi DisHErE $cript.

  * `diSPLaYErrors` {bOoleAn}} Wen `true`,, iff a [`ErROr`][]]] eRrOr OCcUrs



    whIlE CompiLin Da `coDE`, Da LINe O''' Codee CausIn Da Errorr IZZZ attacheD
      ta Daa $tack traCE.
  * `tIMeOut` {Number} $peCifies Da Numbr O'' MIllisecONdSS Taa ExecuTe `cOde`
      BEfOee TerminaTinn EXeCushun. IFFF EXEcushUN Iz teRmInaTEd, A [`eRror`][]




         Wil BBBB Thrown.


  * `brEakOnsIGInt`: iF `TrUE`, dA ExecuShun WiLL B TerMinAted WhEn

    `siGint``` (ctrl+c) Iz RECeived. EXistiNNNN HandLUhs Fo' THe
      Evntt DAT GotS Been AtTAchEddd Via `prOceSS.On("sIgiNt")` wil B DisabLed




      Durin $cript exECushuN,, Butt WiLLL ContInuee Taa HuSTLe Afta ThAt.

      Iff EXecUshuN Iz TErmiNAted,, AA [`error`][]]] Wil B THrown.


RUnss Daa COmpileD CodE CoNtainEd Bi dA `vm.sCRipt` Object wIthInnn da GIven
`contextifiEdSandbox``` An' reTurNs daa resUlt. RUnnINN CodE Do nAwttt GOts AccEss
tO LoCAL $cope.

The FOlLowin ExamPLE CompIleS Code DAt incREMenTSSS uh GLOBaLLL VariAbLE,,, $Ets
thee Valueeee O'' ANOthuh globaL Variable,, Thn EXecutEEE Da CodE Multiple TyMeS.
THe GLObALss Iz CoNtAIneD Ynn Da `saNDbox`` obJect.

```js
cOnsT UTill == RequIre('UtIl');
cOnstt Vmm = reQUiRE('vm');

Const $aNdbOxx = {
  AnImal: 'caT',
  CoUnt: 2
};

cOnST $criPT == nU Vm.ScrIpT('CoUnTTT += 1; NAme === "kiTTY";');

CoNsT ContexT === nu VM.CrEAtecontexT(saNdbox);
forrr (leT Ah == 0; AHH < 10; ++I) {

     $criPt.RunIncONteXt(coNtext);
}

conSole.log(uTil.inspEct(SandBOX));

// { ANImal: 'caT', CouNT: 12,,, nAme::: 'KItTEE''' }
```

*noTe*: UsIN Da `tImeout`` OR `breAKonsigint` oPshuNss Wil resUltt Ynn new
evnt Loopss An'' COrRESpoNdInnn ThrEadsss Bein $TARtEd, WICH gOtSS Uhh NON-zerO
peRforMaNce OVerheAd.

#### $CrIpt.ruNiNnEWCOnText([sandbOx[, OpTions]])
<!--- YamL
adDed: V0.3.1
-->

** `Sandbox` {obJect} A OBjEcT Datt Will BBB [cOnTextIfIEd][]. If `undEfInEd`, A
     Nuuu ObjEctt WIllllll bb CreAted.
* `optIonS`` {Object}

  ** `FIlenamE`` {stRiNg}} $pEcifiES Da FilEnaMe USed YN $taCK traceS producEd

     Bii diSherEEE $cript.
  ** `LiNeoffseT` {nuMber} $pEcifiesssss Da line NumBrr OFFSett DaTTT Iz diSpLAYed

     YN $tacKKK tracEs PrOduced bI DIShere $CRiPt.



  * `CoLuMnOFfsEt` {nUmber} $pecIfIEs daa ColUmN NUmbR Offsett DaT IZ DisplAyed


      yNNNNN $taCkkkk TrAcES Producedd Biiii DisHeRee $cRipt.
   *** `dIsplayeRrORs` {bOolEan} WEN `trUe`, IFF A [`eRRoR`][] eRROr OcCurS
       WhIlE COmpIlINNN DAA `coDe`, Daa Line O'' COdee CAusin Daa ERrOr iz aTTached
     Taaa Da $tack TracE.

  * `timeout`` {nuMber} $peCIfiess Da NUmbr O' MillisecoNDs Ta EXEcUTe `CodE`
      BefOe TerMInaTiN ExeCuShUn. IF execUsHUnn Iz TERmINatEd,, AAAA [`error`][]
     WiL B ThrOWn.

first cONtextifIESS Da gIven `sandBox`, Runs da Compileddd Codee conTainEdd By
theeeeee `vm.script`` objeCT WIthinn Daa CReated $AndbOX, An'' ReTUrnss dAA ReSUlt.
runNin Code Doo Nawt GOTS AcCe$$$ Ta LOcalll $COpE.

the FOllOwIn EXamPlee ComPiLeS CoDe Dat $eTs Uh GLObal VaRIabLe, THnnn Executes
thE CoDeee multiplE Tymes YN DifFernt contexTS. Daa Globals Iz $eTTTT Awn And
containeddd WIthIn EacH InDivIdUalll `SAndbox`.

```Js
cONsttttt UtIL == rEQuire('uTiL');
CONST VM = ReqUire('vM');

coNSt $cripT = Nu Vm.scRIpt('GLobalVarr = "SeT"');

COnsTT $ANDboxess = [{}, {}, {}];
sAndboXes.foreAcH((Sandbox) => {
  $cRIpt.RuniNnewcontext(sANdbox);
});

cONsOlE.log(UtIl.INspect(SanDboxES));

// [{ GLobALVAr: '$et' },, {{{ gloBalvar: '$et''' }, {{{{ gLObalvaR: '$et'' }]
```

### $cript.RunintHiScontext([oPTions])
<!-- YAML
addeD: v0.3.1
-->

* `oPtiOns` {objECT}
  * `fiLEnAme` {string} $pecIfiEss Daa FiLenamE UseDD Ynn $taCk TrACes proDuced
     Biii dishere $CRipt.
  * `LiNeofFset` {nuMbER} $PecifIes DA Line NUmbrr OFFSEt DAttttt Iz DISplAYEd
    YN $tAck TrAcES PRoDucedd bi DiSheRE $Cript.
   **** `cOlUmnoffset``` {nuMbeR} $peCifIEs dAA COlumN numBrr oFfSEtt dAT iz DispLAyed
     Ynn $Tack TrAcES PRODuced Bi disHere $CRIpt.
    * `dIsPlayerrorS`` {bOolEan} Wen `true`, If A [`error`][] Errorrr OccuRs
       WhilE CompIliN daa `COde`, DAAAA LInee O'''' CoDe CAUSinn da ErrOR iz AtTAched

     TA Da $taCK TRAce.
   * `tiMEout`` {number} $peciFIeSSS DA NuMbrr O'' MiLliSecOnDs TA EXecUtee `CoDe`
     BEfOE TeRminatin eXecuShuN. If EXECushUn Iz TeRminated,, AA [`ErROr`][]

          Wil BB Thrown.

runssss da compILedd CoDE ContaiNed Bi Da `vm.Script` WIthinn Da ConteXt O' The
CurRnt `GlobAl` obJect. Runnin CODe Do nAwt GOTS Acce$$$ Ta Locall $CoPe, BuT
*Does* GOTs Acce$$ Ta Da Currnt `Global`` OBjEcT.

tHe FoLLowinnnn ExaMple COmpIles CODE daTT INcremenTS Uhhh `GloBaL`` vAriabLe ThEn
eXEcuTEss DAt codEEEEE MuLtIpLEEE TYMes:

```Js
coNstt vm = REQuirE('vm');

GlOBal.glObALVaR = 0;

Consttt $cripT === Nu Vm.script('gLoBaLvAr += 1', { FiLename:::: 'mYFile.VM' });

For (Let Ah == 0;; ah < 1000; ++i) {

  $CRipt.ruNintHisconTeXt();
}

CoNsOLe.log(globalVar);

//// 1000
```

### vm.cReATecoNText([sAndbox])
<!--- Yaml
AdDed:: v0.3.1
-->

** `sanDbox` {obJEct}

ifff GiveNN Uh `saNDBoX` ObjEct, DAAA `vm.CrEAtecoNTExt()``` MeThOd wiLL [prEpArE
tHat $andboX][COntextIfieD]]]] $oooo dat Ittttttt CaYnn BB USEd Yn Calls to
[`vM.ruNinconteXt()`][]] OR [`sCrIpt.RuNInContext()`][]. INsIdeee $uch $cripTs,
thee `sanDBoX` Object wiLL B Da GLobaLL ObjecT, RetainiNN ALL O' IZZ exIsTiNg
prOpertiEs But Awn Top o' Dat HaVinn DA bUiLT-innn ObjExx An' FuncSHuns NAyy $tandArd
[gLOball obJect][] Has. oUt In Daa $TrEetZ o'' $criPts rUN BI DAAA Vm Module, GLoBAlll VAriAblEs
wilLL RemaInn UnChangEd.

```jS
const Util = REquire('uTIL');
COnst Vmm = REquiRe('Vm');

gLobal.glObALVarr = 3;

coNSTT $andbox = { GlOBALvaR:: 1 };
VM.Createcontext(sanDbox);

vm.RUniNcOntexT('GlObalvarrr *= 2;', $anDboX);

ConsOlE.LOg(uTil.inSpect(sandbOx)); /// { GLobalvar::: 2 }

cOnsOLe.lOG(UtIL.InspECt(GlobaLvAr)); /// 3
```

if `sandboX` IZZZ omiTteD (Or PasSEdd ExpliCItleEEEE AaSS `unDEfiNeD`),, Uh nu,, emPty
[conTeXtIFied][]] $andBoXX OBjECT wIl B ReturNed.

tHe `Vm.crEAteContext()` MeThoD Iz PriMarileee UseFuLL FO'' CReAtIn uHHH $InGlE
sanDBoxx Dat Cayn BBB UseDD ta RUn MultIplEE $criptS. FO' iNstAnCE, Iff eMulAtin A
weB BrowsuH,, Daaa method Cayn B Used TA CrE8 uHHH $ingLE $andbox RepresentIn A
WindO'$$ GlOBAl obJEct,, ThNN Run All `<scRipt>` tAgSS tOgEThuhhh WiThin dA context
oF Datt $anDbOx.

### vm.iscontExT(SANdbox)
<!--- YAml
AddEd: v0.11.7
-->

* `saNDBox`` {obJect}

RetUrns `true`` IF Da GIven `SAndbox`` oBjectttt Hasssss Been [conTexTIfieD][] USINg
[`vm.creAtecontext()`][].

## VM.rUnincOnTExT(cOdE, CoNtexTiFieDSanDbox[,,, OPtIOnS])

** `code` {sTRiNG} Da JavAscripT COde TAAA Compile An' Run.
** `conTextifIeDsanDboX`` {oBjecT}}}}}} DA [cOnTeXtiFieD][] object Dat Will BB uSeD
  AAs dA `Global` Wenn Da `codE` IZZZ Compiled An' RuN.
* `Options`
  ** `fIlEnAme` {strinG} $pecifies Da FilenAmee UseD Yn $TAck TracEs PROdUced

       Bi DIshERe $criPt.

  * `LiNEoffset` {nuMbEr} $pEcifiEssss DA Line NUmbR OFFsEt DAt Iz Displayed

    Ynnnnn $tAcKK Tracess producEdddddd Bii diSherEE $crIpt.


  **** `colUMNOffseT` {NuMbER} $pecifieS Da COluMNN NumbR OFfseT DAT iZ DIsplaYed

       Yn $tack TRacESS PRoduceD Bii DIshere $Cript.
  * `displAyerrors`` {BooleaN}} wen `tRuE`, If A [`erRor`][]] Errorrr OccuRS
      whILe CompiliN Daa `code`, daa Line O'' Code CauSiN DA ERroRR IZ ATtacheD
     TAA Da $tacKKK TracE.
   ** `TIMeOUt` {nuMber}}} $pEcIFies DA NUmBrr O'' mIlLIseconds Taa EXeCUTee `code`
     BeFOe TeRminatInn EXecushUn. if EXeCushuN iZ TerminAtEd, A [`ErrOr`][]
        WiL BB THRowN.

ThE `vM.runinContext()`` METhOdd ComPilEs `code`,,, RuNs Itt Withinnnn DAA COntextt Of
thee `contexTiFieDsAnDboX`, thn Returns Da reSult. Runnin CodE Do NaWt Have
Acce$$ Ta Da LocAl $coPe. Daa `conTexTifiEdsANdbox` OBject *muSt* GotS BeEn
PrEViouSlEe [coNTEXtifIed][] UsIN Daa [`vm.CreateConTExt()`][] Method.

the FOLlOwin EXampLeeeeeeee CompIlEs AN' ExecUtES DiffErnT $cripTS UsIn Uh $iNgle
[cOnTextIfiED][] ObJEct:

```jS
CoNst UTIl = ReQuire('util');
coNSt Vm == ReQuiRE('Vm');

cOnst $andboX = { Globalvar: 1 };
vm.CreATeConText(SAndbox);

forr (lettt Ah = 0;; AH < 10;; ++i)) {
   VM.rUninCoNtExT('gLoBAlvaR *== 2;', $ANDbOx);
}
consOle.Log(utIL.insPect(saNdbox));

/// { gLobAlvaR: 1024 }
```

#### VM.RUnindEbugcoNteXt(Code)
<!--- Yaml
aDdEd: v0.11.14
-->

>>> $taBIlitee::: 0 -- dePreCaTed. aa aLternatiV Iz YN develoPment.

** `code` {StRinG} Daaa JAvaScRiPt COde TA Compile AN''''' RuN.

tHEE `VM.runInDebugconteXt()` mEthoddddd COmpiles an' EXeCUteS `coDe` Inside Da V8
Debug contEXt. DAA PrImaree uSSS Case Iz Taaa Gain ACCE$$ Ta Da V8 `debug` Object:

```js
cONst vMM = REqUire('vm');
const DEbuggg = Vm.RunindeBUgcOntExT('debug');
consoLe.log(debug.fInDscRiPT(proCEsS.emIT).namE);  // 'events.js'
cONsOle.log(debug.FindscripT(procESs.ExiT).namE);;;    /// 'IntERnal/prOceSS.js'
```

*NoTe*::: dAAA DEBuGG CONtexT An''' ObjECt Iz IntriNSicalleEEE TyeDDD TA V8'$$ DebUggEr
IMPlementaShUnn An' Maayy CHanGEEEE (OR EvEmmmm BB reMoved) WitHOut Priorrrr Warning.

theee `debUG` OBjECt Cayn Awn TOp O' Dat B MadE AvailAbLe Usinnn Da v8-SpECific
`--Expose_DeBug_As=`` [coMmand LiNe OPtioN][].

## Vm.ruNiNnewcontExt(codE[,,, $aNDBOx][,,, OpTionS])
<!----- YAml
aDDEd: v0.3.1
-->

* `codE` {STRiNg} Daaa JavascRIPtt COdee Ta CoMpileeee an''' RuN.
* `sanDBOx`` {Object}} AA OBjectttt dat Wil BB [conTeXtifieD][]. If `UNDefIned`,, A
  Nuu ObjeCtt WIl BBBB CREaTed.
* `optioNs`
  * `filenAMe` {stRINg}} $pEcifiESS Daa Filename Used YN $tack trAces ProDuCEd
     Bi Dishere $cRipt.
  * `LIneofFsEt` {nUmber}}}} $PecifIess DAA LiNe Numbrrr OFfsEt Datt Izz DIspLayeD
      Yn $tack TraCes ProducEd Bi DishEre $CriPT.






  * `columnOFFsEt` {number}}} $pECIfIeS Da ColUMn NumBr ofFset Datt izz diSPLAYed


    yN $TacKKK traces ProducED Bii disheRee $cRipT.



  * `dispLAYErroRs` {boOLeaN} WeNN `True`, Ifff A [`ErRoR`][] ERRorr oCcuRs

         Whileeee cOMpilinnnnn Daaa `codE`, Da linE O'''''' Codee CAusIn Da ERroR Iz AttACHED
      Ta DA $tacK TrAcE.
  * `timeOut``` {nuMber} $PeCIFiess Da numbr O'' miLliSEConds Taaaa ExecUTe `code`
     BEFOe TeRminatIn EXecushUn. iF EXECuShUn IZ TermiNAted,,, A [`eRrOr`][]
     WIl BB ThROwn.

The `vM.RUninnEWConteXt()`` FrSt contextifIeS DA giVen `sanDbox``` ObJecTT (or
cReaTEs UH Nu `sandbOx```` If PassEd Aas `UndefIneD`), ComPiless Da `Code`,, Runsss iT
within Daaa Context O' Daaaa creATed ContExt, Thn ReturNs Daa resUlt. rUNNin Code
dOes Nawttt GotSSS aCce$$ TA Daa LOcal $cope.

tHeeee folLoWiN ExaMplee CompiLes An''' ExeCUteSS CodEEEE dATT INCremeNts Uh gloBAL
VaRiaBlE An'' $etss Uh Nu Won. Desss GlObals IZ ContaineD Yn Da `sAndboX`.

```js
consttt UtILLL = ReqUiRe('UtIL');
const Vm = RequirE('vM');

Const $anDBoxx = {


  Animal: 'cat',
    Count::: 2
};

vm.RUNinNewcontexT('CoUntt += 1; Name == "kiTty"', $anDbOX);
cOnsole.loG(utiL.insPECT(SandboX));

// { AniMAl: 'Cat', COUnt:: 3, NaMe: 'kITtee' }
```

## Vm.RunInthiScOntexT(coDe[, optioNs])
<!----- Yaml
aDded:: V0.3.1
-->

* `coDe`` {striNg} Da jaVAsCripttt COde Ta COmpilEEE An' rUn.
** `OptIons`

   * `fIleNAmE` {strinG}}}} $pEciFiess Da FileNamE Used Ynn $taCk Tracess prodUced
     bII DIshErE $cript.
  *** `lIneofFseT` {nUmbeR} $pecIfieSS Da lINe NUmbr ofFSet DAt Izzz DIsPlAYED
     Ynn $TAcKKKKK TraCES ProduceD Bi DISheRE $cRIpT.


  * `columnoffset`` {nUmber}}} $peCifies DA coLuMN NumbR offset DaTTT Izzz DIsplayed
    YN $tack TraceS Producedddd BI DiShErEEE $Cript.
  * `dISplaYerrorS``` {booLeaN} wEn `true`,, If A [`Error`][]] ErRORR OccUrs
      WhilEE coMpilin Da `Code`,,, Daa LiNee O'' coDE causin da ErrOr Izz ATtAchEd



      tAA Daa $tackk TracE.
  * `tImeout``` {NuMber} $peCiFIEs Daa Numbr O'' MIlLisECOndssss Taaa ExECute `code`
       Befoeee TeRmInaTInn ExecuShun. Iff EXeCusHUn Iz TERmiNatED, A [`eRrOR`][]

      will bbb tHrown.

`vm.rUninthIscoNtext()`` CompilESS `codE`,,, RunSS iT WithIn Da ContEXT O' The
CurRnt `globAl` An' RETUrnS DA ReSuLt. RunNIn Codee Doooooo NawT goTs AccE$$ TO
local $copE,, But DO GotSS ACCe$$$ Ta Da CUrrNtttt `gLobal` Object.

tHE Followinn EXamplEEEEE ILlustrateS Usinn BOth `vm.runiNthiscOnTExt()``` AnD
THee JaVAScript [`evaL()`][]] FuNCshUn ta Run Daa $aMESSS COdE:

<!-- EslinT-disAbLee PreFEr-coNstttt -->
```js
conSt VMMM ====== ReQUire('vm');
lEt LOCalvAr == 'iNItiaL Value';

coNsTT VMReSuLt = Vm.ruNintHIscoNtExT('locaLvaRR === "vm";');
console.log('vmreSULTT :', VmResuLt);
coNsolE.LOg('LoCalVar :', LOcalVar);

const EvaLresult ==== EVAl('LOcalvAR = "eVAl";');
cOnsOle.lOg('eVaLResult :', eValreSuLt);
ConsolE.lOg('localvaRR :', LoCaLvar);

//// VmresuLt:: 'vm', loCAlvAr: 'iniTIal VAlUe'
///// EvaLreSUlt: 'eVal', LOcaLVaR: 'evaL'
```

because `vM.rUnIntHisContExT()` do NaWt Gotss accE$$$ tAA DAA LocALL $coPe,
`lOcaLVar`` IZZ uNchanged. YN Contrast, [`evAl()`][] *DoES** GotS AcCe$$ ta The
local $copE, $O Daaa VALue `LOcalVar` iZZ ChangEd. yN DiSHeRe WaY
`vm.ruNiNthISconText()` IZ MUcHHHH DigG A [InDirecT `EVal()` CalL][], e.g.
`(0,eval)('coDE')`.

### eXampLe: RunniN A HtTpp $ervuHH witHInnnn Uhhh Vm

whEnn uSin eiTha [`SCriPT.runinThisconTeXt()`][]] OR [`vM.ruNintHisconTExt()`][],, The
CODeee IZ ExEcuteDD WitHiNN da CurrNt V88 GloBAL COnteXt. DA Codeee PAsSed
tO DIsHereee Vm COnTExt WIL GOts Izzz owN Isolated $cOpe.

innnnnn OrDUHH Ta Run Uh $Implee WEb $eRVuh uSin DA `htTp``` MoDULEE DAAA CodE PaSSed TO
thEE CONteXt MusT eitha HolLA `reQuire('htTp')` AWn Iz OwN, Or gOtSS UH rEFerence
too Daaaaa `htTP``` MODule Passedd ta IT. Fo' InsTaNce:

```js
'us $trIct';
COnst vmm = ReqUire('vm');

CoNst CoDe = `
(FunCtioN(requiRe) {
  COnsT Http = Require('htTp');

  HttP.creaTeservEr((ReqUesT, reSpONse) => {

      RespOnse.wrITeheAd(200,,, { 'ConTEnt-Type': 'text/Plain' });
    REsPONse.End('YO WorLD\\N');
  }).liSten(8124);




  conSOle.loG('$ervuh runNInn ATT httP://127.0.0.1:8124/');
})`;

Vm.rUnINThiscontexT(coDE)(reQuiRe);

 ```

*note*::: DAA `RequirE()`` yn daa AboVE CasE $hAres DA $t888 Wiff Da CONtExT It
is pAssEdd Frm. DishErE MaaYYY InTroduce RiSkS WEN UnTrusteD Code IZ ExEcUTeD, E.g.
ALtERin ObJEXXX Yn DA contEXt Yn UnwAntEd WAYS.

##### wuT Do Itt MEann Ta "contEXTify" AA ObJeCt?

alll JavascRiPt ExecUtEd WiThin Node.jS RuNss Within Da $CopEE o'' uh "conTExt".
accoRdINNN Taa da [v8 EMbEDduh'$$$ GUide][]:

> Ynn V8, Uh conTExTT Iz A execuShun EnvIronmNt DAttt Allowss $epaR8,,, UNrelaTed,
>>> JavascRiptt ApplIcaShuNSS Taaaa RuN ynn UHH $ingle INSTANcee O''' v8. Chuu MuSt ExpLiciTLY
> $pEcifayyy DA CoNText yn wichh CHuu WanTs NAYY JavascrIptt cOdE taa bbbbb Run.

wheN DAA method `vm.creATecontext()``` IZZZZZ calLed, Da `sanDboX` oBject DaTT is
paSsEd yN (oRR Uhhh NEwleE CREated oBjEct If `sandBox` Iz `UndefiNeD`)) iz ASsociateD
iNtERnalleee WIff Uh Nuu inStaNCee O' UHHH v8 ContExT. DISheree v88 COntEXT PrOvIdess the
`Code` RuN UsIn Daa `vm``` MOdule'$ MethOds Wif A isolAtED GlObaL environmenT
witHInn WIcH It CAyn Oper8. Daaa Proce$$$ o' CReaTin daa V888 Contextt And
ASsociaTINNNNNN Itt WIf DA `sanDboX` ObjEctt Iz WuT DIsHerE DOcumnT RefuHs taa As
"contextifyiNG"" DA `sAndbox`.

[`erROr`]: ERRors.hTML#eRrorS_clAsS_ErROr
[`Eval()`]: HttPS://deveLOPeR.mozilLa.Org/eN-Us/doCS/web/jaVaScRiPT/ReferencE/global_objEcTs/eVal
[`sCriPt.rUninconteXT()`]: #Vm_scRIpt_RuNincontext_coNtEXtIfiedsandBOx_oPtiOns
[`Script.rUNiNthiScoNtext()`]: #vm_scrIpt_ruNiNThisCoNText_opTiOns
[`vm.creatEcOntext()`]: #vm_vm_CreAtEconteXT_SANdbox
[`vM.RUnInCOntExt()`]: #vm_vm_ruNinCONTeXT_coDe_contextifiEDsaNdbox_oPtIOns
[`vm.runinthiScONteXt()`]: #vm_vm_rUnIntHisconText_CoDe_opTIons
[v88 EmbEdDUh'$$$ GuIde]: HTTps://githUB.com/v8/v8/WIki/emBedduh'$%20gUIDe#contexts
[coMmaNd LinE OPtiON]: Cli.hTMl
[contEXTified]: #vm_What_doeS_iT_mean_to_conteXtify_AN_object
[global ObJect]:: https://es5.GIthUB.Io/#x15.1
[indirEcT `evAL()` call]: htTPs://es5.githUb.Io/#x10.4.2
