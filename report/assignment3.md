# Assignment 3 Report

## Task 1:

Per creare le due FSM per client e server, abbiamo analizzato l'assignment producendo due flow charts. Il primo, quello del client, é il seguente:

![Client.jpg](:storage/7d606228-d03f-41a0-a5b8-c07bed686523/e6b33609.jpg)

Notiamo come, nel caso si abbiano delle interazioni, il messaggio del client é sempre quello sopra la _linea_ mentre quello del server é al di sotto.

Il secondo schema é invece il seguente:

![Server.jpg](:storage/7d606228-d03f-41a0-a5b8-c07bed686523/b2c3b1e2.jpg)

Anche in questo caso il messaggio del client é sempre quello sopra la _linea_ mentre quello del server é al di sotto.

## Task 2:


La soluzione adottata prevede la realizzazione di due componenti principali: client e server.

Il primo prevede di ricevere in input, come argomenti, indirizzo IP e porta del server. 
Come prima cosa esso prova ad aprire una connessione TCP con il server e dopodichè, se la connessione ha successo, procede a richiedere da `stdin` i vari parametri della misurazione (operazione, dimensioni, numero di misurazioni per ogni dimensione, ritardo del server).
Una volta richiesti i parametri si avvia la vera e propria fase di misurazione, dove il comportamento è quello descritto nel diagramma a stati finiti presente nel Task 1.

Il server, invece, prende in input solamente la porta su cui dovrà fare il `bind` in quanto come indirizzo utilizza `INADDR_ANY`. Il comportamento del server è descritto nel diagramma a stati finiti presente nel Task 1.

Per semplificare la lettura, la comprensione e la manutenzione del codice si è deciso di dividere la logica in più file e di accorpare alcune funzioni utili sia a client che a server in un file `utilities` presente nella cartella `src/common/`.
I file relativi al client sono collocati nella cartella `src/client/`, mentre quelli relativi al server sono posti nella cartella `src/server`.

Per compilare i sorgenti e generare i file eseguibili è presente un `Makefile`. È sufficiente eseguire il comando `make` nella home directory del progetto (fuori da `src`) e verranno creati i file `server.exe` e `client.exe`.

Infine sono presenti numerosi commenti che aiutano a comprendere lo scopo delle varie funzioni. Essi sono all'interno degli header `.h` dove presenti, altrimenti nei file `.c`.

---
Per calcolare `RTT` e `Throughut` sono state utilizzate le linee guida presenti nella consegna, perciò:
- l'`average RTT` viene calcolato come media di tutti gli `RTT` registrati 
- l'`average Througput` viene calcolato come media di tutti i `Throughput` registrati, uno per ogni dimensione del messaggio. Ogni `Throughput` viene calcolato come `dimensione del messaggio / average RTT misurato per la dimensione`.
