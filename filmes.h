#ifndef FILMES
#define FILMES

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#define TAM_FILME 192

#define TAM_COD 5
#define TAM_TIT_PT 62
#define TAM_TIT_OG 62
#define TAM_DIR 30
#define TAM_PAIS 21

#define SEPARATOR "@"
#define PADDING '#'
#define DELETION "*|"

typedef struct filme {
    char codigo[TAM_COD + 1];       //composto pelas três primeiras letras do sobrenome do diretor e dois últimos dígitos do ano de lançamento
    char tituloPT[TAM_TIT_PT + 1];  //título em português
    char tituloOG[TAM_TIT_OG + 1];  //título original; "Idem" se for o mesmo
    char diretor[TAM_DIR + 1];      //sobrenome e nome do diretor, separados por vírgula
    char ano[5];                    //ano de lançamento
    char pais[TAM_PAIS + 1];        //país em que o filme foi produzido
    char nota;                      //inteiro entre 0 e 9 com a nota dada pelo colecionador
} Filme;

#endif
