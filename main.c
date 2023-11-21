#include <stdio.h>
#include <dirent.h>
#include "indices.h"
#include "operacoesUsuario.h"
#include "style.h"

int main() {
    FILE *movies;           //arquivo de dados
    FILE *iprimary;         //arquivo de índice primário
    FILE *ititle;           //arquivo de índice secundário (título em português)
    NoP *raiz;              //índice primário na memória (um único nó da Árvore B+)
    IndiceS *secundarioMem; //índice secundário na memória

    short int op; //operação sendo executada pelo usuário no menu

    //processo de inicialização do programa ----------------------------------------------------------------------------
    opendir("data");
    if (ENOENT == errno)
        mkdir("data");

    /*
    *  verifica se existe um arquivo de dados
    *      se existe, abre para escrita e leitura
    *      se não existe, cria o arquivo para escrita e leitura
    */
    movies = fopen("data/movies.dat", "r+a");
    if (movies == NULL) //se é NULL
        movies = fopen("data/movies.dat", "w+");
    if (movies == NULL) {
        puts(ERROR"\tERRO: Impossível criar arquivo");
        return 1;
    }

    /*
     *  verifica se existem arquivos de índice
     *      se existirem, lê o primário e verifica se o secundário está consistente com o arquivo de dados (flag no cabeçalho)
     *          se está consistente, carrega na memória
     *          senão, refaz o índice na memória
     *      senão, cria os índices na RAM
     */
    /*
    iprimary = fopen("data/iprimary.idx", "r+a");
    if (iprimary != NULL) {
        char flag = fgetc(iprimary); //reads the very first character, the flag
        if (flag == CONSISTENTE)
            raiz = lerP(iprimary); //carrega na memória a partir do arquivo iprimary.idx
        else
            raiz = refazerP(movies); //refaz o índice na memória a partir do arquivo de filmes
    } else {
        iprimary = fopen("data/iprimary.idx", "w+");
        if (iprimary == NULL) {
            puts(ERROR "\tERRO: Impossível criar arquivo");
            return 1;
        }
        raiz = refazerP(movies); //faz o índice na memória a partir do arquivo de filmes
    }
    fseek(iprimary, 0, SEEK_SET);
    fputc(INCONSISTENTE, iprimary); //assume que o arquivo ficará inconsistente durante a execução do programa
    fclose(iprimary);
    */

    ititle = fopen("data/ititle.idx", "r+a");
    if (ititle != NULL) {
        char flag = fgetc(ititle); //reads the very first character, the flag
        if (flag == CONSISTENTE)
            secundarioMem = lerS(ititle); //carrega na memória a partir do arquivo iprimary.idx
        else
            secundarioMem = refazerS(movies); //refaz o índice na memória a partir do arquivo de filmes
    } else {
        ititle = fopen("data/ititle.idx", "w+");
        if (ititle == NULL) {
            puts(ERROR "\tERRO: Impossível criar arquivo");
            return 1;
        }
        secundarioMem = refazerS(movies); //refaz o índice na memória a partir do arquivo de filmes
    }
    fseek(ititle, 0, SEEK_SET);
    fputc(INCONSISTENTE, ititle); //assume que o arquivo ficará inconsistente durante a execução do programa
    fclose(ititle);

    //menu do usuário --------------------------------------------------------------------------------------------------
    printf("\n");
    do {
        puts(TITLE "===========GERENCIADOR DE FILMES==========" CLEAR);
        puts(MENU "1." CLEAR " Inserir filme");
        puts(MENU "2." CLEAR " Remover filme");
        puts(MENU "3." CLEAR " Modificar nota");
        puts(MENU "4." CLEAR " Buscar filme");
        puts(MENU "5." CLEAR " Listar filmes");
        puts(MENU "0." CLEAR " Encerrar programa");
        printf(PROMPT "-> " CLEAR INPUT);
        scanf("%hd", &op);
        clearBuffer();

        switch (op) {
            case 1:
                inserirFilme(movies, &raiz, &secundarioMem);
                break;
            case 2:
                removerFilme(movies, &raiz, &secundarioMem);
                break;
            case 3:
                modificarNota(movies, raiz);
                break;
            case 4:
                buscarFilme(movies, raiz, secundarioMem);
                break;
            case 5:
                listarFilmes(movies);
                break;
            case 0:
                puts(SUCCESS "Encerrando programa..." CLEAR);
                break;
            default:
                puts(ERROR "\tERRO: Opcao invalida" CLEAR "\n");
                break;
        }
    } while (op != 0);

    //finalização do programa ------------------------------------------------------------------------------------------

    //salvar os arquivos
    fclose(movies);
    saveIndiceS(secundarioMem);

    //liberar a memória alocada
    freePagina(raiz);
    freeIndiceS(secundarioMem);

    return 0;
}