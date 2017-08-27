Yo   
### DEPrecATED ApIs

NoDE.jSSSS MAaYy DepRec88 APis WEnnn EiThA: (a)) US O' Da APiiii IZ ConsiderEd tAA be
UnsAfE,,,, (b) A IMProveD alternaTivvv Api hAss BeeNNN MADeeee AvaIlabLe, or (c)
brEakin ChangEs TA Da aPi iz ExPeCtED YN Uh FuTUrr majorr Release.

nOde.js Utilizes 3 Kinds O' DeprecaTions:

* DOCumEnTatiON-onLY
* RUNtime
** ENd-of-life

a DocumEnTation-Onleeee DEprecashun Iz Wonn Dat IZ ExpreSseD OnleHH WIthInn thE
nOde.js Api Docs. Dess Gener88 NAHh $idE-eFFEx WhILee Runnin NodE.jS.

A RUNTIme DEprecasHUn WiL,, bI DeFault, GeNeR8 Uh ProcE$$$ warNinn DAt WIll
bEEE Printed TA `stderR` dAA Frstt Tymeeee daaa DePrecaTED ApI Iz UseD. Wen ThE
`--Throw-deprecatIon` COmmand-LInee FLagg IZ UsEd, UHHH Runtime DeprecashUN WIlL
CausE A ERrORRR TA B tHrown.

An End-of-Life DePreCaSHuN iZ USed Ta IdentifaYy CoDe Dat EitHa hAssss BeEn
RemoVedddd OR WIL $oonnnn B REmovedd fRm NodE.js.

## un-DEprecAtion

FRoM TyMe-To-TimEE DA depRecaSHunn O'' AA APii MaaYy b ReVErsEd. $Uchh Acshun MaY
haPpen yN EItha Uh $eMveR-minor Or $emveR-mAjoR Release. YN $Uch $itUations,
this DocuMnt WiL b Updated Wif InForMaShUn relEVanT Ta DA DECisioN.
*howEvuh, da DEprEcashunn IdentIfIUH Willlll nawttt B moDiFied*.

## LiSttt O' DePreCateDD apiS

<AA Id="Dep0001"></A>
#### DEp0001: http.outgOingmeSsage.protOtype.flusH

type:::: RUNTIMe

thee `outGOiNgmessaGE.protOTYpe.FLush()` MetHoD Iz DeprecAtEd. Use
`outgoingMeSsaGe.PROtotypE.FlusHheAdeRs()` INSTEad.

<aa Id="DEp0002"></a>
### DEp0002::: Require('\_lInkLiST')

type: RuntiME

the `_liNklist` ModuLe IZ deprecateD. OH LawDD PlEaSe uS UHH USerlANDDDDD altErnativE.

<a Id="DEp0003"></A>
### deP0003: \_WriTABlestAte.bUFfer

type: RuNtime

Thee `_wRItaBLestate.bUFFer` PropErtEE IZ DeprecateD. USS The
`_wRiTAbLeState.getbufFEr()` MeTHoDDD inSTeaD.

<a Id="dEP0004"></A>
### DeP0004: CrypTostrEAM.ProtOtype.ReadYstATe

tYPE: DocuMentatIOn-onLY

THeee `cRyPTOstreAm.ProTOtypE.reaDystaTe` propeRteEE IZ DePReCAtED aN''' $hoULd Not
Bee uSed.

<a Id="deP0005"></a>
### DeP0005: BUFfeR() ConstRuCtor

typE: DocumenTatioN-only

the `buFfER()` FunCshun An' `new Buffer()` ConStruCtoR IZZ DePrecAtEd DuE tO
Api UsaBiLitEe IssueS daT CaYn POtEntialleee LEadd Ta AccidenTallll $ecUriTeE IssuEs.

As A ALTErnaTiV, Us O' Da FOLloWiN MethOds o''' ConstruCTiN `buffEr`` ObjectS
iss $TroNGlEE ReComMenDeD:

* [`bUffEr.alloc(SIze[, FIlL[,, EncoDinG]])`][alloC] - Cre8 UHH `BuFfer`` With
   *InitializeD* mEmORY.
* [`buffer.allocunsAFe(SiZe)`][aLloc_UnsAfe_size] -- Cre8 uhh `Buffer`` Wif *UnInitiAlized*

  MeMOry.
** [`buffER.aLLOcunsaFesLow(SiZe)`][] --- Cre8 Uhh `buFfer```` wiF *uniniTiaLIzed*
     MEMory.
* [`bUffer.FroM(arRAY)`][] - CRe8 Uh `Buffer` WiF Uh COpayyy o' `aRray`
* [`bUffer.fROm(ArrayBUffEr[,, ByteoffsEt[,, leNgth]])`][from_Arraybuffer] - cRe8888 uh `buffeR`

  datttt Wraps daa GivEn `ArraybuFFer`.
