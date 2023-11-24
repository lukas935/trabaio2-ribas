#include "indices.h"

NoP *novaPagina(int rnn) {
    NoP *novo = malloc(sizeof(NoP));
    novo->rnn = rnn;
    novo->serFolha = false;
    novo->chaves = malloc((ORDER - 1) * sizeof(string));
    for (int i = 0; i < ORDER - 1; i++)
        novo->chaves[i] = malloc(6 + 1);
    novo->rnnDados = (int *) malloc(ORDER * sizeof(int));
    novo->filhos = (int *) malloc(ORDER * sizeof(int));
    novo->numChaves = 0;
    novo->pai = -1;
    novo->prox = -1;
    return novo;
}

NoP *lerPagina(FILE *index, int rnn) {
    //se o arquivo não foi aberto, ou o rnn passado é inválido (negativo, ou após o fim do arquivo)
    if (index == NULL || rnn < 0 || rnn > calculaRnnFinal(index))
        return NULL;

    int i;
    NoP *novo = novaPagina(rnn); //aloca espaço para o NoP sendo lido
    string entrada = malloc(TAM_PAGINA + 1); //aloca espaço para ler a entrada no arquivo

    fseek(index, 3 + (rnn * TAM_PAGINA), SEEK_SET); //encontramos a posição da entrada que queremos, pulando o header

    fgets(entrada, TAM_PAGINA + 1, index); //lemos a entrada de tamanho fixo

    //quebrando a entrada por campos
    string serFolha = strdup(strtok(entrada, "@"));
    string chaves = strdup(strtok(NULL, "@"));
    string rnnDados = strdup(strtok(NULL, "@"));
    string filhos = strdup(strtok(NULL, "@"));
    string numChaves = strdup(strtok(NULL, "@"));
    string pai = strdup(strtok(NULL, "@"));
    string prox = strdup(strtok(NULL, "@"));

    free(entrada);

    // é ou não folha
    novo->serFolha = (strcmp(serFolha, "1") == 0) ? true : false;
    free(serFolha);

    // verificamos as chaves
    for (i = 0; i < ORDER - 1; i++) {
        char *token = strtok((i == 0) ? chaves : NULL, ",");
        novo->chaves[i] = strdup(token);
    }
    free(chaves);

    // se é folha,
    if (novo->serFolha) {

        // verificamos os RNNs dos dados
        for (i = 0; i < ORDER; i++) {
            char *token = strtok((i == 0) ? rnnDados : NULL, ",");

            if (token == NULL || strcmp(token, "**") == 0)
                novo->rnnDados[i] = -1;
            else
                novo->rnnDados[i] = atoi(token);
        }
        free(rnnDados);

        // e assumimos que não há filhos
        for (i = 0; i < ORDER; i++)
            novo->filhos[i] = -1;
        free(filhos);

    } else { //se não é folha,

        //assumimos que não há RNN de dados
        for (i = 0; i < ORDER; i++)
            novo->rnnDados[i] = -1;
        free(rnnDados);

        // e verificamos os filhos
        for (i = 0; i < ORDER + 1; i++) {
            char *token = strtok((i == 0) ? filhos : NULL, ",");

            if (token == NULL || strcmp(token, "**") == 0)
                novo->filhos[i] = -1;
            else
                novo->filhos[i] = atoi(token);
        }
        free(filhos);
    }

    // número de chaves
    novo->numChaves = atoi(numChaves);
    free(numChaves);

    // página pai
    if (strcmp(pai, "**") == 0)
        novo->pai = -1;
    else
        novo->pai = atoi(pai);
    free(pai);

    // próxima página
    if (strcmp(prox, "**") == 0)
        novo->prox = -1;
    else
        novo->prox = atoi(prox);
    free(prox);

    return novo;
}

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

