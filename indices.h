#ifndef INDICES
#define INDICES

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include "filmes.h"

#ifndef STRING
#define STRING
typedef char *string;
#endif

#define ORDER 6
#define TAM_PAGINA 76

#define CONSISTENTE '0'
#define INCONSISTENTE '1'

//Definições de estruturas ---------------------------------------------------------------------------------------------

//Nó que representa uma folha na árvore B+ do índice primário
typedef struct {
    int rnn;
    bool serFolha;
    //lista dos valores das chaves no nó (com um espaço a mais para permitir overflow)
    char chaves[ORDER - 1 + 1][TAM_COD + 1];
    //lista dos RNNs dos dados, (se o nó é folha)
    int rnnDados[ORDER - 1];
    //lista de RNNs dos nós filhos (se o nó não é folha)
    int filhos[ORDER];
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

/* TODO: insereCodigo
 * TODO: insereFilme
 * TODO: removeFilmeFromIndice
 */

//Aloca espaço na memória para um NoP (folha da árvore B+), com campo rnn, e demais campos "zerados"
NoP *novaPagina(int rnn);

//Lê a folha na posição rnn no arquivo index, e retorna um NoP
NoP *lerPagina(FILE *index, int rnn);

//Insere um código, associado aos dados em rnnDados, na árvore B++ presente no arquivo index. Quando há overflow, realiza split
void insereCodigo(FILE *index, string codigo, int rnnDados);

//Auxiliar de insereCodigo_Folha(). Busca a folha ideal para inserção de codigo
NoP *buscaFolha(FILE *index, int raiz, string codigo);

//Auxiliar de insereCodigo(). Insere codigo em uma folha da árvore B+
void insereCodigo_Folha(NoP *folha, string codigo, int rnnDados);

//Auxiliar de insereCodigo(). Velho e novo são os nós resultantes de um split; inserimos o codigo promovido no pai, e atualizamos sua lista de filhos
void insereCodigo_Pai(FILE *index, NoP *velho, char *promovido, NoP *novo);

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

//Escreve um NoP folha no arquivo index
void escreverPagina(FILE *index, NoP *pagina);

//Libera a lista de códigos dentro de um NoS
void freeCodigos(NoCodigo *head);

//Libera o espaço alocado para um IndiceS na memória
void freeIndiceS(IndiceS *index);

//Busca um código na árvore B+ do índice primário
NoP *buscaCodigo(FILE *index, int rnn_folha, string codigo, int *retorno_i);

//Insere o filme com o códgio e titulo correspondentes nos índices
void insereFilme(FILE *indexP, IndiceS *indexS, string codigo, string titulo, int rnn_filme);

//Remove o filme com o código e titulo correspondente dos índices
void removeFilmeFromIndice(NoP *indexP, IndiceS *indexS, string codigo, string titulo);

//Retorna o nó folha mais a esquerda, ou seja, o início da lista formada pelas folhas
NoP *getListaFolhas(FILE *index);

//Lê o header de index para determinar o rnn da raiz da árvore B+
int getRoot(FILE *index);

//Escreve o rnn da raiz da árvore B+ no header de index
void setRoot(FILE *index, int raiz);

//Retorna o valor do RNN da última entrada no arquivo de índice primário
int calculaRnnFinal(FILE *index);

#endif