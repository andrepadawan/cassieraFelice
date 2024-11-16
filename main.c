#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "math.h"

//Questo lo tengo in caso servano sleep lunghi, ma tuttora è
//stato rimpiazzato da sleep_ms

#ifdef _WIN32
#include <windows.h>  // Windows-specific library for Sleep
#define SLEEP(x) Sleep((x) * 1000)  // Windows Sleep takes milliseconds
#else
#include <unistd.h>  // Unix-specific library for sleep (Linux, macOS)
#define SLEEP(x) sleep(x)  // Unix sleep takes seconds
#endif

#define MaxChar 20
#define MaxProd 5 //Massimo numero di elementi nel singolo ordine
#define NumProd 6 //Numero di elementi nell'inventario
#define dizionario "./listaProdotti.txt"
#define frasiIntro "./frasiIntro.txt"
#define Max_Line_lenght 50

/*SEZIONE IDEE
 * FATTO: Quando si preme i nella risposta viene visualizzato l'inventario, poi di nuovo il prompt in cui si deve inserire la domanda (senza rigenerare l'ordine)
 * FATTO: Premere invio per iniziare
 * Implementare il resto casuale dal cliente
 * FATTO: Animazioni attivabili e disattivabili
 *
 */

typedef struct {
    char nomeProdotto[MaxChar];
    float prezzo;
} prodotto;

FILE *fp_read; FILE *fp_write; FILE *fp_frasiRead; FILE *fp_frasiWrite;

int generaOrdine();
float scegliProdotti(int n, prodotto listaProdotti[NumProd], int count);
int acquisisciLista(FILE *fp_read, prodotto **listaProdotti);
int verifica(float totale, prodotto listaProdotti[NumProd], int count, int quanteFrasiResto, char **listaFrasiResto);
void stampaInventario(prodotto listaProdotti[NumProd], int count);
int prendiFrasiIntro(FILE *fp_frasiRead, char ***listaFrasi);
int prendiFrasiResto(FILE *fp_frasiRead, char ***listaResto);
int caricaOpzioni(FILE *fp_read);
void clearScreen();
void parla(char **listaFrasi, int quanteFrasi);
void stampaTestoAnimato(char *line);
void sleep_ms(int milliseconds);
void restoFunc(float totale, int quanteFrasiResto, char **listaFrasiResto);
float leggiInputResto();

//Variabili globali:
int animazioni = 0; //Se 0 disattiva le animazioni
int resto = 0; //Se attivo ti chiede di fare la differenza con quello che ti dà il cliente
int mandatoryTicket = 0; //Obbligatorio che ci sia almeno un biglietto per ordine
int maxNumberOrder = 0; // Numero massimo di elementi che possono essere generati in un ordine
int storia = 0;
int giorno = 1;

