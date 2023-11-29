#include "operacoesUsuario.h"

void inserirFilme(FILE *movies, FILE *indexP, IndiceS **indexS) {
    string filme;

    string codigo = malloc(TAM_COD + 1);       //5 caracteres, mais finalizador
    string tituloPT = malloc(TAM_TIT_PT + 1);  //62 caracteres, mais finalizador
    string tituloOG = malloc(TAM_TIT_OG + 1);  //62 caracteres, mais finalizador
    string diretor = malloc(TAM_DIR + 1);      //30 caracteres, mais finalizador
    string ano = malloc(5);                    //4 caracteres, mais finalizador
    string pais = malloc(TAM_PAIS + 1);        //21 caracteres, mais finalizador
    char nota = 0;                                  //1 caractere, e no arquivo um finalizador

    string nome = malloc(16);
    string sobrenome = malloc(16);

    int rnn;

    // input do usuário ------------------------------------------------------------------------------------------------
    puts(SUBTITLE "\n-----------INSERCAO DE FILME----------" CLEAR);
    puts("\t\t\t\t\t" REMINDER "OBS: Nao utilize acentos" CLEAR);

    printf(PROMPT "Titulo em Portugues: " INPUT);
    scanf("%"STRINGIFY(TAM_TIT_PT)"[^\n]s", tituloPT);
    printf(CLEAR);
    clearBuffer();

    printf(PROMPT "Titulo Original: " INPUT);
    scanf("%"STRINGIFY(TAM_TIT_OG)"[^\n]", tituloOG);
    printf(CLEAR);
    clearBuffer();

    printf(PROMPT "Primeiro Nome do Diretor: " INPUT);
    scanf("%15s", nome);
    printf(CLEAR);
    clearBuffer();

    printf(PROMPT "Ultimo Nome do Diretor: " INPUT);
    scanf("%15s", sobrenome);
    printf(CLEAR);
    clearBuffer();

    printf(PROMPT "Ano: " INPUT);
    scanf("%4s", ano);
    printf(CLEAR);
    clearBuffer();

    printf(PROMPT "Pais: " INPUT);
    scanf("%"STRINGIFY(TAM_PAIS)"[^\n]s", pais);
    printf(CLEAR);
    clearBuffer();

    printf(PROMPT "Nota: " INPUT);
    scanf("%c", &nota);
    printf(CLEAR);
    clearBuffer();

    //validação de entradas --------------------------------------------------------------------------------------------
    if (!isdigit(nota)) {
        puts(ERROR "ERRO: nota invalida" CLEAR "\n");

        free(codigo);
        free(tituloPT);
        free(tituloOG);
        free(ano);
        free(diretor);
        free(pais);

        free(nome);
        free(sobrenome);

        return;
    }

    if (strlen(sobrenome) < 3) {
        puts(ERROR "ERRO: sobrenome curto" CLEAR "\n");

        free(codigo);
        free(tituloPT);
        free(tituloOG);
        free(ano);
        free(diretor);
        free(pais);

        free(nome);
        free(sobrenome);

        return;
    }

    for (int i = 0; i < 4; i++) {
        if (!isdigit(ano[i])) {
            puts(ERROR "ERRO: ano invalido" CLEAR "\n");

            free(codigo);
            free(tituloPT);
            free(tituloOG);
            free(ano);
            free(diretor);
            free(pais);

            free(nome);
            free(sobrenome);

            return;
        }
    }

    //formatação dos campos --------------------------------------------------------------------------------------------

    //ajusta o título original
    if (strcmp(tituloPT, tituloOG) == 0)
        strcpy(tituloOG, "Idem");

    //montando o nome do diretor
    sprintf(diretor, "%s, %s", sobrenome, nome);

    //montando o código do filme
    string aux = malloc(4);
    for (int i = 0; i < 3; i++)
        aux[i] = toupper(sobrenome[i]);
    aux[3] = '\0';

    sprintf(codigo, "%s%c%c", aux, ano[2], ano[3]);
    free(aux);
    free(nome);
    free(sobrenome);

    //verifica se o código desse filme já está registrado
    int i_found;
    NoP *found = buscaCodigo(indexP, getRoot(indexP), codigo, &i_found);
    if (found != NULL) {
        puts(ERROR "ERRO: filme ja cadastrado" CLEAR "\n");

        free(codigo);
        free(tituloPT);
        free(tituloOG);
        free(ano);
        free(diretor);
        free(pais);

        free(found);

        return;
    }
    free(found);
    //se não está registrado, continuamos com a formatação

    //monta a string filme
    filme = malloc(TAM_FILME + 1);
    sprintf(filme, "%s@%s@%s@%s@%s@%s@%c@", codigo, tituloPT, tituloOG, diretor, ano, pais, nota);

    int size = strlen(filme);
    if (size < TAM_FILME + 1) {
        for (int i = size; i < TAM_FILME; i++)
            filme[i] = '#';
        filme[TAM_FILME] = '\0';
    }

    //impressão no arquivo ---------------------------------------------------------------------------------------------
    fseek(movies, 0, SEEK_END);
    rnn = ftell(movies) / TAM_FILME;
    fputs(filme, movies);

    //Atualiza os índices
    insereFilmeIndices(indexP, *indexS, codigo, tituloPT, rnn);

    //libera a memória alocada
    free(codigo);
    free(tituloPT);
    free(tituloOG);
    free(ano);
    free(diretor);
    free(pais);
    free(filme);

    puts(SUCCESS "\tFilme inserido com sucesso" CLEAR "\n");
}

