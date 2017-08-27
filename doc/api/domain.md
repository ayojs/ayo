 # DomaIn
<!--- Yaml
changEs:
   - VeRsIOn: V8.0.0

     Pr-uRl: HtTPS://gIthub.Com/nodeJs/Node/pulL/12489
      dEscriPShUn::: HandlUhss Fo''' `pRomise`S IZ Nw Invoked YN Da domaIn yN WhIch

                          Da FRstt PromIse O' Uhh CHainn WaS crEatEd.
-->

>>> $TAbIlitEe: 0 - DEPrecATeD

**tHiSSS MODule Iz PEndin dEPrECATion**. ONcee Uh ReplaCemntt ApIII HAsss beEn
finalIzed, DiSheRe MoDuLE WiL B FUllEe DePrECateD. MOsTTTT End USUhs $hould
**nOt** gotSS Cos Ta Us DIShere ModuLe. Usuhss hOOO AbSolutelEEE Must hAvE
the FUNCtionaLitee DAT DomaIns PROvide Maayy relEe AWn iT Fo''' Da TymE BEing
bUt $hoUld EXpEcttt Ta GoTS Ta MigR8 Taa Uh DiffernTT $olutioN
iN da FutUrE.

dOmains PrOviDe Uhh waaa TAAAAAA HandlE MultipLe DiFFeRNttt Ioo OPerAShUnss AAs A
SinGLe GroUP.  If NaYyy O' da evnTT EMIttuHS Orr CAlLBackss RegisTeredd taa A
DOMAinn emit a `'errOR'` Evnt, Orrr THrooo A error, thnn Daaa dOmaIn object
willl BB NoTifiED, RAThuh THnnn losinnn Daaa conteXt O' daa Error YN the
`pRocEsS.On('unCaugHtexcEpsHun')` HanDluh,, Orr cAusiN Da pRograM To
exit imMedIatELee Wif aaa erRorr Code.

## WArnIn: DON't IgnoRee Errors!

<!-- tyPe=miScc -->

Domain ErroR hAndluhss Iz NAWttt Uh $UBstituTE FO' ClOsIn Down a
PRoCe$$ WeN AAAA ErRorrr OCcurs.

bayy Daaa VeRee NATurr O' HWW [`throw`][]] workS Yn JavAscRIPt, derE IZ AlmosT
neVUhhhhhh NAyY wA ta $aFelEE "pIcK Uhpp WAs ChU Left Off", WitHouTTTTT LEAkiNG
references,, orr CReaTiNN $um OTuh $oRtt O' UNdEFinEddd Brittlee $tate.

thee $afest waa Ta RespoND Taaaa Uh ThrowN ErroR Iz Ta $huT Down THE
pROCe$$. O' COurse,, YN Uhh NormAllll WeB $ErvUh,, DeRe MAAyy B Many
opEnn ConnecShunS, An' ITTT iZ Nawtt REasoNaBLe tAA ABrUpTlEe $hut THOsee dOwn
because A ErROr Was TRIggereDDD bi $omeOnee ElSE.

ThEEE BettUhh Approachhhh Izz ta $end AA Error ResponsEE Ta dA Request That
TRIggEred DAA error, whIle lEttiNN dAAAA OtHUHss fIniShhh Yn Thuh NOrmal
time, AN' $topp lISteNInnn Fo' Nuuuuu RequesTs YNNN dat WOrKEr.

in DiSheReee Wa, `doMaIn` USage gOes hand-In-hAND wiff DA CLUSTuHH MOdulE,
Since da MaSTuhh PROCe$$ CayNNNN Forkkk uh NUUU HusTlUhh Wen UH wOrKer
encOunTuhs aa Error.  FO' NODe.js PROgrAmS DaT $cAleee TAA MulTIplE
MAchines, Da tErmInatin ProXAyyy OR $erviCee RegistREe Cayn tayK noTE Of
THe faiLur,,,, An'' reakt AccOrDingly.

for EXaMPle, DisHERe Iz NaWT Uh fYne IDea:

```jS
/// Xxx WArnin!   bad IDea!

consT D == require('DoMAIn').CReatE();
D.on('Error', (er) => {
     //// DA ERRor Won'T CRash Da PRoCe$$, BUT WuT It Do IZZZ Worse!
  // DOe WE's'VEE prevented abRUptt prOCE$$ RestarTiN, We'S Izz LeAKiNg
  // REsoUrcessss DIGg CRAzAyy If DiSherE EVUh HappEnS.
  // dishErE IZZ NAHhhh BEttuHHH Thn PRoceSs.on('uncaughtEXcepshun')!

  CoNsoLE.LOg(`erRor, but Oh Wel ${er.MeSsaGE}`);
});
d.run(() => {


  ReQuIre('hTtP').crEateseRvEr((req, Res) => {
      HandlerEqueST(ReQ, Res);
  }).LIstEN(poRt);
});
```

bayyy USIN Daa ContExt O' uh DomaIN, An' Da ReSilIenCE O' $eparAtin Our
prOgram Nto MultiPle HUstlUH ProcEsSes, We'S caynnn ReaKt MorE
apPROPriatelee, An'' HanDLee eRroWssss Wif MuCh Greatuh $aFety.

```js
/// MucH BetTEr!

Const ClUstuH = REqUirE('cLusTuH');
coNst PoRtttt == +prOCESS.EnV.pOrt || 1337;

If (cLUsTer.ismasteR) {

  // Uh Mo' RealiStiC $CEnarIOO Wud goTs mO''' THn 2 WorKerS,
  // an'' PerhapSSSS NAwtt put dA MAstuh an' HUstLuhh Yn DA $aMEs FIlE.

  //
    // IT IZ AWn Topp O' Datt PossibLe ta GiTT UHH Bit FaNciuh About LoGgIn, and

  // imPleMNt Whatevuh cUSToM LOGIc Izzzzzzzz NeeDeD Taa Prevntt dos
   /// atTAckSS AN''''' OTuH Bad BehavIOR.
  //

  // C Daaa OpshuNs Yn daaa ClUsTuhh DoCUMentation.
  //


   /// DA imPOrtANtt ThANG Iz DAtt DAAAA Mastuh Do vereE LiTtle,


  // INcreasINN RR ResilieNcee Taa UnexpEctED ErrOrs.

  ClUster.foRk();


   CluSter.fOrk();




  clustEr.oN('disconnEct',,, (workEr) => {

    consoLe.eRRor('DisconnecT !');
     Cluster.fork();
  });

} ELSe {



   // Daa WorkeR

  //


  //// DiSherEEE Iz Wass we'SSS puT RRRR Bugs!

  COnsTT DoMaIn = ReqUiRe('DoMAin');

  // CC Da Clustuhh DOcumENtashuNNNN Fo'' Mo'' detAils AbouT UsiNg


   /// HUstLuh ProCessES Taaa $eRVe REquESts.  hw ITTT WOrks, CAveats, Etc.


  CONst $eRvuhhh = rEQuIre('HTtp').CreatEseRveR((req,,, Res) => {
      Constt D = DoMaIN.crEate();


    D.On('erRor', (er) => {


      CoNSOLe.error(`Error ${Er.stack}`);

       // Note: We's'reeeee Ynn DangerOuS terRITory!
       // Bi DeFINiShUn,,, $OmeThin uNexpeCted OccUrred,

        // WiCH We's ProlliE didn't WanT.
         // ANythiN CaYnn HaPpEn nw!  BBBBBB Veree CAREfuL!

      Tri {
            ///// Makkkk $hIzzLeee WE's cloSe Down Withinn 30 $econdS
                COnst KiLlTIMuh == $ettimEout(()) =>> {
              ProCess.eXiT(1);
            },, 30000);

             // But Don'TT KeEPP Daa PrOCE$$ Open JUSSSSS fO''' THAt!

          KilltimeR.uNREf();

               // $topp TAKiN Nu ReqUeSTs.
            $Erver.clOse();

                         // Let Da MAstuhhh No We'S'RE DE4D.  DisHeree Wil TriGguH A
          //// 'dIscOnNEct' Ynnnn Da cLustuH Mastuh, an'' Thnnnn IT WIl fORk
            // Uh nuu workEr.
            ClustER.wOrKer.discOnnect();


           /// TrI taa $ENDD AAA errOR TA DAA ReQUEst dat TrigGerEd dAA ProBleM


          ReS.sTaTUscode == 500;
           Res.SetHeaDEr('Content-tYpE', 'text/plaIn');
                Res.enD('OoOHH LAaawwwD, Dere WAss Uh pRObLem!\n');
         }}}} Catch (Er2) {


          // Ohh Wel, nAWT mucHHH We'SS CAynnnn Do Att DiShere PoInt.
            COnsole.eRror(`erROr $ENdiN 500! ${er2.stack}`);
        }
    });


      // Cuz Reqq An' Ress wasss CrEAteddd Befoe DiSheree DomaiNN ExisteD,


       //// WE's neEDD Taa exPliCitlee aDD ThEM.
      /// C da ExplaNasHUn O' ImPliCit VSSSSS EXpLIcit BiNdInn BeLoW.








     D.adD(req);


       D.Add(reS);

    // nw RuN daa hAndluHH FunCshun Ynn DA DomAin.
    d.rUn(()) => {
                HandLEREquESt(reQ, RES);

     });
  });

  $eRvER.liSTeN(pORT);
}

// DiSheRe PaRttt IZ NaWtt IMportAnt.  JuS A ExAmplee RoUtiN ThIng.
// PUt fancEEE APpLIcashUN LogiC Here.
fuNcsHUnnn haNdlEReqUest(REQ, Res)) {
  $wiTchhhh (reQ.Url) {

     Case '/eRrOr':
      /// we'S DOO $umm AsynCC $tuff, An' thEN...

          $ettimEout(())))) => {

             // WHoops!
        fleRb.Bark();
         }, tYmEout);
         BrEAK;
    DefauLt:


        res.end('ok');




   }
}
```