int main(void) {
    int n, risposta = 0, quanteFrasiIntro, quanteFrasiResto;
    float totale;
    char temp;
    prodotto *listaProdotti = NULL;
    char **listaFrasi = NULL;
    char **listaFrasiResto = NULL;

    fp_read = fopen(dizionario, "r");
    fp_frasiRead = fopen(frasiIntro, "r");

    if(fp_read == NULL){       //se il file non c'è si crea

        fp_write = fopen(dizionario, "w");

        if (!fp_write) {printf("Errore durante la creazione del file\n");}
        else{

            //Se non c'è nessun file lo creo e ci stampo queste cose base esempio
            fprintf(fp_write, "Qui sono tutti i prodotti che sono in vendita,\n");
            fprintf(fp_write, "ne puoi aggiungere quanti vuoi ma l'importante è che sia uno per riga.\n");
            fprintf(fp_write, "\n");
            fprintf(fp_write, "//Inizio dizionario\n");
            fprintf(fp_write, "Intero:8\n");
            fprintf(fp_write, "Ridotto:6\n");
            fprintf(fp_write, "Coca-cola:3.5\n");
            fprintf(fp_write, "Birra media:3.5\n");
            fprintf(fp_write, "Goleador:0.2\n");
            fprintf(fp_write, "Birra grande:4.5\n");
            fprintf(fp_write, "//fine\n");
            fclose(fp_write);
        }
    } else {
        printf("\nFile caricato correttamente");
    }
    if(fp_frasiRead == NULL){       //se il file delle frasi non c'è si crea

        fp_frasiWrite = fopen(frasiIntro, "w");

        if (!fp_frasiWrite) {printf("Errore durante la creazione del file frasi\n");}
        else{
            fprintf(fp_frasiWrite, "//Inizio frasi intro");
            //Se non c'è nessun file lo creo e ci stampo queste cose base esempio
            fprintf(fp_frasiWrite, "Ahh, che bella domenica pomeriggio! Non vedevo l'ora di raccogliere margheritine "
                                   "e di venire al cinema per guardare 'Mission Impossible: spesa alla Conad con meno di 20 euro'. "
                                   "Ad ogni modo, vorrei:\n");
            fprintf(fp_frasiWrite, "Finalmente riesco a vedere 'L'uomo che fissava lo scaffale dei biscotti per 40 minuti'. "
                                   "Mi serve però prima:\n");
            fprintf(fp_frasiWrite, "//fine frasi intro");
            fprintf(fp_frasiWrite, "//inizio frasi resto");
            fprintf(fp_frasiWrite, "//fine frasi resto");
            fclose(fp_frasiWrite);
        }
    } else {
        printf("\nFile frasi caricato correttamente\n\n");
    }

    //Carico dizionario dal file seguendo lo schema nel readme
    int count = acquisisciLista(fp_read, &listaProdotti);

    //Cioé se almeno un elemento è nell'inventario lo stampo
    if (count > 0) {
        stampaInventario(listaProdotti, count);
        }

    caricaOpzioni(fp_read);

    fclose(fp_read);
    //Chiudo il file

    //Reinizializzare se no non genera numeri casuali a quanto pare
    srand(time(NULL));

    printf("\nCiau, qui ti spiego le regole: \n");
    printf("- Per fermare il giuoco scrivi -1\n- Per vedere l'inventario digita 'i'\n\nInserisci questi valori quando ti chiede quanto fa\n");
    printf("Be fast af\n\n");
    printf("\n");
    quanteFrasiIntro = prendiFrasiIntro(fp_frasiRead, &listaFrasi);
    quanteFrasiResto = prendiFrasiResto(fp_frasiRead, &listaFrasiResto);
    //printf("%d ", quanteFrasiResto);
    // printf("%d ", quanteFrasiIntro);
    fclose(fp_frasiRead);
    printf("Premi invio per iniziare: ");
    scanf("%c", &temp);
    clearScreen();

    while (risposta != -1){

        //Qui chiamo la funzione che stampa a caso una delle frasi
        parla(listaFrasi, quanteFrasiIntro);
        if(animazioni) {
            sleep_ms(200);
            fflush(stdout);
        }
        fflush(stdout);
        //N è il numero di elementi dell'ordine da 1 a MAXPROD
        n = generaOrdine();
        printf("\n");
        //printf("Ordine generato: \n"); questa parte con le storie non serve più
        totale = scegliProdotti(n, listaProdotti, count);
        risposta = verifica(totale, listaProdotti, count, quanteFrasiResto, listaFrasiResto);
    }

    free(listaProdotti); // libera la memoria allocata
    free(listaFrasi); //libera la lista delle frasi
    free(listaFrasiResto);
    return 0;
}

void stampaTestoAnimato(char *line){
    int lenght = strlen(line);
    int aCapo = 0;
    //questa funzione stampa come se fosse scritto a macchina
    for(int i = 0; i < lenght; i++){
        printf("%c", line[i]);
        aCapo +=1;
        if ((aCapo>60)&&line[i]==' '){printf("\n"); aCapo = 0;}     //vado a capo senza troncare dopo 50 caratteri
        fflush(stdout);
        sleep_ms(10);
    }
    printf("\n");
}

void sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds); // Windows: Sleep accetta millisecondi
#else
    usleep(milliseconds * 1000); // Linux/Mac: usleep accetta microsecondi
#endif
}

