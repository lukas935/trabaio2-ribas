#include "indices.h"

NoP *novaPagina(int rnn) {
    NoP *novo = malloc(sizeof(NoP));
    novo->rnn = rnn;
    novo->serFolha = true;
    for (int i = 0; i < ORDER; i++) //com um espaço a mais, para permitir overflow
        strcpy(novo->chaves[i], "*****");
    for (int i = 0; i < ORDER - 1; i++)
        novo->rnnDados[i] = -1;
    for (int i = 0; i < ORDER; i++)
        novo->filhos[i] = -1;
    novo->numChaves = 0;
    novo->pai = -1;
    novo->prox = -1;
    return novo;
}

NoP *lerPagina(FILE *index, int rnn) {
    //se o arquivo não foi aberto, ou o rnn passado é inválido (negativo, ou após o fim do arquivo)
    if (index == NULL || rnn < 0 || rnn > calculaRnnFinal(index))
        return NULL;

    NoP *novo = novaPagina(rnn); //aloca espaço para o NoP sendo lido
    char entrada[TAM_PAGINA + 1]; //buffer para ler a entrada do arquivo

    fseek(index, 3 + (rnn * TAM_PAGINA), SEEK_SET); //encontramos a posição da entrada que queremos, pulando o header
    fgets(entrada, TAM_PAGINA + 1, index); //lemos a entrada de tamanho fixo

    //quebrando a entrada por campos
    string serFolha = strtok(entrada, "@");
    string chaves = strtok(NULL, "@");
    string rnnDados = strtok(NULL, "@");
    string filhos = strtok(NULL, "@");
    string numChaves = strtok(NULL, "@");
    string pai = strtok(NULL, "@");
    string prox = strtok(NULL, "@");

    // é ou não folha
    novo->serFolha = (strcmp(serFolha, "1") == 0) ? true : false;

    // verificamos as chaves
    for (int i = 0; i < ORDER - 1; i++) {
        char *token = strtok((i == 0) ? chaves : NULL, ",");
        strcpy(novo->chaves[i], token);
    }

    // se é folha,
    if (novo->serFolha) {
        // verificamos os RNNs dos dados
        for (int i = 0; i < ORDER - 1; i++) {
            char *token = strtok((i == 0) ? rnnDados : NULL, ",");
            if (token != NULL && strcmp(token, "**") != 0)
                novo->rnnDados[i] = atoi(token);
        }
        //e os filhos já foram inicializados como vazios

    } else { //se não é folha,
        //os RNNs dos dados já foram inicializados como vazios

        // e verificamos os filhos
        for (int i = 0; i < ORDER; i++) {
            char *token = strtok((i == 0) ? filhos : NULL, ",");
            if (token != NULL && strcmp(token, "**") != 0)
                novo->filhos[i] = atoi(token);
        }
    }

    // número de chaves
    novo->numChaves = atoi(numChaves);

    // página pai
    if (strcmp(pai, "**") != 0)
        novo->pai = atoi(pai);

    // próxima página
    if (strcmp(prox, "**") != 0)
        novo->prox = atoi(prox);

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
    fseek(ititle, 1, SEEK_SET); //pula a flag do cabeçalho
    while (fscanf(ititle, "%[^@]@%[^@]@", titulo, codigos) != EOF) {
        auxS = newNoS(titulo); //cria um novo NoS, com o título lido

        //nesse NoS, colocaremos uma lista de códigos
        auxC = NULL;

        //enquanto houverem códigos, lemos as informações
        int i = 0;
        while (sscanf_s(codigos + i, "%"STRINGIFY(TAM_COD)"s", codigo, sizeof(codigo)) == 1) {
            auxC = newNoCodigo(codigo);         //criamos um NoCodigo
            insereNoCodigo(auxS, auxC); //inserimos o NoCodigo no NoS
            i += TAM_COD; // Avança para o próximo conjunto de 5 caracteres
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
    fseek(movies, 1, SEEK_SET); //pula a flag do cabeçalho
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
    char entrada[TAM_PAGINA + 1];

    //declaração de buffers para cada campo da entrada (tamanho do campo + separador "," se aplicável + \0)
    char serFolha[1 + 1] = "";
    char chaves[((TAM_COD + 1) * (ORDER - 1)) + 1] = "";
    char rnnDados[((2 + 1) * ORDER - 1) + 1] = "";
    char filhos[((2 + 1) * ORDER) + 1] = "";
    char numChaves[1 + 1] = "";
    char pai[2 + 1] = "";
    char prox[2 + 1] = "";

    //formatamos o booleano de ser ou não folha
    sprintf(serFolha, "%d", (int) pagina->serFolha);

    //formatamos as chaves
    for (i = 0; i < ORDER - 1; i++) {
        strcat(chaves, pagina->chaves[i]);
        strcat(chaves, ",");
    }

    //se a página é folha
    if (pagina->serFolha) {
        //formatamos os RNNs dos dados
        for (i = 0; i < ORDER - 1; i++) {
            rnn = pagina->rnnDados[i];
            if (rnn == -1)
                strcat(rnnDados, "**,");
            else
                sprintf(rnnDados + strlen(rnnDados), "%02d,", rnn);
        }
        //nó folha não tem filhos
        for (i = 0; i < ORDER; i++)
            strcat(filhos, "**,");
    } else { //se é página interna,
        //RNNs dos dados são nulos
        for (i = 0; i < ORDER - 1; i++)
            strcat(rnnDados, "**,");

        //formatamos os filhos
        for (i = 0; i < ORDER; i++) {
            rnn = pagina->filhos[i];
            if (rnn == -1)
                strcat(filhos, "**,");
            else
                sprintf(filhos + strlen(filhos), "%02d,", rnn);
        }
    }

    //formatamos a quantidade de chaves
    if (pagina->numChaves == ORDER) //se alguma página ainda tem resquícios do tratamento de overflow
        sprintf(numChaves, "%d", ORDER - 1);
    else
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

    //encontramos a posição correta para escrita
    rnn = pagina->rnn;
    int final = calculaRnnFinal(index);
    if (rnn == -1 || rnn > final)
        fseek(index, 0, SEEK_END);
    else
        fseek(index, 3 + (rnn * TAM_PAGINA), SEEK_SET);
    fprintf(index, "%s", entrada);
}

void freeNoCodigos(NoCodigo *head) {
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
        freeNoCodigos(freela->head); // Libera a lista de códigos associada a este NoS
        free(freela); // Libera o NoS
    }
    free(indice); // Libera o IndiceS
}

NoP *buscaCodigo(FILE *index, int rnn, string codigo, int *retorno_i) {
    NoP *pagina = lerPagina(index, rnn);

    if (pagina == NULL || strlen(codigo) != TAM_COD) {
        *retorno_i = -1;
        return NULL;
    }

    //andamos até que o codigo que buscamos seja menor do que a chave na página, ou chegamos ao fim da lista de chaves
    int i = 0;
    while (i < pagina->numChaves && strcmp(codigo, pagina->chaves[i]) >= 0) {
        //se encontramos codigo, atribuímos suas coordenadas às variáveis de retorno
        if (pagina->serFolha && strcmp(codigo, pagina->chaves[i]) == 0) {
            *retorno_i = i;
            return pagina;
        }
        i++;
    }

    //se não encontramos e estamos numa página interna, procuramos no filho correspondente
    if (!pagina->serFolha) {
        rnn = pagina->filhos[i];
        free(pagina);
        return buscaCodigo(index, rnn, codigo, retorno_i);
    }

    //se não, atribuímos valores de NOT FOUND
    *retorno_i = -1;
    return NULL;
}

void insereFilmeIndices(FILE *indexP, IndiceS *indexS, string codigo, string titulo, int rnn_filme) {
    // Inserção no índice primário
    insereCodigo(indexP, codigo, rnn_filme);

    // Inserção no índice secundário ---------------------------------------------------------
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

            freeNoCodigos(cur->head);
            free(cur);
            index->tamanho--;
            return;
        }
        prev = cur;
        cur = cur->prox;
    }
}

