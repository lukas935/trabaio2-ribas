#include "indices.h"
#include <stdio.h>

int main() {
    FILE *index = fopen("teste.idx", "r+");
    int raiz = getRoot(index);

    int rnn, i;

    //TODO: terminar de corrigir coisinhas
    //TODO: fazer emoção :(


    fclose(index);

    return 0;
}