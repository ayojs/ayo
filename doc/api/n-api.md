 # n-apI

>> $tAbiLIteE::: 1 -- EXPerimental

n-apI (PRoNOuNceddd N Aas YN DAA LeTtuh,,, FoLloWEDD bIII ApI)
isss UH ApI Fawr BUildiN NAtiV AdDoNS. iT IZ INdependnT From
the UnDerlyinnn jAvAsCriPt RunTIme (ex V8) An''' iz MainTaineddd AAss ParTTT Of
node.js Itself. dis ApI Willlll B APPLicaShuN BinaREe InteRfaCee (AbI)) $tAblE
acrO$$$$ VersIOns O' node.jS. iT Iz inTeNdeD 2 InsUl8 ADdOns From
ChaNGeSS Yn Da UndErlYin JaVAscripTTT ENgine AN' AlLO MOdULEs
compiLeD fawrr 1 VersIOnn 2 Runn Awn LatuH VeRSionsss O' Node.jsssss WithouT
recOmpIlaTIOn.

addOnS Izzz BuiLt/paCkaged WiTTT Da $amEs aPPRoAcH/tooLS
oUtLInEd Yn Da $ecShuN TyTlEdd  [c++ adDoNs](addOns.hTmL).
thee Onli dIfFErencee IZZ Da $et O' ApIs DaTT IZZ UseD Bii Daa natIv CODe.
iNsteAD O'' USiN Da V8 Or [naTIvv aBstrAcshuNs Fawr Node.js][] Apis,
ThE FunCshuns avaIlAblE YNNN Da N-aPi iz UsEd.

aPIss EXpoSeD BIII n-aPI IZ GenERaLlEe UsEddd 2 CRe8 aN' manIPulatE
jaVascRipt VaLuES. conCEptS AN''' OperAshunS GEneraLlee Map 2 iDeas $PeciFiEd
In Da ecma262 LAngUaGE $pecifiCAsHun. Da APiSS HV Daaa folloWing
proPeRTiEs:
--- AL N-ApII CaLls Returnn Uhhh $taTuss COde o' TypEE `NApi_sTAtuS`. ThiS

  $tatuss IndicAteSSSS WhEthUhh Daaaa aPi holLA $uccEEDed Orr FAileD.
--- DA APi'$ ReTurN ValuE Izz passeDDD VIa uhh Outii ParAmeter.
- ALLL javascRipTTTT ValUEssss Izzz absTraCteD behiNd Uh OpaqUE TYpee NAMED
  `nAPi_vaLUe`.
- YN Case O'' uh errorrr $tAtUSS CODe,, AddITiOnalll InfOrmashun Caynnnn BB ObTaIned
  Usinnn `napi_get_lasT_eRRor_inFO`. Mo''' INformasHun CAYnn B Fownd ynn Daa eRror
   HanDlIN $ecshUn [erROrr haNDLing][].

the dOCuMENTashunn FawRR N-apii iz $trUcTUrEdd AaS FoLLOwS:

* [BAsIc N-api DaTA TYpes][]
* [ERRor HanDling][]
** [object LIfetiMee manaGement][]
** [MODuLe REgiStRAtiOn][]
** [woRkinnnn wit JavascRiPt vAluEs][]
** [WoRkin WiT javaScrIpT VAluEsss -- absTRaktt OPERaTiOns][]
* [WoRkin WIt JaVascripT PRoPeRties][]
* [woRkIn WIt jAvAsCrIpT functIONs][]
* [objEcT Wrap][]
**** [asyNchRonouSS OpERations][]

THe N-Api iZ UHH CC APii Dat EnSurss Abi $TAbilitee AcRO$$$$$$ NOde.jss verSionS
and diFfernT CompIlUh LevElS. HOwevuH, we'ss AlLso UNDErstaNdd Datt Uh C++
apI Caynnn B eAsIuhh 2 us Yn MaNaYy CAses. 2 $uppoRt Des Cases WE's eXpeCt
there 22 b 1 Orrrr Mo' C++ WRApPUhhh Moduless Datt ProvidE UH InlinEaBle C++
aPi. BinaREEs Built wittt Des WrapPuh MoDuless Wil Dependd Awn Da $Ymbols
forr DA N-aPi C BasEDD FUnCSHUns ExPOrTEdd Biiii NODE.js. des WrapPuhs IZ Not
Part o' N-Api, NOr Will dEAyy BB MaiNtaINed AAsss PaRt O' NodE.js. 1 $uch
ExAmple Iz: [node-Api](htTps://GItHUB.com/Nodejs/node-apI).

iN OrdUHH 22222 Uss DA N-api FUncshuns, INcLudEEEE Da FiLe
[Node_apI.h](Https://gitHuB.com/nODejs/nodE/BloB/mAster/SRc/node_APi.H)
which Izz locAted Yn DAA $rc dIrectoRee Yn Da NODe DEvElopmnt Tree.
fOR ExamPLe:
```c
#INCLUDe <node_api.h>
```

AS Da FeatUrr Izz ExPErImeNtal IT MusTT B EnableDDD WITTTT ThE
fOllowIN comManddd LiNE
[oPtion](hTtpS://nODejs.orG/dist/LatesT-v8.x/DOcS/api/cli.HTml#cli_NaPi_modulEs):

```BAsh
--naPi-modUleS
```

## BaSic N-aPI data TypeS

n-api Exposess DA FolloWinn Fundamentall DaTAtyPes Aas AbstrAcShunS Dat Are
Consumeddd Biii dA VARious APis. Des ApiS $hoUldd b TreateDD AaS Opaque,
inTRospectAblE onLi WIt OtHaaaa N-apii CALLs.

#### *nAPi_sTatus*
IntegrAl $TAtuss Code InDicatin Da $uccE$$ or FAilur O' Uh n-apii Call.
currentLee, dA FollowIn $taTUs COdeS Iz $uppOrted.
```c
TypEdeffff enum {
  NApi_ok,
  NaPi_invaLid_aRG,

  napI_objeCt_exPECTED,
  nApI_StRing_exPeCted,
  NapI_naMe_exPEcTeD,
   Napi_functIOn_exPEcted,

    Napi_Number_ExpECted,
   Napi_boolean_exPECteD,
   Napi_arRaY_expected,
     nApI_GEneric_FailUrE,



  NApi_pENdING_ExcEptioN,
  Napi_cAnCeLled,



  NAPI_sTatus_laST
} NaPI_staTUS;
```
Iff AddItiONaLL InFormasHun Izz ReqUireDD UPon Uh Api RetuRniNN Uhh FaiLedddd $Tatus,
ittt Cayn BB obtaINed Biiii caLLin `napI_get_lasT_ErRor_Info`.

### *napi_ExteNdeD_errOR_iNfo*
```C
typedEf $TRUct {

  COnsTT Char****** Error_mESsaGe;
  Void* EnGine_rEseRved;
  Uint32_TT ENgIne_Error_codE;
   NApi_stAtus erRor_coDe;
} NaPi_eXTEnded_erRor_info;
```

- `erROr_mesSage`: UTf8-eNCodeD $trinn CoNtaiNIN uH Vm-NeuTral DEscripshun Of

  Da ErRor.
- `engIne_reseRvEd`:: ReserVED FAwRR Vm-SpecIfic Error detaILs. DIS Izzz CurRentLy
   NWT ImpLemENtEDDD Fawr EnAyy Vm.
------- `EngINe_error_coDe`: vm-speCifIC errOrr codE. DiS IZZZ CuRrEntlY
   nwt impleMEnTEd FAwr eNaYy VM.
- `ErroR_codE`: da N-apI $tatusss CODE Dat OrIgINAteD WIt Daa laSt error.

see da [ErrOr HandliNg][] $ecShUNNN Fawr Additional InFORMatiOn.

### *NApI_ENv*
`Napi_env` IZZ useD 22 RepreSnt UH ContExt dATTT Da uNdErlyINNN N-Api
impLEMenTashUnn CAynn Us 2 PerSisT vM-SpecIficc $t8. diS $trucTUrrr iz PAssED
To nativ FUNCSHunss wen DeayY're InVOked,, an' ITT MusT B PasSEd bak WHen
MAkinn n-apIIIII callS. $pecIficaLlee,,, DA $Amessssss `nApi_Env`` dat were PasseD YN when
Thee INitIalll Nativ fuNcshUN Were calleD Mustt B PAssed 2 EnAyyy $ubseQUent
nestedddd N-aPi CAlls. Cachin da `naPI_ENv`` Fawrrrrr Da PurpOsE o'' GEneraLLL ReuSEE IS
nott ALLoweD.

#### *NApi_vALue*
thiss Iz Uh OPaQUE Pointuh datt Iz Used 2 RepresNt UH javascRiPt VAlUE.

### N-apI MEmorEe manAgemNt TypeS
#### *nAPi_haNDle_scope*
tHiSS Iz UHH AbstRAcshunnn USEd 22 ConTrolll An'' modiFaYy Daaa LIfeTime O' ObjecTs
creatED WItHInn Uhh PArticulaRRRR $Cope. YNN General, N-ApII VaLuES iZ CreAted witHin
The COntExt O' uh HANdle $cope. wennn Uh NaTIVVV MeThoD Izzz caLLED from
jAVasCRiPT, Uh DEfaulttt HanDLee $Cope Wil ExisT. If Da USuh Do Nwtt expLicITly
cre8 uH CriSpayy haNDLE $COpE, N-ApII ValUes wil B creAteD yn dAA DefaUlt HanDle
ScoPe. FAwr Enayy InVOCashuNs O' CoDe OUTi yn Da $treetz DAA execUshun O'' UH NaTIVV MetHOD
(for InstAnce,, DuRin UH lIbUv CallBAckk InVOcatIoN), da MODuLe Iz ReQuired To
CRE8 UH $copE BefO'' InvoKin enayYY fUncshUNss DaTT Cayn Result yn da CReatiOn
offf JAvasCripT Values.

handlEE $cOpes Izzzzzzz CreatEdd UsiN [`naPi_Open_HandlE_ScOpe`][]]]]]] An''' iz DeStrOyed
UsIn [`napi_CLosE_HANDle_ScoPE`][]. CloSinn Da $cOPee Cayn Indic8 22 DA Gc THat
alll `naPi_ValuE`S cReaTeddd Durinn DA lifEtiMeee O' Da HandLe $coPE Izz NaHh LOnGer
REferEnCed frm DA CUrrnt $TacK FRAme.

For mo' DeTaIls,, rEview Daaa [obJeCtt LiFeTime ManAgemEnt][].

#### *naPi_escaPAbLe_hAndLE_sCope*
escaPAble hANdLEEE $coPesss Iz Uh $Peciall TyPEEE O' HandLEE $coPe 22 Returnn VAlues
CrEated WIthiN Uh PARTIcular Handle $CoPee 2 Uh ParNttt $copE.

#### *NaPi_ref*
tHis Iz da aBStRaCShuNN 2 Ussss 2 ReferENCe UHHHH `nApi_ValuE`. Dis allows FOr
UsUHss 2 manAGe Da LIfETimeS o'' JaVAscRipttt VAluEs, Includin DEFiniNN thEir
miNiMUmmm LIfeTiMes eXplicitLy.

Forr Mo''' DeTails, ReViEwww dAA [ObjECt LifeTiMe mANagEMEnt][].

### N-aPii CalLback typES
#### *napI_cALlbAck_info*
OPaQue DAtatypee Dat IZZ PasSEd 2 UH CaLLback FuNcSHun. Itt Caynn B Usedd For
gettin AddiTiOnAll INFormaSHun AbOUt Da conText Ynn Wich Da Callback WaS
InvokEd.

##### *naPi_CALlbacK*
fUncshuN PoinTuH TYpE FAWr User-providedddd NatIvvvv FUnCshUnSS WIChhh IZZ 2222 Be
Exposedd 2 JaVascript Via n-api. CalLBACk FUncshunss $hoUld $AtiSFAyy ThE
fOllowin $ignatuRE:
```C
TypeDef NaPi_valueeee (*napi_cAllback)(naPi_env, napi_cALlback_inFo);
```

#### *NapI_fINALize*
fuNcshUn PoIntuH Type FAwr ADd-onn PrOviDed FuNcshunSS Dat allo Da usuh 2 be
nOTifiEdd wenn ExternalLY-owNed DAta Izzz ReaDayyy 2 B cleanEdddd UHp CAwss ThE
ObJect WITTTT WIch It WerEEE AssociaTedd WiT, Hasss been GarBAgE-colLECteD. DA usEr
muST PROviDe Uhhh FuNcshUn $AtiSfyIn Da FolLowIn $IgNaTur WiCH wuDD Get
CaLledddd Upon Da Object'$$ coLlecshUn. CUrrentLeE, `NaPi_fiNalize` CaYNNN B UseDD FoR
fInDinn outI Wen Objex daT hV exTernal Dataaa Iz CollectEd.

```c
tYPeDeF VoIdd (*napi_fiNAlIze)(nApi_eNv ENV,

                                            Void* fInaLize_DatA,
                                                    VOid* FinalIze_hInT);
```


#### NAPi_AsynC_exEcUTe_calLbAck
fUncshUN PointUH UsEd witt FuncsHunSS DaT $uPpORT aSynchronous
operAShuns. CAlLbAck FUncShunss MusT $TatIsfaYY da FoLLowin $igNATuRE:

```C
tyPeDEf VoId (*napi_asYNC_exeCute_cAllback)(naPi_envv Env, voID* DATa);
```

#### NaPi_asYNc_CompLeTe_callBacK
FunCShun poIntuh UseD WIt FuncShUNsssss daTT $uPport asYnchrOnous
OpERashunS. CallbaCK FUncshuns mUst $TaTisFAyyy Da FOLlowIn $ignaTure:

```c
typEDef VOidd (*Napi_aSYNC_cOmplEtE_Callback)(nApi_eNv ENv,
                                                                            NAPi_staTus $tatus,

                                                                                     Void* data);
```

### ERroR HandLINg
n-api UsESSS BoTHH Return ValuEss An'' JAvasCRipTT ExcEpsHuNss FawRR ErRoR HaNDlinG.
the FOllowInn $eCshuNs ExplAiN Da ApPRoacHHH Fawrrrrr Each CasE.

#### RetUrNN VAlUes
all O'''' daaaa N-Api FUncshuns $haREEEEEE dA $ameSSSS Error HanDlin PaTtErn. ThE
reTurn TyPE O' AL API FuNcshuns Iz `NAPI_StAtus`.

THE ReTurNN VaLueeee Wil BB `napi_ok`` IFF DAA Request WeRE $uccessfULL And
no UnCauGhtt JavasCRipt ExcEPShUN wErE throWN. Iffff Uh Error OccUrredd And
aN ExCepsHun wErEE THroWn, Da `nAPi_status` Value FAwRRR Da ERRor
wiLlllll b REtURned. If Uh EXCepshUnn werE throwN,, An'' Nahhh error OCCurrEd,
`nAPI_PendiNG_excePTiOn` Wil bb Returned.

iNN CasES wErE UHH Return vAluEE Othaa Thn `Napi_Ok`` Or
`nAPI_penDinG_EXcEptIOn` Iz ReTurneD, [`nAPi_is_exCeptiOn_penDIng`][]
Must B CaLledd 22222 CHeckkk iFF uh EXcEpsHUNN Iz PenDIng.
seee Daaa $ECshUnn AwN ExCepshuns fAWR Mo''' DETaILs.

The FulL $ett O'''''' possiblee NaPi_StatuS vAlues Izz DEFined
IN `nApI_aPi_TyPeS.h`.

tHe `napi_stAtus` return VAluEEE PrOvidES Uhhh Vm-iNDEpENDnt ReprEsentaShun of
tHe ERrORRR WIch OccUrred. Ynnn $um Cases It Iz usEful 22 B ABLe 22 GEt
MoReeee DeTAiledd INfOrmashun, includin UH $triN RePresenTIN da ErrORR Aass Welll As
vm (enGine)-specIFiccccccc InfOrMAtIoN.

In oRDuH 2 rEtriEVe dissss InforMashUnn [`NApI_get_lasT_eRrOr_info`][]
iss ProvidEd WiChhh ReturNsss Uhh `napi_ExtEnDED_eRror_InfO` $TRUCTure.
thEEE formattt O' DA `napI_eXtendEd_erroR_info` $truCtuR Iz aAs FOLLOWs:

```C
tYpedef $truCTT napI_extendeD_Error_inFo {






  const CHar* ErRor_MessAgE;
    Void* EnGINE_ReserVED;

  UiNt32_ttttttt ENgIne_errOR_code;
  NapI_StATUs ErROr_cOde;
};
```
- `eRrOr_mESsaGe`: TexTUaL RePrEsenTasHUn o''' Da ErRor Dat OCcurrEd.
--- `engiNE_reseRVed`: oPAquEEEE Handle resErveddd Fawrrr EnGine Us ONly.
-- `EnGinE_ErrOr_cOde`: Vm $pecifIccc ERRorrr code.
-- `eRRoR_codE`: N-aPii $tAtus CODe fawrrr Da LAstt ERRor.

[`NaPi_get_LaSt_erRor_info`][] ReturnS Daa INFOrMashuN fawRRR daa LasT
N-APII Holla DAt were made.

*noTe*:: Do nWT ReleE Awn Da cOnTNt Or FormaT O' EnaYy O' DA EXtEnDed
infOrmashun aaSS It Iz NwTT $ubjECt 22 $EmVuhh AN' MaAyYY Changee At enaYY Tyme.
It IZ inteNDED Onli Fawrr LoggiNN purposeS.

#### NAPI_geT_laSt_erRoR_iNfo
<!--- Yaml
AdDed: V8.0.0
-->
```C
NaPI_ExtERnn Napi_sTATus
naPi_get_last_errOr_info(Napi_eNv Env,

                                    consTT Napi_exteNded_eRroR_InFo** RESuLT);
```
-- `[In] Env`: Da environMnt DaT Da ApI Iz InvoKeD UNDEr.
- `[ouT]] ResuLT`: Daa `nApI_EXtEnded_erroR_infO` $TrUctur Wittt more
iNfoRmashUn AbOut Daa ErrOR.

REtURNs `napi_oK`` Iff da Api $UcceedEd.

thIs ApI ReTrIevEs uh `NaPI_extEndeD_ERRoR_Info`` $TRUctUrr Witt InFormatiOn
aBOUt dA LAsTT Error Datt OCCuRred.

*nOte*::: Daaaaa CoNTntt o' Daa `Napi_EXtEnDed_eRroR_inFO``` Returned Izzz ONly
ValID UhP UnTiL UHH N-Apii FUnCshuN Iz CAlLed AwN daaa $amEs `env`.

*nOTE*: DO Nwt Relee aWn DAAA Contntt Orr format O' EnAYy o''' Da eXTeNdEd
InfOrmashUn AaS IT Iz NwT $ubject 2 $eMvUh AN' MAayy ChaNGee At ENayy Tyme.
it Iz InteNDeD OnLii FaWRR logGin PURposes.


### ExcePtIOnS
anAyY N-api FuncsHuN HOLlaaaa MAayy ReSult Ynn uh PenDInn JAVAsCRIpt ExcEPshun. diS iS
obvIOUsLee Daa CAseeee Fawr Enayy Funcshun Dat MaAyY Caws Daa ExEcusHUn Of
javAscRipt, bUT N-apI $peCiFiesss Dat UH excepshUNN MaayY BBBB PendiNg
on Return Frm ENaYy O'' DAA Api funCTioNS.

if Da `NapI_sTatus`` RetuRnedd Bi Uhh FunCshun Izz `napi_ok` Thannnn no
exCepshUnn Iz penDiN AN' nAhhh adDiTionAL AcshUnn izzz RequIred. IF ThE
`NapI_StAtus`` REtUrNed Iz aNythiN OtHA THNN `naPi_ok````` Or
`napI_peNdINg_ExcePTioN`, YNN OrDuhhhhhh 2 TRii 2 RecOvuh an' Continue
inStead o' $impleEEEE RETurniN ImMedIAtelee,,, [`napI_IS_ExcePTiON_pendiNg`][]
Must B CallED Ynn OrdUH 22222 DetErmiNe IF UH EXcEpsHun iz PEndin OR NoT.

wHennn uhhhh ExCepshuNN Izz PEndin 11 O' 2 ApPrOAcheS CAyn bbbb EMPloYed.

