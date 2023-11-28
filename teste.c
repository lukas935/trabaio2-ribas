#include "indices.h"
#include <stdio.h>

int main() {
    FILE *index = fopen("teste.idx", "r+");
    int raiz = getRoot(index);

    int i;

    buscaCodigo(index, getRoot(index), "NOL14", &i);


    fclose(index);

    return 0;
}