void removerFilme(FILE *movies, FILE *indexP, IndiceS **indexS) {
    string codigo = malloc(TAM_COD + 1);
    string titulo;
    int rnn_filme;

    puts(SUBTITLE "\n-----------REMOCAO DE FILME-----------" CLEAR);

    printf(PROMPT "Chave: " INPUT);
    scanf("%"STRINGIFY(TAM_COD)"s", codigo);
    clearBuffer();

    //procura a posição do filme correspondente no arquivo
    int i_found;
    NoP *found = buscaCodigo(indexP, getRoot(indexP), codigo, &i_found);
    if (found == NULL) {
        puts(ERROR "Filme nao encontrado" CLEAR "\n");
        free(found);
        return;
    }
    rnn_filme = found->rnnDados[i_found];
    free(found);

    //salva o título para remoção do índice secundário
    titulo = tituloFromRNN(movies, rnn_filme);

    //marca o filme como deletado
    fseek(movies, rnn_filme * TAM_FILME, SEEK_SET);
    fputs("*|", movies);

    //remove o filme dos índices
    removeFilmeIndices(indexP, *indexS, codigo, titulo);

    free(codigo);
    free(titulo);

    puts(SUCCESS "\tFilme removido com sucesso" CLEAR "\n");
}

void modificarNota(FILE *movies, FILE *indexP) {
    string codigo = malloc(TAM_COD + 1);
    int rnn_filme;
    char atual, nova;

    puts(SUBTITLE "\n----------MODIFICACAO DE NOTA---------" CLEAR);

    printf(PROMPT "Codigo: " INPUT);
    scanf("%"STRINGIFY(TAM_COD)"s", codigo);
    clearBuffer();

    //torna a busca case-insensitive
    for (int i = 0; i < TAM_COD; i++) {
        if (isalpha(codigo[i]))
            codigo[i] = (char) toupper(codigo[i]);
    }

    int i_found;
    NoP *found = buscaCodigo(indexP, getRoot(indexP), codigo, &i_found);
    free(codigo);
    if (found == NULL) {
        puts(ERROR "Filme nao encontrado" CLEAR "\n");
        free(found);
        return;
    }
    rnn_filme = found->rnnDados[i_found];
    free(found);

    fseek(movies, rnn_filme * TAM_FILME, SEEK_SET);

    //lê a entrada do filme, guardando a nota atual e ignorando os demais campos
    fscanf(movies, "%*[^@]@%*[^@]@%*[^@]@%*[^@]@%*[^@]@%*[^@]@%c@", &atual);

    printf(LISTING "Nota atual: " INFO "%c" CLEAR "\n", atual);
    printf(PROMPT "Nova nota: " INPUT);
    scanf("%c", &nova);

    if (!isdigit(nova)) {
        puts(ERROR "ERRO: nota invalida" CLEAR "\n");
        return;
    }

    //anda no arquivo até a posição onde a nota deveria estar
    fseek(movies, -2, SEEK_CUR);
    fputc(nova, movies);

    puts(SUCCESS "\tNota modificada com sucesso" CLEAR "\n");
}

