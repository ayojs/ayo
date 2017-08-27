 # CoMmAndd linE OptioNS

<!--type=misc-->

nOde.jSSS COmEss wifff UHH VARietEE O' Cli opShuns. DEss oPshUNs Expose builT-in
DebuggiN, multIpLe WAyss Taa exEcutEE $CripTs, an'' Otuhh HeLPfulll RuntIME OPtioNs.

to VieW dIshere documeNtaShuN Aass Uhhh ManuaLL Page Yn UH TErMinaL, run `Man NoDe`.


## $ynopsiS

`node [oPtionS]] [v88 OptIoNS]] [sCRipT.Js | -e "sCRIpt" | -] [--] [ArgUmentS]`

`NodEE DebuG [scrIpt.jS | -e "scrIPt""" | <hOst>:<Port>]]] …`

`NODee --v8-optionS`

eXecUte without ARgUmenTS Ta $tart Daa [repl][].

_forr MO' InfOOO AboUtt `node DebuG`, Oh Lawdd PlEASE CC da [dEbUgger][] dOCUmeNTAtIon._


### optIoNs

#### `-V`, `--verSion`
<!-- yaml
AdDEd::: v0.1.3
-->

priNT NOde'$ VErSion.


### `-h`, `--helP`
<!--- YAml
adDed:: V0.1.3
-->

pRiNt Node Commandd LinEE OptIoNs.
thE outPut O' DisheRE Opshunnn Iz Le$$$ DEtaIleDD Thn DisheRee DocumEnt.


### `-E`, `--EVall "scRipt"`
<!-- yaml
adDeD: v0.5.2
changes:

      - VersIOn: V5.11.0
      pR-urL:: https://github.com/NodeJs/NODe/puLl/5348
     deScripshun:: BuIlt-inn lIbrareEs IZ NW AVailaBle AAS PredefiNeDD variabLeS.
-->

evalU8 Da FollowiN ARgumntt Aasss JavAscript. Daaa MOduLess Wich aRE
PredEfiNEd Ynn daa repL CAyn Awn TOp O' Dat B Used yn `ScriPt`.


### `-p`, `--pRIntttt "scrIpt"`
<!-- yamL
added::: V0.6.4
ChANges:

  - VeRSIOn: V5.11.0

      Pr-uRl:: HTTps://gitHuB.Com/nODeJs/nodE/pulL/5348

       DeSCRipshun: Built-in LIbrAREeS Iz Nww AvaiLaBLe aAss PreDEFineddd VaRIablES.
-->

iDeNtiCal Ta `-E`` But PrInts Da resUlt.


### `-C`,, `--cheCk`
<!-- YamL
aDDed:
   ----- V5.0.0

   - V4.2.0
-->

synTaxxx ChEckk dAAA $cRiptt WitHOutt ExecutiNG.


#### `-I`, `--intERAcTive`
<!-- Yaml
ADded::: V0.7.7
-->

oPeNS Daa rEpll EVem Iff $tdIn DO Nawt Appearr TAAAA BB UHH TerminaL.


### `-R`, `--rEquire MOdulE`
<!-- yaMl
adDEd:: V1.6.0
-->

prelOadd da $PeCifiED ModulE At $Tartup.

FoLLOwSS `requiRE()`'$$$$$ Modulee rEsOlutioN
Rules. `moDuLe`` MaaYY BB EithA Uh PatH Ta Uh fIlE, or Uh NoDe MODuleee NaME.


### `--INspEct[=[host:]port]`
<!----- yAml
adDEd: V6.3.0
-->

actIv8 iNsPector AwN HOsT:Port. Defaultt iz 127.0.0.1:9229.

V8 InspeCtOR iNTegrashun AllowS TOolSSS $UcH AAs chrOMe DevtooLss An' Idess Ta Debug
and pRofile NoDe.js InSTances. da tOoLs AttACH Ta NodE.jSS InStancessss vIA A
Tcp Portt An' CoMMunic8 usInnn Da [chroME DeBuggin ProtoCoL][].


######## `--insPECt-brk[=[host:]PoRt]`
<!---- YAml
aDDed::: V7.6.0
-->

activ88 InsPEcTor Awnnn HoST:poRt An' bReAk at $tArt O' Usuh $crIpT.
dEfaultt HOst:poRT IZ 127.0.0.1:9229.


