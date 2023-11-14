#include "indices.h"

NoP *buscaCodigo(NoP *folha, string codigo);

NoCodigo *newNoCodigo(string codigo) {
    NoCodigo *novo = malloc(sizeof(NoCodigo));
    novo->codigo = malloc(TAM_COD + 1);
    strcpy(novo->codigo, codigo);
    novo->prox = NULL;
    return novo;
}

void insereNoCodigo(NoS *noS, NoCodigo *noC) {
    //se a lista está vazia
    if (noS->head == NULL) {
        noS->head = noC;
        return;
    }

    //começamos pelo começo da lista
    NoCodigo *aux = noS->head;
    NoCodigo *prev = NULL;

    //procuramos a posição certa; sai do while se quando noC é menor do que aux, ou se chegamos no final da lista
    while (aux != NULL && strcmp(noC->codigo, aux->codigo) > 0) {
        prev = aux;
        aux = aux->prox;
    }

    //se estamos no final da lista
    if (aux == NULL) {
        prev->prox = noC;
        return;
    }

    //se estamos na posição certa
    if (strcmp(noC->codigo, aux->codigo) < 0) {
        //se estamos no começo da lista
        if (prev == NULL) {
            noC->prox = noS->head;
            noS->head = noC;
        } else { //se estamos no meio da lista
            prev->prox = noC;
            noC->prox = aux;
        }
    }
}

NoS *newNoS(string titulo) {
    NoS *novo = malloc(sizeof(NoS));
    novo->titulo = malloc(TAM_TIT_PT + 1);
    strcpy(novo->titulo, titulo);
    novo->head = NULL;
    novo->prox = NULL;
    return novo;
}

IndiceS *newIndiceS() {
    IndiceS *novo = malloc(sizeof(IndiceS));
    novo->head = NULL;
    novo->tamanho = 0;
    return novo;
}

void insereNoS(IndiceS *index, NoS *no) {
    //se a lista está vazia
    if (index->head == NULL) {
        index->head = no;
        index->tamanho++;
        return;
    }

    //começamos pelo começo da lista
    NoS *aux = index->head;
    NoS *prev = NULL;

    //procuramos a posição certa; sai do while quando no é menor ou igual que aux, ou se chegamos ao final da lista
    while (aux != NULL && strcmp(no->titulo, aux->titulo) > 0) {
        prev = aux;
        aux = aux->prox;
    }

    //se estamos no final da lista
    if (aux == NULL) {
        prev->prox = no;
        index->tamanho++;
        return;
    }

    //se estamos na posição certa
    if (strcmp(no->titulo, aux->titulo) < 0) {
        //se estamos no começo da lista
        if (prev == NULL) {
            no->prox = index->head;
            index->head = no;
        } else { //se estamos no meio da lista
            prev->prox = no;
            no->prox = aux;
        }
        index->tamanho++;
    }
}

NoS *buscaNoS(IndiceS *index, string titulo) {
    NoS *aux = index->head;

    while (aux) {
        if (strcmp(aux->titulo, titulo) == 0)
            return aux;
        aux = aux->prox;
    }

    return NULL;
}

IndiceS *lerS(FILE *ititle) {
    IndiceS *novo = newIndiceS(); //aloca e inicializa um IndiceS
    NoS *auxS = NULL; //NoS sendo lido
    NoCodigo *auxC = NULL;  //NoCodigo que irá iterar pelos códigos a serem adicionado ao NoS

    string titulo = malloc(TAM_TIT_PT + 1);
    string codigos = malloc((TAM_COD + 1) * 10);
    string codigo = malloc(TAM_COD + 1);

    //enquanto há linhas para ler, armazena as informações
    fseek(ititle, 1, SEEK_SET);
    while (fscanf(ititle, "%[^@]@%[^@]@", titulo, codigos) != EOF) {
        auxS = newNoS(titulo); //cria um novo NoS, com o título lido

        //nesse NoS, colocaremos uma lista de códigos
        auxC = NULL;

        //enquanto houverem códigos, lemos as informações
        int i = 0;
        while (sscanf_s(codigos + i, "%5s", codigo, sizeof(codigo)) == 1) {
            auxC = newNoCodigo(codigo);         //criamos um NoCodigo
            insereNoCodigo(auxS, auxC); //inserimos o NoCodigo no NoS
            i += 5; // Avança para o próximo conjunto de 5 caracteres
        }

        insereNoS(novo, auxS); //inserimos o NoS criado no IndiceS novo
    }
    free(titulo);
    free(codigos);
    free(codigo);
    return novo;
}