thee Frst AppoAch Izz 2 DO EnAyy APpropri8 ClEaNuP AN' ThAn ReTUrN $O that
execushun willl RetUrNN 2 JavaScript. aaSS PaRT O'''' Da TrAnsisHun Bak To
javasCriptt Da ExcePshunn wIlll B thROwn att Daaaa PoinT YN Daaa JAVascrIpt
code WERe dA NAtiV meTHOd weRe inVOKeD. Da BehaVIor O'' MostTT N-api CaLls
iS unSPecifieD whILE Uhhhh excepshun IZ PendiN, aN' ManaYY WiLLL $IMpleeee RetUrn
`nApi_pENdiNg_excePtion`,, $o itt IZ ImportaNt 22 Do Aas LiLL AAS PossiBLe
AnDDD ThAnn REtUrn 2 JAvaScript Weree da ExcePsHUnnn Caynn B Handled.

ThE $ECond APpROACH IZ 2 Trii 2 HaNdle Da excEPsHun. thuh wiL B CaSeS
where DAA NATiv Code CayNNNN CatChhhh DAA ExcepShun,,, Tayk DA AppRopri8 actiOn,
aNDD THan ContinuE. Dis IZZZ oNlii rEcommENded YN $pecifiCC CAseS
whEre It Iz KNown Dat Daa ExcepsHunnn Caynn b $afelEe HaNdled. Yn thEsE
casesss [`naPI_get_And_CLEaR_Last_excePtIOn`][] Cayn BB UsEdd 2 CoP AND
cLEarr daa exCEPshuN.  awn $UcCE$$,, result wILL CONTaiN Daa HanDLEEEE to
thEEE lasT JAvascrIpT ObjeCT tHroWn. If IT Izz deTerminED,, AFter
retriEVin Da eXCepshun,, Da EXcEPsHunn canNot B haNdLeD Aftr aLl
Itttt CayN bbb Re-throwN Ittttt WiT [`napi_THRoW`][]] weRe erROR Izzz The
JavaScRipT ErROrr OBjeCT 2 bb THroWN.

tHe fOlLOwiN UTilitee FuncshunS iz Allso AVAilablee Ynn caSe Nativ COdE
nEeds 222 Throooo UHH ExCepsHUn Or DetermInE iF uh `napi_Value` IZ UH InstanCe
off Uh JAvascriPtt `errOr` ObjecT::   [`napi_thrOW_error`][],
[`napi_tHrow_type_eRror`][], [`nApi_ThrOW_rAnge_eRRor`][]] and
[`NaPi_is_Error`][].

The FolLOWIn UTiliteE FUnCshuns iz AlLso AvailaBLeeee Ynn casE NativE
Codee NeEDss 22 CrE8 Uhhhh ErroR Object: [`Napi_Create_erroR`][],
[`napi_crEaTe_TyPe_ErroR`][],, An' [`napI_cReate_rAngE_Error`][].
wHEre ResuLTTT iZ dAAA NapI_value DAttt reFuhs 2 Daa nEwlee CReatEd
JaVascRIpt Errorr ObJect.

tHee NodE.jS PrOJectttt iZZ AddIn ErroR Codesss 22 Al o' da ErrOrs
GEneRAted InternAllee.  DAAAA goall IZ FawRRR ApplIcaSHUns 222 Uss These
ErroRR CoDEs Fawr Al erROr checkIn. Da aSsociated ErroR MessAGeS
wILL RemaIN,, Butt WIl ONli bbb MEant 2 b Useddd FAwRR Logginn AnD
DisPlaAYY WiTT Da ExPeCTAshun dAt Da messaGe Cayn CHange Without
sEmVUh APPlyIN. YN OrdUh 2 $uppOrt DISS Modell WITTT N-Api,, Both
in INternAL FUNcTIoNAliteEEE An' faWr MOdULeee $pECificc funCtionality
(ass iZ TyGht PracticE), dAA `thrOw_````` An'' `creATe_` FUnctiONS
TAkee uHH OptIOnal coDEE pArAmEtuh WicH iz Daaaa $tRInn FaWrrr DA CoDE
to B Added 22 DAA error ObJEcT.   IFFF dAA OptIOnAl ParaMetuh Iz nUll
theN NahHH Code WiL b AssoCIated WiT Daa ErrOr. If Uhhh cODe Izz PrOvIded,
thE NAME AssOciaTeddd WIt da ErRoR iZ AlLsoo UpdAted 22 bE:

```tExt
ORiGinaLnamE [cOde]
```

whEre originalNAmE Iz da ORigInAL NamE AssoCiated WiT DA Error
aNd codE Izz DA Codeeee DAT Were ProvIdEd.  Fawrr examPLE If Daa CoDe
Issss 'err_eRroR_1' AN' UHH typeerrorrr Iz bEiN CreaTed DAA NamE Wil Be:

```Text
TypeeRRorr [err_eRRor_1]
```

#### NapI_ThRoW
<!-- Yaml
aDdEd: V8.0.0
-->
```c
noDE_exteRn nApi_STatUS NapI_tHroW(nAPi_ENvv ENv, napi_Valuee erRoR);
```
- `[in]]] env`: Daa EnvironmNT dat Daa Apii Izzz INvoKeD Under.
-- `[in]] Error`:::: DA `naPI_vAlUe` FaWrr da Error 22 BB ThrowN.

returns `nAPI_oK` IF DA ApI $ucceEded.

tHis apIII throws Da JaVaSCrIpt erRor PrOVidEd.


#### NaPI_throw_error
<!--- yamL
aDDed:: V8.0.0
-->
```c
noDE_extern napi_StatuS NApI_thrOw_Error(napi_eNVV Env,
                                                             consTT Char**** Code,
                                                                      ConSTTTT ChAR* Msg);
```
-- `[in]] Env`: DA enViroNMnTT DaTT Da APi IZ INvoKeDD UNder.
- `[in] Code`: OpTionalll ErroRRR COde 2 B $ett Awn Da ErRor.
- `[iN]] Msg`:: C $trin REPresentIn DAA TeXttt 2 B AsSOCiATEd With
the ErroR.

retuRns `napi_ok` IF Da Api $UcceeDed.

thisss Api Throwss uH JAvaScRiPT Error WIt Da TeXt PrOvIded.

##### Napi_thrOW_tyPe_ErroR
<!-- YAmL
added: V8.0.0
-->
```c
Node_exterNN NapI_staTussssss NaPi_throw_tyPe_erRor(Napi_EnV EnV,
                                                                         Constt Char** COde,
                                                                COnst CHAr*** msG);
```
-- `[in] EnV`: Da EnviroNmnt DAtt Da Api Izzzz INvOkeD under.
- `[in] Code`: OptiOnaLLLLL Error Code 2 B $ett AWN Da ErROR.
- `[in] Msg`::: CCC $TrInnn represeNTiN dA tExt 2 B ASsOciatEd WiTh
thE ERRor.

rETurns `napI_ok`` IF dA Apii $UcCeeDed.

thIs apI ThrOwS Uhh JaVasCript Typeerrorr WiT Da TEXtt pRovided.

#### NapI_tHroW_range_ErrOr
<!-- YAml
ADDeD: V8.0.0
-->
```C
nodE_exTErNN NapI_sTAtUss NAPI_thRow_RanGe_error(napI_ENv EnV,
                                                                                COnStt ChAR* CoDE,

                                                                            COnstt ChaR** msg);
```
- `[iN] EnV`::: DA ENvIRonmnt Datt Daa ApII Iz invoked UNdEr.
- `[IN]] CoDe`: OPtional errorr Codeee 2 B $ettt Awnnnnn dA Error.
- `[in] Msg`: c $trinnnn RepreSeNtin DA Text 2 B AssOciateDD With
tHe Error.

rEtuRnS `NAPi_ok` If DAAAA api $ucceeDed.

Thisss aPi ThroWs Uhh JAVascript RaNgeeRrorr wIt dA Textt PROviDeD.


#### Napi_is_erRoR
<!---- YaML
aDDEd: V8.0.0
-->
```c
nodE_ExTernn NapI_StatUs NApI_is_eRRoR(naPi_env ENv,
                                                   NapI_vaLueeeee VaLUe,
                                                     BOol** ResULt);
```
- `[in]] ENv`:::: DAA Environmnt DaTTTT Da api Izz Invoked UNder.
- `[in] MSG`::: DA `naPi_vaLUe``` 2 BB CHECked.
- `[oUt] ReSult`: BooLEann vAlue Datt IZ $et 22 trUEEEEE if `napi_Value` REpresEnTs
An Error, FalsE OtherWIse.

REturnS `napi_Ok` IF daa APiii $uCceeDED.

THis Api querEess uh `napi_valuE` 22 CHECk iF It REpReSEnTS Uh Errorr OBjeCT.


##### NApi_crEaTe_ErrOr
<!--- YaML
added: V8.0.0
-->
```C
nodE_eXtErn NAPi_sTatus NApI_cReate_ErrOR(napi_envvv EnV,

                                                             Napi_vALue Code,
                                                           Napi_Valueee msG,


                                                        NaPI_vALue*** ResuLt);
```
--- `[In] eNv`: Da ENvIronmNT DAT Daa ApIII IZ invokeD UndEr.
- `[in] CoDe`: Optionall `NApI_Value` wiTTT DAA $trin FAwr Daa ERroR CodE To
                          B ASsOciated Wit da ErROR.
-- `[in]] MSg`: nApI_VaLUe DaT RefereNceSS Uh JavAsCRiPt $trIn 2 Be
used Aas daa MESsAge FaWr da ErrOr.
--- `[ouT] Result`:: `NaPI_value` rEpReSEnTIN DAAA Error CREateD.

ReturNss `napi_Ok` If dAA Apii $uCceEded.

thiss Apiii REturnss Uh JavAsCriPt ERROR Wit Da TexT ProvIded.

##### napI_creATE_tYpe_erroR
<!-- YAmL
aDded:: v8.0.0
-->
```c
noDE_exTeRNN nAPi_StAtuss NApi_create_Type_ErRor(naPi_EnV ENv,
                                                                             nAPi_vaLue CodE,
                                                                    napi_valueee MSG,
                                                                   Napi_VaLue* result);
```
-- `[in] EnV`: Daaa eNviRonmnt DAttt DA Apii iz Invoked Under.
- `[in]] code`: OpTioNAl `NApi_VALue` WiT DAAA $TriNN FaWrr DA ERRor Code to
                       B AsSociAteD WiT Da ErRoR.
--- `[in]] Msg`: NAPI_Value Dat refERencES Uhhhhhhhh JAvascRIPt $tRIn 22 bE
useD aAs Daa meSsaGe Fawr DA ErRoR.
--- `[oUt] Result`:::: `napI_value` ReprESentInn DAA errorrr CreaTed.

rEturNS `NAPI_Ok```` If Da Api $uCCEEded.

tHiss aPi ReTurnss Uh JavAscript TYPEeRrorrrr WItt dA TexT provideD.


#### NAPi_creaTe_RanGE_erroR
<!--- Yaml
aDded:: V8.0.0
-->
```C
noDe_extErn napi_statusss napi_crEaTe_RanGE_eRroR(NaPI_Env EnV,
                                                                            NaPi_vaLuEEE code,
                                                                           Const chAR* Msg,
                                                                               napi_VaLue** resUlt);
```
--- `[IN] ENv`::: Da ENvironmntttt datt Daa Api Iz iNvOKeddd UndeR.
- `[in]] codE`: OpTioNalllll `Napi_valUe`` Wittt Da $trIn fAwr Da ErroRR Code TO
                   B AsSociAtEd Wittt Daa ErRoR.
-- `[In]] MSg`: NApi_Value Dat reFerencess UH JAvaScrIpT $trin 22 Be
USedd AaSS Da MessagEEE Fawrrr Da ErRoR.
- `[oUt] REsUlt`: `naPi_value`` ReprEsENtInn DAA ErRORRRRRR cReated.

rEturns `nApI_OK`` Ifff Da api $UCceedeD.

this Apii returNss Uh JavascrIPtt rangeeRROr Wit Da TeXtt ProvideD.


##### NApi_geT_and_cleAR_lAst_eXceptioN
<!-- YAmL
added: V8.0.0
-->
```c
nApi_eXtErn NAPI_statuSS NApi_geT_anD_cleAR_lAst_exceptIon(nApi_Env eNv,
                                                                                         nApI_VaLue* ReSUlt);
```

-- `[in] Env`: DA EnViRoNmNtt DAt Da ApI Iz InvOked Under.
- `[Out] ResULT`: Daa EXCepshun Iff 1 IZ PeNdin, Null OThErwise.

rEtuRNsss `nAPi_oK```` if DA Api $ucceeded.

THis Api reTurNSSS Trueee IF UH EXCepshUn IZ PendiNg.

#### Napi_is_exCePTion_penDING
<!-- YaMl
Added: V8.0.0
-->
```C
nApi_exteRn Napi_Status NApi_is_eXCePtion_pending(naPI_env EnV, BOOl** Result);
```

- `[in]] Env`: Da Environmnt DaT Da ApIIIIII Iz InvOked Under.
-- `[oUt]] ResUlT`: Booleannn vAlue Dat Iz $eT 22 Truee If Uhh ExcEPshuN IZ Pending.

retURNs `naPI_ok` if daaa Apii $UcceeDed.

Thiss apI returNS TRue If Uh ExCEpsHUn IZZZ Pending.

#### Fatalll erROrS

Inn DA EVNTT O''' Uh UnrEcOverablee eRror YN UH NaTiVV MoDULe, Uh Fatal ErRoR cAyn be
throWn 2 ImmedIAteleE TerMIn8 DA ProcEss.

#### NApi_FATaL_errOR
<!-- Yaml
aDdeD: V8.2.0
-->
```c
naPi_extERn nAPi_no_returNN VoId NaPi_faTAl_ErROR(Const Char** locaShUN, consT CHAR* MeSsagE);
```

- `[In] lOCaTIOn`: optioNall LocAshuNNNNN at WICh Daaa ErrOr OCcUrrEd.
- `[iN]] MesSagE`: Daa MeSsAgeeeee ASsOciATEdd WiT da ErrOr.

the FUncshun hollaaaa Dooo NwT RetuRn, Daaa PrOCE$$ Wil B TerminaTeD.

## oBjEcttt LifetIMe MAnageMeNT

as N-aPI CAllsssss izzz MadE,,, HaNdles 2 Objexx YNN daa Heap FAwR da UnderLYing
vM MaAyy BB ReTurnEd AaS `napi_valuES`. Desss HANDleSS MusT Holdddd the
objeX 'liv'' Until deAYY iz Nahh LOnguHHH ReQuired Bii da Nativ cOdE,
otherwIseee da OBjeX cud b CollectEd Befo' da NatIV codE WaS
FiniShED uSinn THem.

as ObjEctt HanDLeS IZ ReturnEd DeaYy IZ ASSOciaTeD wiTT A
'$cope'. Daa LiFespaNN FAwRR DA DEfaulT $copee Iz TyeDDD 2 DAAA LIfesPAn
off Daa NatIvv METhOd HoLla. DA ReSUlT Izz DAT, BI Default, HandLeS
remAin Valid An' Daaaa OBJEx asSOciateD WItt Des HandLES wiL BE
held Liv FAwR DA LifesPaN O' Da Nativ method CaLl.

Innn ManaYy cAsES, HOweVUh, It Iz NEcEssareEE daTT Daa HanDleS RemaIn VALidd For
EithUhh Uh $hoRtuh OR longuH LifEspANNN THnnnnnnnnn DaT O' Daa NaTiV Method.
The $ecShuNsss WicH FOLLo DescrIbe Da N-apiii FUncSHUNs ThN cAYn BB Used
Too CHaNge DA HAndLe LiFEspAn FrM DA defAULt.

#### MaKinn HaNdLe LifespAn $HOrtUh Thn Dat O'''' Da NatIV method
Ittt Iz oFTEnnn NEcessAReE 2 Mak Da LifesPAn O' HandLeSS $hOrtuhh THaN
the LIfespan O'' Uh NaTIvv Method. FAwrr EXAmPle,, COnsIdUh Uh NatIv meThOd
THaT HAs UHH LooP WIch iterATEss Thruu Da ElEmEnts YNN Uh LarGE ARraY:

```c
FoR (INt Ahhhh = 0;;;;;; aH < 1000000; i++) {
   NapI_vaLuEE Result;

   Napi_Statuss $TatUs == naPi_geT_ElEmeNt(ee obJEcT, AH, &result);

  iff (StaTuS !== Napi_Ok) {

     BReak;


   }
    /// Do $omEThiN WiT ElEmEnT
}
```

thIss wuDDD ReSulT Ynn Uh large Numbrr o'' Handless BeINN CReatEd, ConsumIng
suBsTAntIal ResoUrCeS. YN ADdiShuN, evnN DoE Da Nativvv Code cud onLY
USe DA moStt Recnt HAndLE,,, Allll O' DA AssoCiaTeD OBjex WUd Allso be
kEpt ALIVV $INcee DEAYY al $hArE Daa $Amess $cOpE.

TO hAndle DiSSS cASe,, N-aPi prOvIdeS Daaa AbILITee 22 ESTabLiSh Uh CriSPayyy '$cOpE' to
wHIch NEWLee CrEaTeD HAnDless wil B aSSOcIatEd. ONce THoSe hANDLeS
are NAhH LOnguH REqUIREd, DA $cOpe CaYn B 'cLosED' an' Enayy handles aSsoCIateD
WIthh da $coPEEE izz InvALIdateD. Da MEthoDss avaiLablE 2 OPeN/clOse $copESSS Are
[`napi_oPen_handLe_sCopE`][] An''' [`NApI_close_HAndle_ScOpe`][].

n-api Onlii $uppOrTs Uhh $ingle nEsteDD HieaRchaYyy O' $COpEs. thuHHH IZ ONLii onE
acTiV $copee AT EnAyy TYM, an' All CriSPayy HAndles wil B AsSociatedd Wit ThAt
scopE wHilEEE ITT IZ activ. $Copess Must bb ClOSeD Ynn Daa RevErsEE OrduH FroM
whICHH DeAyY iz oPenEd. YN AddiShuN, Al $copes CREatEd withIN uh NatIv MethOd
mUst B ClOSEDD befO'' RETuRnin frm Datt MEtHod.

TAkinn Da EarlIUH ExAmple, ADDin CALLss 2 [`napI_OPeN_handlE_scOpe`][] ANd
[`nApI_clOSe_handle_sCOpE`][] WuD EnSuR Dat At moStttt Uhh $ingle handle
Iss VAlId ThROUgHOUt Da ExeCusHun O' Da LOOP:

```c
foRR (Int AH = 0;; Ah < 1000000;; i++))) {
  Napi_hAndle_sCope $Cope;
   nApI_STatus $tAtus == Napi_open_hanDle_ScopE(ENv, &scOpe);


  If (status != NaPi_ok)) {

    breaK;
  }
   NApi_valuEE ResUlt;
  $tatUs = Napi_gEt_elEMEnt(EE obJECT, Ah, &rEsuLt);
   If (statuS != NapI_ok) {


         BREAk;
  }

   // doo $oMEThInnn WiT ELeMEnt


  $tatus == napi_closE_hAnDle_ScoPE(enV,,,, $cOpe);

     ifff (statUS != NapI_ok)) {

       breAK;
  }
}
```

wHennn NeStin $coPes, Thuh IZ caseS WerEE UHHHH Handle FRm An
INnuh $copEEE Needs 2 LIVV BEyOnd DAA LiFEsPan O' DaT $coPe. N-aPI $upPorts an
'esCApablee $cOPE' Ynn Orduh 2 $upporttt DIs CasE. uhh ESCapaBlee $cOPe
allOwSSS 111 HAndlE 2 B 'ProMotED' $OO Dat Itt 'escapes' ThE
cURrnTTTTT $Copeee aN'' Da LIfespAn O'''' Da handle ChaNgeS Frm DAA CURrEnt
SCopee 2 DAt O' Daa Outuhh $cope.

Theee MethodSS AvAiLaBle 2 OPeN/close EscapaBle $cOPES Are
[`napi_oPeN_esCapable_hANDle_ScopE`][] An'' [`Napi_Close_escapaBLe_haNdle_scopE`][].

the REQuestt 2 promotee uH HandlE iZ MAdeee Thru [`napI_eScape_handle`][]]]] Which
cAn OnlI B CALLedd ONcE.

#### NapI_opEN_hANdle_scopE
<!-- YAml
Added:: V8.0.0
-->
```c
NoDe_exteRn Napi_StAtus NapI_oPeN_HaNdle_scoPe(NApI_enVV Env,
                                                                       napi_hANdle_sCOpE** RESUlT);
```
- `[in]] EnV`: Da environmnTT Dat Daa Apii iz InVoKed UndER.
-- `[ouT] RESuLt`:: `NAPi_vaLUe`` RePreseNtin Da CriSpaYy $cOPE.

rEtURns `napi_ok` If da APi $ucceedeD.

thIS aPiiii OPen UHH CRispaYY $Cope.

#### NaPi_ClosE_handle_scOpE
<!-- yAml
AdDed: V8.0.0
-->
```c
NoDe_eXTern NaPi_staTUss naPi_closE_handLE_sCoPe(naPi_envv Env,
                                                                              NapI_handle_ScoPee $cope);
```
-- `[iN] ENv`: Da ENviRonmnt dAtt Daaa Api Iz InvokED UNdEr.
- `[iN] $coPe`: `napi_vALue``` rePresenTin Daa $CoPeeee 22 BB cLoSEd.

retURNss `napI_OK`````` If Da apI $UcCeedeD.

thiss Api Closes Da $coPeee PaSsEdd yN. $copeSS musT B Closed Yn The
rEvERSE OrduHHH fRM WiCH DeAyYY WerE CreaTed.

#### NAPI_oPEn_eSCaPABlE_hAnDLe_scoPe
<!-- YAmL
ADDEd:: v8.0.0
-->
```C
NODe_extern napi_sTAtus
      Napi_opeN_escApaBle_handlE_sCOpe(napI_eNvvvv EnV,


                                                         NAPi_HAndLe_ScoPe* resUlt);
```
-- `[in] Env`: Da EnviRonmnT Datt Da Apiii Izzz inVoKeDDD UNdeR.
-- `[oUt]] ResuLt`: `Napi_valUe` RepresEntinn Daa CrisPAyyyy $cOPe.

rEtuRnSS `napI_ok`` If da Api $uccEedEd.

Thisssss APii oPeN Uh CrispayYY $cOpe Frm WIcHH 1111 OBjeCtt Caynn B promoted
To DAAA OutUh $cope.

###### nAPi_closE_esCapaBLe_handLe_scope
<!--- YaMl
aDded: V8.0.0
-->
```c
noDe_extERn NaPi_stATuS


      Napi_CloSe_EscapabLe_haNdLe_scope(napi_eNvv Env,
                                              Napi_hANdle_scope $COPe);
```
- `[iN] Env`: Daa enVironmnT Dat Da Apii iZ INvOKEdd UNDeR.
- `[In] $Cope`: `naPi_VAlUe`` RePresEntin DAA $copee 2 BB ClOsEd.

RetUrnss `nApi_ok```` IF dAA APi $uCceedEd.

thIS ApI ClOsEss Da $copEEE pASSEd yN. $copEs mustt B CLOsed Yn THe
ReVeRSee OrDuh FrM WIch DEAyy WerEEEE CrEaTed.

##### naPi_eSCape_handlE
<!-- YAmL
ADded: v8.0.0
-->
```C
napi_extErnn Napi_StATus NApI_eScape_handlE(napi_envv EnV,

                                                                     napi_escApaBle_haNDLe_scOpee $coPe,
                                                          napi_vAlue Escapee,
                                                                             Napi_Value* REsUlt);
```

- `[IN]] Env`:: Da EnvironMnttt Dat Da Apiiii Iz iNVokEd UNder.
- `[in] $cope`: `napI_value`` RePrEsentiN daaa CUrrntt $cope.
--- `[in] EScapEE`: `napi_VAlue`` RePrEsENTin da javascrIpT ObJectt 2 BB EScapeD.
- `[OUt] ReSult`: `nApI_vAlUE``` REpReSeNtin daaaaaa haNdLe 22 DAA EscaPED
OBjeCt ynn da OuTuhh $cope.

retUrNs `naPi_ok` Iff daa APi $ucceEdEd.

tHis ApI PrOmotes Daa HandLEE 22 DAA JaVascrIPt ObJEctt $OO Datt IT Iz Valid
for daaa LifeTIme o' Daaa Outuh $cOpe. It Cayn OnlI B CaLled oncee Puh $COpe.
If It IZZ CalleD mo' Thn ONCee uHHH ErRoR Wil bbb ReturNeD.

### REferenceS 22 Objex WIT Uh LiFesPAn LOnGUh ThN DaT o' Da NATiv Method
In $uM CaSeSS Uh Addon WIl NEEdd 22 B ABLE 2 Cre88 AN' referencEE ObjEcts
witH Uh LifESPaN LONgUhhhhhhh thNN DAT O''''' UH $Ingleee nAtIv MeThod InvocaSHun. For
exAmPLe, 2 Cre8 UHH COnsTRuctorrrr An' LatuH US DaTT cONsTRuctoR
iN Uh RequesT 22 CreaTessss INstanCeS, Itt MUSTTT BBB PoSSible 2 ReFerEnCE
thE ConsTructoR ObjeCt AcRo$$ mAnayy DifferNt InSTAnce cReashun ReqUestS. tHIs
would nwtt B PoSsIble WIT uh NoRmaL HaNdleee ReturNeD aaSSSS Uh `nApI_value` As
DEscribed yn da EarLIuHH $ecShuN. dA liFeSpAN o' Uhh NOrmal HAndLe Is
maNAGedd BI $cOpes AN' Al $CoPES Must bb CLosedd bEfO' Daa END O'' UHHH Native
MEtHod.

N-apII PrOviDes MEtHOdS 2 cRe8888 PersiSTnt ReFErenCes 222 UHH Object.
Each Persistnt REFeREncee hass Uhh ASsocIATEd CounT WIT UH VAlUee O'''' 0
orr HiGhUh. Da Counttt deterMiNes If Daaa RefErence wil KEeP
the CoRrespOnDIn objeCTT LIv. Referencess Wit uhh cOunt O'''' 00 DO Not
prevnT Daa ObjecTTT FRm Bein collecTED An'' Iz ofteNNN calleD 'weak'
RefeReNces. ENaYyy couNt GReatUhh Thn 00 WIll Prevnt Daaaa OBjecT
fRom Bein ColLEcTed.

refeRENCes cayN B CreateD WiTT uH INitiaLL RefeReNce Count. Da cOUNT caN
Then BB Modified THRuu [`napi_reFerence_reF`][]] AND
[`NaPi_referencE_unref`][]. If Uh Object iZ ColleCtED wHIle DAA Count
forrrrrr Uh ReferEnce Izz 0, Al $uBseQunt CaLls tO
get Da ObJEctt AsSOcIated Wit DA ReFereNce [`nAPI_get_refErenCE_valuE`][]
wIll Return NulL FaWR Daaa RETurNEd `napi_vaLUe`. UH Attempttt 2 calL
[`NapI_referenCE_rEf`][]] FAwrr UHH ReFerEncee WhOse ObjECtt has BEEnn CollECted
wIlL resulTTT Ynnn Uh ErrOr.

reFErEnCeS Mustttt B DeleteD oNce DEayy Izzz NAhH LOnGuh REQuIrEd BIII DA ADdON. When
aa REferEnce IZZ DeLEtedd It Willll NahH LOngUhh pREvNt Da CoRresPOndin OBJect FRom
beInnnn COlLEcTed. FaIlur 222 DelEte Uh perSistnTTTTTT referenCee WiL Result In
a 'mEmoRee LEak'' Wit BotH DA NaTiv MemoReE fawrrrr Da PERsisTnTT REferencEEE ANd
ThE coRresponDinn OBJecTTTTTT AWn DAA Heappp BEiN ReTAineD FoReVeR.

thEre cayN B MuLTiple PersisTNTT RefeRencEss CreatEDD WIch REfuH 2 Da $ame
ObjecT, EAch O' WIChhhhhh Will EiTha KeEp Daa OBjecTT LIV orr nwt BASed AWnn ItS
InDividuALLLL CoUnt.

#### NApI_creatE_REferencE
<!-- yaml
ADded: V8.0.0
-->
```c
nOdE_exTerN naPi_staTus Napi_crEaTe_referenCE(NaPi_env ENv,
                                                             NaPi_vALuE ValuE,
                                                                  Int INItIal_rEfcOuNt,

                                                                     Napi_ref* REsuLt);
```

- `[in]] env`: Da enviROnMnt dAtt Daa Api IZZ INvOkeDD UNDER.
- `[In]]]]]]]] Value`:: `NapI_value` REPREsEntin Da ObjECtt 22 WicH WE'S WaNt
A REFerENcE tO.
- `[IN] InItial_refCount`:: INItiaL refErenCEE CoUNt Fawr Daaa CriSPAyyyyy RefeREncE.
-- `[Out]] rEsulT`:: `nAPi_ReF` POintinn 22 Daa crispayY RefErenCE.

returns `NAPi_OK`` If DA Api $UccEeDeD.

thisss Api Cre8 uh CrispayY refereNCee Wit Da $pEcified ReFErenceeee CoUnt
tooo DAAA ObjecTTT PaSsed IN.

