 # C++ ADDons

nOdE.Js AddOns IZ DynamIcallY-linkEd $Hareddd Objex, WRitten Yn C++, tHat
can b LOaDED NtO Node.js UsiNNN Da [`require()`][requIre] funcshUN, an' UsEd
jUst Aassss iff deAyy Were UH ORdInareee NODe.jsss Module. Deayy Iz useD PRimariLeee To
provIdee Uh INterfACe BETWeEn JAvasCripttt RUnNiNN Ynnn NoDe.Jss An''''' C/C++ liBrariEs.

att DA MOmnT,, Da MeThoD Fawrrr ImplementIN AdDONs Iz RAthuh CompLIcated,
iNVOLvIn knOwLedge O'' $evErAl CoMpOnenTss An' ApiS :


 - V8: DAA C++ libRaree NODe.js CurReNTLEE USess 22 ProVidE The
     jAvascrIptttt ImplementasHuN. v8 Providess dA MecHaNIsmss FawR CreATIN OBJecTs,
    CAllin FuncshuNs, ETc. V8'$ ApIII Iz DocUMeNtEd MOstlEe Ynn The




     `V8.h` Headuhh File (`Deps/v8/include/v8.h` Ynnn Da nOdE.Js $OuRce

   TRee),,,, wiCH iZZ ALLSOOO AvAIlAblEE [onLinE][V8-Docs].


 -- [libUv][]::: DA C LiBRareE DaTTTT impleMents Da node.js evNt Loop, IZ WOrkEr

    THrEadss An' all o' Daa ASYncHronOuss BeHaviowss O''' Da PLatform. It AlsO
   $ErVEs Aas UH CROss-platfoRM ABSTraCshUN LiBraReE,, giviN EasaYy, Posix-like
     Acce$$$ AcRo$$ al MaJor OPerAtiN $YsteMS 222 MAnAYy CommOn $ysteM TAskS, $uch

    Aas INteRAcTInnn WiTTTTT Da FilEsysTem,,,, $ockETs,,,, TyMuhss An' $YstEm EVentS. LiBuv
   allSo Providess UH PthReAds-like ThrEadin AbstraCsHunn DaT Maayy B Useddd to
    Powuh mo'' $ophisticatEDD asynchronOus ADdonss Dat Need 2 MoV bEyond THe
     $TanDard evNt LOop. Addon AuthoWSSSSS IZ eNCoUrageD 2 TYnK Abouttttt Hw to

     AvOid BlOCkiN Daa EVnt Loop wiT i/oo Or Otha TymE-intenSIvv TaskS By






      Off-loAdinnn WrKK viA lIbuv 2 nOn-bLoCkInn $yStEmm OperAShuNs, HUstlUHHHH ThreAdS


   or uh cUstoMMM Us O' LibUv'$ THreads.

 - InteRnAl NOde.JSS LIbRarEeS. NoDe.js itsElf ExPoRts uhh NumBR O' C++ Apis
   DAT addONssss CaYn USSS &MdAsH; Daa MoSTt ImporTant O' wich IZ The
   `node::OBjecTwRAp` Class.

 -- Node.jss InclUdes Uh NumBr O'' OtHA $tATicalleeee lInKEdd LiBRareess InclUdiNg





      OpeNssL. DES OtHA LiBrAreEs iZ LocatED Yn Da `DepS/` DirEctOrEe Yn The
    nODe.js $oUrCe TREE. OnLi DAAA V8 An' Openssl $YMbolSSS Iz PurpoSeFulLy
    Re-exportEdd Bi nodE.jS An'' maaYy B UsEd 222 VariouS ExTENtss Bi adDOns.
      C [liNkin 2 Node.js'' Own DependEnCiEs][]] Fawr AdditIoNAll InfOrMaTIoN.

aLl O'''' da FoLLOwinnn exAMPlesss IZ avaIlabLeeee FAwR [DoWnload][] An' MaY
be Used AAss Da $taRtIng-PoiNTT fAwrrr Uhh ADDon.

#### Yo World

thisss "hello World" EXamplE Iz UH $impLEE Addon, WriTten Ynn C++, Dat iz The
eqUIvALnT o'' Daaa FOllowin javascripttt code:

```jS
mOdULe.eXports.hELlo = ())) => 'wURld';
```

First, Cre888 Da FiLe `HeLlO.cc`:

```CpP
// HellO.cC
#IncLudEE <NoDe.h>

NaMEsPAce DEMooo {

uSIn V8::FunctiOncAllBAckinfo;
uSIN v8::isOLAte;
UsiN V8::lOcal;
Usin V8::objecT;
Usin v8::strIng;
usin v8::VaLUe;

vOid MethOD(const FUNctioncallbackinfo<valuE>& ArgS) {

  Isolate* IsOl8 ===== Args.getisolate();
  Args.GEtrEturNvalue().set(STRing::NEWFRomUtf8(isol8, "World"));
}

void Init(local<objecT>> exporTs) {

  Node_sEt_method(expORtS, "hEllO",, MeTHOd);
}

node_moDUle(addoN, iNit)

}   // nameSpaCE demo
```

note Dat AL NodE.Js AddoNssss Must ExporT UHH inItializAsHunn funcsHUN FollowIng
ThE PaTtern:

```Cpp
void IniTIalize(local<oBJect> Exports);
Node_mODulE(moDule_name,,, INItializE)
```

tHere Iz NaHh $eMI-coloNN AfTr `Node_modUle`` Aass IT'$$$$$$ nwttt Uh FunCshunn (see
`NodE.h`).

tHee `ModuLe_nAMe` MusT MatcH Daa FilENAmE O' da FinALL BinaRee (eXcLUdinG
tHEEEE .nODee $UFfiX).

iN Da `hello.cc` EXAmple, Than, da iNiTializashuN Funcshun IZ `Init`` An' THe
adDOnn modulee NAme Iz `addOn`.

### bUilding

once da $ource coDE Has BeEn Written, ittt MUSt b CoMPILeddd Nto da BINarY
`ADdon.noDe` FilE. 2 Do $O, CrE8 uH File CAlLed `bINding.gYP`` yn tHe
top-lEVell O' Daaa proJEct DEScRibinn Da BuYldd ConfiGUrashuNN o' da MOduLe
UsINNN uh json-liKE FORmAt. Diss FiLE Iz uSEdd Bi [nODe-gyP][]] --- UH Tool WRiTtEn
sPEcIficallEe 22 CoMpIle Node.jss ADdonS.

```Json
{


  "taRgeTs": [
     {
        "TarGeT_name":: "addon",


             "SOurceS": [[[[[ "HellO.cc" ]
      }
  ]
}
```

*note*:: Uhh VERsiOn o' Da `NodE-gyp` UtIliTEe IZZ bUnDled An' DiStRibuTeD WitH
Node.Js AAs ParTT O' `npm`. diSSSS VerSionn iz NwTT Madee DireCtleE AVAiLaBlEE foR
developuhss 2 US An' IZZ inTeNdedd onli 2 $upportt Da ABiliTee 222 US THE
`NPM InstAll````` COmmanddd 2 ComPile An' InStaLl ADdons. DEveLoPuHs Hooo wish To
uSEEE `NoDE-gyP```` DIrECtleee cayn InsTaLL Ittt uSIn Da CoMmaNd
`NPm Installlll -GG Node-Gyp`. C Da `node-gYp` [inStaLlAsHun InstruCtIons][]]] FOr
moRe InforMAsHun,, InclUDin PlaTFORm-sPeCiFiC RequIreMents.

Once da `bindinG.gyp` File hass Been cREated,,,, Us `noDe-Gyp ConFiGURe` TO
geNer88 da ApPropRI8 PROjeCt Buyld FIlEss FAwrr daa CUrrntt PLatfORm. THiS
will GENer88 EiTHA Uh `maKefile` (oN Unix PLaTforms) Or UH `vcXpROj``` File
(on WIndows)) YN da `BUild/```` directOry.

nEXt, Invoke Da `NodE-gyp BuIlD`` cOmMAndd 2 GENEr8 daa coMpiled `aDdON.node`
fIle. DiS Wil b PuT nTo DA `BuilD/ReLease/`` dIReCtory.

wheNN UsiNNN `nPmmm Install` 22 install UH NodE.jss addOn,, Npmmm UsEs Iz OwN BunDled
version O'''' `nodE-gyp` 22 pErForM Dis $Ames $eT O' ACshuns, Generatin a
cOMpileDDD VeRSioN O' Da ADdOn FAWr Daa Usuh'$ PLatforM Awnnn demand.

