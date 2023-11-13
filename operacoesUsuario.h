#ifndef OPERACOESUSUARIO
#define OPERACOESUSUARIO

#include <ctype.h>
#include "filmes.h"
#include "indices.h"
#include "style.h"

#ifndef STRING
#define STRING
typedef char *string;
#endif

//Menu para inserção de um novo filme
void inserirFilme(FILE *movies, IndiceP **indexP, IndiceS **indexS);

//Menu para remoção de um filme
void removerFilme(FILE *movies, IndiceP **indexP, IndiceS **indexS);

//Menu para modificação de nota
void modificarNota(FILE *movies, IndiceP *indexP);

//Menu para busca de filme (com dois submenus)
void buscarFilme(FILE *movies, IndiceP *indexP, IndiceS *indexS);

//Submenu para busca de filme pela titulo primária
int buscarCodigo(FILE *movies, IndiceP *indexP);

//Submenu para busca de filme pela titulo secundária
int buscarTitulo(FILE *movies, IndiceP *indexP, IndiceS *indexS);

//Listagem de todos os filmes
void listarFilmes(FILE *movies);

//Elimina do arquivo os filmes marcados como deletados
FILE *compactar(FILE *movies, IndiceP **indexP);

//Retorna o código de um filme com determinado código
string tituloFromRNN(FILE *movies, int rnn);

//Impressão de um filme na tela; retorna 0 se o filme nesse RNN foi apagado
short imprimirFilme(FILE *movies, int rnn);

//Limpa o buffer de entrada
void clearBuffer();

#endif