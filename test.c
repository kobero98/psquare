#include <stdio.h>
#include <stdlib.h>
#include "psqare.h"
#include <time.h>

#define N 10000
#define P 25

// Funzione di confronto per qsort
int compare(const void *a, const void *b) {
    if (*(float*)a < *(float*)b) return -1;
    if (*(float*)a > *(float*)b) return 1;
    return 0;
}

// Funzione per calcolare il quantile di ordine p
float quantile(long long *array, int size, int p) {
    int pos = (int)(((float)(p/100.0)) * (size - 1));
    return array[pos];
}

int main() {

    long long *array = (long long*)malloc(N * sizeof(long long));
    if (array == NULL) {
        fprintf(stderr, "Errore di allocazione della memoria\n");
        return 1;
    }
    psqareEstimator stima;
    initPsqaure(&stima,P);

    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    for (int i = 0; i < N; i++) {
    	long long x = rand() % 1000;
        array[i] = x;
        update(&stima,x);

        qsort(array, i + 1, sizeof(long long), compare); // Ordina l'array
        long long q = quantile(array, i + 1, P); // Calcola il quantile
        printf("Inserito: %lld, Quantile di ordine %d: %lld %lld\n", x, P, q,p_estimate(&stima));
    }

    free(array); // Libera la memoria allocata
    return 0;
}

