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
void inserirFilme(FILE *movies, FILE *indexP, IndiceS **indexS);

//Menu para remoção de um filme
void removerFilme(FILE *movies, IndiceP **indexP, IndiceS **indexS);

//Menu para modificação de nota
void modificarNota(FILE *movies, IndiceP *indexP);

//Menu para busca de filme (com dois submenus)
void buscarFilme(FILE *movies, FILE *indexP, IndiceS *indexS);

//Submenu para busca de filme pela titulo primária
bool buscarCodigo(FILE *movies, FILE *indexP);

//Submenu para busca de filme pela titulo secundária
bool buscarTitulo(FILE *movies, FILE *indexP, IndiceS *indexS);

//Listagem de todos os filmes
void listarFilmes(FILE *movies, FILE *indexP);

//Retorna o código de um filme com determinado código
string tituloFromRNN(FILE *movies, int rnn);

//Impressão de um filme na tela; retorna 0 se o filme nesse RNN foi apagado
bool imprimirFilme(FILE *movies, int rnn);

//Limpa o buffer de entrada
void clearBuffer();

#endif