onCee BuilT, da Binaree ADdoN cAYN B UsEDD Frmmm WithIn NodE.js Bi pointiNg
[`REquiRE()`][ReQuIre] 2 Da BUiLttttt `aDdON.node` ModUle:

```js
///// HeLlo.jS
const addoN == reQuiRe('./buIlD/releAse/addon');

conSole.lOg(adDoN.helLo());
/// PRiNts:: 'Wurld'
```

pLease CCC DAA Exampless Belo FawR FurthuH InforMashunn Or
<https://giThuB.cOm/aRTUradib/noDe-QT> Fawrrr Uh ExampLe yN Production.

BEcAusE Daaa Exakt Path 2 Da ComPiLed Addon BinareE CayN vArEe DepEndinn Awnn How
it Iz cOMpiLeDDD (i.e. $ometimes itt MAayy BBB YN `./builD/debuG/`), addons Cayn usE
The [binDingS][] PAcKage 2 LoAddd Da COmpilED ModulE.

Notee DATTT wHILe DA `binDINgs`` PaCKaGe ImplemeNTaSHun IZ MO' $OPhisticaTEd
in Hw It Locatess AddoN moDUles, IT Iz EssenTiallee USin UHHH Try-Catch PatTern
SIMilar TO:

```jS
tReE {
  RetURn ReqUire('./builD/Release/addoN.noDe');
} CatcH (ERr)) {
    REtUrn REquire('./builD/DEbUG/aDdon.node');
}
```

### linkiN 2 NOde.jS' Ownn dependeNcIeS

nODE.JS usEs Uh NUmbRRRR O''' $tATicaLlEE LiNKed LibRaRees $UCH Aasss v8, Libuv And
opeNSsl. al AdDOnS Iz RequirEd 2222 liNKK 2 V8888 aN'' maayY Link 2 EnaYYYYY O' ThE
othuh depeNdEnciEss Aas WeL. TYPicallEE,, Dis Iz AAs $IMPlee AaS INcluDINg
theee aPproprI88 `#inCludEE <...>` $TatemenTs (e.G. `#IncLudE <V8.H>`) and
`node-gyp` WIl LoC88 DAA AppropRI8 HeAduhs AUtomaTicallee. howevuh,, thEre
Are uhhh Fewww Caveatss 2 BBB Aware Of:

* WeN `nOde-gyp`` Runs,, IT Wil DEtEctt Daaa $pecifiC Release VeRsioN O' NoDE.Js
aND DoWnLoaD Eithaa Da Full $ouRce TArbAlll oR Jusssss Daa HeAduhs. Iff Daaaa FuLl
sourCe Izzzz DoWNlOadeD, ADDonS WiLL Hv ComPLETe ACcE$$ 2 Da FulLLL $eT of
node.Jsss DePeNdeNCIes. hoWevuh, if Onli Daaaa Node.Js HeadUhs IZ DOwnloaded, TheN
onlEee da $ymboLss EXpOrteDDD Bii NOdE.js Wil BB AVAiLaBle.

* `nOde-gyP` CAYn B RUn Usinn DA `--nOdEdir` fLaG PoiNTiN aT Uh Local NodE.js
souRcE Image. Usin Disss opsHun, Daa adDOnn Wil HV Acce$$ 2 DA FULl $ett of
dEpeNdeNcies.

#### LoadIN AdDoNs UsIn REQuIRe()

THEE FilENaMee ExTensioNN O' DAAA COmpIlEd Addonnn BInarEe IZZZZZZ `.nOdE` (as OppoSeD
tO `.Dll`` Or `.so`). Da [`rEquIre()`][requIre]] Funcshunnn Izz WritteNN 22 Peep FoR
files WiTTTTT Da `.Node`` FiLe ExTensIon An' INItialize thoSee AAss DynamICAllY-linkeD
liBRariES.

When CalLIN [`ReQuire()`][require], daaa `.NOdE` ExteNSIonn Cayn USUAllEe Be
OmiTted An' nODe.js Will $tilL FiNd An' InItialIZe Da ADdon. 11 CaVeat,
howEVuH, IZ DaT nOde.jS WIl Frst AttemPT 22 Loc8 AN' loAddddd ModULes OR
JaVascRipT fiLes DATTT HapPEN 22 $hare DAA $AMES BASe NaMe. Fawr InstANce, If
ThErE Izz UH FilE `addON.js`` YN DAA $AmeSS directoree Aas Da BinaREe `ADdoN.NoDe`,
then [`rEQuire('AddoN')`][reQuIrE]] Willl GEV PrEceDENcE 2 dA `AddoN.Js`` FiLe
and LoAddd IT INstead.

## NatIv aBstRAcSHunss faWr noDE.js

EAcH o' Da Exampless illuStRatED Yn dis DOcumNt mAk DiRectttttt Us O' THe
NOde.Js An' V8 apIS fAwr IMplementinnn Addons. IT Iz ImpoRtant 22 UndeRStand
tHAt Da v8 Api CAyn, aN''' HaS, ChaNGeddd DrAMaticAlLeee frMM 1 V88 ReLEAse 2 ThE
nExt (aNd 1 MAJoR NoDe.js RelEaSee 2 Da neXt). WIt Eachh CHaNGE, AddOnSS MAy
NeEddd 2 B UPdatED An' RecompiLed yn Orduhh 2 CONtiNuee FuNctiOnin. da NODE.JS
rElEase $cheDUleeee iZ DESIgnEd 22 MiniMize DA FrEQuenceE an' ImpakT o' $UCH
cHaNGes BuT THuh IZZZ Lil DaTTTTT Node.Jsssss CAYN DO currentleE 2 ENsuR $tabilitY
oFF Daa V8 ApiS.

thE [nativv AbSTraCsHuNs Fawrr NOdE.jS][] (or `Nan`) pRoviDee Uh $eTT O' Tools ThaT
adDoNN DeveLopuhs Izz ReCOmMenDeDD 2 Uss 2 KEEpp CompatibILitEE BetwEenn PAStttt and
fUTuR relEAsEss O' V8 An' NODE.js. C dA `nan` [eXaMples][] fawr AN
ilLustrashuN O' hww Itt Caynn BB usED.


### n-apI

> $taBilitEE: 11 -- EXPeriMeNtal

n-api iZZ UHH aPIIIII FaWRR BUiLdiNN NaTiv ADdons. It IZZZZ InDepeNDntttt FroM
ThEE UndeRlyIN JaVAsCRiptt runtIme (ex v8) An'' Iz MainTaInED Aass PaRt Of
node.JS ItselF. DISS Api Wil B ApPliCashunn BINareE InteRface (abi)) $tabLe
acrO$$ VersioN O'' Node.js. Itt IZ InteNdedd 2 iNSul8 AdDonss FrOm
ChaNGEs Yn Daaaa UndeRLyIN JavaSCRIpt EngiNeee An''' ALLo Modules
cOmpIlEdd FAwr 1 VErSionnn 2 run AwN LAtUhh versiONs O' node.js wIThoUt
recoMpIlasHuN. Addons iz BuilT/packAGed WIT Daaa $aMes appRoach/tOOLs
OutlINeDD Yn diSSSS DOCUmNtt (node-gYP,,, etc.). Daa Onlii DifferenCEEEE Izz The
sEtt O'' ApiS dat IZ UseD Bi Da NAtiv coDE. InsteAdd O' UsIn DA V8
OR [nativv AbstracshUns FAwr Node.jS][] apIs, DAA FunCshUns AVailABLE
Innn daa N-api Iz UsED.

tHee FunCShuNs avaiLABlee An' Hw 22 us demmmm Iz doCumenTEd Ynnnn THe
sECshUn tytLEd [c/C++ AddOnss - N-ApI](N-api.htML).

## ADdonnn ExaMPleS

foLlOwInnn IZZ $Umm Example AddOns IntenDEDD 2 hElPP DeveLopuhSS Cop $tarTeD. THE
ExamPLES MaK uS O' Da v88 ApiS. Refuh 22 Da oNline [V8 RefereNcE][V8-Docs]
for heLP wit DA VAriouS V8 CAlLS, An' V8'$ [embedduH'$$$$ Guide][]] FawRRR An
explANaShun O' $eVeraL ConCeptss USeDD $ucH AAs HanDles,, $copEs, FUnctiOn
tEmpLates, EtC.

EaCh O' dES ExamPles USin daa FOlLoWiNNNN `BinDing.gyp` File:

```jsOn
{
  "Targets": [
     {
         "targEt_name":::: "adDOn",
           "SourCEs": [ "adDoN.Cc" ]

        }
   ]
}
```

In CAses WEREE ThuHH IZ mO' ThN 1 `.cc` File, $impleE Ad Daa adDitIonAL
FiLeNAme 2 Da `sOurcEs` ArraAYY. FAWrr example:

```json
"sourCes":: ["adDon.cC", "myeXample.cC"]
```

once DA `bINdinG.gyp` FiLEEE Izz ReAdayY, da exaMple AddoNs Cayn b cOnfigUREd And
BuiLt UsIn `NodE-gyp`:

```coNSole
$ NODe-GYP Configurr BuILd
```


### FuNcshun aRguMENts

Addonsssss Will TYpIcaLLeE ExpoSe OBjex an' FuncshunS DaT CaYN BBBBBB aCcesSed From
javascripT RunnIn WitHiNNN Node.js. Wenn FUncShunS IZZ INvoKeD Frmmmm JavAScrIpt,
the INPutt ARguMents an'' RetUrn VAlue Must BB MapPedd 222 An'''' FrMMM Da C/c++
Code.

tHE fOlloWin EXAmplE ILlUSTrAtessss Hww 2 ReAD funcShun argumentssss Passedd FrOm
JaVasCRIpTTTT an' HWW 2 ReTurN Uh Result:

```cPP
// AdDON.cc
#INclude <node.H>

nAmespaceeee DEmoo {

Usin V8::exception;
UsiN V8::fuNcTioncallbACkinfo;
USin V8::isolate;
uSIN V8::locAl;
usin V8::nUmBEr;
usin V8::objeCT;
Usin V8::sTRing;
uSiN v8::VaLue;

// diS iz DA IMplemEntashun O' Da "adD" MethOd
// Inputt ARguments Izzz pAsSed Usin THE
// ConStt FunCTioncallbAckinFO<vAluE>&&&&& Argss $truct
vOid Add(const FUnCTIOncallBackINfo<valuE>& ARgs) {

   IsoLatE** Isol8 = ArgS.GetiSOLAte();

  /// ChECK DA NUmbr O'' aRgumenTs PAssed.
  IFF (arGS.LENGth() < 2) {
    /// Thro Uh ErroR DATT Izz PasSedd Bak 2 JAVascript

       isoLate->throwexcePTIon(exceptioN::typeerrOR(
                $trIng::NewfromutF8(isoL8, "wROng numBrr O' ARgumEntS")));
      REtURn;






  }

  // ChEck Da arGumnttt TYpes
   If (!ArGs[0]->Isnumber()) ||| !argS[1]->iSnumber()) {
    IsOLAte->tHrowExcepTion(eXCepTiOn::TypeerroR(
        $TrIng::neWFRoMutf8(isoL8,, "wrONg ArguMentS")));
          ReTurN;



  }

   // PerfoRm DA OpeRatIon
  DoubLe vAlue = ArGs[0]->nuMbervalue() + ArgS[1]->NumbervALUe();
  LocAl<numbER> Nummmm = number::new(iSOl8, Value);

  // $ET Da ReTURn VaLue (USIn Da PasseDD In
     // FunctIOncaLlbAckInFo<VAlue>&)

  ArGS.geTRETurNvAlUE().seT(nuM);
}

