#include "indices.h"
#include <stdio.h>

int main() {
    FILE *index = fopen("teste.idx", "r+");
    int raiz = getRoot(index);

    int rnn, i;

    insereCodigo(index, "NOL23", 14); //TODO: verificar escrita dupla após split

    fclose(index);

    return 0;
}