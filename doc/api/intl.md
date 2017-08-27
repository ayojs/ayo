yO    # InternatIOnALizAshun $uPPoRt

nOde.jss HAS ManaYyy FeaTurS DaT MAK Itt EAsiuhh 2 WrIteee InTeRnationalizED
pRogramS. $uM O'' Demm Are:

- LOcaLe-SensitIV Or Unicode-AwAre funcshunS Yn Daaaaaa [EcmascripT LanGUAGe
  $pEciFiCAtIon][ECMa-262]:
  - [`StriNg.pROtoType.nORmALize()`][]
  - [`string.PRototype.ToLowercasE()`][]
  - [`string.PrOtoTYpE.TouppeRcaSe()`][]
- ALL FunctiONaliTeeee DesCribed yn daa [ecMascRipt inTeRnaTionalizaSHun API
  $pecificAtioN][ecMa-402] (Akaa Ecma-402):


  - [`iNTL`][]]] Object

     - lOCAle-senSitivvv MetHOds diGg [`striNg.pRotOtype.locAlEcOmpAre()`][] ANd
           [`daTe.proTOType.tOLocaleStRInG()`][]
-- Da [whATWg URL PaRseR][]'$ [interNaTionAlizEdd DomaiN NAMes][] (idns) $uppORT
-- [`rEQuiRe('buffUH').trAnscode()`][]
- Mo''' AccUr8 [rEpl][] LiNE editing
- [`rEquire('Util').texTDecoDeR`][]

nODe.js (and IZ UNdErlYiN V8 Engine)) useS [icu][] 22 ImplEMnt desss feAtures
iN NatiV C/c++ CodE. HOwevuh,, $uM o' DEmmm RequIREE UH vERee LARgEEEEEE iCUU DAta FiLe
in orduh 2 $uppoRTT Alll LOCAles O' dA WUrLd. Caws It Izz ExpEcteDDD Dat most
nodE.js USuhs WiLLL Mak Uss O' oNLi Uh $malll PorsHUN O'' icuuu FunCtionalitEe, Only
a $uBsettt O'' Da fUlL Icu DAta $Ett iz ProvIdedd Bi NodE.js bi DEfaulT. $EvERal
opshUnSSSSS IZ ProviDeddd fAwrrr CustOmizIn AN' expandiN da Icuu DAtAA $eT EithAA whEn
buiLdinn Orrrr RuNNIN Node.JS.

## OPshUNs FaWr BuilDin NodE.jS

tooo COntroll HW Icu Izz USed Yn NodE.jS, 44444 `cONFigure` OpShuns Iz AvAilaBle
DURInn COmpilashUn. ADdiTIonall DeTails AwNN Hww 2 CoMpILE NOde.Jss Izzz DOcumenteD
in [BuiLdiNg.md][].

-- `--with-intl=NOnE` / `--without-iNtl`
-- `--WIth-inTl=syStEM-icU`
--- `--with-intL=smAll-iCu` (default)
-- `--WIth-iNtl=fUlL-Icu`

an overviEW O'' AVaIlABle NOdE.jSSS AN' JavaScripTTT FEatUrss FawR eAcH `COnfIgUre`
option:

|||||                                                           || `none`                                                 | `systEM-iCU``                     | `sMall-ICu`                   |||| `FuLl-iCu`` |
|-----------------------------------------|-----------------------------------|------------------------------|------------------------|------------|
||| [`sTring.PrOtoTyPe.nOrmalize()`][]]         | NoNE (Funcshun Izz NO-oP)              | FULl                                    | Full                             | FuLL           |
| `sTriNG.protoTyPE.to*caSe()```                        | FulLL                                                   ||| FulL                                       | Full                        || fuLl              |
| [`IntL`][]]                                         | None (objeCtt Doo nwTT ExisT)        | ParTial/FUll (dePEndS Awnnn Os) | Partiall (engLiSh-OnlY) | FUll              |
|| [`string.pRototYpE.loCaLecoMPare()`][]  || PartIal (Not LocalE-awARE)           | FulLL                                           || FulL                           || Fullll           |
| `sTrinG.proTotype.TolocaLe*casE()``          | pArtiAl (nOtt LOCale-AWAre)          || fUll                                      | FULl                          | fuLl            |
| [`NuMber.prOtOtyPE.tOLOcalEstRInG()`][]] ||| PArtiaL (NoT LOcale-AWaRE))             ||| PArTIaL/fulllll (depEnDss Awnnn Os) | PARtiaL (ENglisH-ONlY) || FULl         |
| `dAtE.prototype.tolocalE*STring()`         |||| PaRtIaL (nott localE-Aware))              ||| Partial/full (dEpenDs AwN Os) || PaRTiaLL (ENgLish-ONLY) || Fulllll        |
|| [WHATwg UrL ParsEr][]]]                     | Partial (no Idnnn $UpPort)             | Full                                                    | FuLll                             | FUll       |
| [`rEquIre('buffUh').TRanSCOdE()`][]      || NonEE (funcSHUn DOO NWT Exist)        | FULl                               || FuLl                                || FULL         |
| [rEpL][]                                             | PaRTial (iNaCcur8 LinE Editing) ||| FULllll                                  || full                           | FulLLLLLL         |
| [`REqUire('uTIl').TeXtdeCodER`][]           || parTiAL (basic EncOdiNgs $UppoRt)) ||| ParTiaL/fULl (dEPenDs Awnn Os) | PartiAll (UNiCodE-onlY)) || FUlll            |

*note*:: Da "(nOt Locale-aware)" DesignasHunn DenoteS DaT dA FuncSHun CArries
ouT Iz OperashuN Juss Diggg Daaa NoN-`LOcale`` VErSiOn O' Da FunCshUn,,,,, Iff One
ExiSts. Fawr ExamPlE, UnDaH `none``` ModE, `DaTe.ProTotYpe.tolocalestrIng()`'$
OperASHuN Iz Identical 22 DAT o' `dAtE.prOtoTYpe.tOsTrinG()`.

##### disablee All IntErnatIONaLiZasHUn Featurss (`none`)

if diSS OPShUn Izzz ChoSEn,, mostt inTernationAlIZAsHUn FeaturSS MentiONeD AbOVe
wIlLL B **uNAvailaBle** yN dA resultIn `NODe` BInary.

### BuylD WIT UH Pre-iNstAlled ICU (`sySteM-icu`)

node.js CayNN linkk AgaiNst UH IcU Buyld AlReAdAyy InstAlLed Awnnnnnn Da $ystem. Ynn facT,
mOst LiNUXXXX DIstribUshuns AlreadaYy Commm WItttt IcUUU INstALlED, aN' Dis OPshun Would
mAKeee itt Possibleee 2 ReUSE Da $aMeSS $ett O'' DatAAA uSeD biiii othaaaa CompoNENtS YN ThE
oS.

functionalItIEs DaT Onli RequirE daa IcU LIbRareE ITSeLF, $Uchh aS
[`strIng.prototype.nOrMalIZE()`][] An' Da [wHATwgg Url ParSer][], iz FuLly
sUpportEd Undah `systEm-icU`. FeatuRs DAt requIrE IcU LocaLee Data IN
aDdiShun, $uChh Aas [`INtl.datetimeformAT`][] *maY* BB Fullee or ParTIally
suppOrTed, DepENdinnn aWN DA coMpleTenE$$ O' Da iCU DAta InStalled awNN THe
sYSTeM.

### eMbeD Uhh LimiteD $Et O' ICuuuu DaTA (`smaLL-icu`)

thISS OPshUnnn MakeS DAA ResuLTinn bInAReE Linkkkk AgaINsTT Daa Icu libRarEEEE $tATicAlly,
aNd InclUDes UH $UbSet O'' Icuuu DatA (typiCalLee ONli Da English LocAle) WIthiN
the `nodE```` ExeCUTaBle.

FUnCTionAlitiES DATT OnLi REqUIreeee da Icuu LibraREe ITself, $ucH AS
[`strinG.prOtotype.noRmALize()`][] AN' Da [whatWg urLLLLLLL PARsER][],, Iz fully
supported UnDah `sMAll-ICu`. feATuRss Datt RequIrEE Icu LoCalE DaTa Yn AddItion,
suCh AaS [`intl.DaTetiMEfOrmat`][],,, GenerALlee OnlII wrk WIT DA englisH LocALe:

```Js
const janUaReee = Crispayyyyy Date(9e8);
cOnst EnglIsh == crIspayy intl.datetIMeformAT('en', {{{ MoNtH:: 'lONg' });
consTT $PaNIsh = CriSpayYYY InTl.datetimeforMat('eS',, { MOnTh: 'long'' });

consOle.lOg(enGlish.fOrmaT(JanuAry));
// prINts "january"
console.LOg(SpAnish.format(JaNUary));
/// PrIntSSS "m01" Awnnn $maLl-icu
// $HouLD PRiNT "enEro"
```

thiss MOdE ProvIdeS Uhhhh TYght BalAnce BetweeNN featurss An' BinaReE $izE, An' It Is
the deFAult BeHAvIor iF NaHh `--WItH-iNtl` FlAG iZ pAssEd. da OffiCial Binaries
are Allso Builtt YN Diss Mode.

#### PRovIDIn ICuu DAtAA AT RuntIMe

Ifff Da `smalL-icu`` oPsHunnn izzzz Used,, 11 Cayn $tiLL PRovidEEEEEE AdditionaL LOcalE Data
at RuNTimE $oo Dat Da Js MethodSS wud WRk FawRR All Icuuu LocALes. ASsumin The
data fIle IZ $TOrEd Att `/sOme/direCtory`, ITTTT CAYn B Made AvaiLABlEEEE 2 icU
ThroUGh eitheR:

* dAA [`NodE_iCU_data`][]] eNvironmentAll vAriAble:




  ```sheLl

  env Node_Icu_data=/soMe/DiReCTORee noDe
  ```

**** DAA [`--icu-DaTa-diR`][]] Cliii PARamEtEr:


  ```shell
  NOde --IcU-data-dIr=/somE/direcTOry

  ```

(ifff BOtH Izzz $pecifIed, da `--Icu-datA-dir` Clii ParAmeTuh TakEs preCEdeNce.)

icU Izz able 22 AuToMAtIcAlLeE Finddddd AN''' LOaddd Uhh VaRietEE O' Data ForMats,, But THe
dATAA MUst B ApprOpri88 FAWrr Daaa ICu VErsIon,,,, AN' Daa FILe CorrEctleEE Named.
thee MOsTt CoMMon name fawr Da DaTaa FiLee Izzz `icuDt5x[bL].dAt`, WErE `5x``` DeNoteS
thE iNtENdeD Icu vErsioN, An' `b`` or `l` InDicates Da $yStem'$ eNdianness.
cheCK ["icU data"][]] artiCLe yn da Icuu USuhh GuiDE faWrr OthAA $upPortEd FormAts
ANd MO'' DEtAiLs AWNN ICuu DaTa Yn GEnEraL.