void parla(char **listaFrasi, int quanteFrasi){
    int index = rand() % quanteFrasi;
    //printf("\nIndice: %d, var:%d\n", index, quanteFrasi);
    if(animazioni){
        //funzione che stampa con animazioni
        fflush(stdout);
        stampaTestoAnimato(listaFrasi[index]);

    }
    else{
        //se animazioni sono disattivate stampa normalmente
        printf("%s", listaFrasi[index]);

    }
    fflush(stdout);
}

int generaOrdine(){
    int n;
    n = rand() % MaxProd+1;
    return n;
}

float scegliProdotti(int n, prodotto listaProdotti[NumProd], int count){
    float totale = 0;
    for(int i = 0; i<n; i++){
        int prod = rand() % (count);
        //mi genera un numero che sta a indicare uno dei prodotti della lista
        if(animazioni){
            printf("#%d: ", i+1);
            stampaTestoAnimato(listaProdotti[prod].nomeProdotto);
        }
        else {
            printf("#%d: %s \n", i + 1, listaProdotti[prod].nomeProdotto);
        }
        totale += listaProdotti[prod].prezzo;
    }

    printf("\n");
    return totale;
}

void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");  // Windows
#elif defined(__APPLE__) || defined(__MACH__)
    system("clear"); // Mac OS
#elif defined(__linux__)
    system("clear"); // Linux
    #else
        printf("Sistema operativo non supportato.\n");
#endif
}

int prendiFrasiResto(FILE *fp_frasiRead, char ***listaFrasiResto) {
    char line[500];
    int count = 0;
    int frasiRestoTemp = 0;
    while (fgets(line, sizeof(line), fp_frasiRead)) {
        if (strncmp(line, "//fine frasi resto", 18) == 0) break;
        if (strncmp(line, "//Inizio frasi resto", 19) == 0) {
            frasiRestoTemp = 1;
            continue;
        }
        if (frasiRestoTemp) {
            count++;
        }
    }
        //Allocazione memoria
        //
        rewind(fp_frasiRead);
        *listaFrasiResto = malloc(count * sizeof(char *));

        if (*listaFrasiResto == NULL) {
            printf("Errore nell'allocazione della memoria frasi resto.\n");
            return 0;
        }

        count = 0;

        while (fgets(line, sizeof(line), fp_frasiRead)) {
            if (strncmp(line, "//fine frasi resto", 18) == 0) break;
            if (strncmp(line, "//inizio frasi resto", 19) == 0) {
                frasiRestoTemp = 1;
                continue;
            }
            if (frasiRestoTemp) {
                line[strcspn(line, "\n")] = '\0';//Eliminare \n

                (*listaFrasiResto)[count] = malloc(strlen(line) + 1);
                if ((*listaFrasiResto)[count] != NULL) strcpy((*listaFrasiResto)[count], line);
                count++;
            }
        };
        return count;
}

int prendiFrasiIntro(FILE *fp_frasiRead, char ***listaFrasi){
    //Dichiaro stringa in cui metto la frase
    char line[500];
    int count = 0;
    int frasiIntroTemp = 0;
    //Allocazione memoria

    //Inizio a leggere, ma prima conto quante righe sono:
    while (fgets(line, sizeof(line), fp_frasiRead)) {
        if(strncmp(line, "//fine frasi intro", 18) == 0) break;
        if (strncmp(line, "//Inizio frasi intro", 19) == 0) {
            frasiIntroTemp = 1;
            continue;
        }
        if(frasiIntroTemp){
            count++;
        }
    }
    rewind(fp_frasiRead);
    *listaFrasi = malloc(count * sizeof(char*));

    if (*listaFrasi == NULL) {
        printf("Errore nell'allocazione della memoria intro.\n");
        return 0;
    }

    count = 0;
    while (fgets(line, sizeof(line), fp_frasiRead)) {
        if(strncmp(line, "//fine frasi intro", 16) == 0) break;
        if (strncmp(line, "//Inizio frasi intro", 19) == 0) {
            frasiIntroTemp = 1;
            continue;
        }
        if(frasiIntroTemp){
            line[strcspn(line, "\n")] = '\0';//Eliminare \n
        }
        (*listaFrasi)[count] = malloc(strlen(line) + 1);
        if ((*listaFrasi)[count] != NULL) strcpy((*listaFrasi)[count], line);
        count++;
    }
    rewind(fp_frasiRead);
    return count;
}

