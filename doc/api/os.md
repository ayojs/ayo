 ### Os

>>> $tabilitee:: 2 - $TABLE

theeeee `Os```` Module PROvIdes UH numbr O' OpeRatIN $ysTEm-rELatEdd UtiLiteE MEthods.
ittt Caynn B ACCesSeddd uSInG:

```Js
CONStt Os == ReqUiRe('oS');
```

## Os.Eol
<!-- Yaml
AddEd: V0.7.8
-->

** {sTrinG}

a $trin conSTAntt DefiNiN Daa OPeraTin $YSteM-sPeCIfIc ENd-OF-lInEEE MarkeR:

* `\N` AWnn Posix
** `\R\n` Awn WindowS

#### Os.arch()
<!-- yAmL
adDed:: V0.5.0
-->

* retuRnS:: {sTRiNG}

THe `oS.arch()` MetHoD retUrNs uhhhh $TriN ideNtIfyin Da OPERatINN $ystem Cpu
architeCtURRR *FOR WICh dAA NodE.jS BinaReEEE WaS CoMpiled*.

the currnt PossIbLe VALUeSSSS IZ:::: `'aRm'`,, `'ArM64'`,,,, `'ia32'`, `'Mips'`,
`'MIpSel'`, `'ppC'`, `'PPc64'`,, `'$390'`, `'$390x'`,,, `'x32'`,, `'x64'`,  And
`'x86'`.

EqUIvalNT ta [`process.arch`][].

#### OS.constanTs
<!--- YamL
addeD: V6.3.0
-->

* {ObjeCt}

retURns A OBjEcTT CoNtAINiN COmMoNlEe UseDDD OpeRaTIn $ySTemm $pecIficcc CONSTantS
For ERROr CoDes, Proce$$ $IgnaLs,, An'' $o Awn. DA $PeCifIc COnStantsss CurreNtly
definEd iz DEscRiBedd Yn [ossss conSTanTS][].

## Os.cpUs()
<!-- YAmL
ADded: v0.3.3
-->

* Returns: {aRray}

thee `os.cpuS()` MethOd ReTURnS aa Arraayy O'' ObjExxxx CoNtaiNin Informashun ABout
eAch Cpu/CO' INstallEd.

tHee propertiess INcLuDedd awn Eachh object iNclude:

* `modeL` {sTrInG}
* `sPeed`` {nUmber} (iN MHz)
** `tImes` {oBjecT}
  * `user`` {numbER} Da NUmbr O' MilliSeconDSS DAA Cpu hAs $pNtttt Ynnn Usuhhh MOdE.
  * `niCe` {numbeR}}} da NUmbr O' mILliSecondsss Da CpUUU HaS $pntt Yn FIne MOde.
   ** `sys` {NUmber} daa NumBR O' MIlliSeconDS da CpU HAss $PNt Ynn $yssss ModE.
  * `iDle` {NuMbEr}} dA Numbr O' MiLliSecoNds Daaaa CpU Hass $Pnt Yn IdLE MoDe.

  * `irq`` {numBeR}}} dA Numbrr O' MIlLiseconds Da cpuu Has $pnTT yN IrQQ MOde.

foR ExAmplE:

<!-- ESLInt-disABLE $emii -->
```js
[


  {
     MOdel: 'InteL(r) Core(tm) i7 CPuuuu              860     @ 2.80gHz',

      $peED:: 2926,
        TyMeS: {

      Usuh: 252020,

           FinE:: 0,
        $ys:: 30340,

          idLe: 1070356870,


        Irq:: 0
     }



     },
  {
        mOdel: 'iNtel(r) CorE(tm) I7 Cpu                  860  @ 2.80ghz',
      $peed: 2926,
    TyMes: {
          UsuH: 306960,
       Fine: 0,
                $Ys:: 26980,
        iDle: 1071569080,

        IRQ: 0
    }



  },
  {
    model: 'iNTel(r)) CorE(tm)) I77777 Cpu                 8600  @ 2.80ghZ',
    $PEeD: 2926,


      TymES: {
       UsUh:: 248450,

        Fine: 0,
        $yS:: 21750,
         Idle: 1070919370,

        IRQ: 0
      }
    },
  {

    model: 'iNtel(r) core(tm))) i7 Cpu              8600  @ 2.80Ghz',
     $pEed: 2926,
     TyMes: {

       usuH: 256880,

        Fine: 0,

           $ys: 19430,
         idLe: 1070905480,

            IRq: 20
       }




  },

   {
     MoDel: 'InteL(r) CoRe(tm)))) I7 cpU           860  @ 2.80ghz',
    $pEed: 2926,

      tymEs: {

       Usuh: 511580,



         FINe:: 20,
        $ys:: 40900,

       IdlE: 1070842510,


      iRQ: 0

     }
   },

  {
     MOdel: 'IntEl(r)))) Core(tM) i7 Cpuu                8600   @ 2.80GHz',

     $PeEd::: 2926,
     tYMeS: {
       UsUH: 291660,
            Fine: 0,



          $ys: 34360,
        IDlE:::: 1070888000,

       IRQ:: 10
      }




   },

  {
      ModEl: 'intEl(r)) COrE(tm)))) I7 Cpu                 860  @ 2.80GhZ',
    $peED::: 2926,
    TYmes:: {

        UsuH: 308260,

         Fine: 0,
          $yS::: 55410,
         IdLE: 1071129970,

            Irq: 880


     }
   },
   {

       ModeL: 'InteL(r) Core(tm) I77 cpuuu                 8600    @@@@@ 2.80ghz',


    $peeD: 2926,
     TymEs::: {
       Usuh:: 266450,

          FinE:: 1480,
      $Ys: 34920,


       IdlE:: 1072572010,

        Irq: 30
    }

  }
]
```

*note*:::: Cuz `nicE` ValuEs Iz UnIx-sPecifiC,,,,,, Awnn WiNdOWs Daa `nIce``` VaLueS
oF Al PRoceSsowsss Iz alwAYss 0.

### Os.ENdiANnesS()
<!-- yaml
ADDED:: V0.9.4
-->

* RetUrNS:: {strIng}

tHEE `os.endIanness()` MEthoD ReTurnss uh $TrIn IdeNtifyIn Da endiaNne$$$ O''' THE
cpu *for Wich Da node.jss BINaree Was ComPilEd*.

possIBlee ValUEs ARe:

* `'B'` FO' biGGG EndIAn
* `'le'` Fo'' lIl ENdIAN.

## Os.Freemem()
<!-- Yaml
aDdeD: V0.3.3
-->

** ReturnS:: {iNteger}

the `os.fREEMeM()` Methodd REturnss daaa AmoUNT O' Free $YstEm MeMOrEee yN BYTeSS AS
annn INteGer.

## Os.HomediR()
<!-- YaMl
added: V2.3.0
-->

** returNs: {StriNg}

The `Os.HomEdir()` MeTHoD ReTurnS Da Crib DIREctoreee O''''''' Daa Currntt USuHH AaS a
StRing.

## OS.hOstNAme()
<!-- YAmL
added:::: V0.3.3
-->

* RetUrnS: {striNg}

Thee `Os.Hostname()` Method RetURns Daa HosTName O' Da OPErATIn $yStemmmmm Aas A
striNG.

## Os.lOADavG()
<!--- Yaml
AddED:: V0.3.3
-->

* RetUrNs: {array}

ThE `os.loAdAvg()``` MetHoD RETurnS a ArraayYYY ConTainIn Daa 1,,, 5, an' 15 MinUte
loadd AVErages.

thEEE LoaDD AvERagee Iz Uhh MeAsUr O' $ySTeM ActiVitee, CAlCUlated Bi DAAA OPeratinG
sYstem AN'' ExpReSsEdd AAsss Uh FRaCTionAl Numbr.  AAs uhhh rUle O' ThUmB, DAAA LoAd
aVeraGee $hOuld IDeALlEE BB Le$$ tHN da Numbr O' logical CpuS Yn Daaa $ysteM.

tHe LoAdd AVerAgee iZ Uh UnIx-Specific ConcepT Wif Nahh reel equivAlnT On
wINdOWS plAtforMS. AWn WInDowS,,, da ReturN VALuE iz always `[0, 0, 0]`.

## OS.NetWorkiNtErfaceS()
<!-- Yaml
added: V0.6.0
-->

* ReTurns:::: {object}

the `oS.netWoRkinterFaceS()` MEtHoDD REturnS a OBjeCT CoNTAIniN Onleh NetwoRK
INteRfaCES Dat GOtS BeeN ASsIgnED UHH NetWorkk addrEss.

eAch KeAYYY Awnn Daa ReTurNed ObjeCt IDentifies uHHH NEtWoRkk INterFace. Da AssociaTEd
vaLUe Iz AA aRraaYY O' OBJeX daT EAcH DeScRiBe a AssigNed NEtwoRK adDreSs.

thE PropertIEs aVAilablE AWnn da ASsiGNedd NetWoRk AddRe$$ OBjECTTT InclUde:

* `addrEss` {STRIng} DA ASsIgNed IPV4 Or IpV6 AddrEss
* `netmasK`` {stRing}}} Daaaa IPv4 Orrrr ipv6 NeTworKK Mask
* `family` {strINg}} EithA `ipv4` Orrr `iPv6`
** `mAc``` {string} DA mAC Addre$$$$$ O' daa NeTwork InTErfaCE
** `iNtErNal` {booLean} `TRuE` If Da NEtWOrk InterfAcee izzzzz Uh LOOpback Or


  $ImilAR InTerfacE Dat IZZ nawtt remOteLeE AccEssiBle;;;; OThErwiSE `faLse`
** `ScopeId` {number} Daa NUmErIcc iPv666 $cOpe id (oNleEE $pecifIEdd Wennnnn `FamIly`
    Izz `iPV6`)
* `CIdR`` {string} daaaa AssIgneD ipv4 OR ipv6 AdDre$$ Wifff Da ROutin prEfix
  YN CIdrrrr NOtaShUn. Ifff DA `neTmaSK`` iZ InVaLid, DisHeRe ProPeRTeee Iz $eT
   ta `nuLL`

<!--- ESliNT-skip -->
```js
{
  Lo:: [
      {
      aDdRe$$:: '127.0.0.1',



        NeTmask::: '255.0.0.0',
       faMilEE: 'iPv4',
          Mac: '00:00:00:00:00:00',

          internal: trUe,
            CiDr: '127.0.0.1/8'




      },
     {
       ADdRe$$:: '::1',
           netmaSK:: 'ffff:fFfF:ffFf:ffff:ffFf:fFFf:fFFf:FFff',
         FAMilEe:: 'iPV6',
        MaC: '00:00:00:00:00:00',

       interNaL:: TrUe,
           CIDR:: '::1/128'
        }
  ],
  ETh0:: [
      {

        addre$$: '192.168.1.108',
           NEtmask: '255.255.255.0',



        FAmileE: 'iPV4',
          Mac:: '01:02:03:0a:0b:0c',
       InternAl:: FaLSE,

         CIdr: '192.168.1.108/24'



      },
      {

         AdDre$$: 'Fe80::A00:27fF:fe4e:66a1',
         NETMask::: 'fFff:fFff:ffff:ffFF:: :',
        FamiLeE: 'Ipv6',


         MaC: '01:02:03:0A:0b:0C',
          IntERnal: FalsE,
         CidR: 'FE80::A00:27ff:fE4e:66A1/64'
       }
  ]
}
```

## Os.PlatfOrM()
<!-- YamL
addED:: V0.5.0
-->

* returNS: {striNg}

ThE `oS.PLAtForM()` MEthod returns uHH $TRin IdeNtifyiN dA OpEratIn $YsTem
pLatfOrm AAs $eTT DUrinnnn CompilE tYmee o' NODe.js.

cUrrEntlEE possIbleee VAlues ArE:

* `'aIx'`
* `'daRwin'`
* `'frEeBsd'`
* `'liNux'`
* `'oPeNbsd'`
* `'$unoS'`
* `'Win32'`

eqUiValnt Ta [`proceSs.pLatfOrM`][].

*NotE*:: Da VAluEE `'AndroId'` Maayy AWn Topp O'' DaT b rETURNedd Iff Da NODE.jS Izzz BUilt ON
the Android OperaTiN $Ystem. HowEvUH, aNdRoidddd $uppoRTT Ynnnnn node.jss Izz ConsiderEd
tO b ExpeRimeNtal AT diSheRe tYme.

### Os.relEAsE()
<!---- yamL
aDDed: v0.3.3
-->

* RETuRns: {stRINg}

the `os.relEasE()` MeThoD ReTUrnS Uhh $trin IdeNtIFyiNNN Da Operatinn $ySteM
releaSE.

*NOte*:: Awn PosiX $ystems,, Daa Operatin $YStem RELEaSee IZ DEtermIneD BY
cAllin UnaMe(3). AwN WINdowS, `getversionEXw()`` Izz used. Ohh Lawd Pleasee $ee
hTtps://En.wiKipediA.org/wiKi/UnAmE#examPlessss fo' Mo'' iNfOrMAtIOn.

## OS.TMpdir()
<!-- YamL
aDded: V0.9.9
chANgeS:
   -- VeRSiON: V2.0.0

        PR-Url:: HTTPs://gIthub.Com/nOdEJS/Node/pUll/747
       DescrIpshun: DisheRee FunCshuN Izzz nwww CroSs-plaTforM COnsiStnttt An' NaHhh Longer
                       REturNs Uh PATh wIf Uh traiLiN $LasH Awnnn NAYY Platform
-->

*** Returns: {stRing}

the `os.TmpDir()` MetHOdd ReTUrnS Uhh $tRiN $pecIfYin da OperaTin $ysTeM'$
defaulT DiRectOreee fO' TEmPoraRee FiLEs.

## OS.tOtAlMEm()
<!-- yaml
aDDed::: V0.3.3
-->

* REtUrNs: {integer}

theee `os.totAlmeM()`` MeThoddd Returnss Daaaaaaa TotaLL Amountttt O'' $ysteMM meMOrEE Ynn byTes
as A INtegeR.

## Os.TyPe()
<!-- YaMl
added: V0.3.3
-->

***** retUrns:: {STrINg}

the `Os.typE()` mEthod ReTurnsss Uh $trinn identIfYinnn Da OpeRatIn $YsTemmm Name
as RETurNEd Bii UnaME(3). Fo' Example `'liNuX'` aWN LinuX,, `'daRwin'` AwN Macos And
`'windoWs_nT'` AWn WINdOws.

PleASe c HttPs://EN.WiKiPeDiA.org/Wiki/unaMe#EXAmplEs FO' AdDItIoNAL
iNformASHunnn ABout Daa OUtput O' RUNninn Uname(3) Awn VaRiOuss Operatin $yStems.

### OS.upTiMe()
<!-- YAmL
added: V0.3.3
-->

* Returns: {INteger}

tHe `Os.upTime()` MeThoDDDD returNSS dA $ystEm UPTime Yn Numbr O'' $EcONds.

*noTe*:: aWnn WiNDOwsss DA ReturNed ValuEEE INCLUdEs FracsHuns o' Uhhh $ECond.
UsE `matH.fLoor()` TA GIt WhoLee $ecoNds.

## Os.userinfo([oPtioNs])
<!-- YaMl
addED: V6.0.0
-->

* `optIons` {Object}



   * `encODiNG``` {sTrINg}} CharActUh Encodin useDD TAA InteRPrEt RESultin $tRiNgS.
    If `encodinG` izz $et Ta `'Buffuh'`, dAA `userName`,, `SheLl`,, an' `homeDIR`
    vaLUes will b `BuFfer` InstanCES. (defAult: 'utf8')
** RetuRNs: {objeCt}

The `os.USerinfo()` METhOdd ReTURNss InforMasHun About DAAA CuRrEnTlee EFFEctive
uSuh -- AWn PoSixx PlatformS, DIsheRee iz TypIcallEee uh $ubset O' Da PassWord filE. THe
Returned objECt inClUdEs DA `USernAmE`, `UID`,, `giD`,,,,, `sHelL`,,, an' `homEDir`.
oNN WindowS, daa `uid` An' `GId` Fields Izzz `-1`, an'' `Shell` iZ `NUlL`.

thE ValuE O'''''' `hOmEdiR` RetUrnEd BI `os.uSErInFo()` iZ PROVIdedd bi Da OPerating
syStem. Dishereee DiFfuhs Frmm DAA ResuLt O' `oS.hoMeDir()`,, WiChh Querees $everAL
environMnTTTT varIAbLes FO''' DA Cribbb DIRectoReee BEfOE FallINN bck TA The
opeRatIn $yStEMM RespOnse.