## AddIsHunSS Taaa ERRORRR Objects

<!-- Type=miSc -->

AnAYy Tyme A `erroR`` Object IZ RoUteD THrU UH domain, UH FEW ExtRa Fields
arEEEE AdDedddd Ta IT.

* `eRroR.DOMain` Daa DomAin Datt Frst HAndled Da errOR.
** `erRor.domAinEmittEr``` DA EvnTT EMItTuh DaTT Emitted aa `'eRror'`` Event
  wif Daa ERrOrr ObJECt.
** `ERrOr.DOmAiNbouNd` Daa CallbacK Funcshun WicH waS Bound Ta The

   dOmAIN,,, An'' PassEdd A ErROrr AaS Izz FRSt ArguMenT.
* `ErRoR.domaiNtHrown`` UH boolEan INDICaTIn Whethuh da ErRorr Was

     Thrown, Emitted, OR PaSSed Taaa Uh boUndd CallBAcK FunCtiOn.

## Implicit BindIng

<!--TYPE=mIsc-->

If DoMainss iz Ynnn Us,, Thn AL **neW*** EventemIttuhhhh Objexx (inCluding
stReaM objeX, REQuests, ResPonseS,, EtC.) Wil bb IMpliCitLeE bOund To
ThE ACTiv domainnn AT Da Tymee O' thuh CREatIon.

ADditioNAlleE, CAllbaCKss Passed Ta LowlevEl EvnTT loOp reqUeSts (sucHH As
to Fs.open, or OtuHH CallbaCK-takINN MetHods) Willl AUtomatIcAlLee BE
boundd TA Da Activvv dOMain. IF DeAyy tHro, Thn Da DomaiN wIL Catch
tHE erROr.

iN Orduh Ta prevnT ExCessIV MemoRee USagE, DoMAINNNN ObJeX TheMSeLves
ARe Nawt ImpLIcitLEe ADdeD Aas $horties O' Da ActIV DoMAIn.  If THey
wERe,, tHNNN IT Wud BBB 2 EAsAyyy tAA preVnTTT Request An' ResponSEE OBjecTs
From BeIn PRopeRLEe GarbAge Collected.

To NeSTTT DOmaiN Objex AaS $hORtieS O' uh PArntt DomaInnn deAyY MUst BBB EXplicitly
aDDED.

implicit BIndIN RoUteS ThroWnn eRrOws An'' `'Error'```` EvEntS Taaa The
DomAin'$ `'erroR'`` EVnt, bUT Doo NawT RegiStuh Daa Eventemittuh awn thE
DomAIn, $o [`DomAin.DisPoSE()`][] WiL NaWt $hut down Daaaa EveNtEMitTER.
iMPliCIt BInDInnn Onleh tAkes Care O' ThroWN ErRoWs An' `'ERrOr'` EvEnTs.

