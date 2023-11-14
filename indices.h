#ifndef INDICES
#define INDICES

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include "filmes.h"

#ifndef STRING
#define STRING
typedef char *string;
#endif

#define TAM_FOLHA 43

#define CONSISTENTE '0'
#define INCONSISTENTE '1'

//Definições de estruturas ---------------------------------------------------------------------------------------------

//Nó que representa uma folha na árvore B+ do índice primário (implementado com ponteiros para permitir a determinação dinâmica da ordem da árvore)
typedef struct noP {
    int rnn;
    bool serFolha;
    //lista dos valores das chaves no nó
    string *chaves;
    //lista dos RNNs dos dados, (se o nó é folha)
    int *rnnDados;
    //lista de RNNs dos nós filhos (se o nó não é folha)
    int *filhos;
    int numChaves;
    //RNN do nó pai
    int pai;
    //RNN do próximo nó na lista (se o nó é folha)
    int prox;
} NoP;

//Nó que representa um código de filme em uma lista de códigos
typedef struct noCodigo {
    //Código do filme
    string codigo;
    //Ponteiro para o próximo NoChave na lista
    struct noCodigo *prox;
} NoCodigo;

//Nó que representa uma entrada no índice secundário
typedef struct noS {
    //Chave secundária, o título do filme
    string titulo;
    //Ponteiro para o início da lista de NoCodigo, representando as chaves do índice primário
    NoCodigo *head;
    //Ponteiro para o próximo NoS na lista
    struct noS *prox;
} NoS;

//Representação na memória do índice secundário
typedef struct {
    //Ponteiro para o começo de uma lista de NoS, representando as entradas do índice secundário
    NoS *head;
    int tamanho;
} IndiceS;

//Declarações de Funções -----------------------------------------------------------------------------------------------

//Cria um novo NoP, que contém um código e o endereço correspondente, e aponta para NULL
NoP *newNoP(string codigo, int rnn);

/* TODO: insereCodigo
 * TODO: buscaCodigo
 * TODO: insereFilme
 * TODO: removeFilmeFromIndice
 */

NoP *novaFolha(int rnn);

NoP *lerFolha(FILE* index, int rnn);

void escreverFolha(FILE* index, NoP* folha);

//Libera o espaço alocado para uma folha de árvore B+ na memória
void freeFolha(NoP *folha);

//Cria um novo NoCodigo, que contém um código, e aponta para NULL;
NoCodigo *newNoCodigo(string codigo);

//Insere ordenadamente um NoCodigo na lista que começa em head
void insereNoCodigo(NoS *noS, NoCodigo *noC);

//Remove o código associado a um título no índice secundário
void removeNoCodigo(NoS *no, string codigo);

//Cria um novo NoS, que contém um titulo, tem um ponteiro para uma lista vazia de NoChave, e aponta para NULL
NoS *newNoS(string titulo);

//Cria um novo IndiceS (fila vazia de NoS)
IndiceS *newIndiceS();

//Insere ordenadamente um NoS na lista do IndiceS
void insereNoS(IndiceS *index, NoS *no);

//Por busca sequencial, retorna o endereço do NoS com a chave titulo; NULL se não está presente
NoS *buscaNoS(IndiceS *index, string titulo);

//Remove o título do índice secundário
void removeNoS(IndiceS *index, string titulo);

//Lê um arquivo de índice secundário, e monta um IndiceS com as informações
IndiceS *lerS(FILE *ititle);

//Cria um IndiceS a partir do arquivo de dados
IndiceS *refazerS(FILE *movies);

//Salva as informações de IndiceS em um arquivo
void saveIndiceS(IndiceS *index);

//Libera a lista de códigos dentro de um NoS
void freeCodigos(NoCodigo *head);

//Libera o espaço alocado para um IndiceS na memória
void freeIndiceS(IndiceS *index);

//Retorna o RNN de uma entrada de filme com um dado código; -1 se o código não está presente no IndiceP
int rnnFromCodigo(NoP *folha, string codigo);

//Insere o filme com o códgio e titulo correspondentes nos índices
void insereFilme(IndiceP *indexP, IndiceS *indexS, string codigo, string titulo, int rnn);

//Remove o filme com o código e titulo correspondente dos índices
void removeFilmeFromIndice(IndiceP *indexP, IndiceS *indexS, string codigo, string titulo);

#endif