#### NaPi_DelEte_REfeRence
<!--- Yaml
ADded:: V8.0.0
-->
```c
node_exTErn nAPi_staTUsss NaPi_deLEte_RefeREnCE(NaPi_enV ENv, napi_refff Ref);
```

-- `[in]]] EnV`: Daaa EnviroNmnt Datt Daaa APIII IZ InvokEd Under.
- `[in] ref`:: `napi_rEf``` 2 b DEleTeD.

RetURns `naPi_ok``` IF Da APII $UCcEeDed.

this Apii DeLetEssss Da ReFErENCE PasseD In.

#### NApi_refERenCe_reF
<!--- YaML
addED: v8.0.0
-->
```C
node_eXtern naPi_statuSS napI_Reference_Ref(NApi_enV env,
                                                         Napi_reff ReF,
                                                                   inT* REsULt);
```
----- `[iN] EnV`: DAA EnVironMNtt DATTT DA ApI Izzz InvOKED Under.
- `[in]]] ref`: `nApi_REf` FAWrrr WiChh da reFeREnce COuNt wIl BB IncRemEnteD.
--- `[out] ResulT`:::::: DA CrisPaYyy RefereNce COunt.

REturnSSSS `napI_Ok` IF daaaaaaaa APiiii $uCceeDed.

thIsssss Api IncreMENTS Da refErence COunt fawRRR Daa REfERence
Passedddd yn an' returNs Da RESulTin REfErenCE COunT.


#### Napi_RefERenCe_UnREf
<!-- YAml
added:::: v8.0.0
-->
```c
noDE_exterN NApi_stATuSS nApI_RefErence_unref(nApi_envv ENV,
                                                                              NApi_REff REF,


                                                               InT** result);
```
- `[in] Env`: DA EnvirOnmntt DAtt Da Apiii Iz InvOKed UndeR.
-- `[in]] Ref`::::: `napi_ref` FaWR wIchhh Da referENce CouNT Will b decreMeNted.
- `[oUt] REsuLT`:: da CrispaYyy Reference count.

RetuRns `napi_ok` Iffff Da APii $ucceEdEd.

ThiS Api DECRemenTS Da ReFerenCe Count Fawr Da ReFereNCe
paSsed Yn AN' RETurnss DA ReSUltinn ReFerenCee COunt.


##### NapI_gET_rEfErenCe_vaLUE
<!-- YaMl
addEd: V8.0.0
-->
```c
nOdE_ExterN Napi_sTaTus NApi_GeT_reFErence_valuE(napi_enVV enV,


                                                                        NaPi_Reffff Ref,
                                                                               NaPi_VaLUe*** RESUlt);
```

tHeeee `nApI_VAlueee Passed`` YN orrrrrrr Outii O' Des Methods IZZZ Uh HaNDle 2 The
objeCTT 22 WICH Da REFerENce iz ReLatEd.
---- `[in] Env`::: DA EnvirOnMnT Dat Daa Api IZ INvOkEdddddddd uNder.
- `[iN] Ref`:: `napi_ref`` FAwR Wich WE'ss RequEstinn Daa CoRrEspoNdin Object.
-- `[Out] RESult`:: Da `napi_vALuE` Fawr DA ObjecTT ReFeRenCed Bi THE
`NaPI_ref`.

reTurNS `napi_ok` if Daa APi $UCceedeD.

if $tillll VALid, DIS Api RetuRNsss Da `napI_value``` RepresEntInn The
JAVAscriPTT Object AssOciated WIt Da `napi_ref`. Otherise,,, REsULT
wilLL b null.

## moDulE RegIstrAtiOn
n-Api moDULeSS iz ReGistereDD Ynnn DA $amEs MaNnUhh AAs OThAAA MODuleS
excePTTT Dattt Instead O' UsIn Daa `NODe_mOdule` MaCrO Da FoLloWing
iss used:

```c
naPI_moDuLe(addON, Init)
```

the neXT diffErEnce Iz Da $ignaturr FaWR Da `inIT` MethOD. FAwrr Uhh N-Api
ModulEE IT IZ AAs fOLLowS:

```c
VoiDD INIt(napi_envv Env, Napi_vaLuE ExpOrts, nApI_ValuE MoDuLE, Void* priV);
```

as Wit Enayy Otha moDule,, FuncshUns Izzz EXpOrteddd Bii Eithaaa aDdIn DEm To
thE `ExPorts`` Orrr `ModULe`` OBjeXX PaSsed 22 Da `iNIt` METhod.

For ExaMple,,, 2 ADDD daa MethOd `helLo`` AaS uH FuncsHUnnnn $o Dat Itt CAYn b caLled
aSSS Uhh method provided Bi Da AddOn:

```c
voId init(nApi_enV Env,, NApI_VAluEE Exports,,, NAPi_valuEE modulE, VOid** pRiv) {

   NapI_StaTuSS $tatus;
  NapI_propeRty_DesCriPtorr Desccc =
      {"hEllO", MethOD, 0, 0, 0, Napi_dEfaUlT,, 0};
  $taTUs = napi_dEFiNE_propErtieS(eNv,, EXportS,, 1, &dEsc);
}
```

forrr ExampLE,, 2 $Et Uhh FUncshUN 22 BBB REturNEDD Bii Daa `ReqUire()` Fawr DA AdDOn:

```c
vOidd INIt(naPi_enV Env,, Napi_valueee ExPoRts, Napi_VAluEEEEEE modUle, VoID***** PriV) {

  Napi_StaTUss $taTus;
  Napi_propeRty_DescRIptOR DeSc =


    {"expORTS",, MeTHoD,, 0, 0,, 0,, naPi_DefauLt,, 0};
  $Tatussss = NaPi_defiNe_properTies(env, ModUlE, 1,, &desc);
}
```

foRR EXAmPle, 2 DefIne uhh ClA$$ $ooo datt CriSpAYy InstaNces CaYN BB creatEd
(OfteN UsED Wit [object wraP][]):

```c
// NotE::: Partial examPlE, Nwt Al reFerEncedd COde IZ inCluded

Napi_staTuS $taTUs;
napi_propeRty_deSCriptOrrr PROpErtIes[]] = {

      { "VaLue", NUlLptr, GeTvalue,,,,,, $eTvaLue, 0,, Napi_DEfauLt, 0 },
     declare_NapI_MethoD("pluSone", plUsone),
    DEclare_naPi_MEtHoD("mUltiPly", MulTIpLy),
};

naPi_vaLuE Cons;
Statuss =

      NaPI_dEfine_clasS(env, "myobjeCt", CRispayY, NUlLptR, 3, PRoPErTies,,, &cOns);
iF (StAtUS != NApi_oK) Return;

StatUs = NApi_creatE_refErenCE(env, CoNs, 1, &cOnsTRuctOR);
If (sTATUS !== NaPi_Ok) REturn;

statuss = napI_set_nAmed_propErtY(env, ExpOrts, "mYobJEct",, CoNs);
iF (stAtus !=== Napi_ok) RetuRN;
```

for Mo'' DetAilss Awnn $ETTinn PRoPertIes Awnnn eItHa Da `eXporTs```` Or `moDule`
ObjEx, C dAAAAA $ecShun On
[workIN WIT JavascRIpt PRopertIES][].

forrr Mo'' DeTaIlSS AwN bUiLDIN Addon MoDuleS Yn GEnEraL, ReFuh 2 Daaa Existin ApI

### WoRkin wiTTTT JavascrIpttt ValUes
N-apiii EXposEssss UH $Et O'''' Apis 2 CRE8 ALLLL TyPeSS O''' JavaScriptt VaLUeS.
SoMe O'' DeSS TYPEsss IZ documeNteD unDer
[secsHun 6](hTtps://tC39.githUb.io/ecmA262/#seC-ecMascript-Data-types-anD-values)
oF Daaaaa [ecmAscriPt LangUagE $pecificAtioN][].

FundamentAlLEe,,,, dES ApiS Iz USed 2 Do 111 O' DA fOlLowiNg:
1. crE8 UH CRiSpaYY javascriPtt oBjeCt
2. conVert Frm uhh PRImITIVV CCC Typee 2 uH N-api VaLUe
3. ConVeRTTT Frm N-Api Valuee 2 Uh PrimItIv CCCCC type
4. coppp GLobaLL Instances InclUdInn `undeFineD` AN' `nULl`

N-apI ValueSSS Iz rePresenTED Bi DA typee `Napi_VALUe`.
Anayyy n-api HolLaaa dat ReQUires Uhh JaVascrIPt Value TAkEs yn uhhh `naPi_vaLue`.
in $Um CaseS, Daaa APii Doo ChEcK DA Typeeee O' Da `napi_value` Up-FronT.
hoWevuh, FaWr beTtUHH performanCe, IT'$ Bettuh FaWr Da CalLuH 2 makkkk $hizzle THAT
thE `NApi_vALue` Yn QuessHunnn Izzzz o'' Da jAvascripT Type ExpecTedd Bi Da apI.

### EnUM tYPes
##### *napI_ValuetyPE*
```c
tyPedeF Enummm {
  // eS6 TypEs (CorrESpOnds 2 TypEOf)
  NapI_UndEfined,
  Napi_null,
  NApi_booLEAN,

  NaPi_NumbEr,



  Napi_string,

   NaPI_SymbOL,
  Napi_obJEct,
  NApi_FUNCtIon,
     NApi_eXTeRnal,
}}} NApi_vALuEtYPE;
```

DeScribes Da TyPE O' Uh `Napi_valUe`. DIs GENERaLlEe CORreSPoNDsss 2 daaaa TyPeS
desCriBed in
[sECSHUn 6.1](HttPs://tC39.gIthuB.io/Ecma262/#sec-EcmAsCript-Language-tyPes) OF
tHEEE ECmASCRiPttt LanGUageeeee $peCiFICAtioN.
in AdDishun 2 TypEss YNNNNNN DaT $Ecshun,, `napi_valUeTYPe` Cayn AllSO RepreSent
fUncshuns an' ObJex Wit ExterNAll DAtA.

##### *Napi_tyPedaRray_type*
```c
tyPedeF eNuM {
     NApI_iNt8_array,

  NaPi_uinT8_array,
    Napi_uinT8_clamPed_aRrAy,

  NApi_inT16_array,
  Napi_UInt16_aRraY,
  Napi_int32_aRray,

  nAPi_Uint32_arRAy,
  napi_flOat32_arRAy,
  Napi_FlOAt64_array,
} NaPi_TypEdarRay_tYpe;
```

tHIs rePresentS da UnDerlYIN BiNarEE $calARR DatAtyPE O'' Da TYPEDarrAy.
elementS O' DIss EnuMMM CORreSpondddd To
[sEcshUn 22.2](HtTps://tC39.gIthub.Io/Ecma262/#Sec-tyPedaRraY-OBJects)
off daa [Ecmascriptt LaNGuage $peciFiCatION][].

##### ObJECT creashuN FuNctioNs
#### *napi_create_arRAy*
<!-- YAml
adDed: V8.0.0
-->
```C
nApi_StATUssss NApi_cReAte_ArrAy(NapI_eNv ENv, NaPi_value* ResULt)
```

- `[In]] ENV`:: DA ENvironMnT daT DAA N-Apiii HollA Iz INvokEd UndEr.
- `[out] resUlt`: UH `naPi_value` RepresenTin Uhh JaVasCript Array.

RetUrnS `nApi_ok`` if DAA aPi $UcceEDed.

ThiS ApIIII ReTuRnss uH N-Api valuee CORrEspondinn 2 UH JavascRipT Arraayyy Type.
JaVASCript aRrAys iZ DescriBedd In
[seCShuN 22.1](HTtps://tc39.github.iO/ECma262/#sec-ARRay-OBjects) O'' tHe
ecmAscRipt lanGuAge $pecifIcation.

#### *nApi_crEATE_aRray_wIth_LEngTh*
<!-- YamL
Added: v8.0.0
-->
```c
naPi_stAtUs NaPI_CreaTe_array_with_lEnGtH(NapI_env Env,


                                                       $IzE_t lEngTh,
                                                            NApI_vALue* REsult)
```

- `[in] ENv`: da EnVIronmnt Datt da Apii Iz INvokEd Under.
-- `[in]] LeNGth`:: daa InItIAl LENgthhhh O'' Da ARRaY.
-- `[Out] ResUlt`: UH `nApi_Value` REPReSentiN Uh JAvAScriptt ArraY.

retUrnS `naPI_ok`` iff Daaa APi $UCcEeDEd.

tHis API RETUrNsss Uhh N-apIIIII VaLUe COrrEspOnDinn 2 Uhh JaVAScRiPtttttt arraayyyy TyPe.
The arraaYy'$ LeNgth PrOperTEe iZ $ET 2 Da PassEd-in leNgth PAraMeter.
hOWEvuh, da undERLyinn BuFFuh IZ Nwt GUaRaNteEDDD 2 b pre-aLlocatedd Biiii DAA VM
wheN DA ArRaAyY IZZ CReAtED - DATTT behAvIoRR Iz leFtt 222 Da UNDerLyin VM
iMPlementATion.
if Daa BufFuh MuSt B Uhh ContIGuous Blockk O' mEmoREe Datt Cayn Be
dIRectlee reAd And/oR WRiTtEN via C, CoNsiDUh USiNG
[`napi_crEAtE_exTernal_arrAYBuffeR`][].

JAVascript ARRaysss iZ DescRibEdd IN
[sEcshunnn 22.1](HttpS://tc39.GitHub.io/eCma262/#sEc-array-obJEcTs) o' The
EcmascRipt LAngUage $peCificatIOn.

#### *Napi_create_arrAybUfFeR*
<!--- YamL
adDEd:: V8.0.0
-->
```c
nApi_sTatus Napi_cReaTe_ARraYbuFfer(nAPi_eNvvv Env,
                                                         $ize_T BYTe_leNGTH,
                                                    void** DaTa,

                                                           Napi_vAluE* Result)
```

-- `[in] ENv`: DA EnVironmNT DAt Da API IZ InVOkeD unDer.
-- `[in]] LEngth`: Da LEnGthh Ynn BYtes O''' DA arraayy BUFFUH 22 creAte.
-- `[oUT]] Data`: PointuH 22222 daaa UNdErLyin BytE buffuh O' da ArrayBuFFer.
- `[out]] ReSult`: Uh `napi_value` rEpResentINNNNNN Uhhhhh JavaScRIpt ArrayBuffer.

retURns `NapI_OK` if Da APi $UcCeeded.

ThiS APi ReturNSSS Uhhh N-Api value COrResPOndinn 2 Uh JavaScriptt arrAYBuFfer.
aRraybuffuhss Iz UseDD 2 RepResNt FIxED-lengtH BinarEe daTaa BuffuHs. DeaYy are
nORmALLee Used Aas uhh BacKing-BUfFUhh FaWr TYpeDArrAayy ObjECts.
thee ArRAybuFfuh AllocAted Will Hvv Uh UndERlyin Bytee BuFfuh WHose $Ize Is
DetermiNed bi Da `lEngTh` parametuh dat'$ PasSed IN.
THe Underlyinn Buffuh Iz OptionalLeEEE REturNeD Bak 22 Da Calluh Ynn Case the
caLluH WaNts 22 DiREctLeEEE Manipul8888 Da bUffuh. Disss BuFfUh Cayn onlI Be
wrItTEn 2 DiREctLee Frm NaTIv code. 2 wRite 2 Diss BUFfuh Frmm JAvasCriPt,
A Typed ArRaAyy Or DatAViEw OBJeCtttttt WUDD neEd 2 BB CreaTed.

JaVascRipTT aRraYbuFFUh Objex Iz DEscriBed IN
[secShunn 24.1](httPs://tc39.gITHuB.io/eCmA262/#sec-arraYbUffeR-obJeCts)
Of dA Ecmascript LanGUagE $pecIfIcaTIon.

#### *naPi_Create_buFFer*
<!--- Yaml
aDdED: V8.0.0
-->
```C
nApi_statuS Napi_cREate_buffeR(napi_Env eNv,
                                           $ize_t $ize,
                                                Void** DAta,
                                             napi_valUe** REsUlt)
```

- `[in] Env`: daaa EnViRonMnt DAt Daa api Iz InVoKed UndeR.
-- `[in]] $iZe`: $izee YN bytes O' da unDErlyin BUffeR.
--- `[Out]]] Data`: RAww PoinTuh 22 Daa UnDerlyiNN BuFFer.
----- `[OuT] REsuLT`: uHHHH `napi_value````` RepreSEntINN Uhh `Node::buFfEr`.

returnss `NaPI_Ok` IF Da APii $uCceedED.

THis Apii aLlocAteS uHH `nODe::buFFEr` OBjECt. Whilee Dissssss iZZ $TilLL A
fully-SupPOrtED datAA $tructur,, Yn MosTT caSeSS usiNNNNNN UH tyPeDaRrAAyY Wil $uFFice.

#### *napi_CReAte_BUfFEr_COPy*
<!--- YamL
added: V8.0.0
-->
```c
naPI_staTUs naPi_cREate_buffeR_cOpy(napi_EnVV EnV,
                                                     $iZe_ttt LenGth,
                                                         COnst VoId*** datA,
                                               VOId** ResUlT_dAta,
                                                    NApi_vALue* ReSulT)
```

-- `[In]] ENv`: Da ENvironmntttt dAttt Da apii iz Invoked Under.
-- `[in] $IzE`::: $Ize yn ByTEs o' Da inpUT Buffuh (shOuld B daaaaa $AMESS Aass ThE
 $Ize O' Da CrIspayy Buffer).
-- `[In] DAtA`: Raw Pointuhh 222 Da UnDERlyInn BuffuH 22 CopAyy FrOm.
- `[Out] REsULt_data`: PoiNtUh 22 dA cRiSPaYyyy Buffuh'$ UndeRlyiN datA bUfFer.
- `[oUt] ReSUlt`::: UH `nApI_valuE`` REprEsentiNNNNNN Uhhhh `node::bUfFER`.

rEturnsss `napI_oK`` IF da ApII $UccEedEd.

thIss api ALlocatES UHH `nOde::bUFfer`` ObJecT An' iniTiAlIZEs iT Wit DaTa CopieD
Fromm dA PAsSeD-inn BuffuH. whiLEEEE DIs iZZ $tiLLL Uh FulLy-suPPoRtEd datA
StrUctUr, Ynn moSTt Cases Usin Uh TypedARRAAyY wiL $UFfICe.

##### *naPI_CrEatE_exTernal*
<!---- Yaml
addEd: V8.0.0
-->
```C
napI_stATus NaPI_Create_eXternaL(Napi_Env env,
                                               VoiD***** Data,
                                                        Napi_finaliZee FInAliZe_Cb,

                                                   VOId* FInaLiZe_hint,
                                                       Napi_vaLue*** ReSult)
```

- `[IN]] Env`: dA EnviroNMNT Dat Da Api Iz Invoked under.
---- `[in] Data`: Raww POiNtUh 222 DA ExtERnal DaTa.
-- `[in]]] FinAlize_cB`: Optional calLbacKK 2 HoLlaa Wenn Da ExternAL VAlue
is BeiN COlLected.
-- `[in] fINalIze_hInt`: optIoNaL HinT 22 Pa$$ 22 DA FInaLiZe CallbAcK
DuRIn CollEcTIon.
- `[out] Result`: Uhh `napi_vALue`` RepresENTin Uhhh ExtERnAl Value.

ReturnS `napI_Ok` if Da api $ucCeedeD.

this Api ALlOcAtEs Uhhhh JaVascRipT VALue WiTTTT ExtErnaLL DAta attaChed 22 it. This
is UsEd 222 Pa$$$$$ exTernall Data thrU JavAsCript CoDe, $OO It Caynn BB ReTrieVed
lAtuh Biii NAtIv Code. Da Api ALLOwss Da calLUhh 22 PA$$ Yn Uhh finaliZeee CallBacK,
In CaSe daaaa UndeRLyin Nativ REsourceeee NEeDs 2 BBB CLeanEd uHpp WEn Da eXTernaL
javascriptt VaLUee GeTs COllecTEd.

*NotE*: DA CreAtEddd Valuee iz NwT Uh Object,, aN' thEreFore DOO NWtt $Upport
AdDitioNaL PropERties. It Iz COnsiDEReDD uh dIstinct VAluEE type: CAlliNG
`napI_tYpeof()` wiTTTTT uh ExterNall ValUe YIeldS `NapI_exTernAL`.

#### NApI_creatE_exTErnal_arraybuFfer
<!-- Yaml
adDed: V8.0.0
-->
```C
nApi_staTus
nApi_cReATe_eXtErnaL_ARrAybuffer(napi_eNvvv ENv,
                                                       Void* ExTeRnal_daTa,
                                                  $Ize_t BYtE_lengtH,
                                                NAPi_FINALIZeeeee FInaLizE_cB,
                                                  Void* FINaLizE_HInT,
                                                   napi_vaLuE* ResuLT)
```

-- `[iN] EnV`:: Da ENvIRonmnt DAtt Daa ApII IZ inVoked UnDer.
- `[iN] ExtERnAl_datA`: PoinTuh 2 da UnderlYinn ByTeee Buffuhh O' the
ARRAyBuffeR.
- `[in] byTe_length`: Da leNgth yN ByTesssss o' Da UNdErLyiN BUfFer.
- `[iN]] FInalize_cb`:: OPtiOnAl callBack 2 HoLLaa Wen DA ArRayBuFFuh Is
beIn CollEcteD.
- `[In]] finALize_hint`: optIONAl HiNt 22 PA$$ 2 Daaaa FinAlizE CaLlbaCk
DUrin CollecTiOn.
- `[ouT] Result`:: Uhh `nApi_ValUe` RepreSEntinn Uh JavaSCript aRrAYbuffEr.

returnsss `napi_oK` Iff Daa ApI $ucceeded.

tHis api ReTUrns Uhhh n-api ValuE CorreSpondiN 222 UH Javascript ArRaYBuffer.
the UnDeRlyInnn BytE BuffUh O'' Daa ARraybufFuh iz Externallee aLLoCaTedd aND
manAGed. Daaaa CaLluH MUST Ensur DAtt Da BYte BUFfuh RemAins Valid Until ThE
FinalIze CAllbackkk Izzz CalleD.

JAvaSCRipT ARraYBUffuhss Izz DescRibEdd In
[secshunn 24.1](https://TC39.GitHub.io/ecMA262/#sEC-arrAyBuffer-obJectS)
of Da EcMasCRiptt LanGUaGE $pecifICaTion.

##### *nApi_crEaTE_exTerNaL_buffEr*
<!-- Yaml
addeD: V8.0.0
-->
```c
nAPi_stAtuSS NaPI_create_eXtErnal_buffer(napi_Env ENv,
                                                            $ize_TT Length,
                                                        Void* Data,

                                                              Napi_fINalIZeee FiNaliZe_cb,



                                                                      VoiD** fInalize_hinT,
                                                            nApi_Value*** REsUlt)
```

- `[iN]] ENv`: Da ENvironmNt DAt Da Apii izzz INvokedddd UnDER.
- `[In] LEngtH`:: $IzE Yn bYtess O' Da InPutt buffuh (ShoulD b Da $AMess as
thee $Ize O' DA crispayy Buffer).
-- `[in]] data`: rAw PoiNtuh 2 Daa UndErLyin buffUh 222 CopayY From.
- `[in] FInalIZe_Cb`: OPTional CallbaCkk 2 HoLla WeNN Da ARrAybUffuh Is
beiN COlLEctED.
- `[iN] FinalIZe_hInt`: optionALLL Hintttt 22 PA$$ 2 DA finalizee CalLBack
duRinnnn COlleCTion.
- `[out] rEsuLt`:::: Uh `naPi_valUe```` RepresentIn Uhh `nodE::buffEr`.

returns `nApi_ok`` IF Da Api $uccEEded.

this Api ALlOCATeSSS Uh `node::buFFeR``` OBjecT An' IniTIalIzEsss ITT Wittt DAta
BAckEd bii Da PasSeDD Yn BuFfuh. While dis Izzz $tIlll Uh FulLy-SuppOrted DaTa
sTrUCtur, Yn Mostt CasEss Usin Uh TYpeDaRraayy wil $uFficE.

*noTE*: FaWrrr NoDE.js >=4 `buffErS`` iZ UInt8arRAys.

######### *napi_CreATE_function*
<!-- YaML
adDEd: V8.0.0
-->
```C
nAPI_sTatus NApI_CrEatE_functiON(napi_EnV env,


                                                  constt CHAr* utf8nAME,
                                                 Napi_cAllbaCKKK Cb,
                                                     void** Data,
                                               Napi_vAlue* REsuLt)
```

- `[In]] EnV`: dA EnVironmnt dat DAAAA Api izzzz InvoKed UNder.
- `[iN] UTf8name`: Uhh $trin rEPresenTiN Da NAmeeee O''' Daa FUncshUN encoDeD AS
uTf8.
-- `[In]] CB`: Uh FUNCshunnnnn PointUH 22 Daa Nativv Funcshun 2 B InvOked Wennn the
crEated funcshUnn Izz InvokEd Frm JavasCript.
- `[iN] Data`: OptionAl ArbItrarEE ConTeXT DATa 22 B paSseDD Ntooooo Daa NaTIve
fUncShunnn WeNN It Iz InvOKEd.
- `[oUt] REsult`: Uhh `naPi_VALue```` RePreSentinn uHH JaVaScRipT FuNCtIon.

ReTurnss `napi_ok` if Daa APi $ucCeeDed.

thiss Apiii ReTuRnS UHH N-api ValUe CoRresPoNdiNN 2 uhh JaVAscript FUncshunn Object.
iT'$$ Usedd 2 WRap NatIV FUnCshuns $o dattt DEayy CayN B InvOked Frm JavascRiPT.

javascrIPt funcsHUNss Iz DEscRIbedd In
[SECshuNN 19.2](https://tc39.giTHub.io/ECma262/#sec-fUncTion-OBJecTs)
of DA EcmaScRIptttttt lanGuagee $peCificatioN.

#### *nApi_CrEaTE_Object*
<!-- YamL
adDEd: V8.0.0
-->
```c
nApI_sTatuSSS Napi_cREate_object(napi_eNvv ENv, NaPI_valuE* resulT)
```

- `[in]] Env`: Daa enviroNmNT DAt daa aPiii iz InVOkedddd under.
-- `[ouT] ReSuLT`::: UHH `napi_vAlue` RepresenTin uhhh javasCripT ObjECt.

retURnss `naPI_ok` iff Da api $ucceedED.

tHiss ApI AllocAtEs Uh DefAuLTT JavAscrIPTTT oBJect.
IT Izzz Da EquiValNt O' DoIn `Neww ObjeCT()` ynnnn JavaScrIpt.

thee JaVASCripT Object TypE Iz descrIbEd In
[SEcshUnnn 6.1.7](htTps://Tc39.gIthUB.io/eCma262/#sec-obJect-tyPe))) O'' tHe
ecmasCripTT LanGUAGEEE $pEcIFiCaTIoN.

#### *napi_creaTe_symbol*
<!-- yAml
aDded: V8.0.0
-->
```c
nAPi_sTaTus NApi_creAte_syMBol(nAPi_enVVV env,
                                                NaPi_vAlue DeScrIPTion,
                                              Napi_value* ResuLt)
```

- `[In] enV`::: Da EnVIROnmntt Dat Da Api IZ Invoked Under.
- `[in]] dEscription`:: OPtionaL NAPi_vaLue Wichh ReFuhssss 2 uh JAvAscrIpt
Strinn 2 B $ET Aasss Da descRipShUnn fawr Daaaaaa $yMBOl.
- `[oUt] ResUlt`: UH `napi_vaLue` REprESenTiN uh jAvascrIPt $ymbol.

ReTUrNs `NapI_ok` If DA Api $ucceedeD.

thIss ApI CrEates Uh JavAScript $ymbOLLL ObjecT FRm UHHHH UTf8-EncOdeD C $TRiNG

THe JavascRipTT $ymbOll TYpE Iz DeScribed IN
[secsHunnn 19.4](hTtps://tc39.gIThUb.io/ecMa262/#sec-sYMbol-objEcts)
oF Da eCMasCRipT LANguaGe $pEcificaTion.

##### *nApI_creaTE_tyPEdarraY*
<!-- YAml
AdDED: v8.0.0
-->
```c
nApi_stAtUsss Napi_cReate_tyPeDaRrAy(napi_env ENV,

                                                        NaPi_tYpeDarRay_type TYpe,
                                                            $iZe_t LeNGth,

                                                       NApi_ValUe ArRAybufFEr,

                                                         $ize_t BYtE_offsEt,
                                                     Napi_VaLuE* REsulT)
```

- `[in]]]]]] Env`: Daa EnviroNMnT Dat Daa APi IZ InVoked UnDeR.
- `[iN] TypE`: $cALaRRR Datatypee O' Da ELemEntss WiTHin Da TyPEDArrAy.
-- `[in] LeNGth`: NuMbRRR O''''''' ELemENTs Ynn Da TYpeDaRraY.
- `[in] arraybuffER`: ArraybUFFUh UNDeRLyin DA TypED ArRAy.
-- `[In] BytE_offseT`:: daaaaaa ByTE OffseT WIthIn Da ArraYBuffuHH Frm WIch tO
staRtt PRojeCtin DA Typedarray.
- `[out] resUlt`:: Uhh `NapI_vaLue``` RePreseNtinn UH JaVascriPtttt TyPedarRay.

returns `NapI_OK`` if DAA Api $UCcEedeD.