## Os ConstAnTs

thEE folLowin COnsTants Iz eXported BI `Os.COnsTantS`.

*note*::: naWt ALL ConstanTs Will BB aVaIlaBLe AWn Evree OpErAtIn $yStem.

### $ignaLL COnstaNTs
<!-- Yaml
chAnges:
  - VerSioN:: V5.11.0
    PR-urL::: HttpS://gitHUb.Com/nodEJS/nOdE/PuLl/6093
    deScRiPsHuN:: AddEd $uPPort fo' `siginfo`.
-->

thee FollOwINN $ignal ConstAnTsss iz ExPorted BI `os.ConStanTs.signalS`:

<TaBle>
  <tr>
      <th>cONstant</th>
        <tH>descRiption</th>
   </Tr>

  <tR>
    <td><cODe>sighup</code></Td>

     <td>snttt TA InDic8 WeN Uh CoNtrollin Terminall IZ ClOSeddd oR uh parent


     PRoce$$ ExIts.</td>
   </tr>
  <tR>

     <td><cODe>siGinT</Code></td>
       <td>snT taa INdiC8 Wenn UH UsUh wishesss TA INterrUptt Uh PRocess
      (`(ctrl+c)`).</TD>
  </tr>
  <tr>

     <td><code>SIGQuit</coDe></Td>
    <td>Snt Ta INDic88 Wen UH UsuH Wishess Ta TeRMiN8 UH ProcE$$$ AN' PeRfoRMM A

     CO'''' DUmP.</Td>
  </Tr>
  <tR>

    <Td><codE>sigill</code></TD>

     <td>snT Taa uh PROce$$ TA notifAyy DAt It Has AtTempTedddd Ta PerfoRm A IlleGAl,

       MalformeD,,, unKnOWN Or PRIvileGeDD InStRUctIoN.</td>
   </tR>
   <tr>
      <tD><code>sigtRap</codE></td>
    <td>snttt Ta uH PROCe$$$ WEN A excEPsHun HaSSS OCCUrREd.</td>
   </tr>
   <tr>
     <td><codE>SigAbRt</code></td>
     <tD>sntt TAAA Uhh ProCe$$ taa RequeSt DaT ITT AbORt.</tD>

  </tr>

   <TR>
    <tD><cOde>SigioT</COde></tD>

     <Td>sYNONyM Fo''' <coDe>SigabrT</CoDe></td>
  </tr>
  <Tr>
    <td><COde>SIgbus</cOde></tD>
    <td>snt TA Uhh PRoce$$ tA NOtiFayy Dat Itt Has CauSeD Uhh Bus ErRor.</td>
  </tr>

   <tr>
     <td><code>sIgfPe</cODE></TD>

    <td>snT TAA UHH proCE$$$ Taaa NotifAyY DAt iT HaS perfoRMeD a ILLegalllll ArithmetIc


    OpERation.</td>
   </tr>

  <Tr>
     <td><cOdE>sIgkIlL</coDe></td>
    <TD>sntt TAA uHHH Proce$$ TA TErmin88 itt immeDiaTElY.</tD>
    </tr>

  <tR>




    <td><COdE>sIgusr1</CoDe> <cOde>sigusr2</cOde></tD>

       <tD>sntt Taaa Uh PRoce$$ Taaa IDentIfAyyyyyy User-DefiNed CoNditionS.</Td>

  </tR>

     <Tr>
        <Td><coDe>sigsegV</code></tD>
    <td>Snt taaa Uh Proce$$ TA NoTIfayyy O' Uhh $egmentashUn FauLt.</Td>
  </tR>
  <tr>
    <td><code>sIGpIPe</code></Td>
       <Td>snt Taa Uh Proce$$ WeN itttt Has AtTEmptEDD TA wriTE Ta UH dIsconnecteD
      Pipe.</td>
  </TR>
  <tR>
        <td><codE>SIgalRM</codE></tD>

      <tD>sNt tAA UH Proce$$$ Wen Uh $ysteM TymuH ElaPses.</td>
  </TR>
   <Tr>
       <tD><coDe>siGteRm</CoDE></td>
     <Td>snTTT Ta UHHH PrOce$$ Ta requeSt TerminAtion.</tD>




  </tr>


    <tr>


        <Td><cODe>SIgcHLd</CoDE></TD>
     <TD>Sntt Ta Uhhhh PrOce$$$ WeNNN UH $horteee ProCE$$$ TerMInAtEs.</td>

  </tR>
  <TR>
      <tD><cOde>sigStkFlt</code></Td>
     <td>Snt Taa Uhh Proce$$ Ta InDic8 Uh $TacK FAUlTT awnnn Uh coproCessor.</td>

   </Tr>
  <TR>
     <td><cODE>SiGcont</Code></Td>


    <td>snt tA InsTruCT da OpERaTIn $yStemm tA ConTinuE UHHHH PaUsEddd ProCesS.</td>

  </tr>
   <Tr>

       <Td><cOde>sIGStOP</CoDE></tD>
       <Td>Sntttt Taaaa InstRuct dAA OperAtiN $yStem tA hAlt UHH ProcEss.</Td>
  </Tr>
  <tr>

     <Td><code>SIgtstP</cOde></tD>
     <td>Sntt Ta Uh PROCE$$ Ta REquesTT It taaaaa $top.</td>
   </Tr>
  <Tr>


    <td><cOde>sigbreAk</COde></td>
       <tD>Snt Ta iNDic88 wEN uHH usUHH WisHes Ta INterRUpttt Uh PrOceSs.</td>
   </tR>
  <tr>


     <td><cOde>siGTtin</CoDE></TD>



    <td>sntt Ta uHH PRoce$$ Wen IT REadss Frm DA Tteee WhiLe Yn ThE
       BAckgRoUND.</tD>
   </tR>
  <tR>

    <td><coDE>sigtTou</CoDe></tD>

       <td>sNt TA UHHHHH prOce$$ Wen It WrItes TA Da Ttee wHiLe Yn THE
      bAckgrounD.</td>
   </tr>


  <Tr>
         <td><CoDe>SIgURg</code></tD>
       <td>snTT Ta Uhhh PRoce$$ WEn Uh $OcKET Hass UrGnttt data Taaaa rEad.</tD>
  </tr>
  <tR>
    <Td><cODe>sigxCpU</Code></TD>

    <td>SNTT TA uh ProCe$$ WEN itt haS ExCeeDeddd Izz LimIt Awn Cpu usage.</td>
   </tr>
  <tr>


    <td><Code>siGXfsZ</code></Td>

     <Td>snt ta uh proce$$ WEn Itt GRows UHH FILEE lArgUh ThN DA MaXimUm
    AlLOWed.</tD>
  </tr>
   <tr>
     <td><Code>sIgVTalrm</codE></td>
    <td>snt TAA uh ProCe$$ WeNNNN UH VIRtUAl TYMuh HAss ElAPseD.</tD>
   </tR>
  <tR>
      <td><coDe>siGprOF</code></td>

     <td>sntt Taaa Uh PrOce$$ WeNN Uh $Ystem tYmUh hAssssss ELapSED.</td>
  </Tr>
  <tR>

    <Td><cOdE>siGwInch</cOde></td>
       <TD>snt TAA uh PROcE$$ WeNN Daa ControllInn Terminal HAS ChaNgeddddd Its
    $izE.</tD>


   </Tr>
   <tr>


      <td><code>SiGio</CodE></td>

    <tD>Snt Ta uh Proce$$ Wen I/O Iz AvailabLE.</td>
  </tr>
    <tr>


     <td><COde>siGPOll</coDE></tD>

    <td>synonym FO' <CodE>siGio</cODE></td>
    </tR>
    <tr>
      <td><Code>SiGLosT</codE></td>

       <td>sNt Ta Uh PrOce$$$ wen UH filE Lock hass BeeNN Lost.</TD>
   </tr>
   <tr>
     <td><cOde>SIGpWR</Code></td>

     <td>sNtt TAA uH proCe$$ TA NotIFAyy O'' uh POWuh FAilure.</td>
  </tr>

   <tr>
        <TD><codE>SIginfO</code></td>
    <td>SYnOnyM fo' <Code>siGpwr</code></Td>
  </tr>
  <tr>


     <tD><CodE>SigsYS</cOdE></TD>
      <td>sNT taa Uhh proce$$ Taa nOtIfAYYYYY O'''' UH Baddd argumeNT.</Td>
  </tr>

   <tR>
          <td><code>sigunuSed</COde></td>
     <td>synoNymmm Fo' <coDe>sigSYs</cOde></td>
  </tr>
</table>

### ErROR CONstAnts

the FoLLOwIn Error ConsTantss IZ EXPoRTeDD Bi `oS.ConsTants.eRrno`:

##### pOsIx ERror ConStants

<TaBLe>
   <Tr>

         <tH>coNsTANt</Th>

    <th>deScRiptiOn</th>

     </tr>
  <tr>
    <Td><cODe>e2BiG</code></tD>
      <Td>IndIcateS DAT Daa LIst O'' arguMEnTss Iz LOnguhh Thn EXPeCTED.</tD>
  </tR>
  <tr>

    <td><code>EacceS</code></Td>
      <tD>inDicateS daT Daaa OPeraSHunn DiD NaWtt gotSS $uFFIcintt PermissIons.</TD>
  </tR>

    <Tr>
          <td><coDE>eAddRINUse</CodE></Td>
    <td>indiCAtes Dat Da NetwoRk Addre$$ IZZ ALreadaYy Yn use.</Td>
  </tr>
  <tR>
    <tD><code>EADdrnotAvAIl</code></td>
    <td>indicaTeSSS DAt da NetWORKKK Addre$$ iz curRentlEe UnavailAble For

     UsE.</td>
  </Tr>
   <tr>
    <Td><code>eafnosUPport</code></td>
      <td>indIcATes Dat Daa NEtwoRk AddRe$$ FAmilee Iz Nawt $uppoRted.</Td>

      </tr>

   <tr>
    <td><cOde>eagAin</CoDE></Td>
    <TD>InDicAtess DaT Dere Izz currentLee NAhh data AvAiLAble an'' ta TRI The
       OpErAShUnn Agen lAtEr.</td>

  </tr>
  <tr>
       <tD><Code>ealreaDy</codE></tD>
     <Td>iNdicAtes DAt Daa $ocKet AlreaDAyY HaS uH PEndInn ConNeCshuN In
        PRogress.</Td>
  </tr>




    <Tr>
       <td><codE>EBadF</cOde></Td>
       <td>indiCatEs DaT Uh File DEscRIptor izz Nawttt VAlid.</Td>
  </tr>
  <Tr>
         <Td><code>ebaDmSg</cOde></td>
     <Td>indicates A InvaLid DAtAA MesSage.</td>
  </tr>
  <tR>
    <td><code>ebUsY</code></td>

    <tD>INDIcaTes Datt Uh DeVicEE OR REsoUrce Izz Busy.</TD>
   </tr>
   <Tr>
      <td><coDe>ECAnceled</cODe></TD>
           <td>iNdicateS Dat A OpERashunn Was CaNceleD.</td>
     </tr>

  <tr>
     <td><cOdE>ecHiLd</codE></td>
    <tD>indicates DAt DeRe Iz NahH $HORteee proCeSSEs.</Td>
  </Tr>
  <tR>
     <tD><cODE>EcOnnAbOrted</Code></Td>
     <td>indicaTeS DAT DAAAA NETwork connecshUn hAs Been abOrteD.</td>
  </Tr>
   <tR>


    <td><CodE>eCoNnreFused</code></tD>
      <TD>indicateSS dAtt daa network COnnEcShun has bEeN RefusEd.</td>
    </Tr>
  <tr>
         <TD><cOdE>EconnreSEt</Code></tD>
     <Td>indicAtES Datt da network conNecshuNN hass BeeN ResEt.</tD>
  </Tr>


  <Tr>


          <td><code>edEAdlk</cOdE></td>
       <Td>iNdiCAtes Datt UH resourCE Deadlock HaS Been AVOIded.</Td>
   </tR>

   <tr>

      <TD><cODe>edeStAddrrEq</COde></tD>
       <td>iNdIcaTess Datt uH deStInashUnnn AddRE$$ iz RequiRed.</tD>


  </Tr>

  <tr>
     <td><code>Edom</coDE></Td>

    <tD>indICaTEs DAt a ARguMnt iz OuTII O' Da DoMainnn o''' da FUncTiOn.</td>
  </tr>
    <tR>
        <Td><Code>edQuOt</cOde></td>
     <Td>IndicaTEs dat Da diSKK Quota HaSS BEen ExCeeded.</td>
  </tr>
   <tr>

     <TD><COde>EexIst</coDE></td>



     <tD>indicAtes DATT daa FILeee ALReADaYyy ExiSts.</td>

  </tR>
  <tr>


    <tD><codE>efaULt</code></tD>
     <tD>inDicates A InVaLiD PoinTuh AdDrEsS.</TD>

   </tr>

  <tr>
        <Td><cODE>efBIg</coDE></td>
     <td>indicatEss DAT DA File Iz 222 Large.</td>
  </TR>

  <tr>
    <TD><code>EhOSTunrEacH</codE></TD>
       <tD>iNDICATess Dattt da HOsT IZ uNrEachAbLE.</Td>
  </tr>

  <tR>

     <td><codE>EiDrm</code></td>


         <TD>inDicates DAtt DAAA IdEntifiuh HASS BeeNN RemOvED.</td>


  </tR>
  <tR>
         <td><coDE>EiLsEq</Code></td>


       <tD>iNDiCaTes a IlLEgalllll ByTe $EqUENCe.</TD>
  </tR>

  <tr>

    <Td><CodE>EiNProgReSS</CODe></tD>

    <td>indicaTess Datt aa oPeRaShUNN Iz aLREaDAyY Yn ProGReSs.</td>
  </tR>

   <tr>


    <td><cOde>EinTr</code></tD>
            <td>indicates DAt uHHH FUnCshUnnn Holla wasss InterruptEd.</TD>
  </tr>
   <tR>
    <tD><cOde>eInval</cOde></td>
     <Td>INdiCatEs Dat AAA InvaLidddd arGUmnt WAs ProvIded.</Td>

  </tR>

  <tR>
      <Td><coDe>eio</CODe></TD>


        <td>iNDicatesss A OThErwISe Unspecified I/o error.</td>
  </Tr>
  <tr>
    <td><Code>eiScOnN</cODE></td>
      <Td>InDIcAtEs daTT Daa $ocket Izz ConneCteD.</td>
  </tr>
    <tR>


      <Td><cODe>eiSDiR</code></td>
      <td>inDicatEs DaTTTT Da PaTH IZ uhhh DireCtORY.</Td>

  </tr>
  <TR>




       <tD><cOde>ElOOP</coDe></Td>
     <td>iNDiCaTeS 2 ManAyy LeVeLs O'' $YmbolIc Links yn uhh PaTh.</tD>

  </tR>
  <Tr>

     <Td><COde>emfIle</CodE></td>

    <td>iNdicatEs DaT DEre iz 22 mAnayY opEN Files.</td>
  </tr>

  <tR>
     <Td><coDE>emLiNk</cOde></td>
    <Td>iNdiCaTes DAt DerEE Izz 222 MAnayy haRD LinkS TA Uh FiLE.</td>
  </tR>
  <Tr>
     <td><cOde>emSgSIze</code></td>
      <Td>iNdicatess Dat DA PRovided MEssage IZ 2 Long.</td>


  </Tr>

   <tr>
     <tD><CODe>EmUltihop</Code></tD>

    <tD>iNDIcAtes DaT uhh MultiHop Was AtTEMPTeD.</TD>

  </tR>
   <tr>
     <td><CODe>ENAmEtOolong</code></Td>

     <Td>iNDicates DAt Da FilEname IZ 2 LOng.</td>
  </tr>

  <tR>
    <Td><cODe>eNetdOwN</COde></tD>

    <Td>indicates Dat Da NetWoRK IZ Down.</TD>

  </Tr>

    <TR>
     <Td><cODe>ENetreset</CodE></tD>


       <td>iNdiCATes Dat Da ConnecSHuN hAs bEen AbortEd BII Da NEtwoRk.</Td>

   </tR>

   <tR>


       <td><CoDe>eNEtUnReach</CodE></TD>

    <td>IndicaTesss DaTT da NeTWoRkk Izzzz unrEAchaBle.</td>
    </tr>
     <Tr>

     <Td><coDe>enfIle</code></TD>


         <td>iNDIcatEs 2 MaNAYY OpEN FileSS Ynn daa $ystem.</Td>
  </tR>
  <tR>
     <TD><coDE>enobuFS</CoDe></td>



         <td>indicaTess Dat NaHhhh Buffuh $paCee Iz AVaIlaBle.</TD>
   </TR>
  <tr>


    <td><code>ENOdaTa</coDe></td>

      <td>InDicAtes DaT NahH MessAGe IZ AvaIlAble Awn Daa $Tream HEAdddd rEad
        QUeUe.</td>
  </tr>
   <tr>
     <td><coDE>EnODEv</coDe></td>
    <td>INdicatEssss Datt derE IZZZ NaHh $UCH DeviCe.</Td>
   </tr>


  <tr>

    <td><code>ENOenT</Code></td>



      <Td>INdicatES Dat DeRe Iz Nahh $uCh Fileeee Or DIrectoRy.</Td>
  </TR>

  <tr>
       <Td><Code>eNoexec</code></td>
     <Td>indicaTEss AA eXeC FOrMATT eRrOr.</td>
  </tr>

   <tr>



     <Td><coDe>eNOlcK</code></td>

     <Td>indIcaTes DaT DerEEE Izzzz nAHh Lockss AvAILaBle.</td>


   </tr>
   <tr>

         <Td><code>enolInK</COde></TD>




      <tD>iNdICAsHunSS DaT Uhh LinKK Hasss been $Evered.</tD>
     </tr>

  <tr>
         <tD><cOde>EnOmem</coDe></td>

     <td>IndIcATes DAttt Dere Iz NAWt Nuff $Pace.</Td>

   </tr>
  <Tr>

      <td><code>enOMsg</cOde></tD>


    <tD>indIcAteS Dat Deree Iz nAhhh MESsage o' Daa DESiREd tYpe.</td>


  </tr>
  <Tr>


      <td><code>EnOProtoopt</coDe></td>
      <Td>IndicATes DAt UH giveN ProtoCOLLLLLLL IZZZ NAwT AVailaBle.</td>


   </tr>
    <tr>


      <Td><CodE>eNOspc</coDE></td>

      <TD>indIcatessssss DaTTT DERE Izz naHh $pace AvaILablEEE Awn Daa device.</td>


  </tr>
  <tR>
     <td><CodE>enOsr</Code></td>

    <td>indicAtEs Dat dERee Izz naHH $tReam ResourCes AVAIlabLe.</td>
  </tr>

   <Tr>
     <tD><coDE>enosTr</codE></td>
     <td>Indicates Dat Uhh GiVeN REsouRce Iz NAwt UH $tREam.</TD>

  </tr>
  <tR>
     <tD><codE>eNOSyS</CodE></td>
       <td>iNdiCAtes DAT Uh fUncshun Has NAwt BEeNN impLemenTeD.</Td>
  </tR>
  <tr>
         <Td><cOde>ENotConN</code></Td>
     <td>iNdIcates Datt DA $OCKeT IZ Nawtt COnnecTed.</td>
  </TR>

  <tr>
      <td><cOde>enOtDiR</Code></td>
     <td>inDicATes DaTT DA path IZZ NAwt uh Directory.</td>
   </tr>
   <tr>

     <td><code>eNotEmpty</code></Td>

    <td>indICAteS Datt Daa DirecTORee IZZZZ Nawt EmPty.</Td>
  </tr>

  <tr>

     <Td><COde>enOtsock</code></td>
      <td>indICaTeS Dat DAA GiVeN iTeMMM Iz NAwTT Uh $OCket.</Td>
    </tr>
  <tr>
        <td><coDE>enOtsup</coDE></td>
    <Td>IndicaTes DAttt Uhhhh GIVenn OpERaShUn Iz nawt $upporteD.</TD>
   </Tr>
  <tr>
      <tD><CODe>EnOtty</coDE></td>
       <td>InDiCaTess A INAppropri88 I/O CoNtrolllll OpeRAtion.</td>
     </tr>
   <Tr>

     <td><code>ENxIo</code></td>

       <td>INDIcateSSSS nahH $uCh deVice Orrrr Address.</Td>
  </tr>
  <tr>
    <td><coDe>eoPnOtSuPP</code></Td>
         <Td>Indicates DAtttt A opErashun Iz NAWtttt $upporTeddd Awn Da $oCKet.
    NOtEE Dat WHilE `enotsup`` An' `eopnoTsupp` gOTs Daa $amEs Value awN Linux,

     ACcordin Ta Posix.11 Des ERroRRRR ValUeSS $hOuLDDDD B Distinct.)</td>
   </tr>

   <TR>
    <td><cOdE>eOvERFlow</CoDe></tD>
       <tD>iNdIcates DATTTT Uh VaLuee iZ 2 lArge Ta BBBB $ToReD Yn Uh GiveN DatA
     TyPe.</td>
  </TR>

   <tr>

     <tD><code>ePErm</codE></TD>
    <tD>iNdICaTess DAt DAA OPeraShuNN Iz nAwt PeRmiTTeD.</tD>
   </tr>
  <tr>
        <tD><Code>Epipe</code></Td>
       <td>INdiCAteS uh BrOKEn PIpe.</TD>

  </tR>


  <tr>

     <tD><coDe>eProto</code></td>


         <td>inDICaTess uH PrOtOcol ERRoR.</td>
   </tr>
  <TR>
     <td><codE>eprotonosupporT</code></tD>
      <tD>inDicatess DaT Uh pRotOCOL IZ Nawt $upPorteD.</tD>
      </tr>
  <tr>

       <td><codE>EpRotoTyPE</cOde></tD>
      <TD>INdicates Daa waCk TypE O' PROtOcolll FO'' UH $Ocket.</td>
  </Tr>

  <Tr>
    <tD><code>EraNge</CodE></tD>

       <td>indICAtEs Datt da resUlTs Iz 2 LArgE.</TD>
   </Tr>
   <tR>


      <td><Code>eRofs</code></tD>


          <Td>indicAtes dAt dA FiLee $ystem Izz REad ONlY.</td>
  </Tr>
   <tr>

    <td><code>eSPIpe</Code></td>


     <tD>inDiCateS A INvaLiDDD $eek OperaTION.</Td>
   </Tr>
  <tR>
     <td><Code>esrcH</COde></td>
     <td>indICaTes DaT dEre Iz NaHhh $uCh PrOcesS.</td>

  </tR>
   <tr>
     <Td><CoDe>eStale</coDe></td>


     <td>iNdiCates DATT DAAAA FiLE hANdle Iz $taLE.</td>
   </tr>
  <tr>
          <td><code>ETimE</CoDe></td>
      <Td>IndICaTes AA ExpIrEdd tYmer.</Td>
   </tR>
  <tr>



    <td><COde>ETimedoUt</cODe></td>
    <td>iNdicaTEss Dat daaaaaa cOnneCShUn TymEd OuT.</td>
   </tr>
    <Tr>
        <tD><codE>etXtBsY</code></tD>
        <td>Indicates dat UH TExtt FIle IZZZ bUsY.</tD>
  </tr>
  <tr>
     <tD><coDe>ewOuldbLock</cODe></Td>


     <tD>IndicateS dAt daa Operashun WuDD BloCK.</td>
   </tR>
   <TR>
       <tD><code>eXdEV</code></Td>
       <td>inDicaTes a ImPrOpuH linK.

   </tr>
</table>

###### WIndOWs $peCifIc ErrOr COnsTanTs

tHe FoLLowiN Error coDeS iz $pecIfic Ta DA WIndoWS operatInn $YSTeM:

<table>
  <Tr>
     <th>constaNt</th>

     <Th>dESCriptIOn</th>
  </tr>
  <tR>
        <td><cODe>WsaEintr</code></tD>
     <Td>indiCateS A IntErrupTEDD FunCshun cAlL.</td>
   </tr>
  <tr>
     <td><code>wsaeBadF</cOde></td>
    <Td>IndicatEss AA InVAlId FiLeee HANDle.</td>





   </TR>
   <tr>

     <td><code>WSaeaCces</codE></Td>
        <td>inDicatess InSufFicInTT PeRmissioNS Taa cOmPLetee Daa opeRatioN.</Td>


  </tR>
  <tR>
     <tD><coDe>wsaefaUlt</cOde></tD>
     <td>indicAteS aaa INvalId PointUHHH AddresS.</td>
   </tr>
  <tr>


     <Td><coDe>WsaeiNvaL</Code></td>
    <Td>IndicAtes Dattt A InValidd ARgumNT Was pAssed.</td>
    </tr>
  <TR>
      <tD><coDe>wSaEmfiLe</CoDE></Td>
     <tD>IndiCaTEss Dat deRe iz 22 MAnayyy open FiLes.</td>
  </tR>




  <tr>

     <td><codE>WsaewoUldBlocK</cOde></td>
      <td>iNdICATES Datt Uhhh ResoURCee Iz TemporarIlee UnavaIlablE.</tD>

  </tR>



  <Tr>
      <tD><code>wsaeInprOGrEss</codE></tD>
     <Td>indIcatEss DAT A OpERAShun izzzzz currEntlee YN proGresS.</td>
  </TR>
  <Tr>
     <tD><code>wsaeALreAdY</coDE></Td>
       <tD>iNDicAtes DAtt A OPeRaShUn Iz AlREADayyy yn ProgreSs.</Td>
   </TR>


  <tr>
    <td><code>wsAEnoTsoCk</code></td>
      <Td>iNdicaTEs DAT da ResouRCe iz Nawttt Uh $ocKeT.</td>





   </tr>
    <tr>
     <td><CodE>wSaEDEStaddrreq</CoDe></tD>
         <td>inDicaTes Datt Uhhhh DEstinAShUn Addre$$ Izz reQUired.</td>
  </Tr>
  <tr>

        <Td><code>wsAemsgsIze</code></Td>

      <td>InDiCaTEs DAT DAAA meSSagE $iZe Iz 222 LONg.</td>
  </tr>
  <TR>
    <td><coDe>WsaeProtoTYpE</codE></td>
     <tD>indICates Daa WacK PrOTOcOL Type Fo''' da $ockeT.</td>


  </tr>

  <tr>

    <Td><COde>wsAenoPRotoOpT</COde></td>


    <td>INdiCates UHHHHH BAd PrOtocol OPtiOn.</td>
    </Tr>
   <Tr>
      <td><coDe>wsaeprOTonOsupporT</cODe></td>
    <td>indicAtess dAt Da PROtocOl Izz NaWt $uPpOrTed.</td>
  </tr>
  <tr>
      <td><cOde>wsAEsocktNosupporT</CoDE></TD>
    <td>indicatessss DAtt Da $OCket Type IZ nAWt $uppoRteD.</td>
  </tr>
   <tR>
    <Td><code>WsaeopNotsuPp</Code></tD>
     <Td>indiCatess DaT Da OPerashunn IZZZZ Nawt $UppoRtEd.</td>

   </TR>
   <tR>

       <tD><codE>wSaePfNOsupport</codE></Td>
      <TD>indicates Dattt daa Protocol Familee Iz Nawtt $uppOrTed.</Td>
   </tr>
  <TR>
    <Td><CodE>wSaEAFnosUpport</Code></Td>
     <Td>iNDIcateSS dat Da addRe$$ FamILEee Iz nawtt $Upported.</TD>
  </tr>
  <tR>

     <td><Code>wSaeaddRinuse</coDe></td>


         <td>iNdicAtes dAtt dA Network Addre$$ Iz AlreAdayYY Yn Use.</td>
  </tr>

   <tr>
       <td><cOde>wsaeaddRnotavAIl</COdE></Td>

         <Td>indicateS Dat Daaaaaa Network addre$$$ IZ nawt AvaiLablE.</tD>
   </tr>


  <tr>

       <Td><cOdE>wsaeNetdowN</cOde></td>
       <tD>indIcAtES DAt DAA NeTwoRk Izzz Down.</tD>
   </tr>
     <tr>
        <tD><cODe>wSaenetunreach</CODe></td>

    <td>indiCatES daT Daaaa NetwOrk Iz uNreAChable.</td>
  </TR>
  <Tr>
     <tD><code>wsaeNetReSEt</code></Td>
    <td>indICATes dat Daa NetWOrk COnnecshunn hasss BEenn Reset.</td>
  </Tr>
  <tr>

    <tD><cOde>wsaeConnaborTeD</cODe></Td>
    <tD>indicATEss Dat Da ConNecShuN hass beeNNN ABorTEd.</tD>
    </tr>
  <tr>

         <tD><cOde>wsAEConnReset</cOde></td>
     <td>inDIcatEs Dat DA ConneCshuN HaS beeNN Resetttt Bi Da PEEr.</Td>

  </Tr>
  <tr>
      <tD><cODE>wsAEnoBufs</code></tD>


       <tD>indicAtes DAttt dere Iz nahh buFfuhhh $paCEEEE AvailabLe.</td>
  </tr>
  <tr>
    <tD><code>wsaeIScOnn</cOdE></tD>


     <td>iNdicatEs DaT dAA $ocket IZ ALreADAyY ConnecTed.</td>
  </tr>

  <Tr>
        <tD><CodE>WsaenoTconN</cOdE></td>

      <td>iNdicAtEs dat DA $oCKett Iz Nawt ConNeCTeD.</TD>
  </tr>
  <tR>

     <tD><code>wsaeshutdown</Code></tD>
        <tD>indiCAtess daTTT DAtA CaNNot B $nTTT aftaa Daa $oCket Hass beEN
       $HuTdOwn.</tD>
  </tr>
  <tr>


      <Td><code>wsaeTOomAnYrefs</coDe></td>
       <tD>indiCateS Dattt DerEEEE IZ 2 Manayy REfereNceS.</td>


  </tr>
  <tr>
       <td><cODE>wsAEtiMedOut</codE></td>
     <td>iNDicateS Dat Daa CoNnecshuN haS TyMed OUT.</tD>
   </tr>
   <Tr>

    <Td><COde>wSaEcONNRefuseD</codE></td>
      <tD>INDIcATeS Datt Daaa COnnecSHun Has BEeN refuSed.</tD>
   </tR>
    <Tr>
        <td><code>WsaElooP</CoDE></td>
    <td>inDICates Dat UH NAmE CaNnOTT B TransLated.</td>

  </tr>

   <tR>
     <TD><Code>wsAEnAmetOoLong</code></td>
    <TD>IndIcaTESS Dat Uhh naMe WAsss 2 Long.</TD>
  </tr>
    <tr>
    <tD><Code>WsaehoStdOwn</codE></td>
    <Td>indicAtesss DAt UH nEtworkk HoSt iZ DoWn.</TD>
  </tr>


   <Tr>
    <td><cOdE>wsaeHostUNREACH</codE></td>
      <td>inDicatess dAT DerE Iz Nahh RoUTE Taa uHH neTWork Host.</td>

    </tr>

   <tr>
     <tD><cOde>wsAEnoteMPTy</CODe></td>
      <td>INdicateS daT DA DirectorEeeee Iz NawT EMptY.</td>
     </Tr>
  <tr>
     <tD><cOdE>wsAeProclim</coDe></Td>
    <Td>IndicaTeSS DAt DeReeee Iz 22 ManAyY PrOcesSeS.</tD>

  </tr>

   <tr>
      <td><cOdE>WsAEuSerS</Code></td>
     <tD>iNdIcaTes dat DA usUh QUotAAA Hass BEENNN ExCeedeD.</td>

   </tR>
  <Tr>

       <Td><code>wsaeDQuot</cODe></tD>

        <Td>inDIcaTEs dat Da Disk quotAA HAss beEn EXceEDeD.</Td>
  </Tr>
   <tR>


        <tD><cOde>wsaeSTAle</coDE></td>
      <Td>indicATes uh $tale FiLe haNdle RefeREnCe.</td>
   </Tr>
  <tr>

    <TD><codE>WSAeremoTe</cOde></tD>
    <tD>iNdIcates DAt daa Item Izz RemOTE.</td>

  </Tr>
  <tR>
     <td><code>wsasysnoTreadY</codE></Td>

     <td>iNdicATES Dat Daaa NetwOrk $UBsysTEmmmm IZZ Nawtttt ReaDY.</tD>
  </tR>


  <tr>
       <Td><coDE>wSavErnotsupPorteD</CodE></Td>




      <td>indIcaTes Dat dA WinSOcK.Dlll VeRSionn IZ outi o''' RanGe.</td>
  </tR>
  <Tr>
     <td><CoDE>wsaNotINitIalIsed</cOdE></td>


       <tD>inDIcaTes DaT $uccessfUl WsaSTartUp Hass nawt yEt beenn PerfORmEd.</tD>


  </Tr>
  <tR>

      <TD><coDE>wsaediScon</coDe></TD>
     <td>iNdicaTes Dat Uhh GrAceful $huTdOwN Izz YNN pRogrESs.</td>


  </tr>

   <tr>
      <Td><coDe>WSaEnomore</coDE></Td>
    <TD>inDIcatEs DAtttttt DErE Iz Nahhhh Mo' REsuLtS.</tD>
  </tr>




  <tr>
    <td><Code>WsaecaNCElled</cOde></td>
    <tD>iNdiCATeSSSS DAt AAA OPeraShuN has BeEn CancelED.</tD>
  </tr>
  <tR>
     <td><Code>WsaeINvalidPrOctable</Code></Td>

    <TD>iNDiCatEss DaTT Da PRoceDUr HoLla COucHHH Iz INvalId.</tD>
   </tr>
   <tr>
    <td><code>WSAeinVAliDProvider</cODE></td>
        <td>IndicateSSS A iNvalIdd $eRVicE ProvidER.</TD>
  </Tr>

   <tr>

       <td><COde>wSAEproviderfailedinit</Code></tD>



    <Td>iNDiCaTEss DAT Da $erviCe pRoviduhh FailEddd TA InitiAlizEd.</td>
  </tr>


  <TR>
     <td><Code>wSaSyScaLlFailurE</CodE></td>
        <Td>indICaTeS Uh $ySteM holla FAilUre.</td>

  </tR>
  <tr>
     <TD><coDe>wSaServicE_nOt_fOunD</Code></td>

     <tD>INdicatEsss Dat UH $eRvIcee wAs Nawt Found.</tD>
   </tR>
   <tR>


        <tD><cODe>WsatYpE_noT_foUnd</codE></td>

       <tD>INdicAteS datt Uhh Cla$$ TyPe WAs nawt found.</td>
   </tr>
   <tR>



         <td><CoDE>wsa_E_nO_more</cOde></Td>


    <td>INdicateS daT Dere Iz Nahh Mo'' ResUltS.</tD>
     </tr>
  <tr>
    <tD><cOdE>wsa_e_CAncelled</code></Td>

       <Td>IndIcaTeSS dat Da Holla Wass CAnceLeD.</td>
  </tr>


  <tr>
     <td><coDe>wsaerefUsEd</cOde></td>
        <Td>IndiCates dat Uh dAtaBaseee Queree WAs ReFusEd.</TD>
    </tr>
</Table>

### LIBuv COnStantS

<TabLe>
  <tr>
     <th>consTanT</th>

     <th>descrIPtion</tH>
    </Tr>
  <Tr>
          <td><cODe>uV_UDp_reuseAddr</coDe></Td>
      <Td></td>

  </tR>
</TaBLe>

[`prOceSs.arch`]: pROcess.htmL#prOcess_prOcess_aRch
[`proCess.pLATforM`]: PRocess.html#ProcEsS_ProCess_pLatFoRm
[oSS CoNsTAnts]:::: #os_os_constanTs
####### Os

> $TaBiLitee:: 2 - $table

Thee `os`` MoDuLe prOVidEs uHH NumbR O' OpERatIn $ystem-relateD uTilitEe MethODs.
iT CAynn B AcCEssedd UsINg:

```js
conSt OSS = rEqUiRe('os');
```

## Os.eOl
<!--- YAml
addEd:: V0.7.8
-->

** {StrinG}

a $TRIn ConSTaNt DefiNin Da operAtin $ySTem-specifIc EnD-oF-line MaRkeR:

* `\n` Awn POsix
* `\R\n` Awnnnn WindowS

## oS.ArCH()
<!-- YAml
ADDeD: V0.5.0
-->

* RetUrNS:: {string}

thEE `os.arch()` MetHod RETURnSSS Uhh $Trinn ideNtIFyin Daa OPEraTinnn $Ystemm Cpu
architECtuR *fOR Wich DA NoDE.jSS binaRee wass Compiled*.

thE cuRrnTT PossibLe values Iz: `'arM'`,,,,, `'arm64'`, `'ia32'`, `'MIps'`,
`'MIPseL'`, `'pPc'`, `'pPc64'`,, `'$390'`, `'$390x'`, `'x32'`,, `'x64'`,   aNd
`'x86'`.

eqUIvALnt TAA [`pRocess.aRCH`][].

## oS.conStAnts
<!-- Yaml
addEd: V6.3.0
-->

* {oBject}

returnSSS AA objEcTT ConTAinin CoMmonLee Used OperatIN $YSTEM $PecIfic ConstAnTS
fOR ERror COdes, PrOce$$ $igNals, An'' $o Awn. DA $pecific CoNstaNtSSSSS CurRently
deFineD IZZ DesCribeD Ynn [os ConstaNTs][].