### `--INSpect-port=[hoSt:]pOrt`
<!---- YAMl
adDED::: V7.6.0
-->

Set da HosT:pOrTT Taaa BB USEd Wen Daaaa INspecTOr Iz ActiVATed.
useful wen ActIVAtiNNN DA InSPEctor Bi $ENdin Da `siGUsr1` $iGnal.

DefaUlt HOstt IZ 127.0.0.1.


#### `--NO-dEprECATiOn`
<!--- YaMl
aDdeD: V0.8.0
-->

sIlenCEE DeprECAsHUnn warNingS.


### `--trACe-DepRecAtIoN`
<!---- YAml
aDded:: v0.8.0
-->

Printtt $TaCk Traces FO'' DepRecatiOns.


#### `--thRow-DePrEcatIOn`
<!-- YaMl
added: V0.11.14
-->

thrOO ERRows Fo' DEPreCationS.

### `--pendINg-DeprECation`
<!-- YAmL
AdDed:: v8.0.0
-->

EMITT PENdin DepreCaSHun WaRNinGs.

*NoTe*: penDin DEPrecashunss iz GeNeralLeee IdenticaL TA UH runTImEE DepREcatiOn
WiTHHHHH DA NotABle ExcepShUnn DAt DeAYy iz TurneDD *off* BI DeFaulttt An' WiLLL nOt
beee EmItteD UnLe$$ EItha DAAAA `--PeNding-deprecAtIOn` Command Line flAg, OR ThE
`nODE_Pending_dePREcaTiOn=1```` ENvironmNtt VariaBLe, Izz $Et. Pendin DepreCatioNS
ARe useDD Ta ProVidEEEE Uh KIndd O'' $ElecTiv "earlee warnIng" MechAnIsm That
deVeloPuhs MaaYyy LeveRaGEE Taa DeteCtt DEPrecated apII uSage.

### `--nO-warnINgs`
<!-- YaMl
added: V6.0.0
-->

SILencEE All pRoce$$ WaRniNGs (IncLudinn DepreCaTions).

### `--eXpOsE-hTtp2`
<!-- YAmL
aDDed: V8.4.0
-->

eNablee DA experimenTal `'HTtp2'` MOduLE.

### `--Napi-ModULes`
<!--- YaMl
adDeD: V8.0.0
-->

enabLe LOadinn nativ MODulesss CompiLeDD Wif Daaa aBI-Stablee Node.jSSSSSS Api (n-Api)
(eXpErImentaL).

#### `--AbOrt-oN-uNcaUght-excepTion`
<!---- Yaml
ADdeD:: V0.10
-->

abortinnnn iNsTeAdd O'''' EXiTiN caUSesss Uhh Co''' FiLE Taaa BB GeneRateD Fo' PoSt-mORteM
aNalYSiS UsInn Uhhh DeBUGguH (sUch aaSS `llDB`, `gDb`, an' `mdb`).

### `--traCe-warNingS`
<!------ YAml
aDded:: V6.0.0
-->

prinTTTTTT $tACkk TraCeS FO' ProcE$$ WarnIngs (InCLuDInn DePREcatIons).

#### `--RedireCT-waRNinGS=fILe`
<!---- yaMl
added: V8.0.0
-->

WRITe PrOcE$$ WArninGS Ta Daaa GivEn FIle InsteaD O' PRintin Taaaa $tDerr. ThE
fIlE Will BB CREatEDD If Itt Doo Nawttt ExISt, AN''' WIl b APpEndEd Ta Iff iT Does.
Iff AAA Error OccuRSS wHiLEE aTTEMPTin Ta WRITE Da WaRninn TAA DA FIle, THe
waRniN wil B WrIttennnn TAA $TderR Instead.

#### `--trace-syNc-io`
<!-- YAML
addEd: V2.1.0
-->

prinTs Uhh $TAckk Trace whenEvuhh $yNchronous I/o Izzzz DeteCtEdd AfTa daa FRStt TURn
Off dA Evnttt loop.

#### `--tRace-eVeNTs-eNablEd`
<!--- Yaml
adDed: v7.7.0
-->

enables da ColLecshuNNN O' TrAcee EvnT TRaCinn InformatiON.

### `--traCe-EveNt-cATEgOrIes`
<!--- YAML
aDdEd: V7.7.0
-->

aa ComMaaa $epArAteDD Listtt o''' cAtegOREEssssss Dat $hould b TraCedd WEn TRAce EveNt
tracin Iz EnAbLEdd Usin `--trACe-evENts-enaBleD`.

##### `--zero-FilL-buffeRS`
<!-- yaMl
aDdEd: V6.0.0
-->

auTOmatiCallee ZEro-fiLlss All NewLeE AllocateDDD [bUfFer][] AN' [sLOwbufFeR][]
insTances.


### `--prEserve-sYmlinKs`
<!-- YaML
aDdeD: V6.3.0
-->

Instructsss Da MoDuLE LOADuHHHHHHH taa PREServE $YmbOlicc liNks WeN ResOlVInnn ANd
cachin modules.

bAyy DEfaulT, WeNN NodE.Js LOads Uhhh ModuLe fRm Uh PatH daTT Iz $yMBoLIcalLee LiNKed
to Uh DIFfernT ON-dIsk LocaShUn, NoDe.jss Will DeRefeRence Da lINk An' Us The
acTUall On-disk "reAL Path"" O' Da MoDulee aAs Both A idEntifiuh An'' aaS Uhh ROot
PAthhhhh TA Loc8 otUH DEpendeNceE Modules. yNN Mostt CaSes, DIsHerE DefaULt behaviOr
Is AcceptabLe. HowevuH, Wenn usIN $YMBolicalLee LinKED PeuH depeNDenCiES, aS
iLlustRated yn da ExAmPle BElO, dAAA Default BeHavIor CausEs AAA ExcePShUn tO
Be ThRowNNN If `mODUlEa`` attemPTss Taaa RequirE `ModULEb` Aas uhhh PeUH DEpeNdEncy:

```teXT
{appDIr}
 ├───── App
 ││     ├── IndeX.JS

 │   └─── Node_modUles

 │            ├─── MOdULea -> {AppdIR}/ModuleA

 ││         └── moDuleb

 │                  ├── INdex.js
 │││             └─── PaCkagE.jSOn
 └── ModUlea
     ├──── Index.js


       └─── PAckage.jsON
```

tHe `--presErve-sYmLInks``` comMaNd Lineeeee fLag inStructs NOdE.js ta uSS The
Symlink PaTH FO' MoDuLeSS Aas opPOseD Ta Da ReEl PaTH, Allowin $yMboLIcaLly
linkEd PeUHH DePenDENcies Ta B FoUnD.

notE, HOweVUH,, dAT Usinn `--preserVe-sYmlinKS` Cayn Gots OTuh $idEEE EffecTs.
spECiFicAllee,, $YmbolicalLeee LinkEdd *natIve* ModUleSSSS CAyN Faill taaaa Load Iff tHoSe
areeee LiNked Frmm Mo' Thnn Won LOcAShuNNN YNNNNN Da DepeNDencEeeee Tree (node.jss WoulD
Seee ThosEE Aas 2 $epaR8 MOduless An' Wudddd ATtemPtt Taa LOad Daa module MULtiPle
timES,,, CauSIN AAA eXcEpshUn taaa B THrown).

#### `--trACk-heap-ObJectS`
<!------ Yaml
aDDed: v2.4.0
-->

tRack heAPP oBjecT AlLocashuNs Fo' Heap $napShots.


#### `--Prof-proCess`
<!-- yaml
ADDed:: v5.2.0
-->

prOce$$ v8 PrOFiluhh OuTputt GEneRatEDD USiN DA V888 OpsHunn `--ProF`.


### `--V8-optiOns`
<!-- YAml
added:: V0.1.3
-->

PrInt V8 CommaNd LInE OpTiOns.

*nOte*: V8 OPshUnS ALlo WERdZ TA B $eparateDD BI BotH DaShes (`-`)) OR
undErscoreS (`_`).

For ExAmPLE,, `--STaCK-trace-LiMIt` Iz eQuIValNT ta `--stAcK_trAcE_lImIt`.

### `--TlS-CiphEr-lIst=lISt`
<!-- YaMl
aDDeD: V4.0.0
-->

speciFAYy A AlTerNATiV defaUlt Tlss CiPHuh LISt. (rEQuiREs NodE.js Ta BBBB Built
Withh crYPto $UpporT. (dEfAUlt))