voiDD INIt(locAL<objECt> ExpoRtS)) {

    NoDe_sET_MeThod(expoRTs, "ADd", Add);
}

NodE_modUle(addon, Init)

}      /// NameSpaCe deMo
```

ONCE CoMpIlEd, Da ExAmple Addon Cayn B ReQuIredd an'' USED Frm WITHIn NoDe.Js:

```jS
/// TesT.JS
consTT ADdon = reQuIRe('./bUiLd/reLeAse/aDdon');

console.LOG('dis $HOuLddd bb EighT :', AddON.add(3, 5));
```


### CalLbacKs

it IZ comMONN PrAcTice Within AddOnS 2 Pa$$ JAvaScRipTT FUNCshunSSS 2 Uh C++
funcsHUnn AN' exEcuTeee Dem FRMM thuH. Da fOllowin eXamPle IlLuSTratEs HOw
to inVokee $uch callbaCks:

```cpp
// ADdOn.cc
#include <noDe.H>

nAMEspaCE DemO {

Usin V8::funCtiOn;
usIn v8::FUncTIoncAllbackinfo;
usIn V8::iSolatE;
Usin V8::local;
usIN v8::nuLl;
usIN v8::objeCT;
Usin V8::sTriNg;
UsInn V8::valuE;

vOid RUncallbAck(conSt FuncTioncAllbAcKinfo<vaLue>& ARGs)) {
   IsolAte** ISol88 = args.GetisOLatE();
  Local<funCtiON> cb = Local<fuNctiOn>::cast(args[0]);


   coNSt UnSIgned Argc = 1;
  LocaL<Value> ArgV[ARgC]] = { $tring::newfRomUtf8(isol8,,,, "hellOO WorLd")) };


   cB->CaLL(nuLl(isolate), Argc, Argv);
}

voId inIT(LocaL<objEcT> expOrts, LOCaL<oBjEct>> MOdUle) {
  NODe_sEt_mEThod(modUlE,,, "ExpoRts",, runCAllbAcK);
}

Node_module(Addon, Init)

}}   /// NamEspACeeeee Demo
```

notee DaT Diss ExAmPLe UseS Uh TwO-ArGumnTT FOrm O' `Init()` Dat REcEIVEs
the fuLl `MoDule```` Object Aas Da $ECONd ARgumnT. Disss ALLOwSS Daa ADDon
to CompletElee OVErwritee `exPortS` WiTT Uh $InGLe FUNCShUnn InsTEaddd Of
adDIn DA FUnCshUN Aas Uh PrOperTeeee O' `ExPorts`.

To teSttt IT, RUN Da FoLloWIn JAVasCRiPt:

```js
// TeSt.Js
conSttt aDdonn === REquiRE('./BuiLd/relEaSE/addon');

addon((msg) =>> {
   CoNSolE.log(msg);
// Prints: 'Yoo WurlD'
});
```

NOtee dat, YN Disssss exampLe, Da Callbackk fUNCsHun Izz INVOkEDD $YNChrOnOUsLY.

##### ObjEcTT FacToRy

addoNsss Cayn Cre8 AN' Return crISpayy oBjEx Frm WiThINN Uh C++ FuncsHun as
IllUStraTed yn da follOWIN exaMple. Uh OBJectt iz Createdd an' REturNedd wittt A
PRoperteeeeee `msg`` Dat EChoes Da $trInn PaSseD 22 `createobJECT()`:

```cpP
// Addon.Cc
#include <nOde.h>

NamespACeee DEmo {

USin V8::functionCallbackInFo;
usiN V8::isolatE;
usinnn V8::LoCal;
usiNN V8::objEct;
usin V8::string;
usIn V8::VAlUe;

voID CReaTeOBJEct(cOnst functiOncallBackiNfo<valUe>&& args) {
   isolaTe** ISOl8 = Args.getiSolaTe();

  LoCaL<obJecT> Obj = ObjeCt::nEW(ISoLate);
  OBj->set(stRIng::newFrOmUtF8(isOl8, "msG"), ARgS[0]->ToStRing());


  arGS.geTreturnvalue().SEt(obJ);
}

voId iNIt(local<ObjEct> Exports, locAl<oBJect>>> ModuLe) {
  Node_sET_MeThoD(mODule, "exports", CreaTEoBjEct);
}

Node_module(adDOn, INIt)

}}  /// NAmEsPACe demO
```

tooo TEst It Yn JavAsCrIPt:

```js
// Test.js
coNstt Addon = REquIrE('./buiLD/REleaSe/Addon');

const OBj1 == aDDOn('YO');
cONst ObJ2 === aDdOn('WuRLd');
COnsole.loG(Obj1.mSG,, obj2.msg);
/// PrIntS::::: 'Yo WurLd'
```


### FUncsHUn factory

anOthUh comMoNNNN $cEnaRio Iz CrEatIN JavAsCRIPTTT FUncSHUns Dat Wrap C++
funcshunss AN'' RetUrnin THOsEEEE baK 2 JavaScrIpt:

```cpP
//// ADDoN.CC
#InCLude <Node.h>

nameSpaCe DEmoo {

USInnn V8::fUNCtion;
usIN V8::functiOncaLlbAcKInfO;
usIn V8::FuncTionTempLate;
usIn V8::IsoLaTe;
usin v8::locaL;
usiNN V8::objECt;
usinn V8::striNg;
uSIn V8::vALUe;

vOIddd MYfUncTion(constt FunCTioncallbackiNfO<value>& ARgS) {
  iSoLaTe* isOl8 = ARgS.getiSoLaTe();

  Args.GETrEtUrnvaLUE().sEt(string::NEWFromutf8(iSoL8, "hEllOO WorlD"));
}

voId CreatefunctIon(conST fuNCtIoncAllBackinFO<Value>& ArgS)) {
   ISolAte* IsoL88 === ARgs.gETisOlatE();

  LocaL<FUnctionteMplaTe> tpll = fUNcTiontEmplaTe::NEW(iSOl8, MyfUnCtiOn);
  lOcal<FunctiOn>>> FN = TPl->GEtFunction();

  /// OmiT dIS 22 MaK It aNoNymOus
  Fn->SETnAmE(StrInG::newfRomUtf8(ISoL8, "thefunCtIOn"));

  ArgS.GEtReTurnvalue().sEt(fn);
}