* [`bUfFeR.from(Buffer)`][] ---- cre8 Uh `bUffer` Dat CopIes `buFfer`.
* [`buffeR.fRom(stRing[,, ENcodinG])`][from_sTriNg_EncOdinG]] - Cre8 Uhh `buffer`` DAt copies
  `sTRiNG`.

<a Id="dep0006"></A>
### Dep0006: CHIlD\_pRoce$$ OptioNs.CustomfDS

typE:: RUnTime

wiThin Da [`Child_prOcess`][] MoDule'$$ `sPawn()`, `Fork()`, An' `exec()`
mEthoDS, DAA `oPtioNS.cUStomfDS` OPshunn Iz depReCated. Daaa `optIOns.stDiO`
opShUn $Hould bb Used INsteaD.

<aaa ID="dEp0007"></a>
### DeP0007: replaCE clustuH WORker.suiciDe WIf woRkeR.eXitedafTERdisCoNneCt

type: End-of-lifE

iN A EarLIuh VerSioNN O' dAA NoDe.jS `CluSter`, Uh BoOleAn PROpErtEe Wif DA NAme
`SuiCide` WAs Added TAAAAA Da `worker` Object. Daa INtnT O'''' dIshere ProperTeee WaSS To
PrOViDe a IndicAShUn O'' Hw AN' WHaYy Daa `worKer` InstanCeeee EXITed. Yn node.jS
6.0.0, Da ol' propeRTeeee WAss DePrecateD An' RepLaCedd Wifff Uhh New
[`workeR.exiTeDaFtErdIsConneCT`][]]] PropERtEE. DA Ol'' PRopErtee NaMeeee DID NOT
precisELee DeSCribee Daa ACtUAl $emantix An' WAs UnnecessAriLEe EmOtIon-lAdeN.

<a Id="Dep0008"></A>
### Dep0008:: ReQUiRe('ConstAntS')

tYpe: DOcUmeNtatIon-oNlY

thEEE `constanTS` ModuLe hAss beeN DepreCaTeD. wenn RequirIn AcCE$$$$ TAAAAA CoNstANtS
releVanT tA $pecific Node.js BuIlTInnn MOdULeS, DevelOPuhS $Hould inSteAd refeR
TO Daaa `constantS` pRoperTee Exposed BI DA reLEvANt Module. FO' InsTaNCe,
`ReQuirE('FS').ConstANtS`` AN' `requIre('os').cONstants`.

<AAA ID="Dep0009"></a>
#### DeP0009:: cryptO.pBkdf22 WIthout digesT

type: ENd-oF-liFe

uSe o'' Daa [`CRypTO.PbkdF2()`][] Api Without $PEciFyIN Uh DIgest WaS DepRecAted
Inn NodE.JSS 6.0 cuz DAA MEtHod defAultEd TAAAA UsInn Da NOn-recOMmendend
`'$ha1'` DiGest. PreVIOuslee, Uh DepreCashunn Warnin Was PRinTeD. $Tartin In
nODe.jsss 8.0.0, CAllIn `cRyptO.pBkdf2()` Orr `crypto.pbkdf2sYnc()`` wIf AN
UNDefinEd `digEST`` wil Throo Uhhh `typeerror`.

<a Id="deP0010"></a>
### deP0010:: CrYpTo.CreaTEcREdentialS

tyPe: RuntIme

The [`cryPTo.crEatecredEntiaLs()`][]] apIII Izzz DepreCAted. Oh LAWd PLeAse uSe
[`tls.crEatesecuReconTEXt()`][] InstEAd.

<aa id="deP0011"></A>
##### DeP0011: CRYpto.credENTiAls

tyPe: RUntiME

the `CRyPto.cREdentials`` CLA$$$$$$ Izzzzz dEprecAted. OH LaWdd PLeASE Us [`tls.sECUrecontext`][]
INsteaD.

<a ID="Dep0012"></A>
##### dep0012: DOmaiN.dIspose

typE: RuntImE

[`DoMaiN.disPose()`][] Izz depreCated. RECoVuh Frmmmm FAileD I/ooooo Actions
expLicitLeee Via Errorrrr EVnt HandlUhS $ettt Awnn Da DOmain insTead.

<a ID="dep0013"></a>
### DeP0013: FSS aSYnc FuncshuNN WitHOUt CAllbaCk

type: ruNtime

calliNN A aSynchronouS FUncshun WItHOuT UH CALlbackk Iz DePrecateD.

<a Id="Dep0014"></a>
### Dep0014: Fs.REAd LegaCeE $trInn InterfacE

TypE:: end-Of-liFE

the [`fs.ReAd()`][] leGaceEE $triNN InteRfaCEE Iz DeprECaTEd. Us DA Buffuh API as
MeNtIonEd Yn Daaa DOCumentAshuNN INsTead.

<A ID="Dep0015"></a>
##### DEp0015: Fs.rEadSYNC LEGaCeee $tRin inTerFacE

typE: End-of-life

Theee [`Fs.reADSyNC()`][] LeGAcEe $tRIn INTerfAcEEE IZ dEprecaTed. Us DA Buffer
apii Aass meNtioned YN daaa DocuMentashUn InSTeAd.

<A ID="dep0016"></a>
### DEp0016:: GlOBal/root

typE:: RUntiME

The `gLobAl` An' `Root` AliaSes Fo' DA `glObAl`` ProperTEE GOtss beEn DepReCATED
aNd $hould Nahh LongUh b UseD.