IndiceS *refazerS(FILE *movies) {
    IndiceS *novo = newIndiceS(); //aloca e inicializa um IndiceS

    string filme = malloc(TAM_FILME + 1);
    string codigo = malloc(TAM_COD + 1);
    string titulo = malloc(TAM_TIT_PT + 1);

    NoS *currentS = NULL;
    NoCodigo *currentC = NULL;

    //enquanto há entradas para ler, armazena as informações
    fseek(movies, 0, SEEK_SET);
    while (fscanf(movies, "%"STRINGIFY(TAM_FILME)"[^\n]s", filme) != EOF) {
        if (filme[0] != '*' && filme[1] != '|') {
            sscanf(filme, "%[^@]@%[^@]@", codigo, titulo);

            currentC = newNoCodigo(codigo);

            //verifica se o título já tem um NoS correspondente em IndiceS
            currentS = buscaNoS(novo, titulo);
            if (currentS == NULL) { //se não há um NoS para esse título
                currentS = newNoS(titulo); //cria um novo NoS para esse título
                insereNoCodigo(currentS, currentC); //coloca o código nesse NoS
                insereNoS(novo, currentS); //insere o NoS no IndiceS
            } else //insere um novo NoCodigo, para esse titulo, na lista do NoS já existente
                insereNoCodigo(currentS, currentC);
        }
    }
    free(filme);
    free(codigo);
    free(titulo);
    return novo;
}

void saveIndiceS(IndiceS *index) {
    FILE *ititle = fopen("data/ititle.idx", "w"); //abre o arquivo para ser reescrito

    fputc(INCONSISTENTE, ititle); //marca ele como inconsistente, caso o processo seja interrompido

    //percorre a lista de índice secundário, imprimindo as informações no arquivo
    NoS *printHeadS = index->head;
    NoCodigo *printHeadCod; //para iterar pela lista de códigos de cada NoS
    while (printHeadS) {
        fprintf(ititle, "%s@", printHeadS->titulo); //imprime o campo do título

        //percorre a lista de códigos, imprimindo as informações no campo correspondente
        printHeadCod = printHeadS->head;
        while (printHeadCod) {
            fprintf(ititle, "%s", printHeadCod->codigo);
            printHeadCod = printHeadCod->prox;
        }
        fputc('@', ititle); //encerra o campo dos códigos

        printHeadS = printHeadS->prox;
    }

    ftruncate(fileno(ititle), ftell(ititle)); //garante que não há informações residuais após o final do arquivo

    fseek(ititle, 0, SEEK_SET); //retorna ao começo do arquivo
    fputc(CONSISTENTE, ititle); //marca o arquivo como consistente
    fclose(ititle); //fecha o arquivo
}

void freeFolha(NoP *folha) {
    free(folha->rnnDados);
    free(folha->filhos);
    free(folha->chaves);
    free(folha);
}

void freeCodigos(NoCodigo *head) {
    while (head != NULL) {
        NoCodigo *freela = head;
        head = head->prox;
        free(freela->codigo); // Libera o código do filme
        free(freela); // Libera o NoCodigo
    }
}

void freeIndiceS(IndiceS *indice) {
    NoS *current = indice->head;
    while (current != NULL) {
        NoS *freela = current;
        current = current->prox;
        free(freela->titulo); // Libera o título do filme
        freeCodigos(freela->head); // Libera a lista de códigos associada a este NoS
        free(freela); // Libera o NoS
    }
    free(indice); // Libera o IndiceS
}

int rnnFromCodigo(NoP *folha, string codigo) {
    //garante que a busca é case insensitive
    for (int i = 0; i < 3; i++)
        codigo[i] = toupper(codigo[i]);

    NoP *aux = buscaCodigo(folha, codigo);
    if (aux == NULL)
        return -1;
    return aux->rnn;
}

//TODO: buscaCodigo
NoP *buscaCodigo(NoP *folha, string codigo) {
    return NULL;
}