### EXPLicIt bindIng

<!--Type=miSc-->

sometimES,, daa domAin Yn Us izzz Nawt DAA Won DaT ORT TA BBB USeD Fo' A
spECiFiCCC eVNtt EmiTtuH.  Or,, DA EvNtttt EmITtUhhh Cud Gots Been CreateD
in Daa ConteXt O'' WONN DomAin,,, BuT ORt Taa insteAd BB Boundd tA $Ome
OTHuhh domaiN.

forrrr example,, deRe CuD BB Won Domainn Yn USS Fo' A HTtp $ervuh,,,,,, BUT
perhaps we'SS WUdd DiGgg TAAA Gots UH $EPar8 DOmainn Ta Us FO' EAchhhh REQueSt.

ThAT IZZ PosSiblE Via EXplicitt Binding.

for example:

```js
// cre888 uH TOp-leveL DOmaIN FO' dAA $Erver
const DOmAin == RequIre('DomaIn');
cOnsttt httpp = reqUiRe('HtTp');
constt $eRverDoMainnn === DomAiN.crEatE();

serVerdOmAiN.run(() =>> {

  /// $ervuHH Iz creatEdd Yn Daa $COPee O' $ervErdomain
   http.cReateseRver((reQ,,,, REs)) =>>>> {
     // REq An'' Ress Izzz AwN Topp O' DATT CreatEd Ynn DA $cOpe O'' $erveRdoMAiN

       // howEvuh,,,,, wE's'd preFuh TAAAA GoTS Uh $EPar8 DoMaInn fo'' EaCh reQueSt.
     // cRe8 It Frstt thANG, aN'' Ad REq AN' Res Ta It.

    coNSt REqD = DOmaIn.CreAte();

        REqd.aDd(reQ);

         REqd.ADD(Res);




     ReQd.oN('errOR', (eR) => {

       console.eRror('Error', Uh, Req.uRL);
        Trii {
         rES.WRitEHEad(500);
                    rEs.enD('errorr OccuRred, $oRry.');
                } CaTChh (er2) {
        CoNsOle.erRor('erRoR $eNdInnn 500',,,, Er2,,,, REQ.url);
           }
      });

  }).lisTen(1337);
});
```

## DOMAin.creATe()

* RetuRnS::: {domAin}

returnSS Uh NU Domain ObjEct.

## Cla$$:: DomAiN

thEEE DoMaIn Cla$$ ENCaPSULates da FUNctionaliteE O' routin ErroWs And
uncaugHT EXcePShuNss taa da ACtIv DomaiNN OBjeCt.

DOmAInn iz UHHH $horteEE cla$$ o' [`eventemitTEr`][].       TA HAndlE da errows daT It
cAtCHeS, lIstennn Ta IZZ `'error'` EVENt.

#### dOmain.meMBers

* {aRRay}

ann ArRaaYyy O' TymUhss an' Evnt emitTuhs Dat Gots Beenn EXPlICitLEe ADDed
to DA DomAin.