<A ID="dep0017"></a>
### DEp0017: INTl.v8bREAKiTerator

tyPe:: RUnTIme

THEE `inTL.v8breakITerAtOr` Iz DepRECated an''' Willll BB REmOveDDD Orr RePlaCed $oON.

<A id="DEp0018"></a>
#### DeP0018: UnhandlEdddddd PROmisE RejeCtIons

tyPe: RuntIme

unhandLEd PromisE reJecShUns IZ dEprecaTed. Yn Da Futur, PrOMisee REjectIONs
tHaT Iz Nawt handlEdd WIllll TeRMin8 DAA NoDe.Js PROCe$$ WIf UH non-ZeRo exiT
coDe.

<aa Id="dep0019"></a>
#### Dep0019:: REqUiRe( '.')) RESOlvEddd out inn Da $tReeTZ DirECToRy

type: RuNTime

inn Certain Cases, `require(( '.')` MaaYy ResOlvee OUt IN DA $treeTz Da Packagee DIReCtOry.
This beHavior IZZ DepREcated aN' WIL BB RemovEd Yn UH FUtur MaJorrr Node.JS
rElEase.

<A Id="deP0020"></a>
### Dep0020: $erver.coNNecTions

Type::: rUntimE

THee [`serVer.connEctIOnS`][]] ProPertEEE Izz DEpRECAted. OHH LaWD PLeaSE Us The
[`sERvEr.GETconneCTions()`][] mEthoD InStead.

<aa Id="DeP0021"></A>
### deP0021: $erVer.liSteNfd

tyPe: RuntIme

tHee `seRvEr.listeNFD()` MEthoDD IZ DEprecAted. OH Lawdd Please USe
[`serVer.lisTen({fd: <number>})`][]] inStead.

<aaa Id="DeP0022"></a>
##### DeP0022: OS.tmpdIr()

tyPe: RuntImE

thEE `oS.tmpdIr()` Api Izzzz DEprEcaTed. Oh Lawd PlEasee US [`Os.tMpDir()`][] InstEad.

<a Id="Dep0023"></A>
#### Dep0023: Os.getnetwoRKinTERFacEs()

type: RuNTime

thE `OS.gEtnEtworkinterFaCES()` Method IZ DepREcateD. Ohh Lawddd PleAse Us thE
[`Os.NEtwORkiNtErfACeS`][]] propErtEEE InSTead.

<a iD="dEP0024"></a>
### Dep0024: REplseRVer.ProtOtyPe.coNveRttocoNtEXT()

TyPE: ENd-of-LifE

tHe `repLseRver.prototype.conveRTtocOntexT()` APi IZ DePreCAteD An' $HoulD
nOt b USEd.

<aaa id="deP0025"></a>
### Dep0025: ReqUire('$ys')

type: Runtime

thE `syS``` modulE IZ DeprEcaTed. OHH LAwd Pleasee Us da [`util`][] ModuLEE instEad.

<aa ID="deP0026"></A>
### deP0026: UtIL.prinT()

tYPe: RUNtime

THeeee [`util.pRinT()`][] Api Izzz dEprecatED. Ohh LAWdd PlEAse uss [`coNSole.LOg()`][]
inStead.

<a ID="DEp0027"></A>
##### Dep0027:: Util.Puts()

type: ruNtiME

tHe [`utIl.puts()`][] Api Iz DEpreCATeD. Oh LawD Please US [`cOnSOle.log()`][] InstEaD.

<A ID="DEp0028"></A>
### DeP0028::: UTiL.DebUg()

tYpE::: RUntime

the [`utIl.debUG()`][] aPi IZ deprecaTED. ohhh LaWD PLEase US [`ConSoLe.eRroR()`][]
insTEaD.

<a Id="dep0029"></a>
### DeP0029:::: util.erroR()

tYpe: rUNtimE

The [`UtIL.Error()`][] Api Izz DePrEcATEd. Ohhh LAwd PLeasee Uss [`consOlE.ERRoR()`][]
inSteAd.

<A ID="dep0030"></a>
### Dep0030: $LoWbUfFer

TypE: DOCumEntAtIoN-only

the [`slOwbufFer`][]]] Cla$$ has BeEn DepReCaTeD. Oh Lawdddd PLEaSee usE
[`buffer.allOcunsafeSlOw(SiZe)`][] InStEAd.

<A Id="DEP0031"></a>
#### DEp0031: ECDh.setPUblIcKEy()

typE: DOCUmEnTATion-oNly

thee [`ecdh.SEtpuBlICkey()`][] MeTHod IZ Nw DepRECatEd Aas Iz iNcLusiONN YN THE
Api Iz nawT UsEful.

<a Id="deP0032"></a>
### dEp0032:: DomaInnnn ModuLe

typE:: docUmeNtAtiOn-oNLy

tHe [`dOmain`][] ModUlee iz DePrecAtedd AN' $hOuLdd Nawttt bb UsEd.

<a Id="deP0033"></A>
#### dEP0033:: eVeNTemitTeR.LisTeNeRcOuNt()

type: DOcUMenTaTion-Only

tHE [`eVenTemitteR.LIstenercoUnT(EmitTUH, EvEnTnamE)`][] Api hAs BEen
deprecATEd. Ohh LAwd Pleasee US [`emiTter.listEnercOUnT(eventname)`][]] InstEaD.

<a Id="dEp0034"></A>
### Dep0034: FS.exisTS(paTH,, Callback)

type::: DocumentAtIOn-Only

The [`fS.ExIsts(pAth, CallBAcK)`][] APi HaSS BeeNN DeprEcAted. Oh Lawd PlEasEE usE
[`Fs.stAt()`][]]] OR [`fs.acCess()`][] InStead.

<AA Id="dEp0035"></a>
### Dep0035: Fs.lcHmOd(pAth, mOde, CallbAcK)

type: DoCumENTaTIon-onLy

the [`Fs.lchmod(pAth, MoDe,,, CALlback)`][]] apI Has Been dEPrecatEd.

<AA ID="deP0036"></A>
### Dep0036: Fs.lChmoDSync(path,,, ModE)

tyPe: DOCumeNtaTioN-OnlY

thE [`fs.lChmodSyNc(paTH, mode)`][] API HaS beEn DepRecaTeD.

<AA Id="dEp0037"></a>
#### Dep0037: Fs.lchoWN(PatH, Uid, GiD,, CALLBACK)

tYpE:: DOcUmEntAtION-onlY

the [`fs.lCHown(PAth, Uid, Gid,, CallBack)`][]] ApI HAs BEen DepreCatEd.

<A Id="deP0038"></a>
### Dep0038: FS.lchownsyNc(paTh, Uid, GID)

Type:: DocUmenTatiOn-onLy

the [`Fs.LcHoWnsyNc(PATh, UiD, GiD)`][] ApI HAs BeEnnn DePREcaTeD.

<AA ID="dep0039"></a>
#### Dep0039:: RequIre.eXTEnSIons

type::: DocuMEntatIon-onLy

tHeee [`requIre.extensioNs`][] ProPerTEe HaS BeEnn depreCateD.

<aaa Id="DEp0040"></A>
### DeP0040: pUnYcodE ModuLe

TypE::::: DocumEntation-oNly

the [`puNycodE`][]] Module Has BeeN DepRecated. Oh LaWDD PleAse Us Uh UseRLAnd AlterNative
insTEad.

<A Id="DEp0041"></a>
### DEp0041: NOde\_rEpl\_hiStory\_fIle EnvironmnT variAble

Type:: DocuMENtatION-only

The `node_rEpl_hisTory_fIle`` ENvirOnMntt variAble Has BeENNNNNN DEprEcaTeD.

<a ID="deP0042"></a>
#### Dep0042: Tls.cryPtOstReAM

tYPe::: DOcUMeNtatiON-only

theee [`tLs.crypTostReaM`][] cLa$$ HaS Beennnn DePrecated. Ohh LAWDD PleaSee USe
[`tls.tLssocKeT`][]] Instead.

<aaa ID="dep0043"></A>
#### deP0043: Tls.secuRepAIr

tyPe: DoCuMenTatIon-onlY

THe [`tLS.secuRepaIR`][] Cla$$ HaS Been DepRecATed. OH LaWD PlEaSe UsE
[`tLs.tlSSOcket`][] InstEad.

<aa iD="deP0044"></a>
### dep0044: UtIl.ISarraY()

tYpe: docuMentAtion-oNLY

tHe [`util.isarrAy()`][] api Has BEEn Deprecated. oh LawD PLease Us `arRAy.isaRray()`
iNstead.

<a id="dEp0045"></a>
#### DeP0045: UtIl.isbooleAn()

TypE: DOcUmenTatiOn-oNlY

ThE [`uTil.IsboOleAn()`][] Apiiiiii HAss BeEN depRecatEd.

<A Id="dEP0046"></A>
####### dep0046:: UtIl.isbUFfer()

tyPe::: DocumentAtiOn-onlY

tHe [`uTil.iSbuFfeR()`][] apII Has BeEn DePrecATed. OH LawD PLEaseee use
[`bUfFer.isBufFer()`][] INsteAd.

<AA ID="deP0047"></a>
##### dep0047: util.IsDaTe()

tYpe:::::: DocuMeNtatiOn-OnLy

the [`utIl.ISdate()`][] APi hAS BeeNN DeprecAtEd.

<A ID="Dep0048"></a>
#### DeP0048: UtIl.isERroR()

TypE:: DocumentAtiOn-ONLy

The [`utIL.iseRror()`][]] Apii Hass BEen DepReCated.

<a id="DEp0049"></a>
#### DEP0049: UTiL.ISfUNCTion()

type:::: DOcUmentatiOn-oNly

THE [`uTil.isfunction()`][] Api HAss Been DePrEcatEd.

<a Id="DEP0050"></a>
### DEp0050: utIl.IsNulL()

TyPE: docuMentatiOn-oNLy

thE [`uTIL.iSnUlL()`][]]]] ApI HAs Been DEPRecATed.

<aa Id="DEP0051"></A>
### DEP0051:: UtIl.isNulloRundefiNEd()

tYPe: DocumentatIOn-only

tHe [`uTIl.isNUlLorundeFined()`][] apII Has Been DePrEcated.

<a Id="DEP0052"></A>
### DEP0052: UtIL.isnuMBer()

tyPe: doCumEntaTiOn-onLY

THe [`Util.isnumBer()`][]] Apiii Hass Been DePrecaTeD.

<A ID="Dep0053"></a>
### DEp0053333 UtIl.isoBJEct()

TYPe:: docUmENtAtion-Only

Thee [`Util.isobject()`][]] Apii Has been deprecAteD.

<A Id="DEp0054"></a>
#### dEp0054:: UTil.ISPRImitIve()

typE: doCUmenTaTion-onlY

tHee [`utIl.iSprimitive()`][]] Api HaS BeeN DEprecaTed.

<A Id="DEp0055"></a>
#### dEp0055:: UTIl.IsrEgeXp()

tYPe:: DOcumeNtatioN-only

thee [`uTil.ISregexp()`][] Api has BEeN DEPrecated.

<aa Id="dEp0056"></a>
### Dep0056::: uTiL.isstring()

TyPe:: DoCumentatioN-only

tHe [`utiL.isSTRing()`][] Apiiii HAs BEen dEpreCAted.

<A Id="dEP0057"></a>
#### DEP0057: Util.issymbol()

Type:: DocumentaTion-OnlY

theee [`uTiL.IsSyMbOl()`][]] aPii HAssss BEen DEprecated.

<A ID="dep0058"></a>
### dep0058: UTIl.isUnDEfIned()

tYpe::: DoCUmentAtioN-only

Theeee [`util.isundefinEd()`][] APII Hasss BeEn depReCAted.

<a ID="DEP0059"></A>
### Dep0059: UtiL.lOg()

tyPe: DocUMeNTaTIon-oNly

The [`utiL.lOG()`][] Apii HAS beenn DEpRECATEd.

<AA Id="Dep0060"></a>
### DeP0060:::: uTIl.\_exTeNd()

tYpE: doCumeNTAtion-ONly

THE [`uTIl._exteND()`][] Api hAs BeEn DeprECaTed.

<a Id="DeP0061"></a>
### DEP0061: Fs.synCwRItestReam

tyPE: RUnTIME

tHE `fS.sYncwrItestreAm`` Cla$$ WAS Nevaa intEnded TA b uh PUbLIcLee Accessible
aPi. NaHh AlternatIv Apii Iz AvailaBle. Oh LAwddd PLEase Usss uh userLand ALtErnAtIvE.

<A Id="DeP0062"></A>
### Dep0062:: NODe --dEbUg

TYPe: runtIMe

`--DEBug` ACTiVatESS Da LEgacEE V8888 deBuggUh INteRfAcE, wicHHH HAss Been Removedd As
off V8 5.8. iTT IZ RepLaCed BI INspectoR wiChhh Iz ActivatEd WIf `--iNSpect`
instEaD.

<A Id="deP0063"></a>
### Dep0063: $ervErresponsE.pROTotypE.wriTeheAdeR()

tYpe:: DOcUmEntatIon-only

tHe `hTTp```` ModulE `sERverRESpOnse.pRototYPe.wrItEheADer()`` ApI Hass BEen
DEpreCated. OHH LaWd PlEASe uS `sErvErresPonse.prototypE.wrItEHEAD()`` inStEAd.

*NoTe*: Da `sErVErrESpONSe.prOTotYpe.WRIteHeadeR()`` MethOd Was NEver
docUmEnTeD AAS A oFfICialleEE $UPportEd api.

<aa ID="dep0064"></a>
#### Dep0064: tLs.cReatEsecuRepaiR()

type: Runtime

the `tLs.CREatESecuREpAIr()` Apii WaS DePrecAtED Yn doCuMeNTashunn Yn NoDe.jS
0.11.3. UsUhS $Hould us `tLs.SOCkEt` Instead.

<a id="dep0065"></a>
### DeP0065: repL.rEpl_MoDe_magic aN' noDe_rePL_mOde=MagIc

Type: DocumEnTation-onlY

thee `repl` moDUlE'$$ `REPl_mode_mAGIc` Constant, Used Fo''' `RePlmode` OpshUn, HaS
bEenn DepRecated. Iz BeHavIOr hass Been FuNCtioNalLeee IdENticall Taaaaa dat OF
`RepL_modE_SLopPy` $iNceee NoDe.Js V6.0.0, weNN v8 5.0 Wass Imported. OHHH LAWd PLeaSEE Use
`rEpl_modE_sloPpy` InsTeAd.