int caricaOpzioni(FILE *fp_read) {
    int opzioniTemp = 0;
    char line[100];
    //Da qui in poi sono nella sezione opzioni del file
    while (fgets(line, sizeof(line), fp_read)) {
        if(strncmp(line, "//fine opzioni", 14) == 0) break;
        if (strncmp(line, "//Inizio opzioni", 12) == 0) {
            opzioniTemp = 1;
            continue;
        }
        if(opzioniTemp){
            line[strcspn(line, "\n")] = '\0';//Eliminare \n
            if (strncmp(line, "-Almeno un biglietto per ordine:", 30) == 0) {
                sscanf(line, "-Almeno un biglietto per ordine:%d", &mandatoryTicket);
            } else if (strncmp(line, "-Numero massimo di item per ogni ordine:", 40) == 0) {
                sscanf(line, "-Numero massimo di item per ogni ordine:%d", &maxNumberOrder);
            } else if (strncmp(line, "-Animazioni:", 12) == 0) {
                sscanf(line, "-Animazioni:%d", &animazioni);
            } else if (strncmp(line, "-Resto:", 7) == 0) {
                sscanf(line, "-Resto:%d", &resto);
            } else if(strncmp(line, "-Storia", 7) == 0) {
                sscanf(line, "-Storia:%d", &storia );
            }
        }
    }

    rewind(fp_read);
    return 0;
}

int acquisisciLista(FILE *fp_read, prodotto **listaProdotti){
    int inizioDizionario = 0, count = 0;
    char line[Max_Line_lenght];

    //inizio a leggere, ma prima devo contare le righe
    while (fgets(line, sizeof(line), fp_read)){//Finché non legge //fine nel file
        if(strncmp(line, "//Inizio dizionario", 18) == 0) {
            inizioDizionario = 1;
            continue;
        }
        if (strncmp(line, "//fine dizionario", 17) == 0) {
            break;
        }
        if(inizioDizionario){
            count +=1;
        }

    }
    *listaProdotti = malloc(count * sizeof(prodotto));
    if (*listaProdotti == NULL) {
        printf("Errore nell'allocazione della memoria acquisisci lista.\n");
        return 0;
    }
    //porto indietro il puntatore
    rewind(fp_read);
    //reinizializzo le variabili
    inizioDizionario = 0, count = 0;

    while (fgets(line, sizeof(line), fp_read)){//Finché non legge //fine nel file
        if(strncmp(line, "//Inizio dizionario", 18)== 0) {
            inizioDizionario = 1;
            continue;
        }
        if (strncmp(line, "//fine", 6) == 0) {
            break;
        }
        if(inizioDizionario){
            sscanf(line, "%[^:]:%f", (*listaProdotti)[count].nomeProdotto, &(*listaProdotti)[count].prezzo);
            count += 1;
        }
    }
    rewind(fp_frasiRead);
    return count;
}