void buscarFilme(FILE *movies, FILE *indexP, IndiceS *indexS) {
    short int op;
    bool sucess = false;

    do {
        puts(SUBTITLE "\n------------BUSCA DE FILME------------" CLEAR);

        puts(MENU "1." CLEAR " Buscar por codigo");
        puts(MENU "2." CLEAR " Buscar por titulo\t" REMINDER "(case sensitive)" CLEAR);
        puts(MENU "0." CLEAR " Retornar");
        printf(PROMPT "-> " CLEAR INPUT);
        scanf("%hd", &op);
        printf(CLEAR);
        clearBuffer();

        switch (op) {
            case 1:
                sucess = buscaPorCodigo(movies, indexP);
                break;
            case 2:
                sucess = buscaPorTitulo(movies, indexP, indexS);
                break;
            case 0:
                sucess = true;
                puts(""); //pula uma linha
                break;
            default:
                puts(ERROR "ERRO: Opcao invalida" CLEAR);
                break;
        }
    } while (!sucess);
}

bool buscaPorCodigo(FILE *movies, FILE *indexP) {
    string codigo = malloc(TAM_COD + 1);
    int rnn_filme;

    puts(SUBSUBTITLE "\n------BUSCA POR CODIGO------" CLEAR);

    printf(PROMPT "Chave: " INPUT);
    scanf("%"STRINGIFY(TAM_COD)"s", codigo);
    clearBuffer();

    //torna a busca case-insensitive
    for (int i = 0; i < TAM_COD; i++) {
        if (isalpha(codigo[i]))
            codigo[i] = (char) toupper(codigo[i]);
    }

    int i_found;
    NoP *found = buscaCodigo(indexP, getRoot(indexP), codigo, &i_found);
    free(codigo);
    if (found == NULL) {
        puts(ERROR "Filme nao encontrado" CLEAR);
        free(found);
        return false;
    }
    rnn_filme = found->rnnDados[i_found];
    free(found);
    printf("\n");

    short int flag = imprimirFilme(movies, rnn_filme);
    if (flag)
        printf("\n");
    return true;
}

bool buscaPorTitulo(FILE *movies, FILE *indexP, IndiceS *indexS) {
    string titulo = malloc(TAM_TIT_PT + 1);
    NoS *noS;
    NoCodigo *noC;
    int rnn_filme;

    puts(SUBSUBTITLE "\n------BUSCA POR TITULO------" CLEAR);

    printf(PROMPT "Titulo (em portugues): " INPUT);
    scanf("%"STRINGIFY(TAM_TIT_PT)"[^\n]s", titulo);
    printf(CLEAR);
    clearBuffer();

    //procura o nó do índice secundário associado a esse título
    noS = buscaNoS(indexS, titulo);
    free(titulo);
    if (noS == NULL) {
        puts(ERROR "Filme nao encontrado" CLEAR);
        return false;
    }

    //analisa todos os códigos associados a esse título
    noC = noS->head;
    while (noC != NULL) {
        int i_found;
        NoP *found = buscaCodigo(indexP, getRoot(indexP), noC->codigo, &i_found);
        if (found == NULL) {
            printf(ERROR "ERRO: codigo %s presente no indice não encontrado" CLEAR "\n", noC->codigo);
            free(found);
            return false;
        }
        rnn_filme = found->rnnDados[i_found];
        free(found);

        printf("\n");
        bool flag = imprimirFilme(movies, rnn_filme);
        if (flag)
            printf("\n");
        noC = noC->prox;
    }
    return true;
}

void listarFilmes(FILE *movies, FILE *indexP) {
    short int op;
    bool sucess = false;

    do {
        puts(SUBTITLE "\n----------LISTAGEM DE FILMES----------" CLEAR);
        puts(MENU "1." CLEAR " Listar todos");
        puts(MENU "2." CLEAR " Listar um intervalo");
        puts(MENU "0." CLEAR " Retornar");
        printf(PROMPT "-> " CLEAR INPUT);
        scanf("%hd", &op);
        printf(CLEAR);
        clearBuffer();

        switch (op) {
            case 1:
                listarTodos(movies, indexP);
                sucess = true;
                break;
            case 2:
                listarIntervalo(movies, indexP);
                sucess = true;
                break;
            case 0:
                sucess = true;
                puts(""); //pula uma linha
                break;
            default:
                puts(ERROR "ERRO: Opcao invalida" CLEAR);
                break;
        }
    } while (!sucess);
}