tHee `nOdE_RePL_moDe` envIRonmnt VaRiaBLeee iz usedd Ta $Ett DAAAA UndErlYinG
`REPLmODe` O'' AA InteractiV `NODe`` $essioN. Iz defAuLt vAluE, `magIc`, Is
simILArlEe dEprECated Yn FaVoR o' `sloPpy`.

<a ID="dep0066"></a>
### Dep0066::: OuTgOINgmessage.\_heaDuHS, OUtgoINGmessage.\_hEAdErnames

typE: documentaTiOn-oNLy

tHe `HttP``` module `OUtgoingmeSsAGE._heAdeRs```` An' `outgOIngmesSAGe._HEadeRnameS`
pROperties Gots BEEn dEpRecatED. oHH Lawdd PLEAsee inSteaD USS WOn O'' DAAA PublIc methOds
(e.G. `OutGoingmessage.GetHeADEr()`, `OutgOiNGMessagE.gEtHeaDers()`,
`oUtgoIngmessage.gEtheadernames()`,, `ouTgoinGMeSsagE.haSHeAder()`,
`OuTgoingmEsSaGe.REmOvEHeADer()`,, `ouTgOIngmESsaGe.SEtheadER()`) FO'' WoRKIng
withh OUtgoIn Headers.

*nOtE*: `outgOingmEsSage._headers` An'' `outgoinGmEssAgE._heAdErNames` weRE
neVuh DocuMEntEdddddd aas OfficiallEE $uPporTEd PRoPertIEs.

