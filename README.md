# progetto-SO

Sistemi Operativi
Progetto A.A. 2021/2022 – Uno schema di coordinamento vagamente ispirato alla
Movement Authority per ERTMS/ETCS LV 1 e LV 2

<h4>Obiettivo complessivo</h4>
L'obiettivo del progetto è simulare il comportamento di un insieme di treni che, per realizzare la loro missione ferroviaria, attraversano vari segmenti di binario. La missione ferroviaria di ciascun treno è raggiungere una stazione specificata. Il vincolo principale al movimento dei treni è che ciascun segmento di binario può essere occupato da un solo treno alla volta. Ciascun treno riceve di volta in volta, durante la sua missione ferroviaria, il permesso di accedere al segmento di binario successivo. Il progetto prevede la realizzazione dello schema soprastante in due modalità differenti, che corrispondono a due diversi modi per richiedere e ottenere il permesso di accedere ad un segmento di binario. 
Descrizione del sistema e del suo comportamento. Nella Figura 1 sottostante, si riporta l’insieme di binari considerato. Si tratta di 16 segmenti, numerati da MA1 a MA16. Nel seguito del documento, con MAx si intende un qualunque segmento dei 16 presenti in Figura 1. Ciascun segmento è delimitato da 2 boe, con l’eccezione dei segmenti che contengono interconnessioni: questi ultimi sono delimitati da 4 boe, e sono i segmenti MA3 ed MA12. In Figura 1, ciascuna boa è rappresentata con una barra verticale. 
Figura 1 Binari e segmenti di binario MAx considerati.
Si definiscono 8 stazioni, una per ciascun termine di binario. Le stazioni sono numerate da S1 ad S8 come riportato nella Figura 2 sottostante.
Figura 2 Stazioni numerate da S1 a S8.
Si definiscono 5 treni, numerati da T1 a T5. Ciascun treno ha un itinerario associato, ovvero una stazione di partenza, un insieme di segmenti MAx da attraversare, ed una stazione di destinazione. Si definiscono due tabelle, che associano un itinerario a ciascun treno. Nel seguito, ci riferiremo a Tabella 1 e Tabella 2 col nome MAPPA1 e MAPPA2, rispettivamente.
<table>
  <tr>
    <th colspan="4">MAPPA 1</th>
  </tr>
  <tr>
    <th>#</th>
    <th>Stazione partenza</th>
    <th>Percorso</th>
    <th>Stazione arrivo</th>
  <tr>
    <td>T1</td>
    <td>S1</td>
    <td> MA1, MA2, MA3, MA8 </td>
    <td> S6 </td>
  </tr>
  <tr>
    <td>T2</td>
    <td>S2</td>
    <td> MA5, MA6, MA7, MA3, MA8 </td>
    <td> S6 </td>
  </tr>
  <tr>
    <td>T3</td>
    <td>S7</td>
    <td> MA13, MA12, MA11, MA10, MA9 </td>
    <td> S3 </td>
  </tr>
  <tr>
    <td>T4</td>
    <td>S4</td>
    <td> MA14, MA15, MA16, MA12 </td>
    <td> S8 </td>
  </tr>
  <tr>
    <td>T5</td>
    <td> - </td>
    <td> - </td>
    <td> - </td>
  </tr>
  </table>

<table>
  <tr>
    <th colspan="4">MAPPA 2</th>
  </tr>
  <tr>
    <th>#</th>
    <th>Stazione partenza</th>
    <th>Percorso</th>
    <th>Stazione arrivo</th>
  <tr>
    <td>T1</td>
    <td>S2</td>
    <td> MA5, MA6, MA7, MA3, MA8 </td>
    <td> S6 </td>
  </tr>
  <tr>
    <td>T2</td>
    <td>S3</td>
    <td> MA9, MA10, MA11, MA12 </td>
    <td> S8 </td>
  </tr>
  <tr>
    <td>T3</td>
    <td>S4</td>
    <td> MA14, MA15, MA16, MA12 </td>
    <td> S8 </td>
  </tr>
  <tr>
    <td>T4</td>
    <td>S6</td>
    <td> MA8, MA3, MA2, MA1 </td>
    <td>S1</td>
  </tr>
  <tr>
    <td>T5</td>
    <td>S5</td>
    <td> MA4, MA3, MA2, MA1 </td>
    <td>S1</td>
  </tr>
  </table>