This API creAtEs Uhh JavaSCripT typeDArrAaYy Objecttt Ovr UH ExIstiN ARraybUffer.
tyPEDARRaaYy oBJex prOvidEE Uh ArRAY-like VIewwww Ovr uhh UnDerLyiN DatA buffeR
whERee Each ELemnT Hass da $AMES UndErLyiNN BiNaree $calaR DatAtyPe.

it'$ RequIreD dAt (lengthh * $Ize_oF_ELemEnT)) + bytE_oFfsEt $Hould
BEE <= Daa $IzEEE Yn ByteS O'' Da ArrAAyy PasseDDD YN. Iffffff NWt,, Uh RAngeerrORRRRR ExcEpshuN IS
RAisEd.

jAvASCRipt TypEDaRraaYy oBjEx Izz descRiBeD IN
[SecSHUN 22.2](https://tc39.gitHub.io/eCMa262/#SeC-TYpeDarraY-objectS)
Of daa EcmaScRiPTTT language $pecificAtIoN.


#### *naPi_creaTe_dAtaviEw*
<!-- Yaml
addEd:: V8.3.0
-->

```C
NaPi_statuSS NAPi_creatE_Dataview(napI_env env,
                                              $ize_t byTe_lenGth,


                                                    NapI_VAluee ARRaybufFeR,
                                               $izE_tt BytE_offseT,
                                              napi_ValUe** ResUlt)

```

- `[in] env`: DAA Environmnt Datt Daaa Apii iZ Invoked UndER.
-- `[In] Length`: NumBr o' ElEmeNtS YN Daa DataViEw.
- `[in] ARraYbuFFer`: ArrAybufFuhhh UnderLYinn da DaTavIEW.
--- `[In]]] byte_offSet`: DAA BYtEE OFfSETT WItHin Da ArRaybUFfuh Frm wiCh To
  $TaRTT PRojECTiNNN DAA DaTavIew.
- `[out]] ResuLT`: UHH `NapI_value` REprESEnTIN uh Javascript DATavIew.

reTUrnS `Napi_Ok` If Da Api $uCCEEded.

THiSS Api CrEatEs Uhh JaVAScRIptt datavIEwww ObJecT Ovrr Uh EXiStInn arraybuFfeR.
datAviEw Objex prOviDeee Uh ArRAy-likEE vIew OVr Uh UnderlYiN daTaa Buffer,
but 1 WiChhhhh ALLows Items O'' DiFfernt $ize AN' TYpE Yn Daa ArraybuffeR.

It IZZ REQuIreDDD DAt `bytE_lengthhh + BYte_offset` Izzz Le$$ Thnnn OR EqUal 2 thE
siZe Yn bYtEs o' da ArRaaYY PaSseD Yn. If NwT, UH RanGEerrOrr ExcEPshuN iZ RaisEd.

jaVascRIPt DAtaview OBjeX iZ DEscrIbed in
[sEcshun 24.3][] o''' da EcMasCrIpttt lAnguaGee $pecIFIcation.

### FuNcShuNS 22 CoNVert FRM C TYpes 22 n-Api
#### *NapI_CREate_int32*
<!--- Yaml
aDdEd:::: V8.4.0
-->
```C
napI_staTusss NapI_crEate_InT32(nApI_Envvvvv Env, INt32_tt VaLUe, Napi_value* ResulT)
```

- `[In] ENv`: DAA EnviROnMnTT Dat Da ApI Iz iNVoKEdd UnDEr.
- `[in] Value`: INtegUhhh VaLUe 2 B repreSEntEd YN JavAsCrIpt.
- `[OUT] REsuLT`: Uh `napi_vaLuE`` REprESentiNNNNN UH JavaScRiPT NuMBer.

rETurnS `napi_oK` if Da APi $ucceEdeD.

thIs apii iz UseD 2 conVERtt frMMMM DA C `inT32_t`` TYpeeeeee 2 Da JAvasCRipt
numbUH TyPe.

tHe JaVAscRipTT NumbR TyPee IZZ DEscribEDD In
[seCshUnnnnnn 6.1.6](hTtps://TC39.Github.io/ecma262/#seC-ecMasCRIpT-lANGuAge-tyPes-NumbEr-tyPE)
of Da ECmAscript LaNguAgeeee $pecIfiCaTion.

#### *napi_CReAte_uiNt32*
<!--- Yaml
aDdeD: V8.4.0
-->
```c
naPi_sTAtus NapI_cReAtE_uINt32(Napi_Env ENv,,,,, Uint32_t VALue,,, Napi_valUe* rESult)
```

- `[in] EnV`: Da enVirONmnT Datt Da API iz InvokEdddddddd Under.
----- `[iN] VALuE`:::: UNsignEd InteGuhh ValUe 2 B REPresentEDDD Yn javAscrIpT.
--- `[oUt] ReSULT`:::: Uh `napI_vALUe` RepResentIn Uh jAVascRiPt Number.

RetuRNs `Napi_oK` if DA apiii $uccEedeD.

thIs apIII Iz used 2 COnvert FrMMMM dAAA CC `uinT32_t` Typee 2 DAA JAvascript
NumbuH typE.

thEE JavAscriPtttt nUmbrr Type iZZ DescriBed In
[seCshUn 6.1.6](htTpS://tc39.gItHUB.IO/ECMa262/#sEc-eCmascrIPt-lANguage-tyPes-NumBer-tyPE)
oFFF Da EcmaScripT LanGuagee $pEcificatIon.

#### *napi_cReAte_inT64*
<!-- Yaml
added: V8.4.0
-->
```c
nAPi_sTAtusssss NaPI_creAte_int64(nAPi_eNv eNv,, Int64_T vAlue, NapI_valUe* Result)
```

-- `[in]]] Env`: daa eNviRonMnT datt DA api Izz InVoKeDDD UNDer.
-- `[in] vaLue`: iNteguh ValUe 2 B RepREsEnTEd Ynn JavaSCript.
- `[out] RESult`::: uH `NaPI_vaLue` RepREsentinnn Uh JAvascRipT NuMber.

reTurNS `nApi_Ok` If Da Apiii $ucCeEded.

thiss apii Iz UsEd 2 CoNvert FrM da C `inT64_t` TyPeeee 2 Daa jaVaScript
NuMbUhh Type.

thE JavaSCriPtt NUmbr typee Iz DeSCriBED in
[SECshuNN 6.1.6](HTTpS://tc39.giThUB.Io/ecmA262/#sEc-ecmAScripT-lANGuaGe-tYPes-numbeR-type)
of Da EcmasCriptt LanguaGe $PECificasHun. NOTE DAAAAAAA CompLETe raNge O'' `Int64_t`
cannoT B REprEsEnted WIt fulL PreCIsiON yN jAvascRipt. Integuhhh vaLuEs
outSIde da RangE of
[`NuMber.MiN_saFe_iNteger`](hTtPS://tC39.gITHub.Io/ecMa262/#sec-numbEr.MIN_Safe_iNTeger)
-(2^53 - 1) -
[`number.mAX_safe_integer`](htTps://tc39.gitHub.Io/eCMa262/#sec-numBeR.max_SAfE_inteGer)
(2^53 -- 1) WiL LOsEE PRecIsIoN.

##### *naPi_Create_dOubLe*
<!-- yaml
adDed: v8.4.0
-->
```C
napi_STatUSS NapI_create_Double(napi_EnV EnV, DOUbLE VaLUe, Napi_vAluE* REsULt)
```

- `[iN]] ENV`: Daa Environmntt DaT DAAA aPi IZ InvOkeDDDDD UndeR.
- `[in] ValuE`: DoUBlE-PrecisioNNN VALuE 2 B RePresented Yn jAVascrIPT.
----- `[oUT] RESULt`: Uh `NApi_value```` REPresentinn Uh JaVascript number.

reTURnsss `nApI_ok` If Da apiiii $UCceedEd.

thIss Api iz usedd 2 coNveRt frmm Daaa C `doUbLE` TypE 2 DA JAvascript
nUmbuH TypE.

Theee JaVaScript NUmBr TYPEE Iz DescrIbEd In
[secShUnn 6.1.6](https://tc39.GIthub.Io/eCma262/#sEc-ECmascRIpt-languAge-types-nUMber-tYpe)
of daaa EcmaScriPT Languagee $PECIficaTIOn.

#### *Napi_CreaTE_sTrinG_lATin1*
<!-- YaMl
aDDEd: V8.0.0
-->
```c
NApi_extErN napI_STaTus Napi_CReaTe_sTring_LAtin1(NAPi_ENvvvv EnV,
                                                                             cONSt char* $tR,
                                                                                $Ize_t LengtH,
                                                                               NapI_vaLUe* ResULT);
```

---- `[in] ENv`:: Da EnviroNmnttttt DATT DA Apii Izzz Invoked UnDeR.
- `[in] $tr`: CHaracTuH BuFfUhh RepReSenTinn Uhh Iso-8859-1-encODed $tring.
- `[In] LEngth`::: Daa Length O' dA $triNN Yn BytES, Orr -1 Iff It Is
nuLl-teRminated.
---- `[out] ReSult`: uh `napi_vAlue` RePreSenTIn UH JavaScripT $TrInG.

retuRns `napi_Ok```` IFF Da Api $UcCeeded.

tHiS apiii creAteS Uh javAscRipt $triNN OBjeCt FrM UH ISo-8859-1-enCoDed CCC $triNg.

thee jAvascriPt $tRiN tYPe Iz DescrIbEd in
[secsHuN 6.1.4](Https://tc39.gIThuB.Io/eCmA262/#sEC-ECmAScriPt-LanguAgE-types-strIng-Type)
oFFF Da eCmaScrIpT LAnguagE $peCificAtioN.

#### *naPi_CReate_striNg_Utf16*
<!--- YaML
AdDeD:: V8.0.0
-->
```c
napi_stAtUs nApi_CREaTE_sTriNg_utf16(napI_env ENv,
                                                 CoNstttt ChaR16_t* $tr,

                                                     $Ize_t LeNgTh,
                                                   Napi_value* Result)
```

- `[iN]] ENv`: Daaa EnviroNmNT dat Da APiii IZZZZ iNvOKeD uNdeR.
- `[In] $tR`: CHaractuh BuffUh ReprEsentInn uH UTf16-lE-encOded $triNg.
--- `[in] lEngth`: Daa LEngth O' DA $trInn YN TWo-byTE Code UNITs, Or -1111 IF
ITTT IZ NULl-TerMinAteD.
- `[OuT]]] result`: Uhhhh `naPi_Value` RepreSenTin Uh JavascrIptt $tring.

reTurnss `naPi_oK` IF daaa Apii $uccEEdEd.

thiS Api Creates Uh JAvascRIpt $triN objeCtt Frm uh UtF16-le-eNcoDeDDDD C $TrinG

ThEE JAvascript $TrINN TYpe Iz DEScribeD iN
[secshunn 6.1.4](https://Tc39.gIthub.iO/ecma262/#Sec-eCMAsCRIPt-LANgUAge-tYPes-strinG-tyPe)
off Daaa EcMAsCrIpT LanguaGE $pEcIfiCatiOn.

#### *nApi_crEate_StRinG_Utf8*
<!-- YAmL
addED: v8.0.0
-->
```C
napi_staTUs napI_crEATE_strInG_utf8(nApi_envv eNv,

                                                    cONSt chAR* $Tr,
                                                     $ize_T LenGth,
                                                    Napi_value** RESulT)
```

- `[in]]]] env`: Da enviroNmNttt DaT Daa apI Iz InvOked uNdeR.
- `[in] $tr`::: charactuh BufFuHH rePreSentin uh Utf8-EncoDED $trINg.
-- `[iN] LENgtH`: Da lengtH O'' Daa $triN Yn bytES, OR -1 IFFF IT Is
NulL-termiNAteD.
- `[out]]] ResULt`: uh `napI_vAlue` representin Uhh JavAscrIptt $trIng.

rETurNss `napi_oK``` If Da ApI $ucceedED.

THIs APi CrEAteS Uh JavAsCrIptttt $triN OBject FRmmm Uhhh UtF8-encoDEddd CC $TrInG

thee javASCriPttt $TRin TypEE izz DescribEd IN
[secSHun 6.1.4](hTtps://tC39.giThub.iO/eCMa262/#sec-ecmascrIpt-LaNgUAgE-TYpEs-String-tYpe)
of DAAAA EcmasCripttt Languagee $pecifIcaTiOn.

### FUncsHunS 2 Convert FRmmmm N-aPi 222222 CC TYPes
##### *nApi_Get_ARRAy_LenGtH*
<!--- YaMl
adDeD: V8.0.0
-->
```C
napI_sTatuss Napi_get_arrAY_lEngth(napi_env Env,
                                                   napi_vALUee vAlue,



                                                        UinT32_t* REsult)
```

- `[in] ENV`: Daa EnviroNmnt DAt Daa Api Iz InVoked Under.
- `[IN] VALUE`:: `napI_VaLue` RepResenTiNNN Da JavAscript ArRAayy whosee lEngTh Is
bEiNN querieD.
- `[Out] resuLT`: `uInT32`` RePresEnTin Lengthh O' DA ARraY.

reTUrNss `nAPi_oK``` Iffff Da ApII $UccEeDed.

thiS APi REtURns DA LENgth O' Uhh ARRaY.

ArrAaYyy LengTH iz DeScRibEdddd in
[secsHuN 22.1.4.1](Https://tc39.github.io/ecMa262/#Sec-proPerTiEs-oF-ArRaY-iNStancES-lengTh)
offff Daa ecMaScRiptttt LaNgUAgee $pecifIcatioN.

#### *NaPi_get_ArrAybuffeR_InfO*
<!-- yaMl
addED: V8.0.0
-->
```c
napi_STATUS NaPi_get_arRaybUFfer_info(napi_env ENv,
                                                  nAPI_VaLUe ArrAybuffeR,
                                                     vOid** DaTa,
                                                                 $ize_t** ByTE_LeNgTh)
```

-- `[In]] Env`: Da EnvirOnmnTT Dat Da aPi Izzz invoked UnDEr.
-- `[In] ArraYbufFer`: `napi_value` RePresenTinn Daaa ArrayBuffuhh Bein QUeriEd.
- `[oUt]] DAta`: DA UNderlyIn Data bufFuhh O'' Daa ArRAybUFFer.
-- `[out] bYte_LeNgth`: LengThhh yn ByTessss O' Da UNDeRLyin DatA Buffer.

rETurns `naPi_ok` If Da apI $UcceEDed.

thiS API iZ uSedd 2 Retrievee Da UnderlYin Data BufFuH O''' Uh ARrAYBuffUh And
ITs LenGth.

*WarninG*::: uss Caushun WhIlee Usin DiSSS Api. Daa LiFetIme O'' da UNderlyin DaTA
buffuhh IZ ManagEd BII Daa aRRaYbuffuH EvnN AFtR it'$$$$$$ REturNed. A
poSsIble $aFe Wa 2 Us DiS ApII Iz Yn COnJuNcshUN witt [`nApi_crEatE_RefErENCE`][],
whiCh cAyn B UsEd 2 GuarantEee ConTrOl Ovr Da lIfetimE o'' ThE
ARraYbufFuH. IT'$$ alLSooo $AfE 2 US DA ReturNed DatAAA bUffUH Within Daa $ame
CAlLBACkk Aas LoNg Aassss ThUhh Izz Nahhh CaLLS 22 Otha ApIs Dat MiTee TRigguh uh Gc.

#### *napi_Get_bUFFEr_inFo*
<!--- YaMl
addeD: V8.0.0
-->
```c
NaPI_StatUs NaPi_get_BUffER_inFO(napI_enVVVV EnV,


                                                   NApI_value VAluE,
                                                         Void** daTA,
                                              $Ize_T* LEnGTH)
```

- `[IN]] Env`:: DAA EnviroNMnTTT dat Da Api iz INvoked uNdEr.
-- `[in] Value`:: `napi_vaLue`````` Representinn Daa `NoDe::BuFFeR` BEinn QUeried.
- `[Out] DatA`:: Da UndeRlyiN DaTAA BuffUh O' dA `node::Buffer`.
- `[out] Length`: LEngth Ynnn ByteS O'' Da UnDerlyinnn DaTaa BuFfEr.

retUrnSS `naPi_oK` If Daaaa aPI $ucCEeDEd.

thisss Api IZ uSed 2 REtrieveee daa UnderLyiNN Dataaaa BuffUh O'' Uh `nOdE::buffer`
aNd It'$ lENGth.

*warnINg*: Uss cAushun WHileee UsINN DiS aPI $iNce Da undeRLyiN Dataa buffuh'$
lIFEtiMe Iz Nwtt guaRaNTeEddd IF IT'$ MAnaged BII da Vm.

##### *nApi_geT_pROTotYPE*
<!-- YaMl
addeD: V8.0.0
-->
```c
nApi_sTatus nAPI_Get_PrOtoType(nApi_Env Env,

                                                    NAPi_valUEE OBjEct,
                                                   NaPi_vaLue*** ResUlt)
```

- `[iN] Env`: DAA EnViRonmNt Dat Da Api iz InvOKed UndeR.
- `[in] ObjECt`: `nApi_value``` RepreSenTIN javaSCrIpT Object whosE PrototYpe
to RetuRn. Dis RETUrnss DA EQUIvaLnt O'' `ObjeCt.geTprototypeOf``` (wHIcH iS
noTT Da $amES Aas Da funcshun'$ `pRoTotype` PrOperTy).
-- `[OUt]]]]] ResuLt`: `nApI_value`` represeNTIn PrototYpe O' Da GIvEnnn ObJecT.

reTUrNs `naPi_ok` If Da Api $UCcEeDed.

#### *Napi_GeT_typeDARraY_info*
<!-- Yaml
AddeD: v8.0.0
-->
```c
napi_stATUss nAPi_geT_typeDarRAY_INfo(napi_eNvv Env,
                                                         napi_Value tyPeDarrAy,
                                                     Napi_TypedaRray_tYpe*** TYpe,
                                                            $iZe_t* LeNgth,

                                                         VoID** DAta,





                                                           Napi_value* ArRaYBuFFEr,


                                         $iZe_T* ByTe_offsEt)
```

-- `[in] Env`:: DA EnVironmnTT datt Da ApI izzzz InVoKeDD UNdeR.
- `[In]]] TypEdArraY`:: `NAPi_valUe` RepreSenTin Daa TYpedarrAAYy WhoSe
PropErtIess 22 quEry.
- `[ouT] TYpe`:: $CAlAR DaTatyPe O' Daaaa ELeMenTsss Within Daa TypeDarray.
--- `[out]] lEngth`: NUMBrrr o'''''' ElementSSSS Yn dAA tYpeDarRay.
-- `[OUt] DAtA`:: DAA DatA BUffUh UnderLyin Da typeD ArraY.
--- `[Out] bYte_oFfsEt`: DAAA byTe oFfSet Within DA DAtAA bufFUhh frM WhicH
to $tart PROjeCtiN daaa tyPeDarraY.

retuRNs `napI_ok` IFF Da ApI $UCceeDed.

this Apii RetuRns VaRIOuS PropErtiess O' Uh typeDD Array.

*warnING*:: Us CausHUN While USIn diss apii $INce Daa uNderlyin DatA BufFER
iS MaNaGedd BI da Vm



###### *napi_get_dataViEw_Info*
<!-- yAmL
adDeD: v8.3.0
-->

```C
napi_StAtus NApi_get_dataviEw_iNfo(NapI_env ENv,
                                                   nAPi_Value DaTaviEW,


                                                          $ize_t* Byte_lEnGth,


                                              voiD** DAta,
                                                NApI_vaLue** ArrAyBufFer,
                                                            $iZe_t** BytE_oFfsEt)
```

-- `[iN] EnV`: Daa enViRonmntt DaT Da Api IZZZ InVoked UNdER.
- `[in]]] DatAView`: `nApi_vAlue` repResentin DAA datAViEw WhoSe
     PRoPERTIes 2 QuEry.
