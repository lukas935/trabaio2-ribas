#include "indices.h"
#include <stdio.h>

int main(){
    FILE *indice = fopen("teste.idx", "r+");

    NoP *pagina = lerPagina(indice, 0);
    escreverPagina(indice, pagina);

    freePagina(pagina);
    fclose(indice);

    return 0;
}