Si definiscono le seguenti regole del sistema ferroviario in oggetto:
<ul>
  <li>Ciascun treno avvia la propria missione ferroviaria da una stazione, ed esegue la propria missione ferroviaria fino al raggiungimento della stazione di destinazione, seguendo l’itinerario prescelto. L’itinerario, cioè la sequenza di segmenti MAx che ciascun treno dovrà attraversare, è definito in MAPPA1 e MAPPA2; chiaramente, una solo mappa è utilizzata per ciascuna esecuzione del sistema ferroviario in oggetto.</li>
  <li>Tutti i treni avviano la propria missione ferroviaria nello stesso momento; in altre parole, partono   contemporaneamente.</li>
  <li>La missione ferroviaria di un treno termina quando raggiunge la stazione di destinazione.</li>
  <li>MAPPA1 e MAPPA2 sono conservati in un registro, e forniti ai treni al momento della partenza. I treni non possono deviare dall’itinerario specificato in MAPPA1 e MAPPA2.</li>
  <li>Un segmento MAx può essere attraversato da un solo treno alla volta; in altre parole, in un dato istante, ciascun segmento MAx può essere occupato da al più un treno. </li>
  <li>Si impiegano 2 secondi per attraversare un qualsiasi segmento MAx. </li>
<li>Ciascun treno chiede, alla partenza dalla stazione oppure al termine dell’attraversamento di un qualsiasi segmento MAx, il permesso di attraversare il segmento successivo o di entrare nella stazione di destinazione. Il permesso può essere richiesto in due modalità differenti, descritte nel seguito del progetto.</li>
<li>Ovviamente, se due o più treni richiedono contemporaneamente accesso allo stesso segmento di binario MAx, soltanto un treno potrà accedere (o meglio, occupare) il segmento; gli altri treni dovranno invece restare fermi, in attesa che il segmento di binario sia libero. </li>
  <li>  Nelle stazioni di destinazione può risiedere un qualunque numero di treni (le stazioni non hanno il vincolo di accesso unico che invece si applica al segmento di binario).</li>
  </ul>
<h4>Dettagli implementativi del programma richiesto.</h4>
Si richiede di sviluppare un programma che realizza il sistema descritto sopra, implementando i seguenti requisiti prescrittivi.
<h5>Requisiti per l’avvio del programma.</h5> 
Il set di itinerario da utilizzare, ovvero MAPPA1 o MAPPA2, dovrà essere specificato al momento dell’avvio del programma.
Inoltre, il programma è eseguibile in due modalità, che chiameremo ETCS1 ed ETCS2.
<h6>Modalità ETCS1: </h6>Il programma è avviato da una shell, specificando l’opzione ETCS1
Esempio: shell# ./nomeprogramma ETCS1 MAPPA1
<h6>Modalità ETCS2:</h6> Il programma è avviato usando due shell diverse: in una shell, l’applicativo è lanciato
specificando l’opzione ETCS2, mentre nell’altra shell è lanciato specificando l’opzione ETCS2 RBC.
Esempio:
shell1# ./nomeprogramma ETCS2 MAPPA1
shell2# ./nomeprogramma ETCS2 RBC MAPPA1
Il ruolo delle opzioni ETCS1, ETCS2 ed ETCS2 RBC sarà descritto nel seguito (sezione Modalità di richiesta di
accesso).
<h5>Requisiti dell’esecuzione del programma. </h5>
I cinque treni sono rappresentati tramite 5 processi, che chiameremo nel seguito PROCESSI_TRENI. 
I <b>PROCESSI_ TRENI</b> sono figli di un unico processo che chiameremo nel seguito PADRE_TRENI.
<b>PADRE_TRENI</b>è responsabile di creare 16 file di testo, che rappresentano i 16 segmenti MAx. Ad esempio, i file possono avere nome MA1, MA2, etc. Questi file sono inizializzati con il carattere "0" (zero) al loro interno (ovvero, PADRE_TRENI scrive uno 0 all’interno di tali file). 
I file sono impostati per avere accesso in lettura e scrittura da parte di tutti, e privilegi posti a 666.
Un processo <b>REGISTRO</b> mantiene gli itinerari MAPPA1 e MAPPA2. Il processo REGISTRO comunica ai vari treni l’itinerario, qualora richiesto. 

<b>Nota:</b> Per comunicare l’itinerario a ciascun treno, il processo REGISTRO necessita solo di conoscere il nome del PROCESSO_TRENO e la mappa in uso (MAPPA1 o MAPPA2).
Ciascun PROCESSO_TRENO, dopo la sua creazione, chiede il proprio itinerario al processo REGISTRO, e lo conserva in una struttura dati.
Tutti i PROCESSI_TRENI avviano la propria missione contemporaneamente, a meno di inevitabili ritardi computazionali.
Iterativamente, ciascun PROCESSO_TRENO:
<ol type="A">
  <li>Legge il segmento MAx successivo o la stazione successiva, dalla propria struttura dati dove ha memorizzato l’itinerario di missione.</li>