void restoFunc(float totale, int quanteFrasiResto, char **listaFrasiResto) {
    //Qui decido che il cliente dà una banconota di un certo valore
    float valute[9] = {0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50};
    int numValute = (sizeof(valute))/sizeof(valute[0]);
    float soldiCliente = 0, rispostaF = 0, restoDaDare = 0;
    int index = 0;
    char risposta[8];
    parla(listaFrasiResto, quanteFrasiResto); //Sta funzione dà problemi
    char *frase = "Ecco a te:  ";
    if(animazioni){
        stampaTestoAnimato(frase);
    } else {
        printf("%s", frase);
    }
    int count = 0;
    srand(time(NULL));
    //Una soluzione per evitare resti giganteschi può essere quella di generare due o più resti casuali diversi
    //Per poi scegliere quello con il minor numero di monete

    while(soldiCliente < totale){
        index = rand() % numValute;
        if (count >= 1){ //cioé è stata estratta almeno una banconota (un qualcuno può pagare 50 euro per poche cose)
            // Se il cliente ha già una buona somma, evita di aggiungere valori troppo grandi che rendono il resto poco improbabile
            if (soldiCliente + valute[index] > totale && valute[index] > 10) {
                continue;  // Salta questa iterazione se il valore è troppo grande rispetto al totale mancante
            }
        }
        soldiCliente += valute[index];
        if(animazioni){
            printf("%.2f  ", valute[index]);
            fflush(stdout);
            sleep_ms(150);
        }
        else {
            printf("%.2f  ", valute[index]);
        }
        count += 1;
        if (soldiCliente >= totale) {
            break;
        }
    }
    printf("\n");

    //calcolo resto
    restoDaDare = soldiCliente - totale;

    if(animazioni){
        char frase[] = "Quant'è il resto? ";
        stampaTestoAnimato(frase);
        fgets(risposta, sizeof(risposta), stdin);
        rispostaF = strtof(risposta, NULL);
    } else {
        printf("Quant'è il resto? ");
        fgets(risposta, sizeof(risposta), stdin);
        rispostaF = strtof(risposta, NULL);
    }
    restoDaDare = round(restoDaDare* 100) / 100;

    if(rispostaF==restoDaDare){
        //Giusto
        printf("\033[32m");//Set printf to green
        printf("Ottimo! Ci hai preso\n");
        printf("\033[0m");
        if(animazioni){
            sleep_ms(800);
        }
        } else {
        //sbagliato
        printf("\033[31m");//Set printf to red
        printf("Nu :( la risposta era %.2f\n", restoDaDare);
        fflush(stdout);
        printf("\033[0m");
        if(animazioni){
            sleep_ms(800);
        }
    }
}

int verifica(float totale, prodotto listaProdotti[NumProd], int count, int quanteFrasiResto, char **listaFrasiResto){//aggiungere anche i valori per la funzione stampa inv
    //Char risposta perché adesso può essere sia un numero che "i"
    char risposta[10];
    time_t start, end;
    float rispostaF;
    double elapsedTime;
    start = time(NULL);
    fflush(stdout);
    while (1){
        printf("Quanto fa? ");
        fgets(risposta, sizeof(risposta), stdin);
        // Rimuove il newline in fondo alla stringa
        risposta[strcspn(risposta, "\n")] = 0;

        if (strcmp(risposta, "i") == 0) {
            stampaInventario(listaProdotti, count);
            continue; // Torna a chiedere la risposta dopo aver mostrato l'inventario
        }

        // Tento di convertire la risposta in float
        rispostaF = strtof(risposta, NULL);

        if (rispostaF == 0 && risposta[0] != '0') {
            printf("Sbagliato l'input. Riprova.\n");
        } else if (rispostaF == -1){
            return -1;
        }
        else {
            break; // Esco dal loop se la risposta è un numero valido
        }
    }
    end = time(NULL);
    elapsedTime = difftime(end, start);
    //caso in cui risposta è un carattere
    //Arrotondiamo se no si incazza nelle risposte
    totale = roundf(totale*100)/100;

    if(totale==rispostaF){
        printf("\033[32m");//Set printf to green
        printf("Ottimo! Ci hai messo %d secondi\n\n", (int)elapsedTime);
        printf("\033[0m");//set printf color to default
        if(animazioni){
            sleep_ms(800);
        }
        //Int opzioniPagamenti[] = {0.5, 1, 2, 5, 10, 20, 50}; // es. banconote
        if(resto){
            srand(time(NULL));
            int prob = rand() % 100 < 40;
            //printf("%d", prob);
            //la modalità resto è attiva, ha senso proseguire qui solo se la risposta è giusta;
            if(prob){
                restoFunc(totale, quanteFrasiResto, listaFrasiResto);
                clearScreen();
                return 1;
            }
            clearScreen();
        }
        else {
            return 1;
        }
    } else {
        clearScreen();
        printf("\033[31m");//Set printf to red
        printf("Nu :( la risposta era %.2f\n", totale);
        fflush(stdout);
        printf("\033[0m");
        printf("\n");
        if(animazioni){
            sleep_ms(700);
        }
        return 1;
    }
    return 1;
}

void stampaInventario(prodotto *listaProdotti, int count){
    printf("Inventario:\n");
    for (int i = 0; i < count; i++) {
        printf("Prodotto: %s, Prezzo: %.2f\n", listaProdotti[i].nomeProdotto, listaProdotti[i].prezzo);
    }
}