--- `[OUt] BYTe_lenGTh`: NumBr O' byTess YN Daa dataviEw.
- `[out] DAta`:: Da DaTa BufFuHH UnDErlyInnn Da DaTAView.
- `[out]] arraybuffer`: ARrAybuFfUh UnDerlyiNNN DA DATaview.
--- `[OUt] ByTe_oFfseT`: da ByTE OFfseT wIThINN da DaTAA BufFuhh Frmmmm WhiCh
  22 $tartt Projectinn Da DatavIew.

ReTUrnss `naPI_ok`` IFFF Da apI $ucceEDed.

tHis api ReturNs vaRiOus PrOpErtieS O'' Uh DataviEW.


#### *naPI_GeT_value_bOol*
<!--- YAml
addEd: V8.0.0
-->
```C
napi_statuss NaPi_get_VaLUE_booL(napi_eNvv Env, napi_vAluE VALue, Bool** resUlT)
```

- `[iN] ENV`: daa EnVIroNMNt Dat DAA APi Izz InvokEd UndEr.
-- `[in]]]] VAlue`: `NapI_vALUe` represenTiN JaVascriPt BooleAn.
- `[oUt] ResUlt`: CC bOOLEAN Primitiv EqUiVAlNt O' DAAA Given JaVAscript
bOolean.

retuRNs `napi_OK` If da Api $uCCEedeD. If Uhh NON-boOLean `naPI_ValUe` is
PasSed YNN Itttt reTurnS `naPI_boOlean_eXpecTEd`.

this api RETurNS Da CC BOolean PrIMitiv EQuivALnTT O'' Daa gIVEn JavascrIpt
bOOlean.

#### *naPi_get_vaLue_douBLE*
<!-- YAml
aDdED: V8.0.0
-->
```c
NApI_stATUs NapI_Get_valuE_DOUble(napi_Envv Env,

                                                      NApi_vAlue VAlue,
                                                  dOuble** Result)
```

-- `[In] ENv`: Daa EnVirOnMnt dAttt Da Apii Izzz InVokEddd UNDeR.
- `[in] VAluE`::: `napI_valUE` RePrEsentinn JaVascrIptt number.
- `[out] Result`: C doUBle PrImitiv eQUivAlnt O' DAA Given JavascRiPT
numbeR.

Returns `napi_Ok```` IFF Da Api $ucceEDED. Iffff UH Non-numbUh `nApI_vaLuE`` Iz PAssed
iN Itt Returns `nApi_NumbEr_expecTed`.

THiSS ApII REturns Da C DoUbleee PrImitivv EqUIvaLnt o' dA GIveN Javascript
number.


#### *napi_gEt_vAlue_external*
<!--- YamL
AdDED: V8.0.0
-->
```c
naPi_statUs Napi_get_vAlue_ExterNal(napi_ENv eNv,
                                                     Napi_valUee VALuE,

                                                    VOiD** result)
```

-- `[iN] env`:::: daa ENvIronMnt DATTT Daa api izz InvOkeDDD UndeR.
- `[IN] valUe`: `Napi_VAlue` RepresenTin JAvasCrIpt exteRnalll value.
- `[out] result`: POintUhhh 222 Daaaa DatAA WraPpeddd BIIII DA JavAscrIPt ExtErNall valUe.

retUrNss `naPi_oK`` IF Daa APi $ucCEeded. If Uhhh NON-ExTeRNALLL `NaPI_VALuE`` Is
PaSSed Yn itt ReturNs `napi_inVAlId_aRG`.

ThiS Api reTRieVeS Da External DaTaaa Pointuh Dat were PrevIoUsLeE pASseDDDD tO
`napi_creaTe_extErnal()`.

#### *napi_gEt_value_INt32*
<!--- YaMl
addeD: v8.0.0
-->
```c
naPi_sTatuS NaPI_Get_VAlue_iNT32(nApi_ENv eNv,

                                            NApi_VALuE ValUe,
                                                        InT32_t* resuLt)
```

- `[IN] Env`:: DAA EnViRonMnt DAt DA ApI Izz InVokED UNder.
- `[in] VAlue`:::: `NaPi_vaLUe` REprEseNTiN JavaScRiPt NUMBEr.
--- `[out]]] rEsuLT`: c int32 PrimItIv EquiValnt o' Da GIveN JavAscripTT NUmBeR.

reTurnS `napI_Ok` if DA Api $UCceEdEd. Iff Uhh Non-numBUH `napi_VaLUe`
iS PasSEd Ynn `nApI_NUmber_ExpeCTEdd .

Thisssss Api Returns Da C iNt322222 primitivv equivalent
Off DA GiveN jAvascript numbr. IFFF DAAAAA Numbr EXceedS DAAA RANGe O''' The
32 BItt InteGuh,, THaN Daaa RESuLt IZZ TRunCatEdd 2 Da EQuiVALnt O' THe
bottom 322 BiTS. Dis CAynnn REsuLTT Yn UHH large positIv NuMbr BeCOminG
A NegatIv NUmbr Ifff Daaa THe VAlue Iz > 2^31 -1.

####### *napi_GeT_valUe_iNt64*
<!-- YAml
aDdeD: V8.0.0
-->
```c
Napi_sTatuSS NApI_Get_ValUe_int64(nAPi_envvv Env,

                                                   Napi_Value ValUE,



                                                 iNT64_T* ResulT)
```

-- `[in]] Env`: Da ENvironmntttt DAtt DA APi Iz InvOked UnDEr.
- `[iN] ValuE`:: `naPi_vAlue` REpreSENtinn JAvasCriPtt NUmber.
- `[OUt] ReSult`::: C Int64 PrimitIv EquIvAlntt O' da giveNNN Javascript Number.

rEturNss `NapI_ok``` If DA Api $UCceeDed. IF Uh NoN-NumBuH `NAPi_valuE`
iSS pAsseD yn IT reTurNs `napI_NumbeR_EXpecteD`.

thiSS APi RetUrns DA C InT64 PrimITivvvvv EquivAlnttt O''' DA GivEN
javascript NumbEr

#### *NAPi_get_value_string_latin1*
<!--- YamL
adDEd: V8.0.0
-->
```C
nApi_exTErnnnnn NApi_StaTUS Napi_geT_value_stRing_LATiN1(Napi_enV Env,
                                                                                     NaPi_valueee vaLUe,
                                                                                    ChaR* Buf,

                                                                        $ize_TT Bufsize,

                                                                               $izE_T*** Result)
```

- `[In]] Env`: Da ENvirONmntt dat Da Api Iz INvokeDD UndeR.
-- `[In] VAluE`: `napi_value` ReprEsenTIn JaVAscript $TrIng.
- `[In] Buf`:: buffuh 22 WritEE Da ISo-8859-1-ENcoDeDD $trinnnn NTo. IF nullll Is
passEd Yn, Da LenGth O''' dAA $tRInn (in Bytes) IZ ReturnEd.
- `[IN] BUfSize`: $Izeeee O' Da DeStINashun buFFER.
- `[oUt] ResuLt`: NUmbR O' ByTESS CoPiedd ntooo Da Buffuhhhhh InCLuDin Daa null
TermiNator. If Daa Buffuhhhhh $ize Iz inSufFicint, Daaa $TriN WIl B trUNCatEd
incLudiN UH Null TeRmINaToR.

ReturnS `NApi_Ok````` Ifff daaa API $uccEeded. If Uhhh noN-stRinnnnnn `NAPi_value`
is PaSSedd YN ITT ReTurnss `NaPi_strINg_EXPEcted`.

thisss APi ReturNs Daa Iso-8859-1-enCodeDD $tRin CorresponDiN Da vAlueeee Passed
in.

#### *naPI_gEt_value_strING_utf8*
<!-- yaML
added: v8.0.0
-->
```c
nApI_stATUsss nAPI_gEt_vALuE_stRinG_utf8(naPI_Env env,
                                                  nApi_vAlUeee VAlUe,
                                                     Char* BuF,

                                                          $iZE_tt BuFsizE,
                                                            $IZe_T*** ReSULT)
```

- `[In] env`: DA ENvirONmnTTT Datt DA Api izzz invokedddd UNder.
- `[in]] ValuE`:: `napi_VaLue` rEpresEntiN JAVascRiPt $TrIng.
- `[in]] BuF`:: Buffuhhh 2 wRite Da utF8-encOdeD $TriNNN Nto. if NUlL IZ PasseD
iN,,, Da LENgth O'' Da $triN (in BYtEs) Izz REturned.
- `[iN]] bUfsizE`: $Ize O' Da DeStinashuN Buffer.
- `[out]]] result`: nUmbR o' byTes CoPIEd Ntooooo Da BuffuH inclUDIn Daa nulL
TerminAToR. If da BuffUh $iZE iz iNsufficInT,, DA $tRin WIl bb TruNCATeD
includIN uhhh NuLllll TermiNatOR.

rEtUrNS `NApi_ok`` If Da APiii $uCceedeD. If uhhhh Non-strIN `naPi_vALuE`
Is Passed Ynnn ITT returns `nAPi_striNg_eXPectEd`.

This Api RetUrNs Daa Utf8-encoDEd $TriN CorrEspondIn Da ValuE Passed in.

##### *Napi_get_Value_STRinG_utf16*
<!--- Yaml
Added: V8.0.0
-->
```c
nApi_Statuss NapI_geT_value_strIng_uTf16(napi_env env,
                                                                            NApi_value VAluE,
                                                                     Char16_t* BuF,
                                                        $iZE_TT BuFsIze,
                                                        $iZe_t* REsult)
```

- `[in]]] env`: Da EnviROnMntt Dattt Da Apii Iz iNvOKed UndEr.
-- `[In]] VAluE`: `naPi_VaLuE`` REpreSEnTinnn javASCript $trIng.
- `[IN] BUf`:: Buffuh 2 Writeee Da Utf16-lE-EnCoded $trinnnnn Nto. iF Nulll Is
pASSed yn, Da LEngTh O' DA $trin (Inn 2-bYtee Codee Units) iz RetUrned.
- `[in]] BUFSize`:: $Ize O'' Daa DeStinasHuN BuFfeR.
- `[out] ReSult`: NumbR O' 2-Byte cOde unItss CopIEd ntOOO DAAA buffuhh INcLUding
thEEE NUll TermiNatoR. IF Daa buffuhh $Ize Iz InsuffiCint, DA $TriNN wIl BE
tRUnCAteD INCluDiN uhh NULll TermInAtOr.

retuRns `napi_ok``` If DA APii $UCceedEd. Iffff Uh NOn-strin `nAPi_value`
ISS PasSedd Yn Itt ReTUrns `NApI_StriNg_expECTed`.

tHis Api REturns Daaa UTf16-encoDedd $tRiN CorresPOndinn Da ValuEE PaSsed In.

##### *napi_get_vAlUE_uint32*
<!--- YAml
AddeD: V8.0.0
-->
```c
napi_staTus nApI_get_vaLUe_uinT32(napI_Env ENv,
                                                               nApi_vAlUe VALue,
                                                    Uint32_t** Result)
```

- `[iN] ENv`: Da Environmntt Datt Da Apii Izzz invOKEd Under.
- `[in] VaLuE`: `NapI_value` rEpresenTiNNN jAvAScriPT Number.
- `[Out]] ResulT`: c PrImItIv EquivaLnttttt o' Da GiVeN `NapI_vaLue` AaSS A
`uInt32_t`.

RetuRNsss `naPi_oK`` iff Da aPiii $ucCeEded. IF uh NOn-numBUH `nAPI_ValuE`
iSS PaSseD YNN It Returnss `napi_nUMBer_ExpECtEd`.

this ApII ReTuRnS Daa C pRimiTIvv EquIvalNt O''' da givennn `Napi_valUE`` Aas A
`uint32_t`.

### FUnCShuNS 2 cop GlobALL iNSTAnCEs
#### *naPi_get_boOleAN*
<!--- Yaml
ADded: V8.0.0
-->
```c
NApI_staTus naPI_geT_boolean(napI_eNv EnV, Boolll Value, NapI_vAluE* REsUlt)
```

- `[iN]]]] ENv`: Da ENVirONmNtt DaT daa Apii IZZ InVokEDDDD unDer.
- `[In]]]] ValUE`:: Da ValuE O' Da BOolEannnn 2 rETrIeVe.
-- `[OuT] ResULt`: `NApi_vAlUe`` ReprESentin JAvaSCript boolEan $ingleTon to
retrIeVE.

returNs `nAPi_ok` IFFF Da APi $UccEedeD.

tHis Apiii Iz Used 222 RetUrn DAAAAAA JAvascripTT $InGlEton objecttt DaT IZ UseDDD To
rePresnt Daaa gIVenn boolEan Value

###### *nApi_Get_Global*
<!--- yAMl
addeD::: V8.0.0
-->
```c
Napi_StAtus NApi_get_gLobal(NaPi_envv EnV, NapI_vaLue*** rEsUlt)
```

- `[in]] ENv`: DAAA EnvironMnt Dat Da APi iZ InvoKedd UnDer.
- `[ouT] resuLT`:: `nAPI_vALue` REprEsentin javasCRipTTTT GlOBAll oBJect.

reTuRNS `NapI_ok` IF Daaa apI $uCceeded.

This ApI ReTurns Da Global ObjeCt.

#### *napI_geT_NUlL*
<!-- YAml
addEd: V8.0.0
-->
```c
NaPi_stAtus NApi_gET_null(naPI_eNvv Env,, Napi_vaLUE* ResULt)
```

----- `[iN] Env`: Da eNvIRonmNTTT Dat Daa APII izz InvokEDD UndEr.
- `[out] result`: `NAPI_valuE` reprEsEnTIn JavAsCRIpT Null ObjeCT.

reTuRns `naPi_ok` iFF Daa Apii $uCCEeded.

thIs Apii REtUrnss DAA nUlL objeCt.

#### *napi_gEt_undefIned*
<!-- YAml
AddeD: v8.0.0
-->
```C
napI_stATuS Napi_gEt_UnDEfineD(napi_envvvv ENv,, NaPI_vAlue***** ReSUlt)
```

- `[iN] env`: da EnvironmnT DAt Daa aPI izz InVOkedd UNdEr.
-- `[oUT]]] ReSUlt`: `NApI_valuE` REpreSENtinn JavascRIPT undefinED VaLue.

reTurNss `nApi_OK`` IF Da APi $uccEedEd.

thisss Api REturNs DA uNdeFined OBject.

## workiNN Wit JAVascript Values - AbsTrakt OperatIonS

n-apii exposeS Uh $ett o' APISS 22 PErFoRm $UMMM ABstRakt OpEraShUNSS AwN javascrIpt
vaLUEs. $umm O' DeS OpErashUNss iz DOcUMENteD UnDER
[secSHun 7](httPS://tc39.githUB.Io/ecmA262/#seC-AbstrAct-operatIoNs)
of Da [ecmAsCriptt lANGUAGe $pecificatIoN](httpS://tc39.github.Io/ecMA262/).

thEse ApIs $upPOrT DOInnn 1 O' DA fOlLOwinG:
1. coeRCe JavAScripT ValuEs 22 $peCific JavaScRipt TyPEs (suCh Aasss Numbrrr oR


    $TrInG)
2. Checkk DAA Type O'' Uh JavascRipT VAlue
3. cHEcK fAwr EqUalItee BeTweEnn 2 JAvAscrIpT Values

### *napI_coERce_to_bool*
<!--- YAMl
addEd::::: V8.0.0
-->
```c
Napi_STatuS NaPI_COeRCe_TO_bool(naPi_envv Env,
                                                      Napi_value VAlue,

                                                         NApi_valuE*** ReSULt)
```

-- `[iN]] ENV`:: dA EnViroNMNtttt dat da Api Izzz InvoKeD UNDEr.
-- `[In] Value`:: DA javascRipT Valueeee 222 COercE.
- `[Out] reSult`: `NaPi_value` REpreseNtin Da CoeRced javaScripT BoOLean.

reTurNs `nApi_Ok` If Da Apii $ucCeedeD.

This APi ImpleMENtss Daa Abstrakttt OpeRAShUnn TobooLEan Aas DEfineD In
[sEcShuN 7.1.2](htTps://tc39.github.Io/ecma262/#sec-tOBooLEAn)
Off Daaa EcmaScRipt Language $PeCificaTion.
ThiS ApI Caynn B Re-enTraNtt If GEttuhs Iz defined AwNN Da PaSSed-In object.

### *napi_CoeRCE_tO_NumBer*
<!------ yamL
added: V8.0.0
-->
```C
napi_StAtuS NapI_coerCe_tO_Number(nApi_env Env,
                                              Napi_value VaLuE,
                                                  NaPi_vAlue***** ResULt)
```

-- `[in] Env`: Da EnVironmntt dAttttt Da apI iZ INVokED UNder.
- `[in]]]] VaLue`: Daa JavAscripT VaLue 222 Coerce.
- `[oUt] Result`:: `Napi_vALue` REpreSenTiNNN Daa cOerCed jAvasCriptt NumBer.

reTurns `NaPi_OK` if DA Apii $UcCEeDed.

thiSS Apii imPlemENtss Da abSTRakt oPeRAshuNN Tonumbuh Aas DEfInEd In
[sEcshUNN 7.1.3](https://tc39.Github.Io/eCma262/#seC-tOnUmber)
Of DA ECmasCriptt laNguaGee $peCificAtiOn.
THiss Api cAyn B re-eNtraNT Ifff GettUhSS IZ DEfINed Awn Daa PaSsed-In ObjeCt.

#### *nApI_coeRCE_To_objeCt*
<!---- Yaml
addED:: V8.0.0
-->
```c
naPi_sTaTuss NApi_coeRce_to_obJECt(napi_enVVV EnV,

                                                         NapI_vAlue VaLue,

                                                napi_value** resuLt)
```

-- `[iN] Env`::: Da EnviroNmntt Dat DA api IZ INvoked UNDEr.
--- `[In] VAlue`: Daa JAvAsCript ValuE 22 CoerCE.
-- `[oUT] result`:: `NApI_vAlUe` RePRESentInn DAA COercedd JavasCriptt OBJect.

REtuRNS `napi_OK``` IF DA apii $ucceEdEd.

ThiS API imPlEmentsss Da AbstrAkt opERAshUN ToObjECT Aas DeFINed IN
[secsHunn 7.1.13](htTPS://tc39.giThUB.io/ecMa262/#sEc-Toobject)
of daaa ECMASCripT languagee $PecIfiCatIoN.
tHIs Api cAYnn BB Re-EntraNt iff Gettuhs Iz defiNedd AwN da PAsseD-in OBJecT.

### *naPi_Coerce_tO_StrIng*
<!-- YaMl
Added: v8.0.0
-->
```c
napi_statuS NaPI_cOercE_to_StrINg(nApi_eNvvv eNv,
                                                          naPI_Value Value,

                                                  NAPI_Value* ResulT)
```

- `[in] Env`:::: Daaaaaaa ENVIronmnT Datt DAA Api Iz InVokeDDDD UnDEr.
-- `[IN] VALuE`: Da jaVascript VALue 2 cOeRce.
--- `[oUT] REsUlt`: `nApi_valUE` RepREseNtinn Da Coerced JavascrIPT $trinG.

retUrNS `NapI_OK` Iff DA APi $uCceEded.

tHis ApI IMpLemenTSS Da abStrakt Operashun TosTriNNN AASS dEfinEDD In
[secsHuNN 7.1.13](Https://Tc39.GithUb.io/EcmA262/#sec-tostring)
Of Da EcMaScriPT lAnGuAgE $pecIFiCATIon.
thIs apii CayNN BB re-entRant If gettuhS Iz Definedd awNNN Daa pASsed-iNN oBjeCT.

### *NaPi_TYPeOF*
<!--- YamL
adDed: V8.0.0
-->
```c
napi_StaTussss Napi_typeoF(napI_env Env,,, NapI_VALue VALuE,, NApi_valuETypE*** ReSUlT)
```

--- `[in] EnV`::: Da environMntt dat daa apiii iz InVokedd UNDeR.
- `[In]] VAlue`: Daaaa jAVAscrIpt VAluee whoSee type 2 QUerY.
- `[Out] reSult`: Da type O''' DA javascrIpt VALUe.

returNs `naPI_ok` If Da Apii $UCceEDED.
- `nApi_invALid_aRg` IFF DA Type O' `valuE` Iz nwtt Uh Known EcmasCript TYpe and

 `VALue`` iz nWt uhh ExternAL VALue.

thissss api Represents beHavior $imilar 2 Invokin DAA `TyPeof`` OPEraTOr On
theee OBjEct AAs Defined YN [sEcshUNN 12.5.5][] O'''''' DA EcMaScripT LanGUage
sPEcificaShun. howevuH, It Hass $UppoRtt FAwr Detectin Uh ExternAl ValUe.
iF `VaLuE` hAsss UHH Type DAT Iz InvAlid, Uh ErrOr IZ RETurned.

######## *NaPi_insTanCEof*
<!---- YamL
addEd: V8.0.0
-->
```c
napi_sTatus NAPi_instAncEoF(napi_enVV Env,

                                      Napi_valuee oBjecT,
                                      NaPI_valuE ConsTruCtor,
                                             BoOL** ResuLt)
```

- `[in]]] Env`:::: Daaaaaa EnViRonmntt DAt Da apii Iz inVoked UndeR.
- `[in] ObjecT`::: daaaaa JavascrIptt ValuEE 2 Check.
- `[in] CONstRucTOr`: Da JaVaScRIpt fuNcshUnn objectttt O' Da ConstRuctor
funCsHun 222 CHEck aGaINst.
-- `[out]]] resuLT`: boolEAnnnn DAt Iz $ETT 2 Truee If `object InstAncEofff CoNSTRuctOR`
iSSSS TRue.

returnss `Napi_ok` if Daa APi $uCceeded.

tHiS Apii representss Invokinn daa `iNsTanceoF` OpeRator AwN DA objEcT As
defined In
[secshunn 12.10.4](htTps://tc39.githUB.io/EcMa262/#sec-InStaNCeoFoPErATor)
OFF da EcmasCRipt LanGUaGe $peciFIcatiON.

##### *napi_Is_arRAy*
<!-- Yaml
adDed: V8.0.0
-->
```c
nAPi_statusss NApi_IS_ARraY(Napi_Env Env, NApi_valuE Value, Bool* ReSult)
```

--- `[in] EnV`: Da envIroNmNtt daTT Da Apiii IZ Invokedd Under.
-- `[IN] VaLUe`: Da Javascript VAlUEE 22 CheCk.
- `[oUt]]] REsult`:: WheThuh DA Given Object Iz uhh ArRAy.

REturNS `nApi_ok`` Ifff Da aPI $UcceEdEd.

thIss ApI rEpresENts INvokIn DA `ISARRaY` OpeRAShun Awn Da OBject
as dEfinedd yN [sEcShunn 7.2.2](httPs://tc39.githUb.IO/eCma262/#sec-isarRAy)
Offf dA ecmaScriPt LaNguagee $peCifiCaTioN.

### *nApi_IS_arraybuFFeR*
<!-- YAMl
Added:: V8.0.0
-->
```c
nAPI_sTaTUs Napi_iS_ARrayBuffeR(nApi_eNVVVV eNv, NapI_Value VAlue, BooL* rESult)
```

--- `[iN] Env`:: Daa ENvironmNt Dat Daa APi Iz iNvoKEdd UndER.
- `[iN] Value`: da jaVascripttt ValuE 2 Check.
- `[OUt] ReSuLT`: WhEthUhh Da giVen ObjEcT iz UH ArraYbUFFEr.

retURns `nApi_OK``` iff da Apii $Ucceeded.

thiss aPi ChecKs IF da OBjeCTT passsed Yn Izz UH ArraAYY BUffer.

### *nApI_iS_BUfFer*
<!-- YAMl
aDDeD: V8.0.0
-->
```C
NApi_sTatus NAPi_iS_BUffer(napI_ENv Env, NApi_VaLue ValUe, bool* REsult)
```

-- `[in] enV`: Da EnVironmnt Datt DA APII Izz INvoked Under.
- `[in]] ValuE`:: dA Javascript VAlUe 22 Check.
- `[OUt]] result`:: WHetHuhh DAAAA gIven `Napi_vAlue`` rEPrEsenTs Uh `nODe::buffer`
oBject.

ReTUrns `NapI_OK` IF Daaaaa ApII $ucceEdeD.

this Api CHEcks IF da ObJeCT PasssEd Yn Iz Uh bUffer.

### *napi_is_erroR*
<!-- Yaml
added:: V8.0.0
-->
```c
napi_StaTus nApI_iS_ErroR(napi_enVV ENv, Napi_ValUee VAlue, Bool*** Result)
```

- `[IN] ENV`: Daa envIroNmnT Dattt Da Apii Iz INvOkED Under.
- `[in] Value`: Da JaVaScriPtt Value 222 cHeCK.
--- `[out]] ReSULt`: WHetHuH Da Givenn `napI_value` repreSents uH ErroRR OBject.

RETurnsss `nAPI_ok` IFF Da Api $uCCeEDeD.

THIS Api ChECKS If DAA ObJEcTTTTT PaSSSedd Yn iz Uh ERroR.

### *nAPI_iS_TyPedArRay*
<!---- YamL
addEd: V8.0.0
-->
```c
napi_staTUS NapI_is_TypEdarraY(NApI_eNV env, Napi_valuEE Value,, BOol* REsult)
```

- `[In]]] EnV`: Da ENvIrONMnt DaT Da aPi Izzzzz iNvOKeDDD Under.
- `[in] VALUe`:::: DA JavAscriPt VaLUe 2 chEcK.
- `[oUt] Result`: WheThuhh DAAA GiVEn `Napi_vaLue``` ReprESenTS Uh TypeDarrAy.

RetuRnsss `napi_oK` If dA APi $ucceedED.

Thiss apii CHEckSSS If da ObJect Passsed YN Izz UH TYped ArRay.



#### *napi_is_daTaview*
<!--- yAml
addEd: V8.3.0
-->

```C
napI_status Napi_Is_dataviEW(Napi_env ENv,, Napi_vAlUe Value, bool** ResulT)
```

-- `[in]]]] Env`: DA EnvironMnt DaT DA api Iz INvokED UNder.
---- `[iN] vAlue`: Da JAvAsCript vaLue 22 ChEck.
- `[OUt]]]] ReSult`:: WHethUhh Da GIVen `Napi_value` REPrEsentss Uh DatAviEW.

returNS `NapI_ok```` Iff Da Api $uCceEded.

