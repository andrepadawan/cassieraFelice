#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MaxChar 20
#define MaxProd 5 //Massimo numero di elementi nel singolo ordine
#define NumProd 6 //Numero di elementi nell'inventario
#define dizionario "../listaProdotti.txt"
#define Max_Line_lenght 50

typedef struct {
    char nomeProdotto[MaxChar];
    float prezzo;
} prodotto;
FILE *fp_read; FILE *fp_write;

int generaOrdine();
float scegliProdotti(int n, prodotto listaProdotti[NumProd], int count);
int acquisisciLista(FILE *fp_read, prodotto **listaProdotti);
int verifica(float totale);

int main(void) {
    int n, risposta = 0;
    float totale;
    fp_read = fopen(dizionario, "r");
    if(fp_read == NULL){//se il file non c'è si crea
        fp_write = fopen(dizionario, "w");
        if (!fp_write) printf("Errore durante la creazione del file\n");
    } else {
        printf("\nFile caricato correttamente\n\n");
    }

    //Carico dizionario dal file seguendo lo schema nel readme
    prodotto *listaProdotti = NULL;
    int count = acquisisciLista(fp_read, &listaProdotti);
    if (count > 0) {
        printf("Prodotti letti:\n");
        for (int i = 0; i < count; i++) {
            printf("Prodotto: %s, Prezzo: %.2f\n", listaProdotti[i].nomeProdotto, listaProdotti[i].prezzo);
        }
    }
    fclose(fp_read);
    //Reinizializzare se no non genera numeri casuali a quanto pare
    srand(time(NULL));

    printf("\nCiau, qui ti spiego le regole: \n");
    printf("Per fermare il giuoco scrivi -1 quando ti chiede quanto fa\n");
    printf("Be fast af\n\n");
    printf("\n");

    while (risposta != -1){
        n = generaOrdine();//N è il numero di elementi dell'ordine da 1 a MAXPROD
        printf("Ordine generato: \n");
        totale = scegliProdotti(n, listaProdotti, count);
        risposta = verifica(totale);
        printf("\n");
    }
    free(listaProdotti); // libera la memoria allocata
    return 0;
}

int generaOrdine(){
    int n;
    n = rand() % MaxProd+1;
    return n;
}

float scegliProdotti(int n, prodotto listaProdotti[NumProd], int count){
    float totale = 0;
    for(int i = 0; i<n; i++){
        int prod = rand() % (count -1);
        //mi genera un numero che sta ad indicare uno dei prodotti della lista
        printf("#%d: %s \n", i+1, listaProdotti[prod].nomeProdotto);
        totale += listaProdotti[prod].prezzo;
    }
    //printf("Totale: %f", totale);
    printf("\n");
    return totale;
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


int verifica(float totale){
    float risposta;
    time_t start, end;
    double elapsedTime;
    start = time(NULL);
    printf("Quanto fa? ");
    fscanf(stdin, "%f", &risposta);
    end = time(NULL);
    elapsedTime = difftime(end, start);
    for(int j = 0; j<10;j++){
        printf("\n");
    }
    if(totale==risposta){
        printf("Ottimo! Ci hai messo %d secondi\n", (int)elapsedTime);
        return risposta;
    } else if (risposta == -1){
        printf("Alla prossima!\n");
        return risposta;
    } else {
        printf("Nu :( la risposta era %.2f\n", totale);
        return risposta;
    }
}