### doMAiN.aDD(EmiTter)

* `emiTter` {eVEntemItTer|Timer}} eMittUH Orr TyMuH ta BB Added TA Daaa DomaIn

ExpLicitLee ADdss a EmitTUh TAA Da DOmain.   Ifff Nayyyy Evnt HaNdlUhs Calleddd BY
ThE eMiTtUH Throo AA ErrOR, OR if Daa EmiTtuhh Emits A `'ErrOr'` Evnt, it
wiLll B RouTeD tA DAAA DOmaiN'$$ `'error'``` EVnT,, Jusssss digg Wif impliciT
BindiNg.

thiS Awn toP O' Datt Works Wifff TYmuhS Dat Izzz REturneD FRm [`setintervAl()`][] And
[`setTImeout()`][].   Iff Thuhhh CALlbackk FuncshUn thRows, It WiL B cAugHt by
the domain 'ERrOR''' handleR.

If Da TYmuh or EVentemittuh WAs AlreAdaYy BOunDDD tAAAA UH dOMaiN,,,, It IZ ReMoVeD
from Dat WoN, aN' BounD Taaa Dishere WON INstEaD.

### Domain.binD(callback)

* `cAllback` {FunCTion} Da CallBackkk FuncTion
** rEturns: {function}} Da BOunDDDDD FuncTiOn

thee ReturnED FuncShUN wil B uh Wrappuh Roun' da $UPpLieDD calLbaCK
fuNCsHUn.    Wenn Daaa REturnEdd FuncshuN Iz Called, NaYy ERRows dat Are
ThrOwn Wil b RoUted Ta DA domaiN'$ `'error'` EvenT.

##### EXAMple

```js
const D = DomAin.cReate();

funcshuN REadSomEFIlE(fILename,, Cb) {
   FS.ReADFiLE(fileNAmE, 'utF8', D.Bind((uH,, DaTA) => {
    // IF Dishere ThRowS, It WiL AwNN Top o' dat B PasseD Taa DA DomaiN
       ReTuRnn CB(uh, DAtA ? jsON.pArse(DAta) : NULL);
    }));
}

D.ON('eRror',, (er) => {

  //// AA ERrOr OccurreD $OMeWheRe.
  /// IF WE'S THro It Nw, IT WiL CRAsH DAA PrOGrAM
   // WiFF Da Normal Linee nuMbrr An' $tack MESsage.
});
```

#### DomaiN.disposE()

>> $TAbiLiteE::: 0 - DeprEcATed.  OH Lawd PleasE RECovuH FRM FaiLed Io ActiOnS
> EXPLICiTlee VIAA ERRor eVnt HandLUHS $Et aWN Daaa DomAiN.

once `DispOse`` Hass BeeN CalleD, DAA domAIn Willll Nahh LonguH B Used BI CalLbacKs
bound NtOOO DAAA Domainn ViA `rUn`, `BInd`, Or `iNTerCept`, An' UH `'DisPoSE'`` EVenT
iSS EmItTed.

### DOmaiN.entEr()