<li>Richiede l’autorizzazione di accedere a tale segmento MAx o alla stazione. Questa richiesta è effettuata in modo differente a seconda che il programma sia avviato con l’opzione ETCS1 o ETCS2 (si veda la sezione Modalità di richiesta di accesso).</li>
<ul>
  <li>Se PROCESSO_TRENO ha richiesto ed ottiene accesso ad una stazione, il PROCESSO_TRENO entra nella stazione, imposta a 0 il contenuto del file MAx corrispondente al segmento che occupava precedentemente, quindi termina (ha completato la propria missione).</li>
  <li>Se PROCESSO_TRENO ha richiesto ed ottiene accesso ad un segmento MAx, il PROCESSO_TRENO:</li>
<ul> <li>imposta il contenuto del file MAx ad 1. Imposta a 0 il contenuto del file MAx corrispondente al segmento che occupava precedentemente.</li> </ul>
  </ul>
  <li>Dorme 3 secondi.</li>
  <li>Ripete il ciclo ripartendo dal punto A</li>
  </ul>
  </ol>
<b>Chiarimento:</b> in altre parole, quando il treno si sposta, imposta a 0 il contenuto del segmento che occupava precedentemente (scrive 0 nel file corrispondente): questo vuol dire che “libera” il segmento, e altri treni potranno accedervi. Invece, il treno imposta ad 1 il contenuto del segmento in cui si sposta, ovvero lo “occupa”: nessun altro treno potrà accedervi. Se il permesso per spostarsi in un nuovo segmento è negato, il treno rimane fermo nel segmento attuale: riproverà a chiedere l’accesso al nuovo segmento nell’iterazione successiva. Sia che ottenga accesso, sia che non ottenga accesso, il treno aspetta 3 secondi. 
<h5>Modalità di richiesta di accesso.</h5> 
La richiesta di accesso ad un segmento MAx o una stazione è differente a seconda che il programma sia stato avviato con opzione ETCS1 o ETCS2.
<h6>Avvio con ETCS1.</h6> 
Se un PROCESSO_TRENO richiede l’autorizzazione di accedere ad una stazione, l’accesso è sempre garantito. Se un processo treno richiede l’autorizzazione di accedere ad un segmento MAx, il PROCESSO_TRENO verifica il contenuto del file MAx.
<ul><li>Se è 0, il PROCESSO_TRENO ha l’autorizzazione a spostarsi nel segmento MAx.</li>
  <li>Altrimenti, l’autorizzazione è negata.</li></ul>
<b>Facoltativo:</b> implementare soluzioni per gestire letture/scritture concorrenti.
<h6>Avvio con ETCS2.</h6> 
In questa versione, le autorizzazioni ad occupare i segmenti MAx sono concesse da un server socket AF_UNIX che chiameremo RBC. RBC gestisce le richieste di accesso ai segmenti ed alle stazioni. Il programma avviato con opzioni “ETCS2 RBC” genera questo server socket, mentre il programma avviato con la sola opzione “ETCS2” genera il PADRE_TRENO ed i PROCESSI_TRENO.
Al suo avvio, RBC richiede e ottiene dal processo REGISTRO tutti i percorsi dei treni. Inoltre, RBC crea e mantiene una struttura dati per memorizzare lo stato dei segmenti MAx e delle stazioni, secondo le seguenti regole:
<ul><li>Lo stato di un segmento MAx può essere libero (impostato a 0) o occupato (impostato ad 1). Ovviamente, in fase di inizializzazione tutti i segmenti sono liberi.</li>
  <li>Lo stato di una stazione è il numero di treni collocati in tale stazione. </li></ul>