void removeFilmeIndices(FILE *indexP, IndiceS *indexS, string codigo, string titulo) {
    //remoção do índice primário
    removeCodigo(indexP, getRoot(indexP), codigo);

    //remoção do índice secundário --------------------------------------------------
    NoS *noS = buscaNoS(indexS, titulo); //encontra o título no índice secundário
    removeNoCodigo(noS, codigo); //remove a associação do código com o título no índice secundário

    //se não há mais filmes com esse título
    if (noS->head == NULL)
        removeNoS(indexS, titulo); //remove o título do índice secundário
}

//TODO: não finalizado
void removeCodigo(FILE *index, int raiz, string codigo) {
    //encontramos a folha com codigo
    NoP *no = buscaFolha(index, raiz, codigo);

    if (no == NULL)
        return;

    //removemos a chave da folha
    removeCodigo_Folha(index, no, codigo);

    //lida com a propagação
    NoP *pai = lerPagina(index, no->pai);


}

void removeCodigo_Folha(FILE *index, NoP *folha, string codigo) {

}

void insereCodigo(FILE *index, string codigo, int rnnDados) {
    int raiz = getRoot(index);

    //se a árvore está vazia, criamos uma nova raiz
    if (raiz == -1) {
        NoP *novaRaiz = novaPagina(0);
        insereCodigo_Folha(novaRaiz, codigo, rnnDados);
        setRoot(index, novaRaiz->rnn);
        escreverPagina(index, novaRaiz);
        free(novaRaiz);
        return;
    }

    //do contrário, buscamos a folha apropriada para inserção
    NoP *velho = buscaFolha(index, raiz, codigo);
    insereCodigo_Folha(velho, codigo, rnnDados);

    //se a folha está cheio após a inserção, há split
    if (velho->numChaves == ORDER) {
        NoP *novo = novaPagina(calculaRnnFinal(index) + 1);
        novo->pai = velho->pai;

        //encontramos o meio
        int meio = (int) ceil(velho->numChaves / 2.0) - 1;

        //copiamos as chaves do meio pra cima para o novo nó
        for (int i = 0; i < ORDER - meio - 1; i++) {
            strcpy(novo->chaves[i], velho->chaves[meio + i + 1]);
            novo->rnnDados[i] = velho->rnnDados[meio + i + 1];

            novo->numChaves++;
            velho->numChaves--;

            strcpy(velho->chaves[meio + i + 1], "*****");
            velho->rnnDados[meio + i + 1] = -1;
        }

        //atualiza a lista formada pelas folhas
        novo->prox = velho->prox;
        velho->prox = novo->rnn;

        //insere o valor do meio no nó pai
        insereCodigo_Pai(index, velho, novo->chaves[0], novo);

        //escrevemos o novo nó no arquivo
        escreverPagina(index, novo);
        free(novo);
    }
    escreverPagina(index, velho);
    free(velho);
}