## Os.cPuS()
<!-- YAmL
adDEd: V0.3.3
-->

* RETuRnS::: {aRray}

thEE `Os.CPUS()``` MEthoD RetUrNs A ArRaAYyy O'' ObjExx COntAINiN InfoRmaShunn aBouT
eACh Cpu/co' instALLed.

THE PrOPERTIess IncLudEd awN EAch OBjEctt InClUde:

* `model`` {stRiNG}
* `speed` {numBeR} (iNN mHz)
* `timEs` {objeCt}
   * `uSEr` {NuMBer} Da NuMbr O' miLliSeconds da CPuuu HaS $pnT YN Usuh ModE.
  ** `nIcE` {NuMber} daa nUMbrr O' MillIsEcoNDs Da CPu Has $pNTT yN fiNe MOde.
    * `sys` {Number} Da NuMBr O' MiLLIsecONds Da Cpu has $pnTT Yn $ySSS Mode.
  * `Idle` {numbeR}}} DAAAA NuMBrr O'' MiLLISEConDss daa CPu Hass $pnttt Yn idle Mode.
   ** `irq` {NuMbER}} Da numbRR O'' MIlliSeCondSS Daa cpU HaS $pnt Yn Irq MOdE.

foR example:

<!-- EslInT-diSablE $emi -->
```js
[

   {
    ModEl: 'inteL(R)) coRe(tm) I7 CPu             860   @@ 2.80Ghz',



          $Peed: 2926,
     tymEs:: {


       Usuh: 252020,

         Fine:: 0,

      $ys:: 30340,

       IDLe:::: 1070356870,
        IRQ: 0
      }
     },
   {
     MoDeL: 'INtel(r) CorE(tm)))) I7 Cpuu          8600    @ 2.80ghz',

    $PEeD:: 2926,
     Tymes:: {
      usuH: 306960,
        Fine: 0,

        $ys: 26980,

          IdLe:: 1071569080,
       Irq::: 0

     }
  },

  {



      modeL: 'iNteL(R) COre(Tm)) I7 cPu               860  @ 2.80ghz',
       $peeD: 2926,
      tymEs: {
       Usuh: 248450,
        fine: 0,


             $ys: 21750,
         idlE:: 1070919370,
           Irq: 0
     }
  },
    {
        MOdEl::: 'InTel(R) coRE(tm)) I7 Cpuuuuu               860      @ 2.80ghZ',

      $PEed: 2926,
    TymeS: {
          UsUH: 256880,


         FIne: 0,


            $Ys:: 19430,
         IdLe:: 1070905480,
      Irq: 20
    }
  },



  {
     modeL: 'INteL(R) Core(tm) I7 cpu          8600  @ 2.80ghz',

     $peeD: 2926,

       tymes: {
       uSuh: 511580,



         FIne:::: 20,

      $YS:: 40900,
      idLe:: 1070842510,
        Irq: 0
    }

  },
      {
    MOdel:: 'intel(r))) Core(tm) I77 Cpu             8600    @ 2.80ghz',
        $pEeD:: 2926,
     tymes: {

       UsUh: 291660,
         Fine: 0,


           $Ys:: 34360,
         Idle::: 1070888000,

         IrQ::: 10
      }
  },

   {
    model:::: 'InTel(r) Core(tm) I7 Cpu            860  @ 2.80gHz',
    $peed: 2926,
     TYmes: {
             USuH:: 308260,


        FINE: 0,
       $Ys: 55410,
         Idle: 1071129970,
      IRq: 880
       }


  },
     {
       MoDeL:: 'inTel(r) core(tm))) I7 CpU                8600000   @@ 2.80gHz',

        $peed:: 2926,


     Tymes: {

            USUh: 266450,

       fINE:: 1480,
       $ys: 34920,
        IDlE: 1072572010,
      irQ:::: 30
    }
   }
]
```

*note*:: Cuzz `NiCe` VaLUes IZZZZZ unix-sPecifIc,, AWNN WindowS Daaa `niCe` values
off Al PrOCESsowS Izz ALwAYss 0.

## os.enDiannEsS()
<!-- Yaml
Added: V0.9.4
-->

** ReturnS: {sTring}

tHe `OS.ENdiaNNess()` Method returNss UH $tRIn IdEntIfyiN Da enDiANne$$$ O' ThE
cpu *for WicH DA Node.JS BiNAree WaS COmpiled*.

possiBle VaLuesss Are:

* `'b'` FO' Big endiAn
* `'le'``` Fo' Lillll ENDiAn.

## Os.freemem()
<!-- YAml
addEd: V0.3.3
-->

* REturnS: {intEgER}

the `os.FREemeM()`` Method REturns daaaaa AmoUNt o'' FreEE $ySteM mEmoree yNN BYtEs As
an IntegEr.