Un PROCESSO_TRENO deve richiedere ad RCB l’autorizzazione di accedere ad un segmento MAx o ad una stazione. Quando riceve tali richieste, RBC risponde al PROCESSO_TRENO nel seguente modo:
<ul><li>l’autorizzazione per la richiesta ad accedere ad una stazione è sempre concessa.</li>
<li>Se invece PROCESSO_TRENO richiede di accedere al segmento MAx, il server socket RBC controlla lo stato del segmento, e concede l’autorizzazione soltanto se lo stato è libero. Lo stato del segmento è quindi impostato ad occupato.</li></ul>
RBC si occupa anche di liberare lo stato dei segmenti: in altre parole, quando un PROCESSO_TRENO lascia un segmento, comunica l’evento ad RBC che conseguentemente cambia lo stato del segmento in libero.
<br><b>Nota</b>: i PROCESSI_TRENI continuano comunque a scrivere i valori nei file MAx, che infatti non sono utilizzati da RBC.
<br><b>Facoltativo</b>: in cas o di informazione discordante tra RBC e boe (ovvero, le informazioni dell’RBC non corrispondono con i requisiti delle boe), il TRENO rimane fermo.
<h5>Data logging</h5>
Ciascun PROCESSO_TRENO riempie un file di log, per un totale di cinque file di log di nome T1.log, T2.log, T3.log, T4.log, T5.log. Nei file di log, ciascun PROCESSO_TRENO registra informazioni sulla missione, cioè vi riporta continuamente il settore MAx in uso (oppure la stazione di partenza), il prossimo settore (oppure la stazione di destinazione), la data. 
A titolo esclusivamente di esempio, si mostra un possibile file di log per il PROCESSO_TRENO corrispondente a T3 (quindi, T3.log):
<ul><li>[Attuale: S4], [Next: MA14], 27 Aprile 2018 16:14:10</li>
  <li>[Attuale: MA14], [Next: MA15], 27 Aprile 2018 16:14:13</li>
  <li>[Attuale: MA15], [Next: MA16], 27 Aprile 2018 16:14:16</li>
  <li>[Attuale: MA16], [Next: MA12], 27 Aprile 2018 16:14:19</li>
  <li>[Attuale: MA12], [Next: S8], 27 Aprile 2018 16:14:22</li>
  <li>[Attuale: S8], [Next: --], 27 Aprile 2018 16:14:25</li></ul>
RBC invece scrive un file RBC.log, che contiene le autorizzazioni concesse e negate, i destinatari, e la data. 
A titolo esclusivamente di esempio, un possibile estratto del file di log RBC.log potrebbe essere:
<ul><li>[TRENO RICHIEDENTE AUTORIZZAZIONE: T4], [SEGMENTO ATTUALE: S6], [SEGMENTO RICHIESTO: MA8], [AUTORIZZATO: SI], [DATA: 27 Aprile 2018 16:15:22]</li>
<li>[TRENO RICHIEDENTE AUTORIZZAZIONE: T4], [SEGMENTO ATTUALE: MA8], [SEGMENTO RICHIESTO: MA3], [AUTORIZZATO: NO], [DATA: 27 Aprile 2018 16:15:25]</li>
<li>[TRENO RICHIEDENTE AUTORIZZAZIONE: T4], [SEGMENTO ATTUALE: MA8], [SEGMENTO RICHIESTO: MA3], [AUTORIZZATO: SI], [DATA: 27 Aprile 2018 16:15:28]</li></ul>
<h4>Regole per la presentazione del progetto.</h4>
Il progetto assegnato è valido fino agli appelli di Febbraio 2023 compresi.
Il progetto deve essere svolto in gruppi di massimo 3 persone.
E' necessario consegnare:
<ul><li>Il codice sviluppato inclusi tutti i file necessari alla sua compilazione.</li>
<li>Una relazione sul progetto. Ogni gruppo di lavoro dovrà produrre una sola relazione, i cui autori saranno quindi tutti i membri del gruppo stesso. La relazione dovrà essere in formato pdf. La relazione dovrà essere massimo di 8 pagine. Si consiglia di dare evidenza alle scelte progettuali relativi all’architettura complessiva del sistema, ed agli elementi facoltativi implementati.</li>
<li>Un archivio .zip oppure .tar.gz contenente il codice e la relazione dovrà essere caricato sul sito del corso, seguendo l'apposito link che verrà reso disponibile per ciascun appello.</li>
  <li>Non si accettano consegne in formati differenti da quelli sopra indicati</li></ul>
<h4>Contenuto della relazione.</h4> 
La relazione dovrà contenere:
<ul><li>Informazioni su tutti gli autori e data di consegna</li>
  <ul><li>Nome, Cognome, Numero di matricola, Indirizzo e-mail</li>
    <li>Data di consegna</li></ul>
  <li>Istruzioni dettagliate per compilazione ed esecuzione</li>
  <li>Sistema obiettivo: caratteristiche SW e HW, ad esempio la distribuzione Linux utilizzata</li>
<li>Indicazione degli elementi facoltativi realizzati, riempiendo opportunamente la seguente tabella:</li>
  <table><tr><th>Elemento Facoltativo</th><th> Realizzato(SI/NO)</th> <th>Descrizione e metodo o file principale</th></tr>
    <tr><td>implementare soluzioni per gestire letture/scritture concorrenti.</td><td></td><td></td></tr>
    <tr><td>in caso di informazione discordante tra RBC e boe, il TRENO rimane fermo</td><td></td><td></td></tr></table>
<lI>Progettazione ed implementazione: presentazione ad alto livello della soluzione adottata, e principali caratteristiche o scelte progettuali</li></ul>