void escreverPagina(FILE *index, NoP *pagina) {
    if (index == NULL || pagina == NULL)
        return;

    int i, rnn;
    string aux;
    string entrada = malloc(TAM_PAGINA + 1);

    //declaração de buffers para cada campo da entrada (tamanho do campo + separador "," se aplicável + \0)
    string serFolha = calloc(1 + 1, sizeof(char));
    string chaves = calloc(((6 + 1) * (ORDER - 1)) + 1, sizeof(char));
    string rnnDados = calloc(((2 + 1) * ORDER) + 1, sizeof(char));
    string filhos = calloc(((2 + 1) * ORDER) + 1, sizeof(char));
    string numChaves = calloc(1 + 1, sizeof(char));
    string pai = calloc(2 + 1, sizeof(char));
    string prox = calloc(2 + 1, sizeof(char));

    //formatamos o booleano de ser ou não folha
    sprintf(serFolha, "%d", (int) pagina->serFolha);

    //formatamos as chaves
    for (i = 0; i < ORDER - 1; i++) {
        strcat(chaves, pagina->chaves[i]);
        strcat(chaves, ",");
    }

    //se a página é interna,
    if (!pagina->serFolha) {

        //RNNs dos dados são nulos
        for (i = 0; i < ORDER; i++)
            strcat(rnnDados, "**,");

        //formatamos os filhos
        aux = malloc(3 + 1);
        for (i = 0; i < ORDER; i++) {
            rnn = pagina->filhos[i];
            if (rnn == -1)
                strcpy(aux, "**,");
            else
                sprintf(aux, "%02d,", rnn);
            strcat(filhos, aux);
        }
        free(aux);

    } else { //se a página é folha,

        //formatamos os RNNs dos dados
        aux = malloc(3 + 1);
        for (i = 0; i < ORDER; i++) {
            rnn = pagina->rnnDados[i];
            if (rnn == -1)
                strcpy(aux, "**,");
            else
                sprintf(aux, "%02d,", rnn);
            strcat(rnnDados, aux);
        }
        free(aux);

        //nó folha não tem filhos
        for (i = 0; i < ORDER; i++)
            strcat(filhos, "**,");
    }

    //formatamos a quantidade de chaves
    sprintf(numChaves, "%d", pagina->numChaves);

    //formatamos o pai
    if (pagina->pai == -1)
        strcpy(pai, "**");
    else
        sprintf(pai, "%02d", pagina->pai);

    //formatamos prox
    if (pagina->prox == -1)
        strcpy(prox, "**");
    else
        sprintf(prox, "%02d", pagina->prox);

    //utilizamos os campos formatados e separadores para compor a entrada
    sprintf(entrada, "%s@%s@%s@%s@%s@%s@%s@", serFolha, chaves, rnnDados, filhos, numChaves, pai, prox);

    //liberamos a memória alocada
    free(serFolha);
    free(chaves);
    free(rnnDados);
    free(filhos);
    free(numChaves);
    free(pai);
    free(prox);

    //encontramos a posição
    rnn = pagina->rnn;
    int final = calculaRnnFinal(index);
    if (rnn == -1 || rnn > final)
        fseek(index, 0, SEEK_END);
    else {
        fseek(index, 3, SEEK_SET);
        fseek(index, rnn * TAM_PAGINA, SEEK_CUR);
    }
    fprintf(index, "%s", entrada);

    free(entrada);
}

void freePagina(NoP *pagina) {
    if (pagina != NULL) {
        for (int i = 0; i < ORDER - 1; i++)
            free(pagina->chaves[i]);
        free(pagina->chaves);
        free(pagina->rnnDados);
        free(pagina->filhos);
        free(pagina);
    }
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

//TODO: algo errado com o free
bool buscaCodigo(FILE *index, int rnn_folha, string codigo, int *retorno_rnn, int *retorno_i) {
    int i;
    NoP *pagina = lerPagina(index, rnn_folha);

    if (pagina == NULL)
        return false;

    //buscamos até que codigo seja menor do que a chave no índice, ou chegamos ao fim da lista
    i = 0;
    while (i < pagina->numChaves && strcmp(codigo, pagina->chaves[i]) >= 0) {
        //se encontramos codigo, atribuímos suas coordenadas às variáveis de retorno
        if (pagina->serFolha && strcmp(codigo, pagina->chaves[i]) == 0) {
            *retorno_rnn = pagina->rnn;
            *retorno_i = i;
            freePagina(pagina);
            return true;
        }
        i++;
    }

    //se não encontramos e estamos numa página interna, procuramos no filho correspondende
    if (!pagina->serFolha) {
        rnn_folha = pagina->filhos[i];
        freePagina(pagina);
        return buscaCodigo(index, rnn_folha, codigo, retorno_rnn, retorno_i);
    }

    //se não, atribuímos valores de NOT FOUND
    *retorno_rnn = -1;
    *retorno_i = -1;
    freePagina(pagina);
    return false;
}

//TODO: insereFilme
void insereFilme(NoP *indexP, IndiceS *indexS, string codigo, string titulo, int rnn) {
    //NoP *noP = newNoP(codigo, rnn); //cria um NoP
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
void removeFilmeFromIndice(NoP *indexP, IndiceS *indexS, string codigo, string titulo) {
    NoS *noS = buscaNoS(indexS, titulo); //encontra o título no índice secundário
    removeNoCodigo(noS, codigo); //remove a associação do código com o título no índice secundário

    //se não há mais filmes com esse título
    if (noS->head == NULL)
        removeNoS(indexS, titulo); //remove o título do índice secundário
}

int calculaRnnFinal(FILE *index) {
    fseek(index, 0, SEEK_END);
    return (((int) ftell(index) - 3)) / TAM_PAGINA;
}