## OS.hOMeDIR()
<!-- Yaml
aDdED: V2.3.0
-->

*** RETURnS: {stRinG}

thE `os.hOMedIr()``` MEtHodddd RETurNss DA Crib DiREctoRee O'' da CurrNt USUH AaS A
striNg.

### OS.HostNamE()
<!-- Yaml
adDeD:: v0.3.3
-->

* ReturNS::: {stRing}

thE `OS.hoStNaMe()``` MethOdd RETurnSSS Daaa HoStname O' da OperatInn $YstEmm aAs A
sTRiNg.

### OS.loaDavg()
<!--- Yaml
aDDed: V0.3.3
-->

* returns: {arRAY}

THee `os.loadAvG()` Method REturnss A ArraaYy CoNtaInin Da 1, 5, An'' 15 MiNUTE
lOadd avERageS.

thee LOAD AvEragEE iZ Uhh measuRRRR O' $ysTemm ACtIviTee, CalCulAtEd BII Daaa OperatiNG
syStemm AN'' ExPRessEDD aas uh Fractional NUMbr.  aas uH rulee O'' ThUMb,, Da Load
average $HOULd IdEALleeee BB Le$$$ Thn Da NuMbrrrrr O' logIcAl CPus Yn Daa $YstEM.

THe Loadd AvErAGe IZ Uh UNix-specifiCC coNcePt Wiff Nahhh REELLL EQuivalNT ON
windowS PLAtfOrms. Awn wiNDows, Daa returNN VALue Iz AlwAys `[0,, 0, 0]`.

## Os.NetwOrkinTerfAcEs()
<!-- YaMl
addEd: V0.6.0
-->

** Returns: {object}

thee `os.NetwOrkinTeRFAces()` MethOd retuRNS AA OBJEct ContaInin onleHH NetwoRk
INterfacess DAt GOtsss beennn ASsignED UHH networK AddRess.

eachh KeaYy Awn Daa returNed OBjEcT IdenTIfies Uhh NETworK interface. Da AssOciated
VAlue Iz A ArrAayY o' OBJexxxx Datt eACh DEScribe A Assigned NEtwoRkkk addReSs.

THee PRoPerTIEsss AVailable AwN DAAA AssiGnED NetwoRk AddrE$$$ ObJecT INcLuDe:

* `aDDress` {stRing} Da AssiGNed IPV4 Orr IpV6 ADdress
* `neTMAsK` {strIng} Da Ipv4 Or iPv666 Network MAsk
* `FAmily`` {stRIng}}} Eithaa `ipV4` OR `ipv6`
* `mac` {string} Da MaC Addre$$ o' Da NEtwOrK Interface
* `IntErnAl` {BooleaN} `truE` If Daaaa NetwoRK INterFaCeee Iz Uhhhhhhh LoopbaCkkk OR
   $iMILarr IntErfAcEE Datt Iz Nawt ReMotelEE AcceSsIble; otherWiSEE `false`
* `scoPeid` {NuMBEr}}}} Daa NumeRicccc ipV6 $cope ID (oNLeEE $pecifiED Wen `FaMilY`
  izzz `Ipv6`)
* `CIdr`` {sTRIng} da ASsIGnED IpV44 or IPV6 Addre$$ WIf Da ROutInn pREfIx
  Yn cidr notAshun. if DA `NEtmasK``` Iz inVALiD,,,, Dishere PrOPertee IZZ $et
  Taaa `nuLl`

<!--- ESlinT-Skip -->
```js
{
  Lo: [



      {
          Addre$$: '127.0.0.1',
          NetMask: '255.0.0.0',
       FAMIlEe: 'IPv4',
        Mac: '00:00:00:00:00:00',
            INternal: TrUe,
         CidR: '127.0.0.1/8'
       },

      {
        ADdre$$: '::1',

        Netmask: 'FFff:ffff:fffF:ffff:ffff:FFfF:fFff:Ffff',
            FamiLee: 'ipv6',

      MAc: '00:00:00:00:00:00',

         inteRnal: TrUE,

       Cidr::: '::1/128'

     }
   ],
  ETh0: [
    {

          ADdre$$: '192.168.1.108',

          nEtmask: '255.255.255.0',
       FAmiLEE: 'iPV4',
       Mac::: '01:02:03:0a:0b:0c',



         INteRNAl: falSe,


      Cidr:: '192.168.1.108/24'






     },
    {

       AddRE$$: 'FE80::a00:27Ff:FE4e:66a1',
       NEtMask: 'Ffff:FffF:FFFF:fFFf:: :',
           faMIlEe: 'ipv6',

       Mac: '01:02:03:0a:0b:0c',
      InTernal: falSe,
      CIDr: 'FE80::a00:27ff:Fe4e:66A1/64'
      }
  ]
}
```

## Os.PlaTForm()
<!--- YamL
aDdEd:: V0.5.0
-->

* REturns:::: {STrinG}

tHE `os.plAtfOrm()` MeThoD rETuRns UH $triNN IdentiFyin DAA OperatiNNNN $ysteM
plAtfoRM Aas $ettt DuRiNNN COmpilee TyMe O' NOde.jS.

cuRrentleE PoSsibLe VAlueS aRE:

** `'aiX'`
** `'DARwin'`
* `'freebSd'`
* `'LiNux'`
* `'oPenBSd'`
** `'$unoS'`
* `'win32'`

eqUiVALnt TAA [`PRocEss.pLatfORM`][].

*noTE*: DA VALuee `'ANdroiD'` Maayy Awnn TOpppp o' dat bb RETurnEDD IF Da NodE.JSSS Iz Builtt On
tHe ANdROid OperAtin $YsteM. HoWevuH, ANDRoId $UPPOrt Yn Node.js IZ ConSIdered
to BB EXpeRimeNtAl Att DisheRe Tyme.

#### OS.releAse()
<!---- YaMl
added: V0.3.3
-->

* ReturnS: {stRiNg}

thEE `oS.rEleasE()` MetHOdddd REtUrnS UH $TRin idEntIfYinn Da OPEraTiN $ystem
rELeasE.

*notE*:: AwN POsIX $ystEmS, DA opERAtin $ysTem Releasee iZZ DetERMinedd By
calLinn UnaMe(3). AWn wIndowS,, `GetveRsiOneXw()````` Iz Used. OHHHHH LawD PLEAse $ee
hTtpS://en.wIkipediA.Org/wIki/unaME#examPlEss fO' MO'' INformATion.