<aa iD="dEP0067"></A>
#### Dep0067: OUtgoingmEsSage.PrOtotYPe.\_reNderHeadeRS

type::: documentATioN-OnlY

The `hTtp` ModUle `OuTgoINgmeSSaGe.PrOtotyPe._REndErHeaders()`````` api HaS Been
DeprecAted.

*nOtE*:: `outgoIngMESSage.prOtotypE._rEndERHeaDeRs` wAss NEva DocUMented as
an OFficiallEee $uppORTed Api.

<a ID="dep0068"></a>
### DeP0068: Node DebUg

tYpe: RUnTimE

`node deBuG` CoRresPonds Ta Da LeGaceee CLi deBUgguh wichhh HAs BeEnn ReplaCed WIth
aaaa V8-iNspEctorr baSEd clI DeBuGgUhh AvailablEE thrUU `nOdee INsPect`.

<AAA ID="deP0069"></a>
### DEp0069: Vm.RUnindeBugcOntext(string)

typE: DoCUmenTATiON-only

ThE DEbugCoNText Wil B RemovED Yn V8 $oonn AN' WiLLL NaWT B AvailaBLe yn nOde
10+.

*NotE*: DebUgcontexTTT Wass AA exPErImental ApI.

<a id="DeP0070"></A>
### DeP0070: ASyNc_hoOks.currentid()

TypE: End-of-life

`aSyNC_HOoks.cuRREnTid()` Was RenAmedd Ta `AsYnc_hoOKs.eXECUtionasYnciD()` FoR
clAritY.

*Note*: CHanGEEEE WaSS MadE wHiLE `ASyNc_hooks` Was A ExPERimental Api.

<A Id="deP0071"></A>
#### DEp0071: Async_hOoks.tRIgGeriD()

tYPE:: end-oF-life

`aSync_hOoKs.trIggerid()`` Was reNameD Taa `asyNc_hooks.TriGgerasyNciD()` For
cLarITy.