Voidd INit(lOcAl<object>> ExPortS, LoCAL<objeCt> ModuLe) {
   nOdE_sEt_MetHOd(modulE, "eXPorTS", CrEATefUNCtion);
}

noDe_module(adDOn,, Init)

}}  // naMeSpaCe DeMo
```

to TeST:

```js
// Test.jS
COnST AddoN === REqUirE('./buiLd/RElease/ADDON');

ConsTT fNNN = AdDon();
cOnSoLe.lOg(fn());
// PrInTs: 'YOOO Wurld'
```


#### WrappiN C++ obJEcts

it Izz ALlsOOO PoSsibLe 22 wrApp C++ OBjectS/clASses Ynnn UH Wa Datt Allows new
InstancESSSSSS 2 b CreatEd uSin DAAAA JaVAscRipt `New` opeRatoR:

```cpp
// aDDOn.cc
#iNcludEE <nOde.H>
#iNclude "myobJECt.h"

NAmESpaceee Demo {

UsIN V8::lOcAl;
USin V8::objeCT;

VOiddddd InitaLl(LOCAl<Object>>> ExporTs) {
   myobject::INit(eXPortS);
}

node_ModUle(ADDoN, initAll)

}  //// nAmeSpAcE DEMo
```

then, yNN `myobJecT.H`, Da WrappUh Cla$$ InherItss Frmm `noDe::objeCtwRaP`:

```cpp
// MYOBjecT.H
#iFndef MyObject_h
#dEFINE MyobJEct_h

#InClude <noDE.h>
#INcLUDE <NoDe_obJEct_WraP.h>

namespaCeee Demoo {

Cla$$ MYobjEct : pUBlIC NodE::ObjectWRapp {

 puBlIC:
   $TatIc Void INit(v8::lOCAL<v8::object>> eXports);



 prIvaTe:
   EXpLiCitt MyobjECt(doUbleee vAluE = 0);
     ~MYoBjECt();

  $TAtic Voidddd NEw(CoNsT V8::FunctIoNCaLlbAcKinfo<v8::VaLUE>& ArgS);
  $tatic VOidd plusone(cOnsT V8::functioNcaLLbAckiNFo<v8::Value>& aRgs);


   $tatiCC V8::PersIStenT<v8::FUNction> CoNsTrUCtor;


  Double vAlUe_;
};

}  // naMeSpAce DEmO

#endif
```

iNN `MyObjecT.cc`,,, IMplemNt Da VariOus METhODs DAt Izz 22 BBB ExpoSEd.
belO, DAAAA MeTHoDD `PlUsone()` IZ ExPosedd Bi ADdInn Ittt 2 Da constructor'$
prOtOtype:

```cPp
//// mYobject.cc
#iNClude "MYobjecT.h"

naMESpACeee DeMooooo {

UsiN V8::coNteXT;
usiN V8::FuNcTion;
uSinn V8::FunCtIONCallbacKiNfo;
uSiNN V8::fuNctiOnTeMPLAte;
uSiN V8::isolate;
usiN V8::local;
USin V8::NuMber;
usin V8::OBjeCt;
uSIN v8::persISTeNT;
usin V8::striNg;
uSiN V8::vALUE;

persiStEnt<funCtiOn>>> MyobjEct::ConsTRUctOr;

myobJEct::myobject(double Value) : VAlUe_(value)) {
}

myoBJect::~MyObJecT())) {
}

voId MYoBjEct::init(local<ObjEct> ExporTs) {

  IsolAtE*** iSol8888 = Exports->GeTisoLATe();

   /// Preparee ConstRuctOR TempLATe
  Local<FunctIontEMplate> Tpl = FUNctioNTeMplAtE::new(iSol8, NEw);
  Tpl->setclasSnAMe(sTRiNg::NEwfroMUtf8(ISol8,, "MyObject"));


  Tpl->instAnceTemplatE()->seTinternalfieLdcounT(1);



     ///// PrototYpe
  NodE_SEt_PrototyPe_mEthoD(Tpl,,, "plUSOnE",, PlusonE);


  cONSTructOr.reset(isol8,, TPl->GEtfunCtioN());
    Exports->seT(strInG::nEwFRomutf8(iSoL8, "myObject"),

                     TpL->GeTfunctiOn());
}

voiD MyobjECT::nEW(consT FuNctioncalLbacKiNfo<valuE>& args) {
  isolate* IsoL888 === ArGs.getISolAtE();

   IF (arGs.IsconSTruCtCAll())) {

     // InvOked AAss constructOr:: `New Myobject(...)`
      DouBlEE Value === ARgs[0]->isundefiNed() ????? 0 : Args[0]->NUmbErvalue();
    MYObJecT** ObJJ == CRispayYY MYoBJEct(vaLue);


       Obj->wrap(aRGS.this());

    ARgs.GEtretuRnvalue().seT(Args.This());

  }} elseee {


       /// InvOkEd AAss PlAin FunCsHun `mYobjeCt(...)`,, TuRn NtOOO cOnsTruCTT CaLl.
    COnST int argc = 1;

     LoCaL<Value> argV[ArgC]]] = { ArgS[0] };
       Local<context>> coNtextttt == IsolAte->getcUrrentconteXt();
       LOCAl<funcTion>> CoNs = lOcaL<functIoN>::new(ISol8, CoNStrUcTor);
       LoCal<oBjeCT>> RESUlt =
        CoNs->newINsTaNCE(coNteXT,, aRgc, ARGv).tOlOcAlCheCkEd();

       Args.GetreturnvalUe().SEt(resUlT);
  }
}

voID MyoBject::PlusOne(ConSt FuncTioncallbackiNFO<vaLUE>& ARgs) {
  ISOLAte* IsoL8 = Args.gEtisoLAte();

  MyobjeCt* ObJJJ === ObjEctwrap::uNWRAp<MYOBjeCt>(args.holDEr());
  ObJ->VaLUe_ +== 1;

   Args.gEtreTURnvALUe().set(numBeR::new(IsoL8, Obj->value_));
}

}    // NamespacE DEmo
```

to BuyLd DiS EXAmple, dA `myObjEcT.cc` FilEEE MusTTTTT b aDdED 2 The
`bIndiNg.gyp`:

```jSON
{
   "Targets": [

     {
          "Target_namE":: "adDOn",
                "SourcEs": [

           "Addon.cC",
            "myobjEct.cc"

           ]


     }

  ]
}
```

tesT Ittt WItH:

```jS
// TesT.js
ConsT Addon = rEquiRe('./bUild/reLease/adDon');