NoP *buscaFolha(FILE *index, int raiz, string codigo) {
    NoP *atual = lerPagina(index, raiz);

    //Iteramos até encontrar o nó folha apropriado para inserção
    while (!atual->serFolha) {
        int i;
        //Percorre todas as chaves do nó atual
        for (i = 0; i < atual->numChaves; i++) {
            if (strcmp(codigo, atual->chaves[i]) == 0) {
                int filho_direito = atual->filhos[i + 1];
                free(atual);
                atual = lerPagina(index, filho_direito);
                break;
            } else if (strcmp(codigo, atual->chaves[i]) < 0) { //se a chave é menor do que as demais do nó
                int filho_esquerdo = atual->filhos[i];
                free(atual);
                atual = lerPagina(index, filho_esquerdo);
                break;
            }

            //se chegamos ao final do loop sem passar pelas outras condições
            if (i + 1 == atual->numChaves) {
                int ultimo_filho = atual->filhos[i + 1];
                free(atual);
                atual = lerPagina(index, ultimo_filho);
                break;
            }
        }
    }
    return atual;
}

void insereCodigo_Folha(NoP *folha, string codigo, int rnnDados) {
    if (folha->numChaves > 0) {
        int i;
        //encontramos a posição de inserção correta
        for (i = 0; i < folha->numChaves; i++) {
            //se a chave já existe
            if (strcmp(codigo, folha->chaves[i]) == 0)
                break;

            //inserção quando codigo é menor do que os valores seguintes
            if (strcmp(codigo, folha->chaves[i]) < 0) {
                //insere a chave na posição i e faz um shift nas demais
                for (int j = folha->numChaves; j > i; j--) {
                    strcpy(folha->chaves[j], folha->chaves[j - 1]);
                    folha->rnnDados[j] = folha->rnnDados[j - 1];

                    strcpy(folha->chaves[j - 1], "*****");
                    folha->rnnDados[j - 1] = -1;
                }
                strcpy(folha->chaves[i], codigo);
                folha->rnnDados[i] = rnnDados;
                folha->numChaves++;
                break;
            }

            //inserção quando i é maior do todos os valores presentes
            if (i + 1 == folha->numChaves) {
                strcpy(folha->chaves[i + 1], codigo);
                folha->rnnDados[i + 1] = rnnDados;
                folha->numChaves++;
                break;
            }
        }
    } else { //se o nó está vazio, inserimos na primeira posição
        strcpy(folha->chaves[0], codigo);
        folha->rnnDados[0] = rnnDados;
        folha->numChaves++;
    }
}