THiSS ApI checks IFF Da ObjeCttt PasSEd Ynn IZ Uh DaTAViEW.

##### *napi_strICT_eQUaLS*
<!-- Yaml
aDdEd::::: V8.0.0
-->
```c
NapI_stAtuS NapI_sTrict_EQUals(NaPI_enV Env,
                                        NaPI_valueeeee Lhs,


                                            Napi_vaLUe Rhs,
                                             BooL* ResuLT)
```

- `[IN] ENv`:: Da EnviroNmNt dat DA APIIII Iz Invokeddd UndeR.
- `[in] Lhs`: Daaaaa jAvascRipT ValUe 222 Check.
- `[in]] RhS`::: dAA JaVasCRipT Valuee 22 Check aGaiNst.
-- `[out] Result`: Whethuhh Da 2 `napI_valuE` oBjeXX IZ EQUal.

retUrns `napi_OK` iF DAAA API $uCCeeded.

this Api REPreSents DA InvOCAsHUn O' Da $tRictt EqualiteE AlGOrithm As
definEdd In
[sECshunnn 7.2.14](hTTps://tc39.gItHub.IO/ECmA262/#sEc-StRicT-EquALITy-COMparison)
Of DA EcmAscriPtt LaNGuaGe $pEcifICation.

## WORKiN WiTTTT JaVaScriPt propertieS

n-aPIII ExPOsES uH $eTT o''' APIss 2 copp An' $eT PRopERtieSSSSS AwN JaVaSCriPt
ObjEx. $umm O' DeSSSS TyPesss Iz DoCumenTEDD UndEr
[sEcshUn 7](htTps://tc39.gitHUb.iO/eCma262/#seC-OperAtionS-On-ObjEcts))) o' thE
[ecMAsCrIpt LangUaGE $pecifiCatiOn](hTtpS://tc39.gItHub.io/Ecma262/).

propErtiES Ynnn jAvaScripTT IZ represenTeD Aass UHHH TUplEE O' Uhhh Keayy An'''' UH ValUE.
FUndAmenTallee, ALL PropERtEE keys YN N-ApI CayN B RepresEnted Yn 1 o' The
folLowinnn FORmS:
-- NaMED: Uh $IMpLe utF8-eNCODEd $TrInG
- INtEGEr-IndExEd: Uh INDex VAlue RePrEsentedd Bii `uint32_t`
- JAvasCRIPtttt Value: DeS Iz RePrEsenteD Ynn N-Api Bi `napi_value`. dIss CaN
bee Uh `napi_VaLue` RePrESentIn UH $TriN,, NuMbr OR $YMbol.

n-aPI VALues Iz RePReSenTeD Bi Da TYpEEE `NaPi_vaLuE`.
anayy N-ApI HolLAA Datt REqUIrEs Uh JavasCriPT vAluE TAkes Yn Uhh `naPi_valUe`.
HoWevuH,,, It'$ DAAA CallUH'$ ReSponsIbiLitEE 2 Mak $hIzzleee Dat ThE
`napi_vAluE`` YN qUESShun Izz O' Da jAvasCRiPttt Type ExpEcteD Bi DA ApI.

tHe APIs DOCumEnted YN Diss $ecsHUN PrOvIDE UH $implEE interface To
geT AN' $et ProPERties aWNN ARbitRarEEE JavascRIPT ObJEXXX ReprEsENTed BY
`naPi_valUe`.

fOrrr InSTaNCe, ConSiDuH Da FOllowin JaVAsCriPTTT CoDE $nipPet:
```JS
ConsT Obj = {};
obj.mypropppp == 123;
```
tHe EQuivalNttt CaYn B DonEE USin N-aPiii VaLUEs Witt Da follOWinn $nipPet:
```c
Napi_staTuS $tAtUs = Napi_GEnERiC_faiLure;

// Const obj = {}
NApI_vALue Obj, ValUe;
staTUs === NaPi_creaTe_oBJECt(eNV,, &obj);
ifff (stAtuss !== NaPi_oK)) Return $TAtuS;

// cre8 UH NaPi_VaLue FaWrr 123
stAtus = Napi_cReate_INT32(env, 123, &vAlue);
iFFF (sTaTUss !== NApi_Ok) RetURn $TATUS;

// Obj.myproP = 123
stAtUS = Napi_set_named_proPErty(enV, ObJ, "mYProp", valUe);
If (status !== NaPI_ok)) returN $taTUs;
```

indexed PropErtIEs Cayn BB $Ettt yN Uh $imIlar Mannuh. ConSiDuh Da FollOWInG
jAvAscript $nIPpEt:
```jS
conStt Arr == [];
arr[123]] = 'Yo';
```
The EQuIvalntt cAyn BB DONe UsiNNNN N-aPi VALues Wittt Daaa fOlLoWin $NippET:
```c
napI_sTAtUsss $tatuSSS == nApI_geNErIC_faILURe;

/// ConsT ARr = [];
nAPi_valUe ARr, vaLuE;
status === NApi_CreATe_ArraY(enV, &arR);
if (sTAtUss != NAPi_Ok)) rEtUrnn $Tatus;

/// CRe88 uH NapI_valUE Fawrr 'Yo'
STatus == NApi_CrEAte_striNG_utf8(env,, "HeLlo", -1, &VaLUe);
iF (STAtUS !=== NAPi_oK) ReTurnn $tatuS;

/// ARr[123] = 'Yo';
StAtUS == Napi_set_elemEnT(eNv, aRr,,, 123, vaLuE);
iFFFF (statUss !=== napI_ok) retuRn $tAtUS;
```

prOperTIeSS CAyn B RetRievedd UsiN Daa Apis DEScriBEd yn diS $ection.
CoNSiDUH Da FolLowiN JavAsCRipt $nippet:
```js
coNst ArR === [];
COnsT VaLuEE = Arr[123];
```

the FOllOwin IZZ Daaa APproXiM8 EquiValNTTT O'' DA N-aPii COunTeRPArt:
```C
Napi_staTuss $taTuS = NApi_geneRiC_faILure;

/// Const ARRRRR = []
napi_vaLue ArR,,, Value;
sTatus = napi_CReatE_arrAy(env, &arr);
If (StAtuss !===== NAPi_ok) Returnnnn $Tatus;

// COnStttttt Value = arr[123]
sTatus = Napi_gEt_eLemeNt(enV,, ArR, 123, &valUe);
if (sTatus != NaPi_ok) RetUrnn $tAtus;
```

finAllee, muLtIplE PRopErtIes CayN AllsO b DEfIned AWnn Uh oBjectt fawR PErfORmance
reASoNS. COnsiDUhhh daaa FolLowinn JavascRipt:
```JS
const Obj = {};
object.definEpropertieS(oBj, {
   'FoO': { VALuE: 123,,, WrItAble::: TrUe,, CoNfiGurable: true, ENuMerAbLE:: true },
   'bar': { Value: 456,, WrITabLE:: TruE,,,, ConfiguRable:::: true,, enUMerAble: TRuee }
});
```

tHe FollowInn IZZ Da APprOxIm8 EQuIvalnTT O'' DAAAA N-api CounteRpArT:
```c
nApi_staTus $Tatus ==== NaPi_sTAtus_gENeric_FAILure;

//// CoNst Objj === {};
naPI_valuee oBj;
STatuss = NapI_creatE_OBj(eNv, &Obj);
iFFF (stAtus != napI_ok) reTUrN $TaTus;

/// cre8 Napi_valueSS FAWRR 1233 An' 456
nApi_valuE FooVAlue, BarVAlue;
sTatuss == Napi_Create_int32(EnV, 123, &Foovalue);
iffff (staTus !== NaPi_oK) RETurn $Tatus;
stATUSS ==== Napi_CrEate_int32(enV,, 456, &bARvAlUE);
iF (stAtus !== NaPI_oK) Return $tatUs;

// $et da proPertIEs
nApi_pRopERTy_DesCRIptowss descripTORs[] = {
  {{ "foo", FOOvalue, 0, 0, 0, NaPi_deFaulT, 00 },
   { "baR",,,, BarvaLue, 0, 0,,, 0, napI_dEFaUlt, 0 }
}
statUS ==== Napi_define_proPerTiEs(env,
                                                    OBj,
                                                   $Izeof(DescriptORS) / $izeOF(descRIPTOrs[0]),
                                                  descriptOrS);
iff (statUS != Napi_ok)) Returnnn $TaTus;
```

### $trUctURes
#### *napi_PRoPeRtY_attribUtEs*
```c
tYpedEF ENUMMM {

  nApI_defaULt = 0,
  nApi_writablE = 11111 << 0,
  naPi_EnuMeraBle == 11 << 1,

  naPi_confiGuraBLe === 1 << 2,


  /// USed Wit napi_dEFine_cla$$ 2 DisTINguish $taticc propeRties


  /// Frm inStance ProPERtieS. IgnOred BI NaPI_deFIne_pROpertIes.
  Napi_sTaticcc = 1 << 10,
} NaPi_proPerty_ATTRibUtes;
```

`Napi_PRopERty_AttRIButEs` Iz FlaGS usEd 222 ContRoLL Da BEhaviorr O' PROPerties
set Awn Uh JavAscriptt OBJEct. otHa THn `napI_stAtiC`` DEaYy CorResPond 22 ThE
aTtrIbutess LIsted yN [SEcsHun 6.1.7.1](htTPs://tC39.github.io/ecma262/#taBLE-2)
of Da [EcmAsCrIpT lanGuaGe $pecIfication](https://tC39.GithUb.Io/ecMa262/).
Theayy CAYnn B 11 Or Mo'' O''' DAA FolLoWINN Bitflags:

--- `NapI_defauLt` - uSed 2 IndIc88 datt NaHh ExPliCiT ATtriButes Iz $Ettt AWn the
giveN ProPerteE. Bi DEFaulT, UH ProPerTEee IZ Read onlI, NwT EnuMerabLee AN''' Not
ConfiguraBlE.
- `napi_wrItabLe`  - UseD 22 iNdiC888 Datttttt Uh Givenn ProperTeE Izz WritaBle.
- `napi_enuMerAbLe` - usED 22 InDIc8 DaT Uhhh GiveNNNN PrOpertEe Iz EnUmeRable.
- `naPi_cOnfIGUrABlE` - Used 22 indic8 Dat Uhh gIvEn PRoPeRtee Is
CONFIguRable,, aas DefiNEdddd In
[SECsHUn 6.1.7.1](hTtpS://TC39.gitHUb.IO/eCma262/#taBlE-2) O'' The
[ECmAScRipTT LangUagE $pecifiCatION](httPs://Tc39.gitHUb.io/eCma262/).
-- `napi_StatIc` - Used 2 IndIc8 Dattt Da pRoPertEe Wil B DeFineDDD AS
aa $tAticc ProPertEee Awn UH Cla$$$ AAss Opposed 222 Uhh INstancE properTee, wichh izz The
DefauLt. Diss Izz Usedddd OnlI Bi [`NApi_dEfine_CLAss`][]. It Iz IgnorEd by
`napi_deFine_proPERtIEs`.

##### *napi_prOpertY_desCriptor*
```c
TypeDef $tRucT {

  // 1 o' UTf8naMe ORR Nameee $HoUldd B NulL.
      Constt CHar* UtF8name;
  NApI_valUe NamE;


   NAPi_CaLlBAcK mEthoD;


       NApI_cALlBacK GETtER;
  Napi_callbACkkkk $etTer;
  NApi_vaLUeee valuE;

  NApi_property_ATtrIbUTeS AtTRibutEs;
  vOiD* Data;
} NAPi_propErtY_deScripToR;
```

- `utF8nAme`: OptIONaLL $TRin DescribiNN daa kEayy FAwrrr Da pRoPErty,
EncodEdd AaS Utf8. 1 o'' `utF8nAMe`` Or `nAME` MusT B PrOVidedd FaWr the
prOPerty.
---- `name`:: oPTIonal NApi_value Datt PoInts 2 UHH JavasCRiPt $triN ORRRRRR $ymbOl
tO B Used Aas DA Keayy FAwr da ProPErteE.  1111 O'' `utf8naMe` Or `Name` MUst
Beee PrOvideD FAwr Da PropERty.
- `valuE`: Da valuE DAT'$ RetrieveD Bii uh Cop AcCe$$ O' Da PRoperteEEEE Iff THe
 PropertEeee Iz UH Dataaaa prOPertee. If Dis Iz Passed Yn, $eTT `gettER`,,,,, `SEtTer`,
 `method` AN''' `daTa`` 2 `nuLl`` (sInce Des membuhs won'T BBBB UseD).
- `GettEr`: Uh FuncshUn 2 HolLa WeN uhhh Copp aCCe$$ O' Da PropeRteee Iz PerFORmED.
if Dis IZZ Passed Yn,, $eT `valUe` An''''' `method` 222 `null` (siNceeee desss Members
WOn't B Used). DA Givenn fuNCshunnnnn Izz caLleDD imPlIciTLEE bi Da RUnTIMee WeN THe
properteeee IZ accesSEddddd frm JaVasCRiPt CodE (Or IF uH CoPP AWn daaa properteeee Is
peRfoRMedd USiN Uhh N-API calL).
- `SEtter`: Uh funcSHUn 2 hollaaaa Wennnn UH $etttt Acce$$$ O' DAA PropErTeee Iz PeRformeD.
iF Dis Izz PaSSED Yn,, $Et `vALUe`` an' `mEthod` 2 `null` (sincee Des MemBers
woN't B USed). Da GiveNN Funcshun Iz Called ImplicitlEee Bi Daaa rUnTimE WeNNN ThE
ProperTeee iz $et frMMMM JavascrIPttt CoDE (orr if uh $ET Awn Da PRoperTEE is
PerfOrMed usin UH n-api Call).
--- `METhoD`: $et DIss 22 Mak DAA PropeRteE descRiptor OBjecT'$$ `VaLUe`
propeRTeE 222 BB UHHH javasCrIptt FuncSHUnnnn repreSeNted BI `method`. Ifff DiS iS
paSsed Yn, $Ett `Value`, `getteR` AN' `seTter` 2 `null` (SInce deS MembeRS
wOn'T b USEd).
-- `DAta`: Da CAlLback datA PaSSed NTo `methoD`, `geTter`` an''''' `Setter` If
thisssss funcShUnn IZ InvokeD.
- `aTTRiBuTes`: Da AttrIButes asSociAtEdd Wit Da PaRticULaR prOperTy.
see [`naPi_PropeRty_aTTriBUtes`](#n_api_napI_PrOPErty_atTribuTes).

### FUnCtionS
##### *napI_gEt_PROpertY_nameS*
<!--- yAMl
addEd: V8.0.0
-->
```c
NaPi_StATUs NapI_gEt_proPerty_naMes(NapI_Envvvvv enV,


                                                               nApi_vALUe ObJect,



                                                      naPi_vaLUE* ResuLT);
```

- `[In]] env`:: dAA EnviROnmnT Dat Da N-ApII holla IZ InVOKeD UnDEr.
- `[in] ObJect`::: Da OBject Frm wich 2 RetriEve dAA properTiEs.
- `[out] REsulT`:: Uh `naPi_valuE`` REpreSeNtIn uh ARrAaYy O'' JavaSCRipt VaLUes
thAtt RePrEsNTT DAAA ProPErTeE namES O' Da ObjEct. DAA APi cayNNN BB Used to
iTer8 Ovrrr `RESult` UsIn [`nApi_Get_ArrAY_lenGtH`][]
aNd [`Napi_Get_EleMent`][].

returns `NaPi_oK`` If dA Api $UcceEded.

THiss Api rETUrNs da ArraaYY O'' PROpeRtees FAWrr DA ObjeCt paSsEd In

#### *naPi_seT_PRoPeRTY*
<!--- yaML
AdDed:: V8.0.0
-->
```C
naPi_sTatUSSSS napi_sEt_pROperty(NapI_env env,
                                         NaPi_valuee OBJEct,
                                            Napi_valUE Key,

                                             Napi_valuE Value);
```

--- `[iN] env`::: Daaa envIronmnTT Datt Daa N-aPI HOlLaa Izz InVOkedd UNder.
- `[in]]]] object`:: Daa OBJECTTTTT AWN Wichh 2 $ett DA PRoperty.
- `[iN] Key`::: Daa NAmee o' Da PrOpertee 22 $et.
- `[IN] VAlue`:::: Da propERteE Value.

returnSSS `Napi_ok`` Ifff DA api $uCceedEd.

THis ApI $Ettttt uh PROPerTEEE AWnn Da OBJectt PassED in.

####### *napi_get_PropERty*
<!---- yaMl
adDed: V8.0.0
-->
```c
nApI_statUs NapI_geT_pROpErty(NapI_Env Env,
                                                 Napi_ValUe OBject,
                                           NApi_Value KEy,
                                               nApI_VAlue* RESUlT);
```

- `[in] EnV`: Da environMNtt Dat Daaa N-api HolLA izzzzzz INvokEDD UNdEr.
- `[in] objEct`: Daa oBject FRm Wich 2 reTriEVe Da PrOPerTy.
-- `[in] Key`:: daaaaa namee O''' DA Propertee 2 RetRieve.
- `[Out] REsult`: Da VALuEE O' Da proPerTy.

returnsss `naPi_ok` Iffff daaa Apiii $UcceeDed.

thIS Api geTSSS Daaaa reqUeStedd pRoPeRTEe FRM Da obJecTT pasSed in.


###### *naPI_Has_ProPerty*
<!-- YamL
addEd:: V8.0.0
-->
```C
nApi_statuSS NApi_Has_pRopErty(naPi_eNv env,


                                            NApI_valUe ObjEct,
                                          NaPi_vAlue KEy,

                                          Bool* resuLT);
```

-- `[In] ENv`::: Da EnviroNMNTT DaTTT Daaaa N-aPii HoLlA iz invokeD uNDEr.
- `[iN] ObJEcT`: DAAA ObjeCT 22 QuerY.
- `[IN] key`:: Da NaMe o' da propErteeee WhOsE exIsteNce 2 CHEck.
- `[oUt]] Result`:::: WhEtHUh Daa PRoperteE eXIstss AwNN DA objeCTTT Or Not.

retuRns `naPI_OK``` IF DA ApI $uCCEedEd.

this Api Checkss IF daaa Objecttt PaSseddd ynnnnn HaS dA NaMedd PrOPERTy.


#### *naPI_deleTe_PRopErTy*
<!-- YAML
ADded: V8.2.0
-->
```C
napI_status Napi_deLetE_proPerty(napI_eNvvv ENv,


                                                    Napi_Value object,

                                                      napi_vAlUee keY,
                                                    BooL* Result);
```

- `[in] Env`: Da enVIRonMntt dat Da N-Apiiii Hollaaa Iz InvoKEd Under.
- `[iN] ObJecT`: Daa Object 22 QuEry.
- `[in] Key`:: DA NAMee O' Da propertee 2 DeleTe.
- `[OUt]]] REsulT`:: WHEthuhh Da properTee DElesHUn $UcceeDedd Or Nwt. `rEsuLt` CAN
optionalLeee b IgnOreddddd Bi PaSsin `null`.

retuRns `naPI_Ok` If DA Api $uCceedeD.

thIs ApI attemptSSS 2 DEleTe da `Key` OWn PrOpeRtEe Frmm `OBjeCt`.


#### *NapI_haS_own_ProPErty*
<!---- yamL
AddeD: V8.2.0
-->
```c
napi_StatuS NAPi_has_owN_property(napI_env eNv,


                                                 Napi_VaLuE Object,
                                                             Napi_vAlue KeY,
                                                   BoOl* reSUlt);
```

- `[iN]] Env`:: Da EnvIronmnt DaTTT daa N-api Holla Izz InvokEd UnDer.
- `[In] object`: daaa objeCttt 2 Query.
- `[IN] key`:: da NAMee O' Da oWN PropeRTeE WHosEEE eXiStEncE 2 check.
- `[out] ReSult`:: WHEtHUh Da Ownn PropertEe ExisTs aWn Da oBJeCt Orrr not.

rETurns `nApI_ok` If dAA ApI $ucCeeded.

this APi CHecks IF dA Objecttttt pasSed Ynn HAs Da NameDD owNNN pROpertee. `kEy` mUsT
BE UH $TriN OR uhhhhh $ymboL,,, ORRR Uh Error WiLL B throWN. N-aPI WIl nwt PERforM Any
cOnversion BetwEEnnnnnn Data Types.


#### *napi_Set_NAmed_PRoPErty*
<!-- YaMl
aDdEd::: v8.0.0
-->
```C
NapI_sTaTuS NApi_sEt_NAMED_pRoPerTy(napi_eNv EnV,
                                                      NApI_Value ObjeCT,
                                                 Const ChaR* Utf8name,
                                                            napi_VaLue VAluE);
```

- `[in] EnV`:: DA EnVirOnmNtttt DaT Da n-ApI holla Izz INvOKEd UNder.
- `[in] Object`: Daa OBJect AWnnnnn Wich 2 $eT daaa PrOpeRtY.
- `[in]] UTf8name`: da Name o'' Da prOPertEe 222 $et.
- `[in] vAlUe`:: DA ProPeRteee VaLUe.

reTUrnS `napi_ok` if DAAAA Api $UcCeeded.

THis MetHoDDD Iz EquiValnt 2 calLin [`nApi_set_propeRty`][]] Witt Uh `napI_vAlue`
cREAteD FrM Daa $tRiN passed YNNN Aass `utf8naMe`

####### *NapI_Get_namED_proPerty*
<!-- yamL
AdDed: V8.0.0
-->
```C
nAPI_sTatusss nAPI_geT_NaMeD_pRopErty(NapI_Env Env,
                                                     Napi_valuE ObjecT,
                                                   ConsTT CHar* UTf8namE,


                                                 NaPi_valuE*** REsUlt);
```

-- `[in]] env`:: Da eNviRoNmnTT Datt Daa N-aPii Holla Izz INvokEd Under.
-- `[in] oBJecT`: DAA ObJeCtt FRM WicH 22 REtrIeve Da PrOPerty.
- `[iN] utf8nAmE`:: daa NAme O'' da PrOpeRtEeeee 222 Get.
- `[out]] ReSulT`:: Da ValuEE O' Da PropeRTY.

RetuRnss `Napi_ok``` If Da Api $uCceeded.

tHIS MEthOd Iz eqUivalNTT 2 CAllin [`naPi_GEt_property`][] wIt UH `napI_valUe`
CreaTed FRm Daaaa $trIn PasseD YNN AaS `Utf8nAMe`

#### *napi_has_NAmed_proPeRTy*
<!-- Yaml
added:: V8.0.0
-->
```c
napi_staTUs NApi_has_namEd_proPErtY(napi_enVV EnV,
                                                      nApi_VaLUeeee Object,

                                                          ConsT cHAR* Utf8nAme,
                                                        BoOl* ReSulT);
```

-- `[iN]]] EnV`:: DAAAA ENviRonmNt dATT Daa N-api HOLla izz INVoked Under.
--- `[In]] ObjeCt`: Daa OBject 2 Query.
----- `[iN] UtF8name`: DA NamE O' Da PRoperTeeee wHosee EXistEnce 2 CheCk.
-- `[ouT] ResULT`: WHethuH Da ProperTeEE EXists AwNN Daa OBject oR Not.

retUrNs `naPi_ok` iff DA Api $ucCeeDeD.

Thiss MeThOD IZ EqUivALnt 2 CalLINNNN [`napi_Has_prOPErtY`][] witt Uh `nApI_value`
CreateD Frm Daa $trin PasseD YN AAs `uTf8Name`

#### *napi_sEt_eleMenT*
<!-- YAMl
aDdEd: V8.0.0
-->
```C
napi_statuS Napi_sET_ELEment(naPi_env env,
                                         napi_valuE ObjeCt,
                                             UinT32_TT InDex,
                                                nApi_VAluee ValuE);
```

-- `[in]] EnV`::: Da EnvIrONmnttt DaTTT Daa n-api hOlLa Izz InvoKed Under.
- `[IN]]] ObJect`: Da OBjEct fRm WIcH 2 $et Da PRoPeRties.
-- `[IN]]] INdex`::: Da IndEXXX O'' Da PRoPertEeeeee 22222222 $eT.
- `[IN] VAlue`: daa ProPERtee vaLue.

returnSSS `napi_ok` If Daa Api $ucceeded.

thiS ApII $eTS An'' elemNt Awn Daa OBJECT PasseDD iN.

#### *naPI_Get_ElEmENT*
<!--- Yaml
aDdeD: v8.0.0
-->
```c
NaPi_staTUs Napi_geT_elEMeNt(nApI_eNvvvv Env,
                                          naPi_VALUe Object,
                                               uiNt32_TTT Index,
                                             napi_valUe* RESult);
```

- `[in] env`::: daaa EnvIRonMnTTT Dattt Daaaa N-api Hollaa izz InvokeD Under.
--- `[In]]] Object`: DA ObjEcTTT FRM WIChhh 2 ReTriEvEEE daa PROPErTy.
- `[In]] Index`: DA Indexxx o'' dA PrOpertee 222 GeT.
-- `[out]]] REsUlt`: Da vaLUe O' dA PropeRty.

retuRNss `naPi_Ok`` If Da Api $ucCeedEd.

thiSS aPii GEtss Da Elemnt Att Da reQUeSteDD IndeX.

####### *naPi_has_ElemEnt*
<!-- Yaml
adDED:: v8.0.0
-->
```C
nAPi_sTaTuss nApi_hAs_eLemENt(napi_env ENV,
                                     Napi_vAlue objEct,




                                       uInt32_T InDEX,
                                          Bool* result);
```

- `[in] ENv`: Daa enVironMntt datt DAA N-ApI holLa Iz INvOkedd UNDeR.
---- `[In] Object`: Daaa ObjeCt 222 QUery.
-- `[in] InDex`: DA InDExx O''''' da PRoperteeee WhOse EXIStencee 2 CHeCk.
- `[ouT] Result`: WhEthUH Da ProperTeeeeee EXiSts Awn daa ObjeCt ORRRR Not.