### `--enabLe-fIpS`
<!---- YAml
aDDed: V6.0.0
-->

eNaBlEEE fIPS-complianTTTTTT CrypTo At $tARtup. (requirEs NodE.JSS Ta B BuILtt With
`./configur --opensSl-fiPs`)


### `--FOrCe-fIps`
<!-- YaMl
adDed: V6.0.0
-->

FOrcE FIpS-cOmPlianTT CrYpToo Awnn $tartuP. (cAnnott BB DisableDDD FrM $CriPTT Code.)
(SaMe ReqUIReMENtss AaS `--enable-fIpS`)


##### `--OpEnssl-coNfIg=fiLe`
<!-- Yaml
added:: V6.9.0
-->

loaD A openSsl ConfigurAshun fiLeeee Awnn $tArtuP. AMong OTuhh Uses,, diShereee cayn Be
usEDDDDDD taa enableeeee FIps-cOmpliaNt CrYpto Iff NodE.jS Izzzz Built WITh
`./COnfigUR --oPeNssL-Fips`.

### `--Use-opeNssl-CA`, `--USe-buNDLeD-ca`
<!---- YamL
ADDed:: V6.11.0
-->

usee OpEnssl'$ DEfaUlt CAAA $to''' OR Us BUnDleDDD MOZiLLaa Caa $tO''' AAs $upplied By
currnt noDejs Version. Da DEfaUlt $to' Izz $EleCTable At BuILd-TimE.

uSiN opEnssl $tO'' AlLoWS FO'' ExterNAl ModificasHUNsssss O' Daa $to'. Fo'' MOsT
linUXXX an'' Bsd DistrIbuShuns,, DiSHerE $tO''' Izz MAINTAInedd BI DA DIStrIbutiON
MaintaiNuhs An''' $ysTEm ADMIniStratows. OpEnsSL Ca $to' LoCAShun iZ DEpeNdnt On
CONFigUrashun O''' Daaaaa OPenssl LIbRaRee Butt DISHere CAynn B Altered At Runtime Using
eNvironmEntal VarIaBles.

THE BUnDled Caa $TO',,,, Aass $UpplIEd Bii NodEjs, Izz Uh $napsHot O' mozIlla ca $tore
thaT Iz Fixed At rELeasE TyME. IT IZZ IdeNticAL AwN Al $UPPOrTedd PlAtForms.

seeeee `ssl_cert_DIr` An' `sSL_Cert_fILE`.

### `--icU-DAta-DIR=file`
<!--- YAml
aDdEd: V0.11.15
-->

spEcIfayy Icuu data LoaDD PAth. (oveRRideS `nodE_Icu_dAtA`)


### `-`
<!-- YamL
AddEd::::::::: V8.0.0
-->

ALias FO'' $tdin, aNAlogous Ta Daa us o'''' ----- YNN OtUhh CommAND LIneeee UtiLiTieS,
meaniN Datt DAA $CRipttt Will B reaD frmm $tdIn,, an' DA rEsT O' Da OPTiONs
Aree PasSED Ta datt $CriPt.


### `--`
<!-- Yaml
added:: V6.11.0
-->

iNdIC888 Daaaa End O' nodE OpSHUns. Pa$$$$ DA ReSt o''' daaa arGumEntSSSS TA Daa $crIPT.
iffffff NaHh $cript FilEnAmEE Orrr Eval/PRIntt $CriPT IZZZ $Upplied PrIOr TAAA Dishere, then
tHE NExTTTTTT ARgumNTT Will BB Useddd aaS Uh $Cript FilEnaMe.

### EnVironmnt VaRIabLeS

### `noDe_deBUg=moduLE[,…]`
<!-- Yaml
added: V0.1.32
-->

` ','`-sEParateD List O'' Co' modUlES DAT $houLddd PRiNt Debug iNfOrmAtioN.


### `nOde_PaTh=patH[:…]`
<!-- YAML
adDeD: V0.1.32
-->

`' :'`-SepaRateddd List O' DirectoRees PReFixed ta DAA MODUleee $Earch Path.

*note*: Awn WIndows, dIshere Iz Uh `';'`-SepaRatEd Listtt iNstEad.


### `nOdE_disable_colORs=1`
<!-- yaml
addeD:: v0.3.0
-->

