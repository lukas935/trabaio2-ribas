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
void removerFilme(FILE *movies, FILE *indexP, IndiceS **indexS);

//Menu para modificação de nota
void modificarNota(FILE *movies, FILE *indexP);

//Menu para busca de filme (com dois submenus)
void buscarFilme(FILE *movies, FILE *indexP, IndiceS *indexS);

//Submenu para busca de filme pela titulo primária
bool buscaPorCodigo(FILE *movies, FILE *indexP);

//Submenu para busca de filme pela titulo secundária
bool buscaPorTitulo(FILE *movies, FILE *indexP, IndiceS *indexS);

//Submenu para listagem de filmes
void listarFilmes(FILE *movies, FILE *indexP);

//Lista todos os filmes, em ordem alfabética
void listarTodos(FILE *movies, FILE* indexP);

//Pede para o usuário um intervalo, e lista em ordem alfabética todos os filmes nesse intervalo
void listarIntervalo(FILE *movies, FILE *indexP);

//Retorna o código de um filme com determinado código
string tituloFromRNN(FILE *movies, int rnn);

//Impressão de um filme na tela; retorna 0 se o filme nesse RNN foi apagado
bool imprimirFilme(FILE *movies, int rnn);

//Limpa o buffer de entrada
void clearBuffer();

#endif