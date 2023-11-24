#include "indices.h"
#include <stdio.h>

//TODO: que se explodam os ponteiros

int main() {
    FILE *indice = fopen("teste.idx", "r+");

    int raiz;
    fscanf(indice, "%d@", &raiz);

    int rnn, index;
    string codigo = malloc(6);

    strcpy(codigo, "NOL15");
    bool found = buscaCodigo(indice, raiz, codigo, &rnn, &index);

    fclose(indice);

    return 0;
}