//TODO: insereFilme
void insereFilme(IndiceP *indexP, IndiceS *indexS, string codigo, string titulo, int rnn) {
    NoP *noP = newNoP(codigo, rnn); //cria um NoP
    NoS *noS = NULL; //noS do título
    NoCodigo *novoC = newNoCodigo(codigo); //cria um NoCodigo

    //verifica se já existe um NoS para o título sendo inserido
    noS = buscaNoS(indexS, titulo);
    if (noS == NULL) { //se não existe
        noS = newNoS(titulo); //cria um NoS
        insereNoCodigo(noS, novoC); //insere o NoCodigo no NoS
        insereNoS(indexS, noS); //insere o NoS no IndiceS
    } else //se existe
        insereNoCodigo(noS, novoC); //insere o NoC no NoS encontrado
}

void removeNoCodigo(NoS *no, char *codigo) {
    NoCodigo *cur = no->head;
    NoCodigo *prev = NULL;

    while (cur != NULL && strcmp(cur->codigo, codigo) != 0) {
        prev = cur;
        cur = cur->prox;
    }

    if (cur != NULL) {
        //se estamos no começo da lista
        if (prev == NULL)
            no->head = cur->prox;
        else //se estamos no meio ou no final da lista
            prev->prox = cur->prox;

        free((cur->codigo));
        free(cur);
    }
}

void removeNoS(IndiceS *index, string titulo) {
    NoS *cur = index->head;
    NoS *prev = NULL;

    //enquanto há nós para percorrer
    while (cur != NULL) {
        if (strcmp(cur->titulo, titulo) == 0) {
            //se estamos no começo da lista
            if (prev == NULL)
                index->head = cur->prox;
            else //se estamos no meio ou no final da lista
                prev->prox = cur->prox;

            freeCodigos(cur->head);
            free(cur);
            index->tamanho--;
            return;
        }
        prev = cur;
        cur = cur->prox;
    }
}

//TODO: removeFilmeFromIndice
void removeFilmeFromIndice(IndiceP *indexP, IndiceS *indexS, string codigo, string titulo) {
    NoS *noS = buscaNoS(indexS, titulo); //encontra o título no índice secundário
    removeNoCodigo(noS, codigo); //remove a associação do código com o título no índice secundário

    //se não há mais filmes com esse título
    if (noS->head == NULL)
        removeNoS(indexS, titulo); //remove o título do índice secundário
}

NoP *novaFolha(int rnn) {
    NoP *novo = malloc(sizeof(NoP));
    novo->rnn = rnn;
    novo->serFolha = false;
    novo->chaves = malloc(3 * sizeof(int));
    novo->rnnDados = malloc(3 * sizeof(int));
    novo->filhos = malloc(3 * sizeof(int));
    novo->numChaves = 0;
    novo->pai = -1;
    novo->prox = -1;
    return novo;
}

NoP *lerFolha(FILE *index, int rnn) {
    NoP *novo = novaFolha(rnn);
    string entrada;

    fscanf(index, "%*d@");
    fseek(index, rnn * TAM_FOLHA, SEEK_CUR);

    fgets(entrada, TAM_FOLHA, index);

    sscanf(entrada, "%d@%[^,],%[^,],%[^,]@%d,%d,%d@%d,%d,%d,%d@%d@%d@%d@", &novo->serFolha, novo->chaves[0],
           novo->chaves[1], novo->chaves[2], &novo->rnnDados[0], &novo->rnnDados[1], &novo->rnnDados[2],
           &novo->filhos[0], &novo->filhos[1], &novo->filhos[2], &novo->filhos[3], &novo->numChaves, &novo->pai,
           &novo->prox);
    return novo;
}

void escreverFolha(FILE *index, NoP *folha) {
    string entrada = malloc(TAM_FOLHA);

    sprintf(entrada, "%d@%s,%s,%s@%d,%d,%d@%d,%d,%d,%d@%d@%d@%d@", folha->serFolha, folha->chaves[0], folha->chaves[1],
            folha->chaves[2], folha->rnnDados[0], folha->rnnDados[1], folha->rnnDados[2], folha->filhos[0],
            folha->filhos[1],
            folha->filhos[2], folha->filhos[3], folha->numChaves, folha->pai, folha->prox);

    int size = strlen(entrada);
    if (size < TAM_FOLHA + 1) {
        for (int i = size; i < TAM_FOLHA; i++)
            entrada[i] = '#';
        entrada[TAM_FOLHA] = '\0';
    }

    int rnn = 0; //TODO: determinar o RNN
    fscanf(index, "%*d@");
    fseek(index, rnn * TAM_FOLHA, SEEK_CUR);

    fprintf(index, "%s", entrada);
    //TODO: imprimir "entrada" no arquivo
}