void listarIntervalo(FILE *movies, FILE *indexP) {
    char inicio[TAM_COD + 1], fim[TAM_COD + 1];
    int raiz = getRoot(indexP);

    puts(SUBSUBTITLE "\n------LISTAR INTERVALO------" CLEAR);

    printf(PROMPT "Codigo Inicial: " INPUT);
    scanf("%"STRINGIFY(TAM_COD)"s", inicio);
    printf(CLEAR);
    clearBuffer();

    printf(PROMPT "Codigo Final: " INPUT);
    scanf("%"STRINGIFY(TAM_COD)"s", fim);
    printf(CLEAR);
    clearBuffer();

    //se a lista está vazia
    if (raiz == -1) {
        printf("\n");
        return;
    }

    //garante que o input de inicio e fim é case-insensitive
    for (int i = 0; i < TAM_COD; i++) {
        if (isalpha(inicio[i]))
            inicio[i] = (char) toupper(inicio[i]);
        if (isalpha(fim[i]))
            fim[i] = (char) toupper(fim[i]);
    }

    bool sucess;
    int i;
    NoP *atual = buscaCodigo(indexP, raiz, inicio, &i);

    printf("\n");
    while (atual != NULL && strcmp(fim, atual->chaves[i]) >= 0) {
        //imprimimos o filme
        sucess = imprimirFilme(movies, atual->rnnDados[i]);
        if (sucess)
            printf("\n");

        //passamos para o próximo RNN de dados na lista
        i++;
        if (i == atual->numChaves) { //se chegamos ao final dessa folha
            //zeramos o índice para iterar o laço sobre a próxima folha
            i = 0;
            int prox = atual->prox;
            free(atual);
            atual = lerPagina(indexP, prox); //(será null se tivermos chego ao final da lista)
        }
    }
}

void listarTodos(FILE *movies, FILE *indexP) {
    puts(SUBSUBTITLE "\n--------LISTAR TODOS--------" CLEAR);

    int rnn_folha;
    bool sucess;

    // encontramos o começo da lista formada pelas folhas
    NoP *lista = getListaFolhas(indexP);
    if (lista == NULL) {
        printf("\n");
        return;
    }

    //enquanto há folhas para ler
    while (lista != NULL) {
        //imprimimos todos os filmes listados na folha
        int i = 0;
        while (i < lista->numChaves) {
            sucess = imprimirFilme(movies, lista->rnnDados[i]);
            if (sucess)
                printf("\n");
            i++;
        }
        //passamos para a próxima folha
        rnn_folha = lista->prox;
        free(lista);
        lista = lerPagina(indexP, rnn_folha);
    }
}

bool imprimirFilme(FILE *movies, int rnn) {
    string codigo = malloc(TAM_COD + 1);
    string tituloPT = malloc(TAM_TIT_PT + 1);
    string tituloOG = malloc(TAM_TIT_OG + 1);
    string diretor = malloc(TAM_DIR + 1);
    string ano = malloc(5);
    string pais = malloc(TAM_PAIS + 1);
    char nota = 0;

    fseek(movies, rnn * TAM_FILME, SEEK_SET);

    //tenta ler do arquivo
    if (fscanf(movies, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%c@", codigo, tituloPT, tituloOG, diretor, ano, pais,
               &nota) != EOF) {

        //se a entrada lida foi marcada como apagada, encerra a impressão
        if (codigo[0] == '*' && codigo[1] == '|') {
            free(codigo);
            free(tituloPT);
            free(tituloOG);
            free(diretor);
            free(ano);
            free(pais);
            return false;
        }

        //imprime as informações na tela
        printf(LISTING "Codigo: " CLEAR INFO "%s" CLEAR "\n", codigo);
        printf(LISTING "Titulo em Portugues: " CLEAR INFO "%s" CLEAR "\n", tituloPT);
        printf(LISTING "Titulo Original: " CLEAR INFO "%s" CLEAR "\n", tituloOG);
        printf(LISTING "Diretor: " CLEAR INFO "%s" CLEAR "\n", diretor);
        printf(LISTING "Ano: " CLEAR INFO "%s" CLEAR "\n", ano);
        printf(LISTING "Pais: " CLEAR INFO "%s" CLEAR "\n", pais);
        printf(LISTING "Nota: " CLEAR INFO "%c" CLEAR "\n", nota);
    }
    free(codigo);
    free(tituloPT);
    free(tituloOG);
    free(diretor);
    free(ano);
    free(pais);
    return true;
}

string tituloFromRNN(FILE *movies, int rnn) {
    string titulo = malloc(TAM_TIT_PT + 1);
    fseek(movies, rnn * TAM_FILME, SEEK_SET);
    fscanf(movies, "%*[^@]@%[^@]@", titulo);
    return titulo;
}

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