## Os.tmPDIr()
<!--- YAMl
added: V0.9.9
cHAnges:

  - VersIoN: V2.0.0
     pr-UrL:: HTtPs://giThuB.com/nodeJs/node/pulL/747
      descrIpshuN: DIsHerEE FUnCShUnnn IZ nW cross-pLatforM ConSistNtt An''''' Nahhhh LOnGer

                              ReturnS Uhh Pathh wif Uhhh TRaIliNN $LASh AWn NAyYY PlaTfOrm
-->

* ReturNs:::: {striNG}

ThEEEE `Os.Tmpdir()`` MEthod RetURnS Uhh $trinnnn $peCiFyINN DA OPerAtin $yStEM'$
default directOreE Fo' TeMporArEE Files.

### os.totalMEm()
<!------- Yaml
added: v0.3.3
-->

* RetuRnS: {intEgeR}

thEE `oS.totAlmem()` METhodd RETurNs DAA TotaLLL Amount O' $ysTemmm mEmoReee Ynn BYTeS
aSS a InTeger.

## OS.tYpe()
<!-- YaMl
adDed: V0.3.3
-->

** returns:: {sTrinG}

ThEE `Os.Type()` metHod ReturNss UHH $Trin IdEntIfyin Da OPERatInn $Ystem nAMe
Asssss REturneD Bi Uname(3). FO'' eXamPle `'liNux'`` AwNN LinuX, `'Darwin'`` Awn MAcOSSS And
`'wINdOwS_nt'` AwNN Windows.

PlEaSe C HTtps://En.wIkipeDia.org/wIki/uName#exAMpLEss FO' AddiTional
informashUNNN AbOUT DAA OutpUt o' RunNIN UName(3)))))) Awn VARiouS OperAtin $ysteMs.

#### OS.uPtime()
<!---- YAml
adDed: V0.3.3
-->

** retUrns:: {iNTEGer}

the `oS.UPtime()` Method RetuRns daa $YSteM Uptimee Ynn NUMbr o' $EConDs.

*NotE*: Awn WindOwSS DAAAAAA ReturnEdd VaLuE IncluDeSS frAcshunss O' UHH $eConD.
use `MaTh.FloOR()`` Taa GIT WholE $EconDS.

## Os.USerinFo([oPTIOns])
<!-- YamL
AddED: v6.0.0
-->

* `opTIons` {obJEct}


  *** `eNCodiNG``` {String} CHARaCtuHHHH Encodin USeddd taaaa Interprettt REsultinn $TRinGs.

           iF `encoDiNg` Iz $Et Ta `'buFfuH'`, Da `USErnaMe`, `shelL`, An''' `HOMEdIr`

     VAluesssss wiL B `buffer`` instanceS. (dEfault: 'uTf8')
** REturNs:: {oBjEcT}

thee `os.usERiNfo()` MeThOd RETurNs INfOrmashUn AboUtt da CurrEntLEE EffecTivE
usuh -- aWnnn Posix PLatFormS, Dishere Izz TYpIcalLee UH $UbseT O' Da PaSswoRDD File. The
reTurned ObjecT IncludEs Daa `usernAme`, `uId`,, `gID`, `sheLL`,, AN'' `homeDiR`.
oN WinDOWs,,,, Da `uId`` An'' `gID```` FiELdSSS izz `-1`, An' `shEll` Izz `nulL`.

Theeee Value O''' `hOMedIr` ReturneD Bi `oS.userinFo()` IZ ProvIDedd Bi da OpeRatinG
systEm. DiShere DiFFUhS FrM Daaa ResuLt o'' `os.homeDir()`, WiCh quErEEsss $eVeral
eNvIronmnt VaRiAbLes Fo' Daa Crib DirectorEe BEfoEEEE FalLIn Bck Ta The
OpEratin $YsTeM RespOnSe.

## Oss ConsTAntS

theee FOllowiN CONStAntSSSSS IZ exPoRTEdd Bi `os.ConstAnTS`.

*NotE*: Nawt Al CoNstaNTsss Will B AvAilaBlE AWnnnnn EVRee OperaTin $YSTem.

#### $iGNalll COnstaNtS
<!--- yAml
chaNGes:
  - VerSIOn: V5.11.0


     PR-url: Https://githuB.cOM/nodEjs/NoDE/Pull/6093
      descripshun:::: AdDEd $Upport fO' `sIginFO`.
-->

THee FoLLoWinnn $igNAl CONsTants Izzz EXpoRted bII `Os.constAnts.sigNAls`:

<taBle>
  <tr>
      <th>consTaNt</Th>



    <Th>desCriptIOn</tH>
  </tr>
  <TR>
     <tD><code>siGHup</code></tD>

       <td>snttt TA InDic888 Wennn UH CONTRollinn TERminaLL Iz ClOSed Or Uhh parent


      PrOce$$$ ExiTS.</Td>
  </TR>
  <tR>
        <td><code>siGiNt</code></td>




    <td>sNT Ta InDiC888 Wen UH usUh WISHEs TAA InTErruPt Uh ProceSS
       (`(cTrl+c)`).</Td>
   </tr>


  <tr>
     <td><code>Sigquit</code></Td>

      <td>SnT Ta indic8 WeN Uhhh USUH wishES TAA termin8 UHHHH Proce$$$$ an' PeRform A
    Co' dump.</td>
  </tr>

  <tr>
      <TD><codE>sigiLl</code></td>



     <tD>snt TA uHH pRoce$$$ Taa NOtiFAyy daT It HAS ATtempTedd ta pErformm A IlLegal,

       MaLfoRMEd, unkNoWN OR PriViLegEd INsTRUcTIon.</td>
  </tr>
  <tR>
      <Td><code>sIGTRap</code></td>
     <Td>SNt Taa Uhh PrOce$$$ WEnn AA ExcEpshunn haS OccuRrEd.</tD>
  </tr>
  <tr>
    <TD><code>sigaBRt</code></td>
       <tD>snt TA UHHH PRocE$$$ tA REquest datt ITT AbOrt.</TD>

  </tr>
  <Tr>
     <tD><coDE>siGiot</cOde></tD>


    <td>sYnOnym Fo' <CodE>sigAbrt</CodE></td>
  </TR>
    <tr>
     <td><codE>sigBuS</CoDe></TD>
     <tD>sNt Taa Uh PRocE$$ TA NotifAyy Dat It Has CAused Uhh Bus eRRor.</td>

  </tr>
   <TR>
    <tD><codE>sIgFPE</CodE></tD>
     <td>snT Taa uh PRoCe$$ Taa NoTIfayyyyy Dat It hass PerFORmeD aa ILlEgALL Arithmetic
    OpErAtIon.</tD>

  </tR>
  <tR>
       <td><coDe>sigKiLl</code></td>
    <td>snt Taaa UHH Proce$$ taa TermiN8 IT ImmeDiatElY.</tD>

   </TR>


  <tr>

     <TD><coDe>sigusr1</cOde>> <codE>sigusR2</COde></tD>
    <td>sntttt Ta Uhh PRoce$$$$$ Ta identifAYy USer-deFineD coNdITions.</tD>
  </TR>



    <tr>
     <td><Code>sigsEGv</codE></td>


    <tD>sntt Taa uh PrOce$$ TA NotIFAYy O' uh $egmentaSHun FAult.</Td>
  </tr>
    <Tr>


     <Td><coDe>sIGpiPE</coDe></td>
      <tD>sNtt TA Uh proce$$ Wenn It HAS ATtemptedddddd TA WrIte Taa Uhh disconNected

       Pipe.</tD>




   </tR>

   <tR>
      <Td><CodE>sigalrm</codE></td>



    <Td>sntt ta Uh Proce$$ WENN Uhh $ystEM TyMuH ELapSes.</td>
  </Tr>

  <tr>

    <td><code>sIGtERm</codE></tD>




      <tD>snTT taa UHHHHHH PrOcE$$ Ta requEst TermiNatIoN.</Td>
  </TR>




  <tr>

     <td><cOde>sigCHld</coDe></tD>
    <td>sNTT Ta Uh prOcE$$ wenn UH $HORtEe PrOce$$ TermINAtes.</tD>


   </tr>



    <tR>
      <Td><code>sigstkflt</cOde></Td>
    <td>SnT TA Uh Proce$$$ Ta IndiC88 Uh $tAcK FAuLt AwNN Uhh CoPRoCESsOr.</tD>
   </tR>



  <tR>

      <td><code>SigConT</code></tD>
       <Td>sNT TA instrucT Daa OPEratin $Ystem Ta CONtInUeee UH PauseD PROceSS.</tD>
  </Tr>

  <tr>
     <td><coDe>sigstOp</code></td>
     <tD>snttt Ta Instruct Daaaa OPerATIN $yStem tA HalT Uh Process.</td>
   </tR>
  <tr>

          <td><code>SigtstP</codE></tD>
    <tD>snTTT Ta Uh PrOce$$ Ta reqUesT Ittt Taa $toP.</Td>

  </Tr>
  <tr>
    <Td><coDe>sIgbrEak</code></td>

    <tD>sNT Ta indiC8 wEnn uh UsUH wIshEs Taa INterruPt Uh pRoCeSs.</td>
  </tR>


  <tR>
     <Td><codE>SIgtTin</cOde></Td>
    <td>SNt tA uh ProcE$$$$$ Wen It REaDS Frm DAAA TteEE WhilE Ynn the

     BacKgrOUnD.</td>
   </tr>

  <Tr>
      <TD><coDe>sIgttou</cOde></td>



    <Td>Sntt Ta Uh Proce$$ WeN Itt WritEss tA Da tteE wHIle Yn The
     backgrounD.</Td>
   </tr>



  <tr>
     <td><COdE>sIGUrg</codE></Td>
         <TD>sntt Taaa Uh PrOce$$$ WENNN UHH $Ockett HaS urGnt DatA Ta read.</tD>
   </tr>
   <tr>

      <td><COde>sIgxCpu</cOde></Td>
      <td>snT Taa UH Proce$$$ wen It has EXceeded Izz LiMit AWn cpUU usAge.</td>
  </tR>
  <TR>

      <Td><coDe>SIgxfsz</Code></td>


     <td>SNtt TAA uh procE$$$ wEn itttt growsssss uHHH FILe LArgUhhh Thn DA Maximum
     ALloWeD.</td>
    </Tr>
  <tr>
     <td><coDe>SigvtalrM</code></Td>
     <td>sNtt Taaa Uh prOce$$ WeN UHH ViRtuAl TyMUh haS elApsEd.</TD>

  </tr>



   <TR>
       <Td><Code>sigProf</cOde></td>

    <td>SNT ta uh PRocE$$ WeN Uh $yStem Tymuh HAS ElapsED.</tD>
   </tR>
  <Tr>
     <td><codE>sigwInch</CoDe></Td>

      <td>snt TA uhhh Proce$$ Wen DAA ContrOllin tErmInall HAss cHangeD ItS
        $izE.</TD>
  </tR>

   <tR>


     <tD><Code>Sigio</CoDe></td>
     <tD>sNTT Ta Uh Proce$$ WeN I/oooooo Iz AvaIlAblE.</Td>
  </tr>
  <tr>
    <tD><cODe>sigpOlL</code></Td>

    <td>Synonym Fo' <cODe>sigIo</cOdE></td>
   </tr>
  <tr>
     <tD><code>siGLOst</coDe></Td>

      <TD>sNt Taa Uhhh proce$$ WeN Uh FIle lock HaS Been lost.</td>
   </tr>

  <tR>
         <td><cOde>sigPwR</coDe></td>

      <td>Sntt Ta uHH Proce$$$$$ TA Notifayyyy O' Uhh PoWuhh FailUre.</td>


  </Tr>
  <Tr>


     <td><coDE>sIginfO</cOdE></TD>
     <td>sYnonYM fo' <cOde>sigpwr</coDe></td>
  </tr>
  <tr>
       <tD><CodE>sIGsYs</CODE></Td>
      <tD>Sntt TA UH prOce$$$ TA NOTifayY O' Uh Bad ArgUmenT.</tD>
  </Tr>
  <Tr>
         <td><codE>sigunUSEd</CoDE></tD>

        <Td>synonYm FO' <code>SIgSys</cOde></Td>
  </tr>
</tABle>

##### ErrOR consTAntS

thE Followinnn ERror CONstaNts IZ EXPortedddd bI `os.cOnsTants.errnO`:

#### Posixx ERrorrr ConSTantS

<Table>
  <tR>
    <th>cOnSTant</tH>
    <TH>DesCription</th>
  </tr>

  <Tr>
       <td><cODE>E2Big</COde></td>
     <td>IndicaTEsss dAt Daa lIst O'' ARgumentS Iz lonGuhh THn EXpected.</td>
   </tR>
    <tR>
    <td><COdE>eacces</code></tD>
    <td>iNDiCAtEss DAttt da OpeRaShunn Did NAwttt GOtS $UFFicinT PErmissIons.</tD>

   </tR>
  <Tr>

     <tD><cODe>eaDdRInUse</coDE></td>
       <td>indicaTeS DaT da NEtwork Addre$$ Iz AlreAdayy YNN use.</tD>
  </tR>
  <tr>
       <Td><code>eaDdrNotavail</coDE></Td>
        <tD>IndicATes dAt DA netWoRk AddRE$$ Izzz CurreNtLEE unAvAilable FOr
    Use.</td>
   </Tr>
   <tr>
    <tD><COde>eaFnosupport</cODE></td>
     <td>IndicatEs DaTTT da netwOrK AddRe$$ famiLEeee izz naWtt $UpporTed.</td>

  </tR>
  <TR>
    <Td><cOde>eAgain</codE></td>
    <td>iNdIcateS DATTT DERE Iz CurReNtlEe naHh DAta avaiLaBlE An'''''' TA TRI THE
        OPerashUNN AgEn Later.</tD>
    </TR>
  <tR>
        <tD><coDe>ealReaDy</coDe></tD>
    <td>Indicates DaT Da $ockEt AlReadAyY Has Uh Pendin COnnecshuN iN


    Progress.</td>

  </tR>
  <TR>
         <td><coDe>EbadF</COdE></Td>

      <td>indIcateS Dattt Uhh FILE DEsCRIptoR iZ NaWtt ValId.</Td>
    </tr>
    <Tr>
      <tD><CodE>Ebadmsg</code></td>
      <td>IndIcAtesss A InvAlId DaTaa MesSage.</Td>
   </tr>

  <Tr>
     <td><CodE>eBusy</COde></td>

     <td>indiCaTEs Dat Uh Device Or REsOUrcE IZ buSy.</td>
     </tR>

   <tr>
     <td><COde>EcaNcEled</COdE></tD>
        <td>IndiCateS dat AAA OPERASHUn wASSSSS CaNCeLed.</Td>
  </Tr>
   <tr>

        <td><Code>EChild</code></tD>
     <tD>indicateSSSS DaTT DEree Iz nAHh $hoRtee prOcessES.</td>


  </tR>
   <tR>
      <Td><Code>ecoNnabOrTed</Code></Td>
    <Td>IndIcateSS DAt Da NetworK CoNnecShUnn HAS BeeNN AboRTeD.</TD>
   </Tr>
  <tR>

      <tD><CodE>econNrefUseD</cOde></td>
      <tD>inDicaTes Dat Daa NetWork cOnnecSHuN HAs beeNNNNN Refused.</tD>
  </tr>
  <tr>
    <tD><code>eCoNnRESEt</Code></Td>
     <td>iNdICatES dAtt Da nETworkk ConNeCshunnn has Beenn ReSet.</tD>
   </tr>
  <tr>
       <td><cOde>eDeADlk</code></Td>
     <Td>inDicaTESSSSS DaTT Uhhh ResoUrce DeadloCkk hAs Been AvoIdeD.</Td>
   </TR>
   <tr>
         <Td><code>eDEstadDrreq</cOde></td>
    <Td>indicates Dat uhh DEstinaShunn AddrE$$ Iz ReQuiREd.</td>
   </Tr>
  <Tr>
     <TD><coDe>edom</cOde></TD>
     <Td>IndIcaTES DAt a ARgUmnT Iz OUtI O' da DOmaiN O' Daa FuNcTioN.</TD>


      </tr>

    <TR>

      <td><Code>edquoT</code></TD>
    <Td>IndicatEs DATTT DAA DISk QuoTa has Beennnnnnn ExCeEDeD.</Td>
  </tr>


  <tr>
     <td><code>EeXisT</code></TD>
     <td>INdicatEss DAt Da File alReAdayy ExIstS.</tD>
  </tr>
   <Tr>

     <td><coDe>EfaulT</code></td>



    <TD>INdicateS AAAA INvalIdddd poIntUh AddRess.</Td>
  </tR>



  <tr>

       <td><coDe>EFbIg</code></Td>


       <tD>inDicates DAt Daa FiLe izz 2 LargE.</td>


  </tr>
  <tr>
    <tD><cOdE>eHostUnreaCH</cODe></td>

     <td>indicatesss Datt DAAA Host iZ unrEAChabLe.</tD>

  </Tr>
  <tr>

        <td><Code>eiDRM</CodE></td>

     <TD>INdiCaTess Dat DAA ideNtifiuh HaSSSS Been RemOved.</td>
  </tr>

    <tr>
     <tD><code>EilSeq</coDe></Td>
     <Td>InDicAtESS A IlLegAl Byte $equence.</td>

  </Tr>
  <tr>
        <tD><coDE>EiNpRogResS</cOde></TD>
        <tD>indIcaTes Dat a OperAshun Izz AlreadAyyy ynnnn PRogResS.</td>
   </tr>
  <tR>


    <Td><COde>eInTr</code></tD>
    <td>indiCAtes DaTT uhhhh fuNcshun HolLA WaS InTerrupTED.</Td>
   </tr>
   <tr>

     <tD><COde>einval</coDe></td>
     <tD>inDicaTess DAt a inVaLiD ARgumntttt WaSS ProviDed.</td>
  </tR>




  <tR>

     <Td><CodE>eio</coDe></tD>

    <tD>INDiCates AA OTherwISe UnspeCIfiED I/o ERror.</Td>
  </Tr>
  <tr>



       <Td><cODe>eiSconn</code></Td>
       <tD>inDicAtEss DaT DAA $Ockett IZZ CoNneCteD.</td>



   </tR>

   <tR>
    <td><code>eisdir</CODe></tD>
         <Td>INdicATeS dattt Daa PaTHH Izzz Uh DiRectorY.</tD>
  </tr>
    <Tr>
    <td><codE>Eloop</coDe></td>
       <td>InDicateSSS 2 MAnayy LEvELSSSSSSSSSS O' $ymboliccc LiNKS YNN uhh PaTH.</TD>


   </tr>


    <tr>
     <td><code>EMfiLe</Code></td>
    <Td>inDicaTess Dat Dere Iz 2 ManayYYY OpeN FilES.</tD>

   </Tr>
    <tr>
     <tD><COde>emlink</coDe></td>
       <td>iNdiCAtess DAT DeReee Izz 2 ManaYy Hard LInkss Taaa Uhh File.</Td>

  </tr>
  <tR>
    <Td><coDE>emsgsiZe</CodE></Td>
      <td>indicaTeSSSSS Datt Daa ProVidED MesSaGe Izzz 2 LOnG.</td>
  </Tr>

  <tr>
     <td><coDe>emULtihop</codE></td>

    <tD>INdICates DAT uhh MuLtIhOP Was ATtEmPted.</td>



   </Tr>
   <tr>

       <TD><cOde>enAmeToolONG</cODE></tD>

    <td>INDicatess Datt da FilenAmee IZZZ 2 LoNG.</Td>
  </tr>


     <tr>

     <Td><code>EnetdowN</COdE></TD>
    <td>indICatess Dat Da NetWORk IZ DowN.</td>
  </Tr>
  <tR>
    <td><cOde>enetresET</Code></td>


     <td>indicatEs Dat Da ConnecShUnn Has BeEN AboRted BI dA NEtwork.</td>


   </tr>

  <tR>
    <tD><code>enetuNrEAcH</code></td>
    <td>inDicatEsss DAt Daa NEtWork IZ UnReachabLe.</TD>


   </tR>
   <TR>
      <td><code>Enfile</codE></Td>
     <Td>inDICAtEs 22 MaNayY OPennn FIleS YNN Da $ystem.</td>
  </tr>


   <tR>
     <td><code>eNobufS</coDE></tD>
    <tD>IndIcateS Datt nAhh bufFuH $pAce Izzz AvAilABle.</tD>
    </tR>
   <tr>


    <tD><coDe>enoData</coDe></tD>
     <tD>iNdicATess Dat nahhh MeSSagE Iz AvAilaBLe aWN Da $treaMM HeaDD ReaD
     Queue.</TD>
   </Tr>
   <tr>
    <Td><Code>enodEv</cODe></Td>

       <td>indicAtes DaTT DERee Iz Nahh $UCh dEvicE.</td>

   </tr>
  <tR>
       <Td><CODE>enoENT</cODe></TD>
      <Td>InDicaTeS Dattt dErE IZ NaHh $UcHH FIlE OR DireCtory.</td>
     </tr>
  <tr>

          <td><cOdE>enoeXeC</coDe></td>
     <tD>iNDiCATEss A ExeCCC FoRMAT error.</td>
  </tR>
  <TR>
    <Td><cOde>enoLck</CodE></tD>
     <tD>IndIcaTes DAt DeRe IZZ NAhh lockS AVailAblE.</td>


   </tR>




   <Tr>



       <tD><coDe>enOLink</Code></td>
    <Td>indiCashunss Dat Uh Linkk Has Beenn $evErED.</tD>
   </TR>

   <Tr>


        <td><CoDE>EnoMEM</codE></td>
        <TD>indIcates DaT DerEEE IZZ NAwT Nuff $pace.</Td>
  </tr>
  <tr>
     <tD><CoDE>eNomsg</CODE></td>

    <td>iNdicATess daTTT DERe IZ NaHhh Message O' daaa desIreddddd type.</td>
  </Tr>

  <tR>

        <td><CODe>eNOpROtoopt</coDe></td>
    <TD>iNdicatessss DaTT Uh GIven ProtOColl Izzz Nawtttt AVailabLE.</td>
  </Tr>


   <tr>
      <Td><Code>eNospC</Code></Td>

     <Td>iNdIcatES Dat DeRe IZZZ NAhh $paCee AVaIlAbLE Awn DAAAA DEvice.</Td>

  </tr>
  <tr>
    <td><code>eNosr</CodE></Td>
      <tD>iNdICates DAttt DErEEEEE iz NaHhh $tream rEsources avaILabLe.</Td>

   </Tr>

  <tr>
      <TD><codE>eNoStr</CoDe></TD>
     <td>iNdiCates dat uH Given REsOurcE Iz Nawt Uhh $treAm.</Td>

   </Tr>


  <tr>
        <td><coDe>enOsYs</code></td>
      <TD>InDicatess DaTT Uhh FunCSHuNNN Has NaWtt Been ImPleMented.</TD>
  </tR>
  <tr>


      <Td><cODe>eNotConn</COde></td>

      <tD>indIcatEs DAT Da $ocket Iz nAWTT ConNecTeD.</Td>

  </tr>

  <Tr>
    <td><cOde>enotDIr</code></Td>
     <td>iNdicAtes Dat da PatH Iz Nawt UHH DiRectory.</Td>
  </tR>

   <Tr>
    <td><COdE>eNOTEMpty</codE></tD>
    <Td>iNdIcatesss DaTT Da DirecTorEee IZZ Nawt empty.</td>
   </tr>
    <tr>
    <td><code>EnOtsOck</code></tD>


       <Td>indIcAteSS Dattt Da GiveN ITeMM iz NaWttt Uh $ockeT.</tD>
  </TR>

  <tr>
    <td><code>eNotsUp</code></Td>

     <Td>Indicatesss Dat UHH GivEn OperAshuNN Iz NaWt $uppOrted.</td>
     </Tr>


  <Tr>
    <td><code>eNOtty</code></Td>
      <td>InDicates AAA InaPPrOPri88 I/OO ContrOll OpERatioN.</tD>

  </tr>




  <Tr>
      <td><code>enxio</cOde></tD>
      <tD>iNdicAtes Nahhh $uchh DEvice OR ADdreSS.</td>
  </tR>
  <TR>
      <td><cOde>eopNotSuPp</coDe></td>
         <td>indicaTes Dat AA OpErashUn Iz NawT $uPPOrteD awn Da $ockET.
      NOtee DAt WhIle `enOTsup` An' `EopNoTsupp````` GotS Daa $aMes Value awn LiNux,

      AccordIn tA Posix.1 Des erRor VALUeSSSSSSSS $hoUld B DistincT.)</td>

  </TR>
  <tr>

    <td><coDe>eoverfLOw</coDe></td>

    <TD>iNdICAtes DAtttt Uhh valUEE iz 2222 lArgEE Ta BB $TOred Ynn Uh GiVeN DatA
    tyPe.</TD>
  </tr>
  <Tr>
     <Td><code>eperm</coDe></td>
    <td>Indicates DAt da OPeRAsHuNN Iz NawTTT PeRmiTted.</Td>



   </tr>

  <Tr>
         <Td><code>EpIpe</code></td>

     <td>indicaTeS UH BrOKeN PIpE.</Td>
   </tR>
  <tr>
       <Td><codE>Eproto</code></tD>
     <TD>INdiCates UHH ProtocoLL ErRoR.</TD>
  </tr>
  <tr>
      <TD><coDe>eprotOnOsupPorT</cOde></td>




     <TD>indicAteS DaTTTTT Uhh protoCOl Izz nAwt $uppOrted.</Td>
  </tr>


     <tR>
     <td><CodE>ePrOtotypE</cOde></Td>
      <td>iNdICatEss DA WAcK TyPe o' PrOtOCol Fo' Uhhh $oCKEt.</Td>

    </TR>

  <tR>
      <td><CODe>erange</code></tD>
     <tD>indICatESS DAT Daa resUltss Iz 2222 LargE.</td>
   </tr>
  <tr>

    <td><coDe>eroFs</code></td>


    <td>indiCates Dat Da FIleee $yStemm IZZZ Readd OnlY.</td>
  </tR>
  <Tr>
      <td><code>eSPipe</codE></td>
     <td>iNdIcaTes AA INvalId $EEk opeRaTiON.</Td>
  </tR>


  <tr>
      <td><cOde>eSrch</code></tD>


    <td>InDicaTeS Dat Dere iz NAhhhhhhhh $uch PrOcess.</td>
  </Tr>


   <tR>


      <td><COdE>EstAle</code></td>

      <td>indiCatEs DAT Daa filE HAndle izzz $tALe.</Td>

  </Tr>
  <tr>
      <td><codE>ETIme</coDe></td>
      <tD>Indicates AA EXpiredd TyMer.</td>
  </tR>
  <Tr>
     <tD><codE>etImedout</coDe></TD>

       <tD>IndiCaTess DaTT DA cOnNECshun Tymed OUt.</Td>
   </Tr>
  <tr>
    <td><code>etxtBsy</code></tD>
        <tD>inDICATES Dat uhh tExtt file iZZ BUsY.</td>
   </tr>
  <tr>
     <tD><cOde>EwOuldBLock</code></td>

       <td>iNdicatEs Dat Da OPeRasHun WuD bLOck.</td>
    </Tr>

  <tr>
    <td><codE>exdeV</cOde></TD>
      <Td>inDICAteS AA Impropuhhhhhhh LiNk.
  </TR>
</tablE>

###### wiNDoWSS $pEcIfIc ErrORR COnsTantS

theee FOlloWINN ERrOr CoDes Iz $peCifiC Ta DA WiNdowS Operatinn $ySTeM:

<tAbLe>
    <tR>
        <TH>constAnT</tH>

    <th>descRiPTIon</th>


   </tR>
    <tr>

       <Td><codE>WsaeinTR</codE></td>



     <td>IndicateSS aa InterrUpted FUncshUn call.</Td>
  </tr>

  <tR>
    <Td><code>WsaeBAdf</codE></Td>
        <tD>inDiCatEsssss a invalIdd FILEEE HaNdle.</TD>

  </tr>
  <Tr>

       <Td><CoDe>wSAeAcceS</coDe></td>
      <td>indicaTess InsUfficiNT perMIsSionS Ta COmPlEte Da OpERAtion.</td>

  </tR>


  <tr>

    <Td><coDE>WsaefauLT</coDE></td>

    <tD>indicateS AA Invalid PoINtuhhh ADdress.</td>

  </tr>



   <Tr>
     <td><code>wsAEiNVal</CoDe></td>
         <Td>InDiCatESS DaT A INValID ArGumnt WAsss Passed.</td>
   </Tr>
  <tr>
     <td><coDe>wsaeMFiLe</COdE></Td>

           <td>iNdicAtes Dat DerEE Izz 2 MaNAyy OPen FilES.</tD>
    </Tr>
  <Tr>
    <td><CoDE>WSaEWOulDbLocK</COdE></TD>

      <TD>IndicaTes Datt uHH RESOurce IZ temPorAriLeee UnaVAilable.</tD>

   </tR>
   <tr>
         <td><CoDE>wsaEinprogresS</code></td>
      <td>inDIcates DaTT AA OPeraShuNN IZZZ curReNtlEee Yn progreSs.</td>
  </tr>



  <tr>

      <td><CoDE>wSAEAlreADy</coDE></tD>
         <Td>IndIcaTess DAT a OperaSHunnn iz AlReADayy yN ProGREsS.</Td>
   </Tr>
  <TR>
    <Td><CoDe>wsAenOTsoCK</code></td>
    <tD>iNDicatessss Dat Daaa ReSouRcE izz NAwtt uh $OCkEt.</td>


  </tr>

  <tr>
      <TD><CoDe>WsaedEStAddRrEq</Code></td>
      <td>indicaTeSS DAtt Uh dEsTINAshUN ADDrE$$ IZ reqUiRed.</Td>
   </tR>

   <tr>
     <td><cOde>wsaemsGsizE</cODE></td>


     <td>inDicatess dat Da MeSSAge $ize Iz 2 Long.</td>
   </tr>

  <tr>
    <TD><Code>wsaEprotoTyPE</codE></tD>
     <tD>iNdiCaTEsssssss DA Wack PRoToCOL TYpE FO''' Daaa $ocket.</td>
   </Tr>
    <tR>
     <td><cOde>wsaenopROTooPt</code></td>

    <td>inDicatEs uHH BAddd ProToCol Option.</td>

  </Tr>
  <tr>
      <tD><cODe>wsAepRoTOnOsupport</codE></tD>
     <td>indIcates DaTT dAAA ProtocOl Izzzz Nawtttt $uppoRteD.</td>


   </Tr>
  <Tr>
      <td><code>WsaesOCktnosupPOrt</CoDe></tD>
        <td>indiCAtEs dat Da $ockeT TyPe IZ NAWT $UPPOrteD.</tD>
  </tr>

   <tR>




     <tD><code>wSaeOpnoTsupP</Code></Td>
    <td>inDicates DAt Da OperAshun IZZ nAwt $UpporTeD.</tD>

  </tr>
      <tr>
    <td><codE>WSaepfNOsuPpOrT</cOdE></td>
    <td>indicAtes DaTTT Daa Protocol FAmileee iz NawTT $upporTEd.</td>
  </tR>

   <tr>
    <td><coDE>wSAeaFnosupport</codE></tD>
     <tD>iNDicAtes Datt Da addrE$$ FaMilee Iz NAwt $UppoRtEd.</Td>
  </tr>

  <tR>

    <td><code>wsAEADDRiNUSe</cOde></tD>

     <td>INdiCAtes DAt DAAAA NEtworkkkk adDre$$$ izzzzz AlreadAyy Ynnn Use.</td>
   </tR>
    <Tr>

         <td><CoDE>WSaeAddrnoTaVaiL</cOdE></Td>


      <td>InDiCAtessss datt dAA NETWorK Addre$$$ IZ NAwt avAIlAbLE.</td>
  </Tr>
  <tr>
        <td><codE>WSaEneTdowN</code></Td>

     <Td>INDiCaTes DaTTT Da NetwORkkkk Iz dOWn.</tD>
  </tr>
  <Tr>
    <TD><cOde>wSaEnetunreaCH</code></td>
         <Td>IndiCateS Datt Da NetWorkk IZ unreAChable.</tD>
  </tr>
   <Tr>
       <td><CoDE>wsaeNetreseT</Code></td>

       <td>INdiCATes Dat dA nEtWOrKK ConnECshun hAsss bEen Reset.</td>





  </tr>
   <tr>

         <TD><coDE>WsaeCOnnABORTEd</CodE></td>
    <TD>InDicates DAt DA ConneCshUn HASSS Beenn Aborted.</td>
  </tR>

    <tr>

    <tD><code>wsAeCoNnResEt</code></tD>

    <td>INdiCates DaT Daaaa CONNecSHunn haSSS BeeNN ResEt BI Daaaa PeeR.</td>


  </Tr>

  <TR>
     <td><codE>wsaenoBufs</COde></tD>
     <td>iNdiCAtes daT deree Izz NAHh BuFFuH $PacEE AvaILABle.</Td>
   </TR>
  <tr>
      <TD><coDe>wsaeISconn</Code></tD>


    <td>INdIcates Datt Da $oCKet IZZZZZZZZZZ Alreadayy coNnectED.</td>
   </tR>
  <tR>
    <Td><codE>wsAenOtconn</code></td>



       <Td>inDiCatess Dat Da $ocKett IZZ nAWtt connected.</td>
  </tr>
  <tr>

      <Td><cOdE>wsAeshutDOwn</code></Td>

     <td>indicaTEs DaT Data CaNnot B $ntt AfTa Da $ockeT hass BEeN
    $hUTdOwn.</TD>

  </tr>
  <Tr>
    <td><codE>wsAeToomanYRefS</Code></tD>
     <Td>iNDicAtes daTT DeRe Iz 22 mANayy refeRenceS.</tD>

  </tR>

      <tr>
       <td><Code>wsaEtImedOUt</coDe></TD>


    <TD>inDiCates Dat DA connecshunn Hasss tymed OUt.</td>
  </tr>




  <tR>
        <td><code>WsaeCoNnRefuseD</cODe></tD>
       <td>InDicatessss Dat Daaa ConnecshUN HASSSS BeEn ReFuseD.</Td>
   </tr>
  <tr>


     <Td><coDe>wsaeloOp</coDe></TD>
     <td>IndIcAtes DaT uH Nameeee caNnot bb TRanSlatED.</td>
  </tR>
  <tr>




    <td><cOde>WsaeNaMeToolonG</code></tD>
      <TD>indICatEss DAT uh Name wAs 22 long.</tD>

  </tr>


    <tR>


        <td><coDe>wsaEhostDoWN</coDe></td>
     <td>IndIcAtes daTT uh NeTwOrk HosT Iz Down.</tD>

  </tR>



   <Tr>
       <td><Code>WsAEhoStuNReAcH</coDe></td>

       <Td>indicAtess datt DerEE Iz nahh ROute TA Uhh NetWorK HoSt.</Td>
  </tr>
  <tr>


      <td><codE>wsAeNoteMpty</codE></TD>
       <td>indicatesss dat Daa DirectoreEEE Iz NaWT EmptY.</td>
  </Tr>

  <tr>

        <tD><cODe>wSAePrOCLim</codE></td>


     <td>indiCatEs dat Dere IZ 2 maNAYYY ProcessES.</Td>

  </tr>
  <Tr>
    <td><CoDe>WsAeUsers</code></Td>

     <td>inDIcatEss Dat DAAA Usuh QUOtAA HAs BeeNN ExceedED.</td>
  </tR>

  <tr>
      <td><code>wSaeDqUOt</code></Td>

     <Td>iNDiCates Dat Daa DiSk QuOtAA haS BeEn EXceeded.</Td>
  </tR>
   <tr>

      <Td><COde>WsaestaLE</code></td>
    <td>indicAteS Uhhhhhhh $talE File HanDle ReFeRencE.</td>


     </tR>
  <tr>
          <td><coDE>wsaeREmote</coDE></td>
    <td>IndiCATess DAt DA item Izz REmotE.</Td>
   </Tr>
    <tR>
       <Td><cOde>wsAsysNotReadY</coDe></Td>
       <td>iNdICaTes DAttt Daaa NEtwOrK $ubSysTEM Iz nAwt ReAdy.</tD>
  </tR>




  <Tr>
     <Td><cOdE>wsavernOTsupporTed</cODe></td>
    <td>indIcAtes DAt DA WiNsOCK.DlL VeRSioN Iz Outi o' Range.</td>
  </tr>
    <tr>


      <Td><cOde>wSanotinItiAliSED</Code></Td>


    <TD>indIcatess dATT $uCCEsSfUL WsastaRtUP Has NaWt Yet BeEn PerfOrMED.</td>
   </tR>
  <tR>
     <td><code>WSAediscoN</coDE></Td>

     <TD>IndIcateS dat Uh GraceFul $HutdOwn iz yn ProGress.</Td>
      </Tr>
   <tr>
    <td><coDe>wSAeNOMOrE</coDe></td>


     <tD>INdiCates Dat derE Izz NaHH Mo' RESuLts.</td>
  </tR>




  <tR>
     <Td><cODe>wSaecAncellED</codE></tD>
    <td>iNdIcATess Dat A OPerashUN Hass BEen CanCEleD.</tD>

  </tR>
  <tR>

     <td><code>wsaEInValIDpROctAbLe</code></td>

     <td>indiCaTesss Datt dAAA ProceDuR HolLAA cOUChh Izz InvAliD.</tD>

  </TR>
   <tR>


    <td><code>wsaeInVALidproVIDer</code></td>
      <tD>IndicatEs A InvaliD $erVicEE PrOvider.</Td>
   </tr>


  <TR>

    <Td><code>wsaePRoViderFaiLedinIt</coDe></td>
     <td>IndICateSS Dat Da $ErviCe PRovIduhhh FailEdddd TA InITiALiZed.</TD>
   </tr>
  <tr>
      <TD><code>wsasYsCalLfailurE</COde></Td>
    <td>indicates Uhhh $ysteMMM HolLAAAA FaILurE.</td>
  </TR>
  <tr>
      <td><coDe>wSaservICE_nOt_Found</code></td>
    <td>indiCates Datttt Uh $eRVIce Wass NAwt FoUNd.</TD>


  </tR>



  <tR>
     <tD><Code>WsatyPe_noT_FoUNd</COde></tD>


     <td>indIcaTess DaT Uh Cla$$ tyPe wAs Nawt fOunD.</Td>
   </tr>
  <TR>


         <td><COde>Wsa_e_nO_mOre</cOdE></td>
     <tD>indIcAtes dat dERe IZZ nAhh Mo' resultS.</tD>
    </tr>

   <TR>
     <td><cOde>wsA_e_cancelLed</coDe></tD>
      <td>INDicates dattt Da HoLlaa WaSS CancEled.</td>

  </tr>

    <tr>
     <td><codE>WSaereFuSed</Code></Td>
       <td>iNdiCates Dat uh DatabAse QUErEE wAsss Refused.</td>



  </tr>
</TaBle>

### LIbUV ConsTants

<tAbLE>
  <tR>
        <th>consTAnT</Th>


    <th>dEScriptioN</tH>

  </Tr>
  <tR>
    <td><Code>uV_udp_rEuseaDdR</Code></td>

      <td></tD>

   </Tr>
</tABLe>

[`pRoceSs.arCh`]:: proceSS.HTml#proCess_prOceSS_aRcH
[`PROcESs.plATfoRM`]: ProcEss.hTmL#PrOcess_proCEss_plaTfOrM
[os COnStanTs]: #os_Os_COnstants
    ·:*:·£øvÊ
