#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define MaxChar 20
#define MaxProd 5 //Massimo numero di elementi nel singolo ordine
#define NumProd 6 //Numero di elementi nell'inventario
#define dizionario "./listaProdotti.txt"
#define frasiIntro "./frasiIntro.txt"
#define Max_Line_lenght 50
/*SEZIONE IDEE
 * FATTO: Quando si preme i nella risposta viene visualizzato l'inventario, poi di nuovo il prompt in cui si deve inserire la domanda (senza rigenerare l'ordine)
 * Premere invio per iniziare
 * Implementare il resto casuale dal cliente
 * */

typedef struct {
    char nomeProdotto[MaxChar];
    float prezzo;
} prodotto;

FILE *fp_read; FILE *fp_write; FILE *fp_frasiRead; FILE *fp_frasiWrite;

int generaOrdine();
float scegliProdotti(int n, prodotto listaProdotti[NumProd], int count);
int acquisisciLista(FILE *fp_read, prodotto **listaProdotti);
int verifica(float totale, prodotto listaProdotti[NumProd], int count);
void stampaInventario(prodotto listaProdotti[NumProd], int count);
int prendiFrasiIntro(FILE *fp_frasiRead, char ***listaFrasi);
void clearScreen();
void parlaIntro(char **listaFrasi, int quanteFrasi);

int main(void) {

    int n, risposta = 0, quanteFrasi;
    float totale;
    char temp;
    prodotto *listaProdotti = NULL;
    char **listaFrasi = NULL;

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
            //Se non c'è nessun file lo creo e ci stampo queste cose base esempio
            fprintf(fp_frasiWrite, "Ahh, che bella domenica pomeriggio! Non vedevo l'ora di raccogliere margheritine\n e di venire al cinema per guardare 'Mission Impossible: spesa alla Conad con meno di 20 euro'. Ad ogni modo, vorrei:\n");
            fprintf(fp_frasiWrite, "Finalmente riesco a vedere 'L'uomo che fissava lo scaffale dei biscotti per 40 minuti'. Mi serve però prima:\n");
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

    fclose(fp_read);
    //Chiudo il file
    //Reinizializzare se no non genera numeri casuali a quanto pare
    srand(time(NULL));

    printf("\nCiau, qui ti spiego le regole: \n");
    printf("- Per fermare il giuoco scrivi -1\n- Per vedere l'inventario digita 'i'\n\nInserisci questi valori quando ti chiede quanto fa\n");
    printf("Be fast af\n\n");
    printf("\n");
    quanteFrasi = prendiFrasiIntro(fp_frasiRead, &listaFrasi);
    fclose(fp_frasiRead);
    printf("Premi invio per iniziare: ");
    scanf("%c", &temp);

    while (risposta != -1){

        //Qui chiamo la funzione che stampa a caso una delle frasi
        parlaIntro(listaFrasi, quanteFrasi);

        //N è il numero di elementi dell'ordine da 1 a MAXPROD
        n = generaOrdine();
        //printf("Ordine generato: \n"); questa parte con le storie non serve più
        totale = scegliProdotti(n, listaProdotti, count);
        risposta = verifica(totale, listaProdotti, count);
        printf("\n");
    }

    free(listaProdotti); // libera la memoria allocata
    free(listaFrasi);

    return 0;
}

void parlaIntro(char **listaFrasi, int quanteFrasi){
    int index = rand() % quanteFrasi;
    printf("%s", listaFrasi[index]);
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
        printf("#%d: %s \n", i+1, listaProdotti[prod].nomeProdotto);
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

int prendiFrasiIntro(FILE *fp_frasiRead, char ***listaFrasi){
    //Dichiaro stringa in cui metto la frase
    char line[500];
    int count = 0;

    //Inizio a leggere, ma prima conto quante righe sono:
    while (fgets(line, sizeof(line), fp_frasiRead)) count++;
    rewind(fp_frasiRead);
    //Allocazione memoria
    *listaFrasi = malloc(count * sizeof(char*));
    //Riporto indietro il puntatore
    count = 0;
    if (*listaFrasi == NULL) {
        printf("Errore nell'allocazione della memoria.\n");
        return 0;
    }
    while (fgets(line, sizeof(line), fp_frasiRead)) {
        (*listaFrasi)[count] = malloc(strlen(line) + 1);
        if ((*listaFrasi)[count] != NULL) strcpy((*listaFrasi)[count], line);
        count++;
    }
    return count;
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
        if (strncmp(line, "//fine", 5) == 0) {
            break;
        }
        if(inizioDizionario){
            count +=1;
        }

    }
    *listaProdotti = malloc(count * sizeof(prodotto));
    if (*listaProdotti == NULL) {
        printf("Errore nell'allocazione della memoria.\n");
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
    return count;
}


int verifica(float totale, prodotto listaProdotti[NumProd], int count){//aggiungere anche i valori per la funzione stampa inv
    //Char risposta perché adesso può essere sia un numero che "i"
    char risposta[10];
    time_t start, end;
    float rispostaF;
    double elapsedTime;
    start = time(NULL);
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
    if(totale==rispostaF){
        clearScreen();
        printf("Ottimo! Ci hai messo %d secondi\n", (int)elapsedTime);
        //int opzioniPagamenti[] = {0.5, 1, 2, 5, 10, 20, 50}; // es. banconote
        return 1;
    } else {
        clearScreen();
        printf("Nu :( la risposta era %.2f\n", totale);
        return 1;
    }
}

void stampaInventario(prodotto *listaProdotti, int count){
    printf("Inventario:\n");
    for (int i = 0; i < count; i++) {
        printf("Prodotto: %s, Prezzo: %.2f\n", listaProdotti[i].nomeProdotto, listaProdotti[i].prezzo);
    }
}