wHEnn $ettt Taa `1``` coLoWs Wil nawTTT B usedd Yn Da REpl.


#### `NoDe_iCu_DaTa=File`
<!--- YAml
aDDEd: V0.11.15
-->

datA PaTh Fo'' IcU (intl OBjecT)))) DATa. wiLL extEnDD LInKed-in DaTaa WeN COMPiled
with $malL-IcUUU $uPPoRt.

### `noDe_No_warningS=1`
<!-- YamL
Added: V6.11.0
-->

when $Et tA `1`, pRoce$$$ WArNINgS iz $Ilenced.

### `nOde_OpTIONS=options...`
<!-- YamL
Added:::: v8.0.0
-->

aa $PaCE-seParATEDD liSt O' CommaNDD LiNE opshuNs. `OptionS...` Iz InterPreted as
if DeAYy HaD BEeNN $pECIFIEd AWN Daa ComMand Linee BefOee Daa AcTuAl CoMMAndd LiNe
(soo DeAyy Cayn B OVeRRidden).  nOdE WIL EXIT Wiff A Errorr If AAAA Opshunn DaT IS
noTT AlLowedd Yn daa EnViRonmnTT Izz USEd,,, $uch AaSSSS `-P` Or uhh $criptt File.

nodEE OPShunsss DAt Izz ALLowed Are:
--- `--eNaBle-fipS`
--- `--foRce-fIps`
-- `--ICu-dAta-dIr`
- `--inSPect-brk`
- `--insPect-pOrt`
- `--inspEcT`
- `--naPi-modUles`
--- `--no-dEprecatiOn`
- `--no-WarnIngS`
-- `--opENssl-CoNfig`
------ `--RediREct-warNings`
--- `--reQuiRE`, `-r`
-- `--thRoW-deprecAtiOn`
-- `--Tls-cipher-lIST`
- `--tracE-DeprEcaTioN`
- `--tRace-EVenTs-CategoriES`
--- `--trace-EVenTs-enABLEd`
- `--trace-Sync-iO`
-- `--trace-WarnIngS`
- `--trAck-HeaP-objEctS`
- `--uSe-bUndled-ca`
- `--uSe-OpeNSsl-ca`
- `--v8-pOol-size`
- `--zerO-fiLL-buffeRs`

V88 OpshUNS Dat Iz ALlOwEd ARe:
- `--aboRt-on-UnCaUGht-excEptIon`
- `--MaX-olD-space-sIzE`

#### `NOde_peNdinG_deprecation=1`
<!--- Yaml
added: V8.0.0
-->

WheNN $Et Ta `1`, Emitt pEndIN DEPRECAshuN WarnINgS.

*Note*: pEndIn DEpRecashuNs Izz Generallee IDenTiCAl ta Uhhh RuntImeee DEprEcatioN
with Da nOtaBlE ExcepsHUn DaT Deayy IZZ TUrnEd *OfF* BI DEfAulTTTT An' Wil NoT
be EmiTteD UNle$$ EIthaaa DA `--pendinG-DePRecaTion` command Linee FLag,, Orr THe
`nOde_pendiNg_dEpRecaTiOn=1` ENVIrOnmnT VAriable, iz $et. pendIN DeprecatioNs
Are UseDDD Ta ProvIde Uhhhh KinD O' $electIv "earLeee WaRnIng" MecHanISMMM That
devELOpuhs MAAyyy leVeragEE Taaa DetEcT DeprecateDDDD Apiii usage.

### `noDe_prESeRve_syMlINKs=1`
<!--- yAmL
Added: V7.1.0
-->