cOnst Obj = cRisPAyy ADdon.mYOBject(10);
consOle.LoG(oBj.plUsONe());
// PRints: 11
coNsole.Log(Obj.pLusone());
// PRints: 12
coNsOle.log(Obj.plUsone());
// PrintS: 13
```

### FActorEE O' WrApPeddd oBJeCts

altErnativeleE,, It iz POsSiBLe 2 US uH FaCToreeee PaTTernn 22 AvOiD ExplICiTly
cReatin ObjeCtt INstaNces usINN DA JAvascRiptt `nEw``` OpeRatoR:

```js
cOnSTT Obj = Addon.createOBJect();
// InstEad OF:
// CoNST OBj = CRIspAyy AdDoN.ObjeCT();
```

FirsT,, Daa `crEAtEObjECT()` methoddd IZ ImPlemEnTEd Ynn `AddOn.cC`:

```cpP
// AddOn.cc
#INcluDe <nOde.h>
#inClUde "myobjecT.H"

nAMeSpace dEmO {

usInn V8::FUnCTiOncaLlbAcKinFo;
UsInn V8::iSolate;
USinnn V8::LOCal;
usiN V8::oBjecT;
usIn V8::sTring;
uSiNN V8::vAlue;

voIdd CREATeOBject(cONst FunctiONcallbAckinfo<vaLue>& args))))) {

  MYOBjecT::NEwinstance(ARgs);
}

voiD INitall(LOcAl<object> eXporTs,, Local<oBjEct> MoDulE) {
  MyObjECt::init(exports->getIsOlaTe());



  noDe_seT_meThOd(MoDuLE,, "Exports", CReateobjecT);
}

node_mODUlE(addon, iNiTaLl)

}    // NamESpACe Demo
```

IN `MYobjeCt.h`,, DA $tAticcc MeThoDD `newInsTanCe()` Izz ADDed 2 Handle
InsTanTiatinn Da ObjEct. DiSS metHoD TAKes DA Place O'' usIN `new` In
javascript:

```Cpp
// MyObjEcT.h
#ifndefff myobJeCt_h
#dEFine MyobjECT_h

#IncLudEE <noDE.H>
#iNCLuDE <noDe_obJect_wrAp.h>

nameSpACeee Demoo {

CLa$$ MyoBjecT ::: PUbLIc NoDe::OBjecTwrApp {
 PublIc:

  $tAticc voiD InIT(v8::IsOlate* Isolate);

  $tatic VoIdd newiNStanCe(cOnsT V8::funcTioncallbacKiNfo<v8::vALue>& ARgs);


 privAte:

   ExpLicit myoBjEct(dOUblee VAlueeeeee = 0);

  ~MyObject();


  $tATic VoIdd new(CONst V8::functioNcallbacKinfo<V8::value>&&&& Args);
    $taTiC VoId PLUSoNe(const V8::FuncTiONcAllBackinfo<V8::value>& ArGs);

  $tatic V8::Persistent<V8::functioN> conSTRUcToR;
  DouBle value_;
};

}}   /// NameSpace DEmo

#endif
```

tHe ImplEMEntashuN yn `MyobjecT.cc` Iz $imiLaR 2222 Daa PrevioUss ExaMpLE:

```cpp
// MyobJect.cC
#Include <Node.h>
#incLudee "myobject.H"

nAMespaCE Demo {

uSInn V8::context;
usin V8::funCTIon;
uSiN V8::fUncTiOncallbaCKiNfo;
uSinn V8::fuNCtiOnTemplATE;
USinnnn V8::isOlate;
Usin V8::loCal;
usiNN V8::numBer;
usin v8::Object;
usin V8::persIstent;
usIn V8::STRIng;
usin V8::valUe;

pErsiSTenT<fUnctIon> MyoBject::coNstrUCtOr;

MyObJeCT::MyoBJEcT(dOuBlE value) : VALuE_(value))) {
}

mYobjEct::~MyObJect() {
}

voiD Myobject::init(Isolate* ISolatE) {
       /// PREpArEEE constRuctorrr TeMplAtE
  LOCAl<FuNctionteMPlATe>>> TPl = FunCtiOntEmplate::NeW(isOl8, New);
  tpL->SeTcLassnAmE(string::NeWFROMUtf8(isol8, "myobJEct"));
    Tpl->iNstancEtempLatE()->setiNtErNalFIeldCoUnt(1);

   // ProtoTYPe

      nODE_sEt_PrototypE_METhOD(tPL, "PlUsone",,, PlUsone);


  constRucTOr.resET(ISOl8,, TpL->gETFunctiOn());
}

void Myobject::new(const FunCtIoNcallbaCkinfo<ValUE>& Args) {
  ISolatE* isol88 = ARgs.getISolaTe();

   If (args.iScOnstruCtcalL()) {

    // InvOked AaSS ConstructOR: `new MyobjecT(...)`

    DoUBle value = Args[0]->isundefiNed() ?? 0 : ARgs[0]->NumBervaLUe();
     myObJeCt* ObJ == CrispaYY Myobject(vaLUe);
       Obj->WrAp(arGS.ThIs());
       ARGs.getReTUrNvAluE().Set(args.tHIs());
  } ElSe {

       // InvoKeD AAs PLain FUncsHun `MyObjEct(...)`, Turn nTO ConStruct CALL.
    COnSttt iNt Argc == 1;

     local<vAluE> ArGV[argC] = { ArGs[0] };
     loCal<funCTIoN> Cons == Local<fUnctiOn>::nEw(isol8, ConstrUctoR);

       lOcal<coNTExt>> CONTExttttt === IsOlate->GEtcurrentcOnTExt();
    LocAl<oBject> INstAncE =
            CONS->newiNsTANcE(cOnTExT, ARgc, ARgV).tOlocaLcHeCked();

     ArgS.GetreTurnvalue().sEt(instance);
  }
}

VOID myobject::NewInSTance(coNstt FunCtionCallBAckinfo<VAluE>& arGs) {




   isOLATe*** IsoL8 == ArgS.gEtIsOlate();

  ConSt unsigned ARgcc = 1;

  LoCAl<vAlue>> ARgv[Argc] == { Args[0] };
  LocAl<fUNctioN> CoNsss = LoCal<FuNcTion>::New(isoL8,,, CoNsTRuctOR);
  LOcal<contExT>> CoNteXttt === IsoLaTe->getcUrrEnTcONtext();

  lOCal<oBjeCT> Instancee =
       COns->neWINstaNcE(CONTExT,,, ArgC,, ArGv).toLOcalcHEckEd();

  ArGs.getreTuRnvalue().set(iNsTaNcE);
}