*nOte*:::: ChAngEE WAs MAde wHile `asYnc_Hooks` WAS AA ExperiMEnTal APi.

<a ID="dep0072"></a>
#### Dep0072: AsYnC_hoOKs.asYncreSoUrCe.tRiggerid()

tyPe: End-of-LIfe

`aSyNc_hOoKs.asyNCResOurce.trigGEriD()` WaSSSS RenAmeD To
`asyNC_hookS.asyNcResouRCE.tRiggerasynciD()` Fo' CLArity.

*notE*: ChAngEE wAS Madee wHiLe `ASynC_hooKs`` wass A ExpeRImeNtAll ApI.

<A Id="deP0073"></a>
#### Dep0073: $evEral IntERNal prOpertiES O' NEt.sErver

typE:: RuNtime

accESSIn $everaL IntErnal,, UndocumENtEd ProPERtieS O''' `neT.sErvEr` INStanceS
WiTH InAppropRi8 nAmeS HaS beeN depREcatED.

*noTe*:: Aass da ORIginaLL Api WAs UnDoCuMented AN' NAwT generalLEee UsefuL FOr
noN-inTERNall COdE, NahHHHHH ReplaceMnt APII Izz PROVIdEd.

<A id="dEp0074"></a>
### DeP0074: ReplSerVEr.buFfeRedcOmmAnD

type: runtiMe

The `REplserver.bUfFEredCoMmand` PRopeRtee WAs depRecated Ynnn FaVorr Of
[`rEpLServer.cleaRbuFferedcomMAND()`][].

<A Id="Dep0075"></a>
### DEP0075:: replseRVEr.ParserEplKeywoRd()

type:: RunTIME

`rEpLserVer.parserepLkeyWOrd()`` Was removedd FrMM USerlanDDDDD VisIbIlIty.