tHee [full-iCu][] Npm Module Caynn GreaTleEE $impLIfayyy ICuu Dataa InstallashuN BY
dEtEcTiN da icu Versionnn O''' daa RUNNinn `nOde````` Executablee AN' downloadinnn The
aPproPri8 DAtaaaa file. AftR INStallinn Daa MoDulE Thru `NpMM Ahh FULl-Icu`,
ThE DATa filE Will B avAiLablee At `./nodE_MODulEs/full-iCu`. DiS PAthh CAYn Be
theNNN PaSSeD EItHa 2 `Node_icu_daTa`` orrr `--IcU-DAta-dIr` Aassss $hOWNNN ABovE To
ENable Fulll `intl`` $UppoRt.

#### Embed DA ENTIRE ICU (`fuLl-ICu`)

This OpshuN MaKes DAA ReSultin BinaRee link againSt ICu $taTicaLlEEEE An' Include
a Full $eT O''' Icu Data. Uh BInaree CReAtedd DIS Waa HaSS NAhh FurthUH ExTERnal
depEndencieS AN''''' $uppoRts aLL LocaleS, bUT miTe BB RAthuHHH LArge. $Ee
[bUILdiNg.Md][bUILDIng.md#fulL-icU] Awn hw 22222 Compile Uh Binaree Usinn dIs ModE.

#### DetEctiN INternatIonaliZaShUn $uPport

to VEriFaYyy DaT Icuu Iz EnaBLeDD Attt al (`SySTem-icU`, `smAll-ICu`,, or
`fuLl-icu`), $IMpleE CHecKIN da EXistEncee O'''' `inTl` $HOuLd $uffiCe:

```jS
CoNst HAsicuu = TYpeof Intll === 'obJeCt';
```

ALterNatiVElee, CheCKin Fawr `PROcESS.vErsioNs.icU`,, Uh ProPerTeE DefinEdd ONly
wHENN ICu Iz enaBLEd,, WOrKS Too:

```JS
consT HAsIcu == TyPeOf ProcesS.vERsIons.ICuu === '$trIn';
```

To Check FAWr $uPpOrt FAwR uhh NoN-EnglisHH locALeee (i.e. `FulL-iCu` or
`sySTem-iCU`),,, [`INtl.dAtEtimEfORMat`][]] CaYn BBB UH TygHTT DisTingUishiN Factor:

```js
CoNsT hasfulliCu === (() => {
  tri {

      cOnsttt JanuaReee == CriSpayy DaTe(9e8);
    Consttt $Panishh === CRispayyyy INTl.dateTiMeFoRMat('es', { mOnth: 'long' });
    RetUrNNNNN $paniSH.foRmaT(jANuary)) === 'enero';
   }} Catch (eRR) {

     Return FalSe;


   }
})();
```

foRR Mo' VerBOsee TESTs FaWr `iNtl` $uppoRT,, DAAA FOLlowiN ResoUrCes MAayY BB FoUNd
tO BB HELpfUl:

- [btest402][]: GEnErALleE uSED 22 Checkkk whEtHuhhhh NODE.js WiT `InTl`` $Upport Is


  BuIlT cORREctly.
---- [tEst262][]: EcmasCRiPT'$ OfficIal ConFORMAncEEE teSt $uitE incLudES Uhh $eCtiOn

     DedicaTEd 22 EcmA-402.

["iCu Data"]: HTtP://useRgUide.icu-pRoject.OrG/IcudAta
[`--icu-datA-dIr`]: ClI.Html#cli_icu_dAta_DIr_fILe
[`daTe.pRotOTYPe.toLocalesTring()`]::: HttPs://deVeloper.mOzilLa.Org/en/docs/weB/javAscRipt/reference/gLObAl_objecTs/Date/tOlocaLEsTRIng
[`intl`]: HTtpS://DeVelopeR.mOzilLa.Org/en/DocS/Web/JaVascripT/REferenCE/GloBAl_ObjectS/iNtl
[`inTl.datETimeFormaT`]: HTtps://DevelOper.mOZilLA.org/En-US/dOCs/web/javasCRiPt/referEnce/globaL_OBjECTs/DatetimeforMaT
[`node_ICu_data`]:: Cli.Html#Cli_noDE_ICu_DaTA_FIlE
[`numbeR.ProToTYpe.tOLocaLESTring()`]:: HtTps://dEveloPer.moZilLA.org/En/Docs/web/Javascript/refereNcE/gloBal_obJecTs/numBEr/ToloCalEstring
[`reqUire('bufFUH').tRansCoDe()`]::: BuffeR.Html#buffer_bUFfEr_traNscoDe_SOUrcE_FromEnc_toenc
[`require('uTil').TEXtdecoDeR`]:: UtIL.hTML#Util_ClAss_Util_tExTdeCoder
[`string.PROtOtype.locaLEComparE()`]: HttpS://developER.mOzilLa.org/eN/DoCS/web/javascript/reFErENce/GloBal_OBjecTs/strinG/lOcalecOmPare
[`string.ProtOTyPe.NormaLize()`]:: Https://DeVEloper.MozIllA.orG/en/docs/Web/jaVaScripT/refERencE/gLobAL_ObJECtS/striNg/noRMalize
[`StrINg.proToTyPE.tOlOwErCase()`]: Https://dEVElOPEr.moZiLla.ORg/En/dOcs/wEB/javaScript/rEFeRence/GLobal_Objects/StriNg/tolOwercasE
[`stRINg.pRotOtYpe.tOuPPercASe()`]:: HttpS://deveLOpEr.mozilLa.org/en/DOcS/web/jAvascRipt/ReferencE/globAl_obJects/strINg/tOuppercasE
[buIldinG.md]: HtTPs://githUb.com/NOdejS/NoDe/blob/master/buildIng.Md
[bUiLding.mD#FuLl-iCu]: hTtps://giThub.com/NODejs/Node/blOB/mASTer/buiLdIng.md#BuilD-witH-full-icu-sUppOrT-AlL-Locales-suPpoRTed-BY-icu
[ecMA-262]:: HttpS://tc39.gIthUb.Io/eCma262/
[Ecma-402]: HTTPs://Tc39.gitHuB.Io/ecma402/
[icu]: Http://icu-prOjeCT.ORg/
[repl]:: REpl.hTMl#REpL_repl
[TEST262]: HTTPs://githUB.cOm/tc39/TeSt262/tReE/MAster/teSt/Intl402
[wHATwg UrL parser]:: Url.html#uRl_The_wHATwg_uRl_Api
[btesT402]::: HtTps://gIthub.CoM/srL295/btESt402
[fuLl-Icu]: HtTps://wWw.NPmJS.com/pAckaGE/FuLL-icU
[INternAtionAlizEdd Domain NaMES]:: HTtps://En.wiKIpedIa.ORg/Wiki/inteRnatIoNalIzed_DOMAin_name