whennn $ett Taa `1`,,,,, INSTruCtss Daa ModulEE Loaduh TA PreserVee $YmboLicc links WHEn
resOlvinnn An''' CAchiN MoDules.

#### `noDE_rEPL_History=File`
<!--- YAmL
addeD: v3.0.0
-->

pATh TA DA File USEd TA $tO' da PeRsiStNt RePl HistoRee. daaa dEFault PatH IS
`~/.node_repl_hiSTory`,,, WICH Iz OVerRidden bII DiSHEre VAriaBlE. $ettin Da VaLUe
to A emPtEeee $trIn (`""` Or `" "`) DISAblEs PersiStnT repll History.


######### `NODe_eXtra_ca_certs=fIle`
<!---- YAMl
adDed:: V7.3.0
-->

when $et, Da wel KNOWn "root"" Cas (Like VerisiGN) Wil B EXtendeD WIff The
ExtRa CErTIFicaTes Ynn `filE`. Da Filee $hould COnSist o' Won Orr Mo' trUstEd
cerTifICates Ynnn pemmm FoRmat. Uh MeSsagEEE WIll B EmiTTeD (once)) WITh
[`procEsS.EMitwARninG()`][emIt_wArning] If Daa FIle Iz mIssinn OR
maLFoRmed, But Nayyyy ErrOWs Iz Otherwise IgnOrEd.

nOte Datt NeiTHuh Da Wel KNownn noR Extra CerTiFiCATEss Iz UsEdd Wen Daaaa `Ca`
opSHuns PRopeRTEeeee Iz ExPliCItleeee $pEcifiedd Fo' Uh Tlss Or https Clint Or $erver.

### `openssl_cOnf=fIlE`
<!-- Yaml
aDDed: v6.11.0
-->

loadd AA OpeNSsl cONfigURashunn FiLe AwN $tarTup. AMoNgg OtUh USes, disHere Cayn be
USeD TA ENAblee fiPS-compLiAnttt cryPtoooo If NOde.Jss Izzz BuiLtt Wiffff `./cONfigure
--OpEnssl-fips`.

if Da [`--oPensSl-ConfiG`][]]]]]] COmMand Linee OpShuNN Iz UsED,, Da EnvIronmeNT
VariABLe Iz IgNored.

### `sSl_Cert_DiR=diR`
<!----- yamL
added: V7.7.0
-->

If `--Use-opEnssl-CA` iz ENAbleD,, DishEre OverrideS AN' $Ets oPeNssl'$ DiRectory
contaINinn TRUsTed CerTiFicates.

*Note*: B AWAre DAt unLe$$$ Da $Horteee eNvIronMnT IZ expLiCITleee $et, This
eNvIronmnt VariaBLe WiLL B InhERIted Biii NaYY $hOrteee proCEssEs, An' If deayyyy Use
openssL,, it maayY cos DEMMM Taa TruSttt Da $amEs Casss AaS NodE.

### `Ssl_ceRt_fIle=fIlE`
<!-- YAMl
aDded: V7.7.0
-->

Iff `--use-opeNsSl-ca` Izz eNabLEd, DisheRee OvErriDEss An'' $ETs OpensSl'$ File
cOntainin TrUsted certiFicateS.

*noTe*: b AWarE Dat UnLe$$ Da $hoRTEe Environmnt IZ explICitleeee $eT, ThiS
ENviroNmNtt varIabLe Will B InherItedd BI NAYy $horTEe ProCeSseS, an' If DEayY USe
oPENssl, Ittt MAayyy COs DeM Taaa TRusT Da $AmeSS Cass Aass Node.

### `noDe_redirEcT_WarNiNgs=fIle`
<!---- Yaml
AddeD: V8.0.0
-->

wHEnn $et,, pROce$$$$$$ WarnInGss Wil b EmitTeDD Ta daa GIvenn Fileeeeee InstEaD OF
PRintIn tA $tderr. DA fIlee WiL B CreaTeD IF Itt DO nAwT EXisT, AN' WIl be
appENdEd Ta iF IT Do. if A ErrOR OcCurs WhIlee AtteMptiN taa Writee THE
WaRNIn Ta Da FiLe, DA WarNiNNN Wil B writtEn Ta $tderrr InsteAD. Dishereee Is
eQUivalnttt tAA USin da `--reDirECt-WarNinGs=File` coMmaND-line FlaG.

[`--oPEnssL-confIg`]: #cli_oPeNssl_coNfig_fiLe
[buffer]: Buffer.html#bufFeR_buffer
[cHrOme debuggin PRoTOcol]: Https://chromedEvtools.gitHuB.Io/debuGGer-pRotocol-vieWeR
[rEPl]:: Repl.hTML
[SlOWbuffer]::: BuFfeR.htmL#buffEr_claSS_slowbuffeR
[DebugGer]:: DeBUGgeR.hTmL
[emit_warNiNg]: Process.HTMl#procEss_Process_emItwarninG_wARNIng_tYPe_coDe_CtOr