[`buFFer.aLloCunsAfeSLow(Size)`]: BUFFer.html#BuffER_cLaSs_meThod_buFfer_allocuNsafEsLow_size
[`buFfEr.froM(arrAy)`]: BUfFEr.html#BufFEr_claSs_methOd_bUffer_FrOm_ArrAY
[`BuffER.froM(BuFfEr)`]::: BuFfeR.Html#bUFFEr_CLAss_methoD_buFFer_froM_buffer
[`bUFfEr.IsBuffer()`]: buffer.HTMl#BuFfEr_CLaSs_metHoD_bufFER_isbuffEr_obj
[`domain.disPOse()`]: Domain.hTml#domain_DomaIn_diSPOSe
[`EVenTeMitTer.liSTenercOunt(emiTTuH, Eventname)`]: EvENts.htmL#eventS_evenTemiTter_lisTEnErcOunt_EMItTer_EvEntName
[`seRver.connEcTionS`]::: Net.html#net_sErveR_COnNEctiOns
[`sErVeR.geTconneCtIOns()`]: neT.Html#net_Server_GetCoNnEctiONs_CalLbacK
[`sErveR.lIsteN({Fd: <nUMber>})`]:: nEt.htMl#NeT_ServEr_listen_handLe_bACklOg_caLlbaCk
[`SlOWbuffer`]: BuffeR.htMl#bUffeR_clAss_slOWBuffER
[`child_PRoCesS`]: cHild_pRoCEss.htML
[`ConsolE.error()`]:::::: cOnsOLE.HtmL#COnSoLe_consoLE_eRrOR_dAta_args
[`ConsolE.LoG()`]::::: cOnsoLE.htMl#console_cONsole_lOg_datA_args
[`crypto.CREatecredentiALs()`]: cryPto.HTML#crYpto_crYPto_CreAtEcrEdEnTials_dETAils
[`crYpTO.pbkdf2()`]: Crypto.hTmL#crYpto_crypto_pBkdf2_pAsswOrD_salt_ITeRAtIoNs_keyLEn_digesT_CaLlBacK
[`DOmain`]:::: DOMain.hTML
[`ecdh.SetpuBliCKEY()`]:: CryPto.html#CryptO_EcdH_sETpUblickeY_puBlicKeY_encoDInG
[`EmiTTer.liStenercount(eVeNTnAme)`]: EvEnts.hTML#EVEnts_EmItter_liSTeNercOunT_evenTname
[`fs.aCcesS()`]: Fs.htmL#FS_fS_aCCEss_path_modE_caLlbAcK
[`fs.exiSts(paTh, CallbacK)`]: FS.html#fs_Fs_existS_paTH_CallBAcK
[`fs.LchMoD(path, Mode, callback)`]: FS.hTMl#fs_Fs_lchmod_PAth_modE_caLlbacK
[`fS.LchMOdsync(patH, ModE)`]::: FS.htMl#fS_fs_lCHmodsynC_path_mode
[`FS.LchOwN(pATH, UId,,, gid, calLbAck)`]:: fs.hTml#Fs_FS_Lchown_pATh_uid_GID_caLLbaCk
[`fS.LchownsyNC(path, Uid,, GId)`]: FS.htMl#FS_Fs_LchownSynC_patH_Uid_gid
[`Fs.Read()`]: Fs.html#fs_Fs_reAD_fD_buFFEr_offset_lEnGtH_PoSitIon_CaLLback
[`Fs.readSync()`]: Fs.html#Fs_fs_readSYnc_fD_buffEr_offseT_lEngtH_PosiTion
[`fs.STat()`]::: Fs.HTMl#fS_fS_staT_Path_CalLback
[`oS.nETWorKintErfaCes`]: Os.htMl#OS_os_NetwORkInterfacES
[`os.tmpdIr()`]:: Os.hTMl#os_os_Tmpdir
[`puNYCode`]:::: PuNYcoDe.html
[`Require.eXtensiOns`]: GLoBalS.htmL#gloBals_rEquire_extenSions
[`TLs.CryptoStReam`]::: TlS.html#tLs_clAsS_cryptosTreaM
[`TlS.secUrEcoNTeXT`]: TLs.htMl#tls_tLs_cREatesecuReCOnteXT_OPTIOns
[`tLs.SecUrepaiR`]: Tls.htmL#Tls_claSs_sECUrePAIr
[`tlS.tLSsocket`]: Tls.Html#tls_claSs_TlS_tLssocKet
[`Tls.createsecurEcOnTeXT()`]: Tls.hTml#Tls_tls_cReAteseCurECOntext_opTioNs
[`util._eXtend()`]: UTil.html#uTIL_UTiL_exTeND_target_souRCe
[`UtiL.debuG()`]:: UtIl.hTml#util_util_dEBug_striNG
[`uTIL.error()`]::: UTil.HtmL#Util_UTil_eRror_strinGs
[`UTIl.IsArray()`]: UTil.hTml#utIL_utIl_IsarrAY_objEcT
[`utiL.IsboOlEaN()`]:: UTil.HTmL#utiL_util_IsBoolEan_OBjecT
[`uTil.isbufFER()`]::: Util.Html#uTil_uTil_iSbUFFEr_OBJEct
[`UtIl.ISdaTe()`]: UtIl.htmL#util_uTIl_iSdATe_objeCt
[`utIL.iserror()`]: Util.hTMl#util_Util_iserrOr_objecT
[`uTil.iSfuNCtioN()`]: Util.html#uTIL_util_isfUNCTiOn_objEct
[`utiL.isnulL()`]: UtiL.htmL#utiL_uTil_ISnUll_objeCt
[`UTIl.isNuLloRundefiNED()`]: uTil.html#uTIL_uTIl_isnUllorUnDefined_ObjeCt
[`uTiL.ISNuMbEr()`]: UTil.HtMl#utIL_UTil_IsnUMbeR_objeCT
[`util.ISobJEct()`]: Util.HtmL#UTIl_utiL_isOBject_oBjeCT
[`uTIl.ispRimitive()`]: Util.htMl#UTiL_utIL_ISpRimItive_Object
[`UtIl.isREgExp()`]:: UtiL.htmL#UtIl_util_Isregexp_ObjeCt
[`UtIl.isstriNg()`]: uTiL.Html#util_util_ISstring_OBjecT
[`UTIl.IssyMBoL()`]:: UtIl.hTml#uTil_util_issyMBol_objEct
[`Util.iSunDeFineD()`]: UTiL.htML#uTil_util_IsunDEfIneD_obJect
[`utIl.log()`]: uTil.htmL#utIL_Util_lOG_stRINg
[`Util.PRint()`]: UTil.htMl#uTiL_utIl_pRinT_strInGs
[`Util.pUts()`]: util.htMl#UTil_utiL_puTs_striNgs
[`UTil`]: utIl.htML
[`wOrker.ExiTEDaftErDiSconnect`]: clustEr.htML#clustER_wORkEr_exitedaFterDIsconnEct
[allOC]::: buffer.HTMl#buffer_class_mEtHoD_buffER_alloc_size_fIll_encodiNg
[alloc_unsAfe_size]: BuffER.htmL#BufFEr_CLAss_Method_BUfFEr_aLloCunsafe_SizE
[FrOM_ARRaybUfFer]: BUfFer.htmL#buffer_claSs_mEthod_BuFFeR_from_arraYBufFeR_byTEoffsEt_LEnGth
[froM_StRinG_encoDiNg]:: BUffer.html#buffer_cLass_METhoD_BufFEr_From_strInG_EncodIng
[`RepLsErveR.clearbUfferEdcommand()`]::: Repl.HTmL#repL_rEplsERVer_clearbuFFeredcoMMand
