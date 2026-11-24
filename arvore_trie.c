#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALFABETO 26
#define MAX_PAL 50

typedef struct No {
    struct No *filhos[ALFABETO];
    int fimPalavra;
} No;

static No *raiz = NULL;

static No *criarNo(void) {
    return calloc(1, sizeof(No));
}

static int temFilhos(const No *no) {
    for (int i = 0; i < ALFABETO; i++)
        if (no->filhos[i] != NULL)
            return 1;
    return 0;
}

static int inserir(const char *palavra, int *reaproveitados, int *criados) {
    No *atual = raiz;
    *reaproveitados = 0;
    *criados = 0;

    for (int i = 0; palavra[i] != '\0'; i++) {
        int idx = palavra[i] - 'a';

        if (atual->filhos[idx] == NULL) {
            atual->filhos[idx] = criarNo();
            if (atual->filhos[idx] == NULL)
                return -1;
            (*criados)++;
        } else {
            (*reaproveitados)++;
        }
        atual = atual->filhos[idx];
    }

    if (atual->fimPalavra)
        return 0;

    atual->fimPalavra = 1;
    return 1;
}

static int buscar(const char *palavra) {
    No *atual = raiz;

    for (int i = 0; palavra[i] != '\0'; i++) {
        atual = atual->filhos[palavra[i] - 'a'];
        if (atual == NULL)
            return 0;
    }
    return atual->fimPalavra ? 1 : 2;
}

static int removerRec(No *no, const char *palavra, int *removido) {
    if (*palavra == '\0') {
        if (!no->fimPalavra)
            return 0;
        no->fimPalavra = 0;
        *removido = 1;
        return !temFilhos(no);
    }

    int idx = *palavra - 'a';
    No *filho = no->filhos[idx];

    if (filho == NULL)
        return 0;

    if (removerRec(filho, palavra + 1, removido)) {
        free(filho);
        no->filhos[idx] = NULL;
    }

    return *removido && !no->fimPalavra && !temFilhos(no);
}

static int remover(const char *palavra) {
    int removido = 0;
    removerRec(raiz, palavra, &removido);
    return removido;
}

static void percorrerRec(const No *no, char *buffer, int nivel, int *contador) {
    if (no->fimPalavra) {
        buffer[nivel] = '\0';
        printf("%s%s", (*contador > 0) ? ", " : "", buffer);
        (*contador)++;
    }

    for (int i = 0; i < ALFABETO; i++) {
        if (no->filhos[i] != NULL) {
            buffer[nivel] = (char)('a' + i);
            percorrerRec(no->filhos[i], buffer, nivel + 1, contador);
        }
    }
}

static void percorrerEmOrdem(void) {
    char buffer[MAX_PAL + 2];
    int contador = 0;

    printf("Palavras em ordem alfabetica: ");
    percorrerRec(raiz, buffer, 0, &contador);

    if (contador == 0)
        printf("(arvore vazia)");
    printf("\n");
}

static void exibirRec(const No *no, char *prefixo, int tam) {
    int ultimo = -1;

    for (int i = 0; i < ALFABETO; i++)
        if (no->filhos[i] != NULL)
            ultimo = i;

    for (int i = 0; i < ALFABETO; i++) {
        if (no->filhos[i] == NULL)
            continue;

        int eUltimo = (i == ultimo);

        printf("%s%s%c%s\n", prefixo, eUltimo ? "`-- " : "|-- ", 'a' + i,
               no->filhos[i]->fimPalavra ? " *" : "");

        memcpy(prefixo + tam, eUltimo ? "    " : "|   ", 4);
        prefixo[tam + 4] = '\0';
        exibirRec(no->filhos[i], prefixo, tam + 4);
        prefixo[tam] = '\0';
    }
}

static void exibirArvore(void) {
    char prefixo[4 * MAX_PAL + 8] = "";

    printf("\n--- Arvore TRIE (* = fim de palavra) ---\n");
    printf("(raiz)\n");
    if (!temFilhos(raiz))
        printf("(vazia)\n");
    else
        exibirRec(raiz, prefixo, 0);
}