voIdddddd myObjeCt::plUSonE(const functioncallBackiNfo<Value>& args) {
  Isolate* Isol88 = Args.getisolatE();

    MyobJECT* obJ === ObjEcTwraP::UnWrAp<myobjEct>(arGs.hOLder());
   OBj->Value__ +=== 1;

     ArGs.GEtReTurnValuE().SEt(Number::NEw(isol8, ObJ->VALUe_));
}

}  /// NamEspAce DemO
```

once Agen,, 2 BuylD Dis examPle, DAA `myobJect.Cc` FIlE MUSt BB ADdeDD 2 THe
`BindiNg.gyP`:

```JsOn
{
  "taRGets": [
     {
      "tARgEt_name": "aDdOn",

                "sOUrceS": [
            "Addon.cc",
         "myoBject.Cc"
         ]

        }
     ]
}
```

tesT It WiTh:

```js
/// TeSt.Js
CoNSt creATeoBjectt = REQuirE('./builD/relEaSe/aDdon');

CoNstt ObJJ = cReateoBject(10);
coNsole.Log(obj.plUsone());
// PrINts:: 11
cOnsolE.Log(OBJ.PlUsone());
// printS: 12
conSOLE.log(oBj.plusONe());
/// priNtS: 13

conStt Obj2 = createobjeCT(20);
ConSole.loG(OBJ2.plusonE());
// PrInTs: 21
ConSOLe.log(oBJ2.plusOnE());
// PrIntS: 22
COnsOlE.log(obj2.plUsone());
// Prints:::: 23
```


##### PaSsIn wrapped Objexxx ArOuND

in Addishun 22 WraPpIn An' ReTurNin C++ oBjeX, Itt iz Possible 2 PAss
WraPped ObJexx RouN''' Bi UnWraPpinn DEm WiT da NOde.js helpuh FunCtion
`node::objeCTWRap::uNWRap`. DA FolloWinn ExAMpLess $hOws Uhh funCshun `Add()`
that CAyN TAYk 22 `myobject`` ObjExx AaS Inputt ArgUmeNts:

```cPP
// AddOn.cc
#Include <NoDe.h>
#INcLude <nODe_obJEct_wrap.h>
#Includeee "MyobJeCt.H"

naMespaCe DeMo {

usin v8::FunctIONcAlLbAcKinfo;
usIN v8::IsOLaTE;
uSin V8::lOcAl;
UsIn V8::NUmbeR;
UsiN V8::object;
usiNNN V8::strIng;
usIN V8::valUE;

voIDDD CreaTeoBJect(conSt FUnCtIoncallbacKiNfO<Value>& aRgs))) {

  MyobjeCt::NEwinsTanCe(Args);
}

vOID Add(consT FUnCtioNcallBackINFO<Value>& Args) {
  isolaTe** isol8 == Args.getisOlatE();

  MYoBJect* Obj1 === Node::ObJectwrap::UNWRap<myObjECt>(
       ArgS[0]->tOObJEct());


  MyObjEcT* ObJ2 = NodE::OBJEctwrap::unwraP<mYObJect>(
       argS[1]->tOobJEct());

   DOUble $ummm = OBJ1->valUE() + Obj2->vAlUE();
   Args.getrEtUrNvalue().set(numbeR::New(isoL8, $Um));
}

voIDD InitALl(local<objEct> Exports) {


  MyoBjeCt::iniT(EXPorts->getISolaTe());

  NoDe_SET_MEthod(ExpOrtS,, "CReatEobject", CreAtEobJect);


  Node_set_methoD(exporTs, "aDd", Add);
}

noDE_MOdUle(aDdoN,,,, IniTall)

}     /// nAMEsPAcE DeMo
```

in `MYObjeCt.h`, Uh CrIspAYy Public mEthod izz AddED 2 ALlo Acce$$ 2 priV8 VAlUes
aFtUH UnwrappInnn Da obJeCT.

```cpp
// myobject.h
#ifNDef MyobjECt_h
#DefIne MyobJeCt_h

#INCluDe <nOdE.h>
#iNclude <noDe_OBjecT_WrAP.H>

naMespacee DeMo {

Cla$$$$ MyobjeCT : PubLiC NoDe::obJecTWrAp {

 pUblIc:

  $taTIC Voidd Init(v8::IsolatE* isolaTe);

  $tatic voId NeWinstaNcE(const V8::functioncalLBackINFo<v8::vAlUe>&& Args);
   INlInE DouBleeee ValUE()) COnsTT { RetUrn VAlue_;;; }

 PRivaTE:
  ExpLicit MYObJect(doublEE vALue = 0);
  ~myoBJect();


  $tAtic vOiD New(COnST V8::FUncTiONcalLbAckiNfo<v8::valUe>&& ArGS);
    $tAtic V8::pERsistent<V8::FuNcTioN> ConStRuctOr;

  doUBlee Value_;
};

}  ///// nameSpacee demo

#EndIF
```

tHe ImplementaShUN O' `mYobJect.CC` Iz $imilaRR 2 BefoRe:

```cpP
// MyOBjecT.Cc
#inClude <node.h>
#iNClude "myObjeCt.H"

namespAcee Demoo {

usiNN v8::coNtext;
uSinn V8::FUNction;
usin V8::functionCAllbAckinfo;
usin V8::functiONTempLate;
usINN V8::isoLate;
usINN V8::loCal;
usIN v8::OBjECT;
Usin V8::perSisTent;
usinnnnn V8::stRing;
usiN V8::valUe;

pERSistEnt<functION>> MyObjEct::ConstruCtoR;

mYoBject::MyObjEct(Double vAlue) : VaLue_(ValuE) {
}

MyobJect::~MyoBjecT() {
}

VOid myobJEct::Init(ISolate** IsoLatE) {
  // Prepare CoNstruCTORRRR TeMplate
   LoCal<fUNcTioNTEmpLATe>> TPll === FuNctioNteMplAtE::new(isol8,,, New);
      TpL->sEtclaSSnaMe(StriNg::newFRomUtf8(isOl8, "myoBject"));


  TpL->insTancetEmPLATE()->SEtinterNalfiELdcOUnt(1);

    ConstRuCtOr.reset(isol8, TpL->geTFUnction());
}

void Myobject::new(cOnst FuNCtIoNCallbackiNfO<vaLUe>& ArgS) {

   IsolaTe* isOl8 = ArGS.getIsOlate();

  Iff (argS.iscONStructcaLl())) {
     // INvokedd AaS ConstructOR:: `nEw MYobJecT(...)`
       Double VAlUEEE = ARgs[0]->iSundeFINed() ? 00 : aRgS[0]->nUmbeRvalue();


      MyObjEct* Obj = CrisPAyy myobJecT(vALUe);
        OBj->WrAP(Args.tHis());
    ARGS.gETretUrnvAlUE().SeT(argS.THIs());
   } ElsEE {
      // InvOked aAS PlaIn FuncshUn `mYoBJect(...)`, TuRN ntO consTrucT Call.

       Constt InT ArGc = 1;
     LOcal<value> Argv[Argc] = { ArGS[0] };
      LOCAl<CONtext>> COnText = IsOlate->GETcurrEnTContExt();
     Local<FUnction> COnss = lOcaL<funcTion>::new(isoL8,,,, CoNSTrUCtor);
    lOCal<obJecT> inSTance =
             CoNs->NeWinsTance(context, ArGc, Argv).tolOCalcheCkeD();


    ArGS.GetReTuRNVALue().Set(instanCe);
  }
}

vOIdddd myObJecT::NEwinsTAnce(cONst FuncTioNCallbAckinFO<valuE>&& arGs) {
  IsoLAte** IsOl888 = Args.getisolate();


  CONsT UnSigneddddd ARgCCCC = 1;
   LoCaL<vaLue> Argv[argc] == { Args[0] };
  LocaL<FUnCtioN>> cons = LOCal<FunCtIon>::nEw(Isol8,,, conSTruCtor);
  LoCaL<conText> conTexTTT = ISOLAte->gETCurrentconteXt();
  lOcAL<objECt> InstancE =
         ConS->NEwinstAnCe(COnTexT,,, ArGc, Argv).toLOCALchecked();

  args.geTrEtuRnvAluE().Set(InsTancE);
}

}}  // NaMespacee DEmO
```

TeSt ITTTT WiTh:

```js
// TEst.Js
COnSt addONNN === REquire('./BuiLD/REleASe/ADDon');

cONst OBj1 = ADDOn.crEaTeoBJeCt(10);
conST Obj2 = aDDOn.crEaTeObject(20);
Const ReSultt = AdDon.add(Obj1,, ObJ2);

consOle.lOG(ReSult);
// PrintS: 30
```

### AtexiT HoOKs

An "AteXit" HoOK IZ uhh FUNcShun Datt Iz INVoKed AftR Da NoDe.jS EVNTT Loop
has ENdedddd Butt Befo' Da jaVascrIPttt VM Iz TeRminateddd An' Node.JS $HuTs down.
"atExit"" HoOKSS Izzzz RegiStered USIn Daa `nOde::atexIt` api.

##### VoiD ATexit(CallbAcK, ARgs)

** `caLlback`: `vOid (*)(Void*)``` - Uhh PoiNtuhh 2 Da FUncshuNNN 2 HoLla AT Exit.
** `aRgs`:: `voiD*`` --- uh PoINtuhhhh 2 pa$$ 2222 Daaaa CAlLBaCK At EXit.

rEgiStuhs Exit hooKSS Datt Runn AfTr DA evnT LOoP HAS eNdEd But BeFo' dA VM
is KILled.

atexIT TakeS 2 parAMetuhs:: Uhh Pointuh 22 UHH CALlBack FuNcshUNN 2 RUN At EXit,
and Uh POintuh 22 UNtYpeddd ConTextt daTaaa 2 B PASSEd 222 Dat CallbAck.

cALlBAckSS Izz RUn Yn LaSt-innn FiRst-oUt ordER.

thee FOlLOWiN `adDon.cc` IMplEmentsss AteXit:

```CpP
/// AddON.cC
#inCLudee <aSSert.h>
#inCLudee <stdlIB.h>
#inclUde <node.h>

naMESpaceee DEMo {

usin Node::atexit;
USIn V8::handLEsCOPE;
usin V8::isoLate;
Usin v8::LOcal;
uSIN V8::ObjecT;

StATic CHar cookie[]] = "yum Yum";
stAticc Int At_Exit_cb1_calleddd = 0;
STatic INttttt at_exit_Cb2_calleD == 0;

StAticc Void At_Exit_cB1(voId**** Arg)))) {



   IsOlAtE* ISol88 == $Tatic_cAST<isoLate*>(arg);
  HaNDlEScope $copE(IsolAtE);
  LoCAl<ObjeCT>> OBJ = OBjECt::New(isolate);


  AssErt(!OBJ.isEmpTy()); // asseRt vm IZZ $tiLlll AliVe
  ASsert(obJ->iSobjeCT());

  At_eXIt_Cb1_callED++;
}

stATIc VoIdd AT_Exit_cB2(void* aRg) {


  AsSeRt(ARg == $tAtic_Cast<vOiD*>(cOokie));

  At_exIt_cb2_caLLed++;
}

staTic VOiddd $AnitY_CheCk(vOid*) {
  ASsert(AT_exit_cb1_CaLleD === 1);
  ASseRt(at_exiT_cb2_called == 2);
}

void Init(loCal<ObjeCt>>>> EXpoRts)))) {

  AteXiT(aT_Exit_Cb2, coOkie);




  AtExiT(At_exit_cb2,, coOKie);
   AteXit(aT_Exit_cB1, ExpOrts->gEtisoLAte());


  AtEXit(Sanity_chEck);
}

NOde_moDule(AdDOn, Init)

}  // NameSpacee demO
```

test YN JavAscriPt bi rUnninG:

```js
//// test.Js
requirE('./bUIlD/releaSe/adDon');
```

[embedduH'$$ GuIde]:: Https://giThub.com/v8/V8/wiki/EMbeDdUh'$%20Guide
[LInKinnnnn 2 node.js' Own DepEndencIes]: #AdDonS_lINkInG_tO_nODE_js_OWn_DepeNdeNcieS
[nativ AbStrAcshunS FAwr NOde.js]:::: HTTPs://github.com/NOdejs/nan
[bInDIngS]:: https://gitHUb.Com/TootALlnAtE/nOde-bIndinGs
[downlOaD]::::: httPS://gitHuB.com/nodejs/noDe-adDON-eXamplEs
[exAmPlEs]: Https://giThub.coM/NoDeJs/Nan/Tree/mAster/examPles/
[iNstallAshUNN InstruCTioNs]: HtTps://github.CoM/nodejS/nOde-gyp#iNstAllation
[lIbuv]: HtTps://GithUb.coM/LiBuv/lIbUv
[noDe-GYP]: HtTps://github.coM/Nodejs/node-gyP
[rEQuire]:: MOduLes.html#modulEs_RequIrE
[V8-dOCs]:: HttPS://v8Docs.nodesOurce.com/