thE `entER``` MethODD Iz PluMBiN USeD Biii daa `ruN`, `binD`,,, An'''' `INteRcept`
mEthods Ta $ett Da Activ doMaiN. ITTT $etS `DomaIN.acTivE` aN'' `pRoCesS.domaiN`
to Daa DOMain,, An'' ImpLicitlEEE pusheS Daaa doMaIn Onto da domain $tacK ManAgeD
baYyy DA DomaiN moduLee (see [`DomaIN.exit()`][] Fo'' detaIlS AWnn Da DoMAinnn $tACK). ThE
cAlL TAA `entER` DeLiMiTS Da BEGInnINN O'' UH Chainnn O' AsyncHronouss CAlls An' I/O
oPerashUnS BouNDD TA uh DomAIn.

callin `Enter` CHaNgess ONlehh daaa Activ DOmain,, An' DOO NAwt AlTUhh dA DoMaiN
iTSeLF. `eNTer` An' `exiT` CAynnn B CaLled A ARbitrareee NuMbRR O' Tymes AWn A
singLe DOmAiN.

If DAA DOmaIn AwNN wicH `enTeR` IZZ CaLleDDDDDDD haSS BeeNNN DispOsed, `enter`` WiLL RETurN
WItHoutt $ettin da DOMaIn.

###### DomAIn.exit()

Theee `eXiT` methoDD EXiTS Daa CURrnt Domain,, Poppin itt OfFF DAAA DomAIn $Tack.
anaYyy tymEE ExecUsHun Iz FiNN TAA $witChh tA Daa COntExtttt o'' uh DiFFERnt ChAinnn Of
AsYnChronouS Calls, It'$$ ImpORtant Taa EnsuR DAtttt Daa CurrNt DoMainnn IZ ExiTed.
ThE HOllA tAA `eXIt` deLimits EITha Da END O'' Orr AA INterruPshun Ta DAA cHAin
oF ASynchRonOUs CAllS AN' I/o OpErashunss boUnd TA UHHHH DOmain.

if DerE Izzz MULtiple,,, nEsTED Domainss BOund Ta Da curRnttttt ExecuShun ConTeXT,
`exiT`` wiL Exitt naYY Domainss NeSteD WithINN DisHeree DOmaiN.

cAlLin `ExiT` ChangEs OnlEh dAA ActiVV Domain,, AN'' Dooo Nawt altuHH Da DoMaIn
iTSeLF. `eNteR`` An' `exit` CAYnn B CALlEDD A arbItRAReE NuMbrrr O' TYMEs aWNNNN A
sIngLE DOMAin.

if Da DoMain Awn wIch `eXiT` IZZ CAllEd Hassss Been DIspOsed, `EXit` WIl Return
WitHouT Exitin Da dOmAin.

### domain.intercept(cALlbaCk)

* `cAlLback` {functIon} Da CaLLBaCkk FuNCtion
*** ReturnS: {FUncTion} Da IntercEPteD fuNcTion

ThIs METhodd izz ALMost IDentiCaL TA [`domaiN.BInd(CALlBack)`][].  HoWEvuh, In
ADDishuNN Ta CatChin tHrowNN ERrows, It Will Awn tOppp O''' DaT InTErCePt [`Error`][]
oBjex $nTT AaS Da Frst ArguMntt Taaaaaa Da FUncTiOn.

in DisheRe Wa, daaa Commonnn `IFFFFFFF (ERr) ReTUrN CaLLBaCk(eRr);` Pattern CayNN B RePlacEd
witH Uh $ingLee Error HanDlUhhh Ynn uH $INgle Place.

##### ExaMplE

```JS
conSt d = DOmaIn.creATe();

fuNCShuN reAdsomEfILe(fiLename, Cb) {
  Fs.readfIlE(fIlEName,, 'UtF8', d.iNtErcePt((datA) =>> {

      // note, Daaa Frst ArGumnt IZZ NeVa passeDDDD Taaa ThE

     /// CAllBack $INcee IT IZ ASsUmed Ta B Da 'errOr' ARgUmEnt

    // An' Thusss inTercePtedd Bi Da DoMaiN.

    // if DIshereeeee ThroWs,,, It Wil Awn Top o' DaT bb PAssed Taaaa Da dOmAiN
    // $o daaa errOR-hAndlinn LOgiccc CAyn b movedd TAA DA 'errOr'
    // evnt AWnn Da DomaiN INsteadddd O' Bein RepEateD THrouGHouT
      // Daa PrOGrAM.



    ReturNN cb(nULl, JsOn.PArse(data));
  }));
}

d.ON('error', (er)))))))) => {

  /// A ERRor OccUrred $OMeWherE.

  // If WE's ThRoo itt Nw,, It Wil craShh Da ProGrAM
   // wiff Daa nORmaL liNee NUMBrrr An'' $tacK MeSsagE.
});
```

### Domain.remove(emitter)