static void liberarRec(No *no) {
    if (no == NULL)
        return;
    for (int i = 0; i < ALFABETO; i++)
        liberarRec(no->filhos[i]);
    free(no);
}

static void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

static int lerInteiro(const char *mensagem, int *destino) {
    printf("%s", mensagem);
    if (scanf("%d", destino) != 1) {
        limparBuffer();
        return 0;
    }
    limparBuffer();
    return 1;
}

static int lerPalavra(const char *mensagem, char *destino) {
    char entrada[256];

    printf("%s", mensagem);
    if (scanf("%255s", entrada) != 1)
        return 0;
    limparBuffer();

    size_t tamanho = strlen(entrada);
    if (tamanho > MAX_PAL) {
        printf("Palavra muito longa (maximo %d letras).\n", MAX_PAL);
        return 0;
    }

    for (size_t i = 0; i < tamanho; i++) {
        int c = tolower((unsigned char)entrada[i]);
        if (c < 'a' || c > 'z') {
            printf("Use apenas letras de a a z, sem acentos ou numeros.\n");
            return 0;
        }
        destino[i] = (char)c;
    }
    destino[tamanho] = '\0';
    return 1;
}

static void exibirMenu(void) {
    printf("\n===== ARVORE TRIE =====\n");
    printf("1 - Percorrer em ordem\n");
    printf("2 - Inserir palavra\n");
    printf("3 - Remover palavra\n");
    printf("4 - Buscar palavra\n");
    printf("5 - Exibir arvore\n");
    printf("0 - Sair\n");
}

int main(void) {
    int opcao, reaproveitados, criados;
    char palavra[MAX_PAL + 1];

    raiz = criarNo();
    if (raiz == NULL) {
        printf("Erro: sem memoria.\n");
        return 1;
    }

    do {
        exibirMenu();

        if (!lerInteiro("Opcao: ", &opcao)) {
            if (feof(stdin))
                break;
            printf("Entrada invalida!\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                percorrerEmOrdem();
                break;

            case 2:
                if (!lerPalavra("Palavra a inserir: ", palavra))
                    break;
                switch (inserir(palavra, &reaproveitados, &criados)) {
                    case 1:
                        if (criados == 0)
                            printf("Palavra '%s' inserida: todas as letras ja existiam, "
                                   "so a flag de fim de palavra foi marcada.\n", palavra);
                        else if (reaproveitados == 0)
                            printf("Palavra '%s' inserida: nenhum prefixo em comum, "
                                   "novo ramo criado a partir da raiz (%d nos novos).\n",
                                   palavra, criados);
                        else
                            printf("Palavra '%s' inserida: reaproveitou %d letra(s) do prefixo "
                                   "e criou %d no(s) novo(s).\n",
                                   palavra, reaproveitados, criados);
                        break;
                    case 0:
                        printf("A palavra '%s' ja existe na arvore.\n", palavra);
                        break;
                    default:
                        printf("Erro: sem memoria.\n");
                }
                break;

            case 3:
                if (!lerPalavra("Palavra a remover: ", palavra))
                    break;
                if (remover(palavra))
                    printf("Palavra '%s' removida.\n", palavra);
                else
                    printf("Palavra '%s' nao encontrada.\n", palavra);
                break;

            case 4:
                if (!lerPalavra("Palavra a buscar: ", palavra))
                    break;
                switch (buscar(palavra)) {
                    case 1:
                        printf("A palavra '%s' EXISTE na arvore.\n", palavra);
                        break;
                    case 2:
                        printf("'%s' e apenas prefixo de outra palavra "
                               "(sem flag de fim), nao esta cadastrada.\n", palavra);
                        break;
                    default:
                        printf("A palavra '%s' NAO existe na arvore.\n", palavra);
                }
                break;

            case 5:
                exibirArvore();
                break;

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    liberarRec(raiz);
    return 0;
}
