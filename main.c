#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MaxChar 20
#define MaxProd 5 //Massimo numero di elementi nel singolo ordine
#define NumProd 6 //Numero di elementi nell'inventario


typedef struct {
    char nomeProdotto[MaxChar];
    float prezzo;
} prodotto;

int generaOrdine();
float scegliProdotti(int n, prodotto listaProdotti[NumProd]);
void acquisisciLista(prodotto listaProdotti[NumProd]);
int verifica(float totale);

int main(void) {
    srand(time(NULL));
    int n, risposta = 0;
    float totale;
    prodotto listaProdotti[NumProd];
    acquisisciLista(listaProdotti);
    printf("Ciau, qui ti spiego le regole: \n");
    printf("Per fermare il giuoco scrivi -1\n");
    printf("\n");
    while (risposta != -1){
        n = generaOrdine();
        printf("Ordine generato: \n");
        totale = scegliProdotti(n, listaProdotti);
        risposta = verifica(totale);
        printf("\n");
    }
    return 0;
}

int generaOrdine(){
    int n;
    n = rand() % MaxProd+1;
    return n;
}

float scegliProdotti(int n, prodotto listaProdotti[NumProd]){
    float totale = 0;
    for(int i = 0; i<n; i++){
        int prod = rand() % (NumProd -1);
        //mi genera un numero che sta ad indicare uno dei prodotti della lista
        printf("#%d: %s \n", i+1, listaProdotti[prod].nomeProdotto);
        totale += listaProdotti[prod].prezzo;
    }
    //printf("Totale: %f", totale);
    printf("\n");
    return totale;
}

void acquisisciLista(prodotto listaProdotti[NumProd]){
    prodotto temp[NumProd] = {
            {"Intero", 8},
            {"Ridotto", 6},
            {"Coca-cola", 3.5},
            {"Goleador", 0.2},
            {"Chinotto", 3.5},
            {"Birra", 4.5}
    };
    for(int i = 0; i < MaxProd; i++) {
        listaProdotti[i] = temp[i];
    }
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
    } else {
        printf("Nu :( la risposta era %.2f\n", totale);
        return risposta;
    }
}