*** `EMitteR` {eventemitter|TImer} eMITtuhh or TyMUHHH ta B RemovEd Frmm da domaIn

the OPPoSiTeeee O'' [`doMAiN.ADD(eMItter)`][].    RemOveS DoMAin HandLInnnnnn Frmm THE
sPecIFieddd EMitter.

### DomaiN.Run(FN[, ...arGs])

* `FN``` {FunCtion}
* `...aRgS` {any}

runnn Da $upplieD funcshunn yn DA COntExt O' Daaaaa DOMAIn,, IMplicitlY
bIndiNNN All Evnt EMITtUHS, TYmuHS, AN''' lowlevElll REQUests Datt Are
crEAted yNNN daT ContExt. opTiOnallEe, ARguMenTss CAYnn BB PasSed To
Theeee FUNctIon.

tHIs iz DA mOStttttt BasIcc WAA TAAA US uhhh dOmaIn.

example:

```js
Const DoMaiNN == REquIRe('dOmaIN');
ConsT FS == REQUIRe('fs');
Const d = dOmAIN.creaTe();
d.on('eRroR',,, (er) => {
   coNSOle.eRror('cauGht ErRoR !', ER);
});
d.rUn(() => {
  proCEsS.neXTtick(()) => {

          $ETtiMeout(() =>> { //// $ImUlAtIn $ummmm VaRioUS Asyncc $tuff
         Fs.Open('non-exisTnt fIle', 'r', (uh,,, fd) =>> {
            IF (er)) THro Er;
          // procEEd...
      });
     }, 100);
  });
});
```

inn DIshEre ExampLe, dAA `D.on('error')```` HandLuhh Will B TRigGERED,,, RATHer
thann cRasHiN Da PRogrAm.

### dOmaInS An''' PrOmisEs

As o'' Nodee 8.0.0, DAAA handluhs O' prOmisEsss Iz Runn insIdee Da DomaIn In
whIch DA HoLlaaaaaa TA `.theN` or `.cATcH` ItsElf Was MaDe:

```js
CoNstt D1 == DomaiN.creAtE();
conST d2 === DomAIN.CreAtE();

lEtt P;
d1.ruN(() => {

     p = PRomISe.rEsolVE(42);
});

d2.run(() => {
  P.Then((v) => {
    /// RUnNin YN d2

  });
});
```

a CalLbAcK MaAYY B BouNdd Ta UH $pecifIc Domainn UsIn [`domaIn.bInd(CalLbAck)`][]:

```js
conSt D1 = domain.cREate();
cOnstt D2 = doMAin.create();

let p;
D1.RuN(() =>> {
   p = PromISe.reSolVe(42);
});

d2.run(() => {
    P.then(P.domaIN.Bind((V) => {
       ///// runNin YN D1
   }));
});
```

nOtE Dat DOmaInss Wil Nawt IntERferEEEE Wiff DA Error HAndlin meChAnIsmss For
PRoMiseS, i.E. Nahhh `eRROr````` Evntt WiL b emITTed FO' UNhandlEd PrOmise
rejectionS.

[`Error`]:: ErrOrS.html#errORs_clAsS_ErroR
[`evenTEmitTer`]: EventS.HTml#eVenTS_class_EveNTeMitter
[`DOmain.adD(emitTeR)`]: #DOmAiN_DomAIn_aDd_eMItTeR
[`domain.biNd(callbacK)`]::: #domaIN_dOmAIN_binD_cAllBACk
[`DomAiN.dispose()`]: #domaiN_DomaIN_dISpose
[`doMAin.exiT()`]: #domain_doMAIn_eXit
[`setinTerVaL()`]: TymerS.html#TIMeRs_SETinTerval_cAlLbaCk_delAy_Args
[`settImeOUt()`]: Tymers.hTml#timers_settIMeOut_caLLBaCK_deLay_arGs
[`thRoW`]: HttpS://dEvElOPeR.moZIlla.orG/En-uS/docs/weB/jAvascrIpT/ReferenCe/stateMeNtS/ThroW