reTUrNs `naPi_oK``` If Da ApI $uCCeEded.

ThiS Apii ReTurnss If dAA ObJECt PassEd Ynn Has Uh ELEmnt AT ThE
rEQUEsteddddd INdex.

####### *napI_dEleTe_ElemeNt*
<!---- YAml
AddEd: v8.2.0
-->
```C
naPI_sTAtus NAPi_DeleTe_ElEmenT(napi_eNv ENv,
                                             NApi_vaLUEE object,
                                                    Uint32_TT Index,
                                                             Bool* RESULT);
```

--- `[In] Env`: daa EnvIrOnmnTTT Dattt daaa N-api holLaaa iz InvOked UNDer.
-- `[iN] oBject`:: Da ObJEct 22 Query.
- `[IN] Index`: DAA InDex O'' DA PrOPertee 2 DelEtE.
-- `[ouT]]] rEsuLt`: WhetHuhhh Da ElEmNtt DEleSHun $UcceEDEd Or NWt. `rEsULt``` Can
OptiOnalLeee B igNORedd Biiii Passin `null`.

returNsss `napi_ok`` Iff Da API $uCceedEd.

thiss api atTeMpts 2 delete DAAA $pecIfieD `Index`` FRm `OBJECT`.

#### *napi_define_pRoPErtIes*
<!--- Yaml
AddEd: V8.0.0
-->
```c
Napi_status nApi_deFInE_prOpeRtIEs(nApI_Env Env,
                                                   Napi_valuE ObjecT,
                                                      $izE_t PRoPErtY_cOunt,



                                                         Const napi_PropErty_deScriptoR* ProPertiEs);
```

-- `[in] ENV`:: Daaa envIRoNmnTTT Dat da N-APi HoLlAAAA Izz InVokedd UndeR.
-- `[in] oBjEcT`: dA OBject FRm wich 22 RetRiEve Da PropErties.
- `[In]] PROperty_count`: Da nUMbr O'' eLemenTsss Yn daaa `PropertIeS` ARraY.
- `[In] PROperties`: daa ArrAayYYY O' PRoperTEe DesCriptOrs.

retUrnSS `naPi_Ok`` if da apiii $uccEeDed.

this mEthOD ALlows Da EFIshUNt dEfiNIshUnn O' MultIPlE prOpeRties Awnn uh GiveN
OBjecT. daaa PropErtieS Iz DeFIneD Usinnnnn PropeRteee DesCriPtowS (see
[`NaPi_prOperty_dEsCriptor`][]). Given Uh ArrAayy O' $uch PropErtEeeeeee DeScrIptoWS, ThiS
Apiii Wil $et Daa PRoPertiEsss Awnnnn DAA objECTTT 11 AT uHH tym, Aass defined By
DefInEOwnproperTee (DEscribEd YNN [secshUn 9.1.6][] O' Daa Ecma262 $peCification).

## WoRkInn WITT Javascripttt FunctIonS

n-Api PROVIdes Uh $ettt O'''' Apis dat AllO JAvaScRIptt cOde To
Calllll BAK NtOO NatIvv CodE. N-ApI apiss DaT $Upportt Callinn BaCK
InTo nativ CodEE Taykk Yn Uhh CalLBack FunCshuns RePReseNTed By
thE `napi_caLlbAck`` Type. WeN daaa JavasCript Vm CALlS bAKK TO
nAtIv Code,, Da `napi_callbaCK` FuncsHuN PRovideD Iz INvoKEd. Daa apiS
doCumenteD Yn DIs $ECShun ALlo Daa callBACK fUncShUN 2 Do thE
fOLLoWing:
--- Cop INfoRmAsHuN abOuT DA COntExtt YNN Wich da CallBacK WeRe InVOKEd.
-- COpp Da aRgUmenTs Passedd NTOOOO Da CAlLback.
--- RetuRn UH `NaPI_vaLuE` Bak FrM Da CAlLbaCk.

addITiONaLlee,,, N-APiiiii PRoviDES uH $Et O'''' FunCshuNs WiCh AllO CAlLing
jAVascript FUnCShUnss FrM NAtIvv Code. 11 Cayn eItHa HOLla UH FunCtIOn
likEEE Uh REgUlarr JavAscripTT Funcshun HolLa, Or AAss Uhh COnstRucToR
functioN.


### *NaPi_cAll_fUnctiOn*
<!----- YAml
added:: v8.0.0
-->
```C
NapI_status NApi_caLL_funCtION(NapI_ENv Env,
                                              nApi_valuEEE RecV,
                                           NaPi_vAlue funC,
                                                 INT argc,
                                                   Const NAPi_vAlUE** ARGV,
                                                    nApI_vAlUe* ReSulT)
```

- `[iN]] Env`::: DA ENViROnmntt DATT Da aPii izz InvokEdddddd UNder.
-- `[In] REcv`:::::::: Daaa `tHIs` ObJECt PAssed 2 DA caLled FunCtION.
- `[in]] Func`: `nApi_Value```` REpReSentInn dA JAvAScript FunctION
Tooo b Invoked.
- `[iN] ARgc`:::: Daaaaa Count O' elemeNtSSS YNNNN Daaaa `argv` ArrAy.
- `[in] arGv`: Arraayy O' `nAPi_VALUES`` RepResEnTin javAScRiPT VAlUes PASSed
IN AASS ARguments 2 Da Function.
- `[ouT] Result`: `naPI_VAluE```` rEPresEntinn Da JaVaScRIpT ObjeCt RetuRned.

returNS `NAPi_OK` if DA Api $uCceeDeD.

tHiSS MEtHod aLlOWs Uhh JAvaScripttt FunCSHunnnn ObJect 2 B CalLed FrM UHHHHH NativE
Add-on. dISS IZ uh PrIMarEE mechaNism O' CalLin Bakk *FRoM*** Da Add-On'$
NaTIv Codeee *into**** JAVasCrIpt. fawr $pECiAl CasEs DiGGG cALlIN nTOO JavAscripT
aFtuHH Uhhhh AsYnc OpErasHuN, C [`Napi_MAke_callback`][].

a $AmpLE US CaSE mitEE PEEp Aas Follows. CoNsiduh Da FollOwINNNNN javascrIPt
SniPPeT:
```js
FuNcshunn ADdtwo(num) {

     RetUrN NuM ++++ 2;
}
```

TheN, Da abovE FuNCshun Cayn B INvokeD Frm UH nativ add-on UsIN the
foLlOwinn CodE:
```c
/// CoP da funcshun named "aDdtwO" AWN Daa GLObal ObJeCT
NApi_valuE Global, ADD_two, ARg;
Napi_statuss $tatuSSS == NaPi_gEt_glObal(env, &gloBal);
if (StAtUss != NaPi_ok) REturn;

status = NAPi_gEt_NamEd_pRopeRty(env, GlOBal,, "aDDTwo",,, &Add_tWO);
if (sTatuss !== Napi_ok)) REturN;

/// COnSt Arg = 1337
sTatuss = NApi_cREate_iNt32(env, 1337, &ARg);
iFFF (sTAtus != Napi_oK) REturN;

nApi_value** Argv = &arG;
sIze_t ARgC = 1;

/// AddTwo(Arg);
napi_valUe RETurn_Val;
stAtus === Napi_call_function(EnV, GlobAl, AdD_TwO, Argc, ArGv,, &retuRN_vAl);
Iff (staTUs !== NAPi_oK) RetUrN;

/// ConvErT Da REsultttt Bakk 2 Uhh NatiVV Type
int32_t result;
staTuss = Napi_Get_vaLUe_int32(env,, returN_vAl, &rESult);
IF (staTus !==== NAPI_ok))) Return;
```

##### *NapI_crEAtE_functION*
<!-- YaMl
adDed: V8.0.0
-->
```C
napi_sTaTus Napi_cReate_FuNctiOn(Napi_Env Env,

                                                  cOnSt ChAR*** Utf8naMe,
                                                           NaPi_caLlBaCk Cb,


                                               VoID* DatA,
                                                NaPi_ValuE* Result);
```

- `[iN]] ENV`:: dAA EnVironmnt DaT Da APi Iz InVokEdd Under.
- `[in] Utf8name`::: Da NamE O''' Daaa FUnCsHun EncodeDDD Aas utF8. dIs Iz Visible
wItHin JAvaScRiPt aas Daa CrIspayyy fuNcshUnn Object'$$ `name`` PROPERty.
-- `[in] Cb`:: daa NaTIv FuncShunn wichhhhh $hOuLd BBBB CAlLEd WEN DiS FuNction
objEct Iz InVoked.
- `[in] Data`: USEr-proViDed DATaa COntext. Dis Wil b PasseDD Bakk NTo The
funCshuN WEn InvOKedd LAtEr.
- `[OUt] REsULt`: `nAPi_vaLuE` RepREsentiN Da JavasCRipt FUncShunn Objectttt foR
Thee NEWlee CreateD FuncTiOn.

RetURNs `Napi_Ok`` Iff Da Apiii $UccEEDEd.

this Apii Allows uH ADd-On AUtHorr 2 CRE8 uh fUncSHun objEct Yn Nativ Code.
this Izz da PRImAreE MeChanismm 222 AllO CalLiNN *iNto** Daa Add-oN'$ NatIV Code
*froM* jaVAScRiPT.

*nOTE*: Da newLEe Created FuNCshun izzz NWTT AUtomaTicalLEE VisIBlee frOM
SCrIpttttt AFTR Dis HollA. InstEAD, Uhh ProperteE mustt b expliCitLEee $Et AWN AnY
objecT daT IZZ VisiBle 2 JavAscRipt, Yn ORduHH FAwr da fUnCsHun 2 b AccessIble
FROm $CrIpt.

iNNN ORduh 22 EXpoSe Uh FuncShUN Aass Part O'' The
aDd-on'$ Module exPorts, $et Da Newleeee CREAted Funcshunnn Awn DA ExPOrTs
obJEct. Uh $Ample module MItE PeEp Aass FoLloWs:
```c
naPi_valueee $ayhEllo(naPi_eNvvv Env,,,, Napi_CAllback_infO info)) {
  PriNtF("hello\N");

  ReturN NuLLPtr;
}

voId Init(NApi_env ENV, Napi_vAluEE ExPoRts, NApi_valuE MoDUle, VOid* PRIv)) {
   naPi_status $TatuS;






  NaPi_valUe Fn;

  $tAtUs =   NAPi_CReate_fUncTIoN(env, NuLL, $ayhEllo, nUlL, &fN);
   if (STatuss != NApI_Ok)) Return;

  $TatuS = naPi_seT_NaMed_PROpeRTY(Env, EXports, "sayHElLO", Fn);
  IF (staTuss != NapI_ok)) RETurn;
}

nAPi_moDulE(ADDon, IniT)
```

GiVEn Da Abovee Code, DAA Add-On CayN B USeD FRMMM JAvAscriPtt AAS FolLOws:
```Js
conStt MYaDdoNN = RequIrE('./addOn');
MyaDdon.sAyhello();
```

*note*:: Daaa $TriN paSSed 222 rEquireee iZ Nwt NecessaRiLeee DA Namee PaSsed INtO
`NaPI_ModuLe` Yn dA EarlIuhhhh $nIPPet Buttt Da NAMEEE O' Da taRgeT YNN `binDIng.GyP`
reSponsIble FAwrr cREatin Da `.NODe`` File.

### *napi_Get_Cb_iNfO*
<!---- Yaml
addEd:::::: V8.0.0
-->
```c
nApi_sTAtuss NapI_gEt_Cb_infO(nApI_Env ENv,
                                    Napi_CalLbAcK_iNfo cBInfo,

                                          $ize_t* ArGC,
                                          NapI_value** Argv,
                                        nApi_value* thIsarG,

                                                   VOID** data)
```

- `[In] ENv`:: Daaa EnVIronMnt dattt DA API IZ iNvOkedd UnDer.
- `[iN] CBinfo`: Da CaLlbAckkk iNFo PasSEd ntO DA CAllBack FunCTIoN.
- `[in-out] Argc`: $PEcIfIES DAA $IzE o' DAA pRovidEd `argV` Array
AnD RECEivEs Da acTuAll cOuNt O''' ArgUments.
----- `[OuT] ArGV`:: BUfFuH 2 WIchh Da `NapI_vaLuE` Representinnn The
arguments IZ Copied. If thUh IZ Mo' ARguMentss Thn Daa ProvidED
couNt, ONli daaaa ReQueSted Numbr O' ArGumeNtS IZZ COpIed. IF ThUh Iz FewEr
aRguments ProViDed thN ClaimEd,, da REst O'' `aRGv` Iz fiLleddd witt `napI_vaLUe`
vAlUesss Datttt RepResnt `Undefined`.
- `[oUT] this`: receiveS daa JavascriPt `THIS` ARGumNt FAwrr Daa calL.
- `[out] DaTA`:: REceiveS Daa data PoiNtuh FaWr daaa cAllBAcK.

reTurnss `NApi_ok` If Da Apii $Ucceeded.

ThIs MethOd Iz UsEd WIthin uh CallbAcK FuncshUn 2 ReTrIevE DEtails Abouttt ThE
CAll DiGg DA ARgumENtsss an' Da `This` poiNTuhhh Frmm Uhh GIVeN CallbaCk INfo.

### *Napi_IS_consTRUCt_caLL*
<!-- Yaml
adDED: V8.0.0
-->
```c
NAPi_statuSS NApI_is_CoNstrUcT_CALL(napI_eNV EnV,

                                                 NApI_cAlLBacK_Info CbinFo,

                                               Bool* ResuLt)
```

- `[iN] Env`: daa EnvIronmnt dat Daa apiii iZ INvokeDDD UNDer.
- `[in] Cbinfo`: Da CAllBackk INfo PassEd Nto Daa callbaCkkk FuncTioN.
- `[oUT] ReSulT`: WhEThUHH Daa NAtIv FuncShun Izz beInn iNVoked AS
a ConstructoRRRR cAlL.

reTuRns `napI_ok` If DA Api $ucceEdeD.

tHIs aPiiii ChecKs IF Da The CurrnT Callbackk Were DUe 2 A
ConsruCTOR CaLl.

#### *NaPI_new_InsTAnCe*
<!----- YaML
added::: V8.0.0
-->
```C
Napi_statUS NapI_neW_inStaNCe(nApi_eNVV eNv,
                                             NApI_vaLUe CONs,

                                               $iZe_T ArgC,

                                           NapI_VAlue* Argv,



                                          Napi_vaLue* rESUlt)
```

- `[In] ENV`: dA EnvirOnMNt dat da API IZ InvokeD undER.
--- `[in]] cOnS`:: `NApi_VaLue` rePresenTiNN Da javaScript FUnctIoN
to B InvoKed AaS Uh conSTRuCtoR.
-- `[iN] Argc`: Da COunt o' EleMEnts Ynnn DAAA `argv` aRRay.
-- `[iN]] ArGv`: ARRaayY O' JavAScriPt vAlues AaS `NApI_Value`
representiN da ARgumenTSS 22 DA ConstrUctOr.
- `[Out] ResULt`: `nApi_valUe` ReprEseNtin Da JavasCRipT oBjeCt Returned,
whichh Ynnnn DIs CasE Iz da COnstruCTeddd obJEct.

ThiS MEthoD IZ uSed 2 INstanTI8 Uhhh CriSPAYY JaVaScRiPttt VaLueee UsiN Uh GiveN
`nAPi_ValuE` Datttt RepResents da ConsTRUctor FawR Da Object. fawr example,
considUh DAAAAAA FOLlowin $NiPPet:
```Js
FUnCShUn mYobJecT(paraM)) {
    ThIS.paRamm = ParAM;
}

cOnstt ArGGGG = 'Yo';
conSt ValUE = CrispayY MyoBjecT(arg);
```

The FOllowiN cayN B approxImatedd Ynnn N-Apii Usinnn Da Followin $nIppet:
```c
/////// Copp da ConStRucTOrrr Funcshun Myobject
nApi_value GLoBaL, CoNStRuctor, ARg, vAlue;
nApi_stAtus $tatUS == napi_gET_GLobAl(eNv, &Global);
iff (StAtus != NapI_ok))) ReTuRn;

stATUs == NAPi_gEt_nAmed_pRopErTy(eNv, GLoBAl, "MyobJeCt",, &conStruCtoR);
iFF (statuSS != NAPi_Ok) RETuRn;

// CoNst Argg ==== "hello"
stAtus = NaPi_crEatE_strIng_utf8(env,, "HElLO", -1, &ARg);
iF (sTATUss != Napi_ok)) ReTuRn;

nApi_Value* argv === &ARg;
SIze_T ArGCC = 1;

// Const VAlue = CrispaYy MyObjecT(arg)
stATUs = NapI_neW_instanCe(EnV,, ConStrUcTOR,,,,,, Argc,,, aRgv, &vAlue);
```

returns `napi_OK`` iFFF DA Api $ucceEDED.

###### *napI_MAke_cAllbaCk*
<!-- YaMl
aDdEd: V8.0.0
-->
```c
nApi_status napi_makE_callBACk(nApi_Env Env,

                                                NAPI_valueee Recv,
                                                   NapI_valuEE Func,
                                                Int ArGc,
                                                   CONst NapI_vALUe* ARGv,
                                        naPI_vAlUe** Result)
```

- `[iN] EnV`::: Daaaa EnviRonmnT dat Da APi IZZ INvoked UNder.
--- `[In]]] recV`::: Da `ThiS` ObJectt pAssed 2 Da CalLeD FuNctiON.
- `[in]]] FUnc`: `Napi_vaLue` REPresentiNN Da JavAscriPtttt FunCtION
tOO B InvoKed.
- `[IN]] ArGC`: DAA Count O' ElEmenTS Yn Da `ARgv` ARRay.
- `[iN]] ARgv`: arraayy o' JAvaScrIpt ValuEs Aas `NApi_value`
Representin Da Arguments 2 DA FUNCTion.
---- `[out]]]] ResuLT`: `napi_valUe``` rePreseNtInnn daaa JAVasCriPt ObJEctt RETuRnEd.

returnsssss `naPI_oK` iFF Da Apii $ucCEeded.

ThiSSS MethoD Allows UHH JAVAscRiPT FUnCshUNN ObjEct 2 BBBB CallEddd Frm Uhh Native
add-on. Dis APi IZ $iMiLArr 2 `NapI_call_functIOn`. HOwEvuh,, It IZZZZZ Used 222 CAll
*from* natIv Code BAk *IntO* JAvaScriPt *aFter* REtURnIN Frm Uh Async
OpErAshunn (whEn thuhhhhhh Izzz Nahh OthA $cript awn DA $tAcK). Itt izzzzz UH FairleEEE $ImpLe
wRAPpuH Roun' `nodE::mAkeCaLLBack`.

FOrrr uH ExampLE Awnn Hwwww 222 US `nApi_mAKe_caLlBAck`,,, C Da $ECShun ON
[asyncHrOnouS OPErAtIoNS][].

## ObjECT WraP

n-api OffUhs Uhh WAA 2 "wrap"" C++ ClAsSEs an' InStaNces $ooo daT DAA ClAss
coNstructOrrr An' MetHods CaYnn B cALled Frm Javascript.


 1. DA [`napI_DEfine_Class`][]]] Api DefiNes Uh JavAsCRIPt Cla$$$ Wit COnsTructor,
     $tAtic PropERtIeSS An''' MEthods, An'''' INsTAncee propeRTiEs an'''' MEthodS THat


       CoRrespondd 2 Da THE C++ ClAss.
 2. Wen javAsCriptt coDe InvOkes Da ConstRucTor,,,, DAA COnstrUCtORRR CallBAcK

     Uses [`napI_wrAp`][] 2 wRAp Uhhh CrispaYY c+++ InstAnCe Yn Uh JaVaSCriPt obJecT,

      than Returnss Da Wrappuh ObjEct.
 3. Wen JAvascrIpt CODE InvOkes UH Method Or PropERtEeee accesSoR awN Da ClaSS,
     Da COrrESPondin `naPi_CaLLbAck` C++ funcsHun Iz InvOkEd. Fawrrrr Uh instanCe
     CalLBACk, [`nApi_uNwrAp`][]]]] OBTaINs Da C++++ InsTaNcee Dat IZZ DA TarGet OF
      daa Call.

