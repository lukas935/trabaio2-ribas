#include "indices.h"
#include <stdio.h>

int main() {
    FILE *index = fopen("teste.idx", "r+");
    int raiz = getRoot(index);

    int rnn, i;

    //TODO: integrar inserção com o front


    fclose(index);

    return 0;
}