void insereCodigo_Pai(FILE *index, NoP *velho, char *promovida, NoP *novo) {
    // se o nó velho é raiz da árvore, criamos uma nova raiz
    if (getRoot(index) == velho->rnn) {
        NoP *novaRaiz = novaPagina(calculaRnnFinal(index));
        novaRaiz->serFolha = false;
        strcpy(novaRaiz->chaves[0], promovida);
        novaRaiz->filhos[0] = velho->rnn;
        novaRaiz->filhos[1] = novo->rnn;
        novaRaiz->numChaves = 1;

        //a nova raiz é pai do nó velho e do nó novo
        velho->pai = novaRaiz->rnn;
        novo->pai = novaRaiz->rnn;

        //atualizamos as informações no arquivo
        setRoot(index, novaRaiz->rnn);
        escreverPagina(index, novaRaiz);

        free(novaRaiz);
        return;
    }

    //se o nó não é raiz, utilizamos o pai do nó atual
    NoP *pai = lerPagina(index, velho->pai);

    //percorremos cada filho do nó pai até encontrar a posição do nó velho
    int i;
    for (i = 0; i <= pai->numChaves; i++) {
        if (pai->filhos[i] == velho->rnn) {
            //inserimos a chave promovida na posição correta das chaves do nó pai
            for (int j = pai->numChaves - 1; j >= i; j--) {
                strcpy(pai->chaves[j + 1], pai->chaves[j]);
                pai->filhos[j + 2] = pai->filhos[j + 1];
            }
            strcpy(pai->chaves[i], promovida);
            pai->filhos[i + 1] = novo->rnn;
            pai->numChaves++;

            break;
        }
    }

    //verificamos se o nó pai está excedendo a capacidade máxima após essa inserção
    int max = ORDER - 1;
    if (pai->numChaves == max) {
        NoP *paiDash = novaPagina(calculaRnnFinal(index) + 1);
        paiDash->serFolha = false;
        paiDash->pai = pai->pai;

        //move as chaves à direita da posição média para o novo nó interno
        int meio = (int) ceil(max / 2.0) - 1;
        for (int j = 0; j < max - meio - 1; j++) {
            strcpy(paiDash->chaves[j], pai->chaves[meio + j + 1]);
            paiDash->filhos[j] = pai->filhos[meio + j + 1];

            strcpy(pai->chaves[meio + j + 1], "*****");
            pai->filhos[meio + j + 1] = -1;
        }
        paiDash->filhos[max - meio - 1] = pai->filhos[max];
        pai->filhos[max] = -1;

        //o valor da posição média será promovido na próxima chamada
        strcpy(promovida, pai->chaves[meio]);

        //atualizamos as chaves do nó pai para refletir a divisão
        if (meio != 0) {
            for (int j = 0; j < meio; j++) {
                strcpy(pai->chaves[j], pai->chaves[j + 1]);
                strcpy(pai->chaves[j + 1], "*****");
            }
            pai->numChaves = meio;
        } else {
            strcpy(pai->chaves[0], pai->chaves[meio]);
        }

        //atualizamos os filhos do nó pai
        for (int j = 0; j <= meio; j++) {
            pai->filhos[j] = pai->filhos[j + 1];
            pai->filhos[j + 1] = -1;
        }

        //atualizamos os ponteiros de pais dos nós filhos dos nós pai e paiDash
        for (int j = 0; j <= meio; j++) {
            NoP *filho = lerPagina(index, pai->filhos[i]);
            filho->pai = pai->rnn;
            escreverPagina(index, filho);
            free(filho);
        }
        for (int j = 0; j < max - meio; j++) {
            NoP *filho = lerPagina(index, paiDash->filhos[i]);
            filho->pai = paiDash->rnn;
            escreverPagina(index, filho);
            free(filho);
        }

        //recursivamente insere o avô
        insereCodigo_Pai(index, pai, promovida, paiDash);

        //escrevemos o nó paiDash que foi criado no arquivo
        escreverPagina(index, paiDash);
        free(paiDash);
    }

    //atualizamos o nó pai
    escreverPagina(index, pai);
    free(pai);
}

NoP *getListaFolhas(FILE *index) {
    int raiz = getRoot(index);

    //se a árvore está vazia
    if (raiz == -1)
        return NULL;

    //Percorremos a árvore até o nível das folhas, indo sempre pelo filho mais à esquerda
    NoP *atual = lerPagina(index, raiz);
    while (!atual->serFolha) {
        raiz = atual->filhos[0];
        free(atual);
        atual = lerPagina(index, raiz);
    }

    return atual;
}

int getRoot(FILE *index) {
    char buffer[4];
    fseek(index, 0, SEEK_SET);
    if (fscanf(index, "%[^@]@", buffer) != 1)
        return -1;  // Retorna -1 se a leitura falha (se o arquivo está vazio)
    return atoi(buffer);
}

void setRoot(FILE *index, int raiz) {
    fseek(index, 0, SEEK_SET);
    fprintf(index, "%02d@", raiz);
}

int calculaRnnFinal(FILE *index) {
    fseek(index, 0, SEEK_END);
    int tamanho = ftell(index);
    if (tamanho < TAM_PAGINA) //se o arquivo é menor do que o tamanho de uma página, assumimos que está vazio
        return -1;
    return (tamanho - 3) / TAM_PAGINA;
}