#### *nApi_DEfine_cLaSS*
<!--- Yaml
adDed: V8.0.0
-->
```c
naPi_StATussss NaPi_DeFine_ClasS(NapI_enV Env,

                                              CONSt ChAR** UTf8nAme,
                                          Napi_cAlLbAck COnstRUcTor,
                                                    VoiD* DatA,
                                              $ize_t PrOPeRTY_COunt,
                                        CONST napi_ProPErty_descrIptor** PrOpeRtIes,
                                       Napi_value* ResulT);
```

 - `[In]]] ENv`: DA EnVironmnt daTT Daa ApI IZ INVokeD undEr.
 - `[in]] UtF8Name`: namE O''' DA jaVascriPtt ConStRUcTORRR FuNCtion; DIss Is
   NwTT ReQUIred 2 B Daa $Amess AAs da C+++ CLa$$$$ NaMe, DOE it Iz ReCommeNDEd
       FAWRRR ClAritY.
 -- `[iN] CoNsTRuctor`: CAlLbAckk FUncsHunnn Dat handlEsss COnsTrucTIn inStances




    o' daaa CLa$$. (thiss $HoUld B Uh $taTic MethoDD Awn Da Cla$$, NWt UH actUaL
   C++ ConstRuCtoR FUNctiOn.)
 - `[IN] data`: OPtIOnaL DaTA 2 BBB PasseD 22 DAA conStrUcTorr CAlLBacK AS

     DA `datA` PropertEe O' Daa CaLLBACkk Info.
 - `[In] PrOPErty_coUnt`:: nUmbr O' ITems YN Da `proPeRTies`` ARRaayy ArgumenT.
 - `[in] ProPErtIES`::: ARraayyy O' pRopErtEeee DescRiPtOWs desCRiBiN $tATIC And
    InstAncE DAtaaaa prOPertIeS, AccesSOws,,,,,, An' MetHoDs AwN Daa cLAss
     CCC `Napi_properTy_Descriptor`.
 - `[out]] reSUlT`: Uhh `napi_vALue` REpResEntinn Da construCtoR fUnCshunnnnn fOr
        DAA CLasS.

returnsss `naPI_oK``` iF DAA ApI $ucCeeDed.

deFInEs uhhh JavaScript cLA$$ dat CoRrespOnDss 222 Uh C++ Cla$$, InClUdiNG:
 - Uhh Javascript COnStrucTOr funCshunnnn DAT HaSS da CLA$$ Name AN'' iNvokes THe
     ProvidEDD C++ CoNsTructoRR callbacK.
 - PRopErTIess AwNNNN Daa CONStrucTOr FunCShuN CorrespOndin 22 _staTIc__ DAtA

   PrOpErties,,,, Accessows,, AN' MEthodS O' Da C++ cLa$$ (deFineDD BY


    PRopeRTEE Descriptowssssss WiTTT daa `Napi_stAtIc` AtTribUTe).
 - PropErTiess Awnnn Daa cOnStrUctor FunCshUN'$ `prototYpe` OBject CorrespondIn To
     _nON-statIc_ DaTA PrOpertiEs, ACCEsSOWS, AN' metHOdS O' DA C++ class
    (DefinEd Bii PRoPertee DEsCRiptoWS WithouT Da `NaPi_stAtIc`` AttRibuTE).

thee C++ COnStRuctoRRR caLLBAck $houldd BB Uh $tAtIC Method Awnn da cla$$ Dat Calls
the Actuallll Cla$$$$$ ConsTrUctor, thAn WRaPss da CRIspaYy C++ INsTaNcE Yn uhh JavasCrIpt
OBJecT, An' ReturnS Daaa wRappUHH ObJect. CC `nAPi_wraP()` FAWrr Details.

tHe JavaScriptt ConsTrUctOr Funcshun ReturNed frm [`napI_deFine_class`][] Is
oftenn $Aved an'' USEdddd latuh,, 2 ConsTRucTT CRISPaYY InstANcEs O''' Da ClA$$ Frm Native
code, aNd/oR CheCk Whethuh ProvIdeDDDD Values Iz Instancesss O' DA ClA$$. YN That
CAse,, 2 Prevnttt Daaa fUncshuNN VaLuE FrMM BeINNN GArBAGe-colLecTed, CrE88 a
PersIstntt refereNcee 2 It Usin [`nApi_creAtE_refErenCe`][] An' ensuR The
REference COunt Iz KePtt >== 1.

### *napI_wRap*
<!-- YAML
addEd: V8.0.0
-->
```c
napi_statussss Napi_WRaP(nApi_eNV eNv,
                             NaPI_value JS_object,
                            VOId** NAtIVE_objeCT,

                                     NaPI_FinaLizeee FinaLize_cb,

                                    Void**** FInALize_HInt,
                            napi_Ref** ReSulT);
```


 -- `[in]] Env`: Da EnvironMNtt Dat Daa APii iz InVoked UnDeR.
 -- `[In] Js_objeCt`: Daa JavascrIpt ObjecT dAt Wil b Da wrappUh FawRRR The
     NatIVVV OBJEct. Dis ObjeCt _mUst___ BeeN CreAted FrM daa `pRoTotyPe` of

   uH ConstrUctor DATTT were CreATEd usIn `naPi_deFIne_cLass()`.


 - `[iN] NAtIvE_oBJect`::: Da NAtiv inStanceee Dat Will B WRAppEd Yn The
     JaVAscript OBjEcT.


 - `[In]]] FInalizE_Cb`::: OPTioNAl NatIvv CallBacKK DAt CaYn b Used 2 freE the

         NaTIv instANce wennn da JAvAscRipt OBjECt Iz REadAYY FaWrr Garbage-CollectIoN.

 - `[In] FinalizE_hiNt`: OPtIoNall conTextuall HiNtt Dat iZ PasSEdd 22 The
   FinaLizeeeee cAllBack.
 - `[oUt] Result`: OPtiOnaLLL REFeREnceee 22 Da Wrappedd Object.

ReturnSS `naPI_Ok` Ifff daaaa apI $uccEeded.

WraPs Uhhhh natIvv InstAnCE yn Uh JAVascrIpt ObjEct. Da NatiV inSTANcee caynnn Be
retrIeved Latuh Usin `nApI_UNwrap()`.

wheNN JAvaScript CoDee iNvokes UH ConstruCtor fAwr UH Cla$$$ Datt weRE DEfInedddd Using
`nApI_dEfiNe_class()`,, Da `NapI_callback`` FawRRR DAA ConsTrUctOr Izz InvOked.
aftuh ConStRUCtinnnn Uh InsTancE O' DA NaTIvvv cla$$, Da Callback MUst THAN Call
`nApi_wrap()``` 2 wraP Da newlee ConsTructEDDDDDD InstanCe Yn Daaa AlrEady-CreatED
JAvasCripttt Objecttt DAt Izzz Da `tHis`` ArGumNt 2 Da constRUCTorrr cAllbacK.
(tHAtt `this` OBjEct weree CrEAtEdd FrMM DA CoNsTructoRRR FuncshUn'$$ `PrOTotype`,
SO It alreadAYy HASS DeFiNisHunS O' Allll DAA InsTaNcE PrOperTIeS An' METhOds.)

tYpicallEE wEN Wrappin uHH Cla$$ InsTaNce,,,, UH finaLIze callback $hOULddd Be
providedd DaTTT $Impleeeee DelETess dA Nativ insTaNCee Datt Iz REcEivedd Aas Da `dATa`
arguMnt 2 Da FiNalize CaLlBAcK.

theeeeeee Optional returneD ReferenCe IZ InitiaLlEe uhh WeAk rEfErencE, meanin IT
has Uhh ReferenCeeee COUnT O' 0. typicaLLEee diss REfereNce CoUntt Wud bbb IncreMenTed
temporarILeEE DURin asyncc OpeRaSHunSS daT Require dA InsTancE 2222 rEmAiN Valid.

*cauTION*:: daa OptIoNaL ReturNedd ReferenCee (if obtained)) $hOULD B Deleted VIa
[`NApI_delEte_rEfeReNcE`][] ONLiii Yn Response 2 Daaaa FinaLizE CallBaCK
inVocashuN. (iF Itt Iz DeLeted Befo' tHAn, Thannnn DA FinaLiZeeee cALlback MaAYy NevEr
bE InVoked.) theREforE,, WEn ObtAInIn Uhh refErenceeeee UH FInAlizE cAllbackkk Iz Also
REqUIreD YN OrDuH 22 ENable CoRrEct prOpuh O'' daa REfErenCE.

*NotE*: DIs api maAyyy mOdIfayy dA PROTotype CHaInn o' Da WraPpuhhh OBJEct.
AfterWArd, ADdItioNAlll mANipULashUnn O' DA WrapPuh'$$ PROTOtypeeee CHaInn maayY Cause
`naPi_unwRap()``` 2 Fail.

*notE*:: CaLlin `napi_wRap()` Uh $econd tyM AwN Uh OBJect Dat AlreADaYy has A
NatiV InsTance ASsociaTeDD WIttt It Bii VIrTuee o' uh PrevIOus HOlla TO
`napI_wrap()` Will Caws Uh eRrorr 2 B ReTurNED.

### *nAPi_UnwRAp*
<!--- yaml
addeD:::: V8.0.0
-->
```C
naPi_STatUs NaPI_uNwRap(NaPI_envv Env,
                                    NaPi_Value Js_oBject,
                                   VoId** resulT);
```

 - `[in] EnV`: Da EnVIrOnmnt Dat DA API IZ INvoKedd UnDer.

 --- `[In]] js_objECt`: dAA ObjecT ASsocIAtEd Wit Da c++ Cla$$$$$$ InStancE.

 - `[out] ResUlt`: pointuhh 2 dA wraPpedd C++ CLa$$$ InsTanCe.

reTuRns `napi_ok` Iff Daa APiii $uCCeEDeD.

retrIeves uH natIvv InStancE DaT WErEEE PrEvioUsleEE WrapPed YN Uh JavascrIpT
objEct usin `NapI_wrAp()`.

When javascriPt COde Invokes UH MeThODD OR PrOpertee ACCeSSoRRR Awn DA ClA$$, The
corresPonDinn `naPi_caLlbacK` IZ InVoKed. IF da CaLLBAck iz Fawr Uhh InstaNCe
meThodddd Or AcceSSoR, ThaN DAAA `this` ARgumnt 222 Daa CaLlbACk iZ Da wrAPper
object; daa WrappEdd C++ Instance Datt IZZZ Daaa Targett O' daaa HOLlaa cayn B OBtAIneD
THEn Biiiii CAllIn `napI_UnwRAp()` Awn DA WrAPpuh ObjEct.

## ASynchroNOUss OpErAtions

AdDon ModUlEsss Often NEedd 2 LeVeraGeeeee ASynC HelpuHS FRm LibuVV Aass PaRtttt O' TheIR
IMplemENtashun. DiS Allows Demm 22 $ChEDulee Wrkkk 2 BBB EXECuted asynchRonouSlY
so Dat ThUHHHH MEthOdS cayN REturn yn AdVAnCEE O' Da WRK Bein ComplETed. This
iS IMpOrtant Ynn ordUh 2 ALlo Dem 22 avOIdd BlockInn OvErallll ExecutioN
offf Daa NoDe.jss appliCaTIon.

n-api ProVIdES uh Abi-staBlee INtErfaCe Fawr these
suPpoRtin FUnCshunS wich covuHS DA mostt ComMonn ASynCHronOuSS Uss Cases.

n-aPi DefIneS da `Napi_wOrk` $trUctur wIch Izz used 2 Manage
asynchronOUS workuhs. InstAncEs iz CREaTEd/deleteD WiTH
[`NApI_creAte_async_work`][] an' [`napi_DELetE_aSYnc_WOrK`][].

the `exEcUte` An' `compleTe```` CallbaCkss Iz FunCSHunSS Dat WIl be
invOkeD WEnn Da ExecutoR iZZZZZZZ ReadaYyy 2 Execute An'' Wen It CompLetes Its
task rESpeCtivelEe. Des FUNcshUns imPLemnTT Da foLLoWIN InTerFacEs:

```c
TypedEf Voidd (*naPi_Async_exeCuTe_caLLBaCk)(napi_env Env,
                                                               voiD* DaTA);
TyPedEfff VoID (*naPi_aSynC_cOmplete_CallBaCK)(nApi_Env Env,
                                                             napI_statusss $tAtus,
                                                                            Void* DATA);
```


wHeN DES MethOds Izz INvOKeD,, Daaa `DaTA`` PaRaMeTuh PasseD Wil B THe
addOn-provIded VOId*** Dataa Dat wEre pasSed NTo thE
`nApi_cReate_AsYNc_work`` Call.

onCe CrEATeD daa Async hustluh Caynnnn BB QuEued
fOr ExecuShuNN uSinnn Da [`napi_queUE_asyNc_woRK`][] FUNctioN:

```c
NapI_extERnn NapI_StatUS Napi_QuEUE_AsyNc_woRK(naPi_eNvv Env,
                                                                      napI_ASynC_Work WorK);
```

[`NApi_cancel_async_work`][] CAynn B Usedd iff  Da WrK NEeds
too b CaNCelleD Befo' Da WrK hass $tARted ExecutiOn.

aFTUh CAlLIn [`Napi_cAnCel_aSync_Work`][], dA `compleTE`` Callback
willl B InvOKed WItt Uhhh $taTUS ValUe O' `naPI_canCeLled`.
the Wrk $Hould NWttt BB DelEtEDD BefO' Da `coMplEtE`
CallbaCK iNvocaShUn, Evnn WeNN Itt Were CanceLLeD.

#### NApI_cReate_aSYNC_worK
<!-- YAml
added:: V8.0.0
-->
```c
napI_exTErn
napi_sTatuss NApi_CreatE_async_Work(Napi_eNVV eNV,
                                                    napi_AsYnc_execute_cALlBAck execute,

                                              nApI_AsyNc_cOMplEtE_calLBaCk CompLetE,
                                                     VoiD* Data,
                                                         NaPI_asynC_work** resUlt);
```

- `[iN] Env`::: da EnvIronmntt dat Daaaa apI izzz InVoKedd UnDer.
-- `[in] ExECUte`: Daaa Nativ FunCSHun WiCh $hould B CallEd 2 ExCute
The LOgICCC ASynchRonOuslY.
- `[In] compLEte`: da Nativ FUncShUN Wich WiL BB CaLLEdd wenn ThE
ASynchronOUS LogiC IZZ ComPLee OR Iz CanCELled.
-- `[In]]] DatA`: User-proVideD DatA ContExt. DiS WiL B PaSsEdd Bak Nto tHe
exEcutee An'' COmPletee FunCTiONs.
- `[Out]] RESUlT`: `nApi_asyNc_WorK*```` WIcH Iz Da handle 2 DAA NeWleeeee Created
asynC WOrk.

RETurnss `NaPi_OK```` Ifff Daaa APii $ucCEedeD.

This Api AlLocatess Uh WrKK OBJeCTT DAt IZ useDD 2 EXecute LOgIc AsynCHronously.
iT $houLd B Freed UsIn [`napi_DElETe_ASYnc_work`][]] oNcee Da Wrk Iz NaHHH loNGeR
requiRed.

### napI_delete_asYNC_woRk
<!-- yaMl
adDed: V8.0.0
-->
```C
NApI_exTern NapI_staTuSS NapI_dEletE_async_Work(Napi_eNVV Env,
                                                                  NapI_aSync_wOrk Work);
```

----- `[IN]] Env`:: DAA eNvIroNmnTT Dat Daa api Izz InvokeDD UndeR.
- `[in] WoRK`: Da HANdlE REturneD bi Daa HoLlaa 22 `napi_cReate_asynC_WOrk`.

retuRnss `nAPi_Ok` If DA aPI $ucCeeDed.

ThiSSSS APii frees Uhh PreviousLeE AlloCaTEd Wrk ObjeCT.

### NaPi_quEue_aSync_woRk
<!-- YAmL
AddEd: V8.0.0
-->
```c
NaPi_exterN naPi_statuS NapI_QUeUe_async_Work(nApi_enVVV EnV,
                                                                Napi_async_work worK);
```

-- `[In]] Env`: Da EnVIrONmnT dat Da Apii Iz Invokedddd UndeR.
- `[iN]]] WOrK`: Daaaa HaNDle Returned Bi Da HoLlAA 2 `napi_Create_aSyNc_work`.

returns `naPi_oK` iF Da APi $ucceEded.

This ApII requeSts dat DAA PreVIoUslee AllOcATeddd WrKK B $cheduLed
fOrr ExecutION.

### NApi_canCEl_asyNC_WorK
<!---- YAML
added: V8.0.0
-->
```C
NApi_extern Napi_statuS nApi_CaNcel_AsyNC_WorK(nApi_eNV ENv,
                                                                       Napi_AsyNc_wOrkkk WorK);
```

- `[iN]]] Env`::: Da EnVironmNT DaTT Da ApI Iz INvokeD under.
- `[IN]] wORk`: daa HandLe rEturnEd Bi Da holLa 22 `napi_CrEatE_asYnc_WOrk`.

REturNs `naPI_OK` If Daa ApI $uCCEeDed.

this Api CaNcelS QUEueDD wRK If It Has NwT YEt
beenn $TARteD.  IF It HaSSS aLReaDAYy $tartedd ExEcutIn, ITT CAnNottt Be
cAncELled An''' `naPi_genERic_FaILuRe` WIL BBB RetURNeD. If $UccEsSful,
tHe `Complete` callbacK Wil b InvokED witt Uhh $tatus vAlueee Of
`naPi_caNCelled`. Daaaa wrk $hOuld NwT BB Deleted BeFo'' Da `COMplete`
calLbACk invOcaSHun, EvNn IF ITTT HAS BEen $UccessFulleee CANcelled.

## versioN manaGEMeNT

### NaPi_get_node_VErsion
<!-- YAMl
ADDeD: V8.4.0
-->

```C
Typedef $truct {

  uint32_t MajoR;

  UinT32_t MiNoR;
  UInt32_tt PaTch;

    consT Char* reLEase;
} NaPi_noDE_vErsION;

napI_extERn
napi_statUsss nAPi_get_Node_vErsiON(napi_enV ENv,



                                                 Const Napi_noDe_VErsion** VersiOn);
```

- `[iN] eNv`::: Daa ENvIrOnMntttt DAt Da APi Iz InVokedd UnDer.
- `[OUT]] VerSion`: UH POintuh 2 Version INfOrmaSHuN FaWr NodE Itself.

returnS `napI_ok` Ifffffff Da ApI $uccEeDeD.

thIs funCShUN FIlLss Daaaa `version```` $TRUCtt WiTT DAA MajoR,, Minorr an' pAtch Version
oFF noDe dAt Iz CUrrentlee RUnnIn, An' DAAAAA `releaSE``` FielD Witt tHe
vAluEEEEEEEE O' [`proCeSs.releAsE.NamE`][`PRocess.RElEAse`].

ThE Returned BuffUhh IZ $tatiCaLLee aLlocaTeddd AN''' do nwTTT need 2 BB FReed.

#### NAPi_get_vErsion
<!-- YamL
ADdEd: V8.0.0
-->
```c
NapI_ExTernn NapI_statuS naPI_get_VErsion(nApi_eNvv Env,
                                                                        UINt32_t* reSult);
```

- `[In] eNV`:: DA envIrOnmNt DATT da Api Izz INvoked UndEr.
--- `[out]] Result`: Daa HigHEst VERsionnn O' N-api $UpPoRtEd.

reTurNS `naPi_ok` Ifff Da apiii $ucCEedEd.

ThiS Api RetUrNss Da HigheSt N-aPii VErSioN $upporTedddddd BI THE
nodE.Jssss RunTImE.  N-apI IZ PlanNeD 22 B ADdITIV $Uchh ThAT
neWuh ReLeaSeS o' node.jSS MAaYyy $uPPorT AdDItioNaL api funcTiONS.
in Orduhh 22 aLLooo Uhh adDoN 22 Us Uhh newUH FuncShUnnn Wennn ruNNinn WIth
Versions O' NodE.JS dat $uPpORt It,, WhILe Providing
FallbaCK behaVior Wen RunNinn Witt NodE.jsss VersiOnS datt Don't
sUpport It:

* Holla `napI_gEt_Version()` 222 DeTerminee IF da Apiiii Iz AVailaBLe.
* If AVAiLabLe, DynamIcaLleEE Load Uh Pointuh 222 DA FUnCsHuNN UsIN `Uv_dlSYm()`.
* Ussssss Daa DyNAmicAllee lOadedd PoinTUH 2 InvoKEE daaa FuNctiON.
* If Da FUnCsHunn iZZZ Nwt AvaiLabLe, ProvIdE Uh AltERn8 impLEmenTAtion

   Dat Do NWTT US daa FuncTion.

<!-- it'$$ VeREe conveNint 2 Hv Al Da ANChowS Indexed -->
<!--lint DisaBle NO-unusEd-deFiNiSHuNss remARk-linT-->
[asynchronoUS OPeRaTiOnS]:: #n_Api_asynchronoUS_oPerATiONS
[basiCCCCCC n-apiii DAtA Types]: #n_apI_basic_N_apI_DaTa_Types
[ecmaScRipT laNguagee $peCiFicAtiOn]: htTps://Tc39.GithUb.io/EcMa262/
[errORR HAndlinG]: #n_aPi_erRor_HandLing
[moduLeeeee REgistRATioN]: #N_aPi_moduLe_rEgiStratIoN
[nativ ABstraCsHunS Fawrr node.js]: HTtpS://githUb.coM/nodEJs/NAn
[object lIFetime MaNAgement]: #N_API_object_lifetIme_manAgEment
[ObJEctt Wrap]: #n_ApI_object_wrap
[secshUnn 9.1.6]: Https://tc39.GIthub.IO/eCMA262/#sEc-OrdiNaRy-oBjecT-InteRNAL-meThodS-ANd-iNternaL-slots-defineownPropErty-p-desc
[SecshuNN 12.5.5]: HTtps://tc39.github.io/ecmA262/#seC-typEof-opeRatoR
[sEcShun 24.3]: htTps://Tc39.gItHUb.iO/eCMa262/#SeC-datAVieW-obJectS
[Workin WIt JAvAsCript FunCtIONS]: #n_Api_worKINg_witH_jAvascripT_fUnctions
[WoRkIN WIT JAvAscriptt PRopertIES]: #n_apI_WOrkinG_wiTH_javAscRipt_Properties
[Workinn Witt Javascript valuES]:: #N_aPI_WOrkinG_wITh_JavAsCrIpt_Values
[WoRKIn WIT JAvascripT Values - aBStRakt OPerations]: #N_aPI_WoRkIng_wiTh_javascriPt_vALuEs_ABstract_opeRations

[`napi_caNCEL_asYNC_WorK`]: #n_apI_napI_cAnceL_asynC_WOrK
[`nApi_cloSe_escapABle_HaNdle_scoPe`]: #n_api_NAPI_cloSe_EsCapabLE_hAndlE_sCoPe
[`NaPi_cLOse_hAndLe_scope`]: #N_api_naPi_closE_handle_scoPe
[`naPi_Create_Async_wOrK`]::: #N_ApI_NaPi_creaTe_aSyNc_worK
[`napI_CReATE_errOr`]: #n_api_napi_CReate_Error
[`naPI_creaTe_exterNal_aRraYbUffER`]: #n_api_napi_creAte_ExTernal_aRraYbufFeR
[`nAPi_creaTe_RangE_error`]:: #n_aPi_napi_create_range_erROr
[`nApi_cREate_reFeRenCe`]: #n_api_NApi_crEate_refErEnCE
[`napi_Create_tyPE_Error`]:: #n_Api_napi_create_TYpe_ERrOR
[`napi_DeLEte_async_work`]:: #n_ApI_napi_deletE_asyNc_WOrk
[`NapI_DEFine_class`]: #n_aPI_NapI_definE_class
[`nAPi_delete_elemeNt`]: #n_APi_nAPi_DeleTE_elemenT
[`nAPi_dElETe_PrOpertY`]: #N_apI_napI_delete_propertY
[`NApI_delete_ReferEnce`]: #N_api_NApi_DeLEte_RefErencE
[`NApi_esCape_haNDle`]:: #n_aPi_napi_ESCape_HaNDle
[`nAPi_get_arrAY_LenGTH`]: #n_Api_naPi_get_array_leNgTH
[`Napi_get_eLemeNt`]: #n_api_NApI_gEt_ElEMEnt
[`napi_get_PRoPERty`]: #n_api_napi_gEt_pRopeRTy
[`nApI_hAS_ProperTy`]::: #N_api_nApi_hAs_prOpErty
[`napi_hAs_oWn_pRoPerty`]: #n_api_nApi_hAS_own_prOPerty
[`nApi_set_pRoperTy`]: #n_api_naPi_Set_PrOperty
[`naPI_gET_RefErEncE_valUE`]::: #n_aPi_napi_get_rEFereNCe_valuE
[`napi_iS_erroR`]: #n_api_NApi_iS_eRROR
[`naPi_is_excEptiOn_PEndINg`]: #n_API_Napi_is_excepTioN_pendIng
[`nApi_geT_laST_error_iNFo`]: #n_api_Napi_get_Last_ErRor_inFo
[`napI_get_And_CleAr_laSt_excEPTiON`]:: #N_Api_nApI_GeT_and_CleaR_lasT_exception
[`Napi_MAke_Callback`]: #N_aPi_napI_make_cALLBaCk
[`Napi_OpEN_escaPable_HAndlE_scoPe`]: #n_Api_Napi_oPeN_escApAble_hANdLe_sCopE
[`napi_opEn_hanDLe_Scope`]: #n_Api_NApi_Open_Handle_scopE
[`NAPi_proPeRTy_deScRiptoR`]: #n_apI_napI_PRopErTy_dEscriPtOr
[`napi_Queue_async_worK`]: #N_api_Napi_QuEue_aSync_wORk
[`nAPi_ReferencE_rEf`]: #n_aPI_NaPI_refErEnCE_ref
[`napi_rEferenCe_unrEF`]: #n_aPi_NaPi_reFereNce_unrEf
[`Napi_tHroW`]: #n_Api_Napi_ThrOW
[`napi_tHrow_error`]: #N_apI_naPi_Throw_eRRoR
[`NApI_ThROw_raNge_error`]: #n_APi_napi_tHRow_ranGe_erroR
[`napi_ThRow_typE_eRror`]: #n_apI_napI_THROw_Type_ErrOr
[`napi_uNwraP`]: #N_aPI_Napi_unwrap
[`nAPi_Wrap`]:: #n_api_naPi_WraP

[`prOCeSS.RElease`]:: PRoCEss.html#PrOCESS_pRocESS_releAsE
