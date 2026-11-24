#include <stdio.h>
#include <stdlib.h>

#define TAM 7

typedef struct No {
    int valor;
    struct No *prox;
} No;

static No *tabela[TAM];

static int hash(int valor) {
    int r = valor % TAM;
    return (r < 0) ? r + TAM : r;
}

static int inserir(int valor) {
    int idx = hash(valor);
    No **pp = &tabela[idx];

    while (*pp != NULL && (*pp)->valor < valor)
        pp = &(*pp)->prox;

    if (*pp != NULL && (*pp)->valor == valor)
        return 0;

    No *novo = malloc(sizeof(No));
    if (novo == NULL)
        return -1;

    novo->valor = valor;
    novo->prox = *pp;
    *pp = novo;
    return 1;
}

static int remover(int valor) {
    No **pp = &tabela[hash(valor)];

    while (*pp != NULL && (*pp)->valor < valor)
        pp = &(*pp)->prox;

    if (*pp == NULL || (*pp)->valor != valor)
        return 0;

    No *alvo = *pp;
    *pp = alvo->prox;
    free(alvo);
    return 1;
}

static int buscar(int valor, int *indice, int *posicao) {
    int idx = hash(valor);
    int pos = 0;

    for (No *p = tabela[idx]; p != NULL && p->valor <= valor; p = p->prox, pos++) {
        if (p->valor == valor) {
            *indice = idx;
            *posicao = pos;
            return 1;
        }
    }
    return 0;
}

static void percorrerEmOrdem(void) {
    int primeiro = 1;

    printf("Valores na ordem dos indices (0 a %d): ", TAM - 1);
    for (int i = 0; i < TAM; i++) {
        for (No *p = tabela[i]; p != NULL; p = p->prox) {
            printf("%s%d", primeiro ? "" : ", ", p->valor);
            primeiro = 0;
        }
    }

    if (primeiro)
        printf("(tabela vazia)");
    printf("\n");
}

static void exibirTabela(void) {
    printf("\n--- Tabela Hash (valor %% %d) ---\n", TAM);
    for (int i = 0; i < TAM; i++) {
        printf("[%d] ", i);
        for (No *p = tabela[i]; p != NULL; p = p->prox)
            printf("-> %d ", p->valor);
        printf("-> NULL\n");
    }
}

static void liberarTabela(void) {
    for (int i = 0; i < TAM; i++) {
        No *p = tabela[i];
        while (p != NULL) {
            No *prox = p->prox;
            free(p);
            p = prox;
        }
        tabela[i] = NULL;
    }
}

static int lerInteiro(const char *mensagem, int *destino) {
    printf("%s", mensagem);
    if (scanf("%d", destino) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return 0;
    }
    return 1;
}

static void exibirMenu(void) {
    printf("\n===== TABELA HASH (MOD 7) =====\n");
    printf("1 - Percorrer em ordem\n");
    printf("2 - Inserir valor\n");
    printf("3 - Remover valor\n");
    printf("4 - Buscar valor\n");
    printf("5 - Exibir arvore (tabela)\n");
    printf("0 - Sair\n");
}

int main(void) {
    int opcao, valor, indice, posicao;

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
                if (!lerInteiro("Valor a inserir: ", &valor)) {
                    printf("Entrada invalida!\n");
                    break;
                }
                switch (inserir(valor)) {
                    case 1:
                        printf("Valor %d inserido no indice %d.\n", valor, hash(valor));
                        break;
                    case 0:
                        printf("Valor %d ja existe na tabela.\n", valor);
                        break;
                    default:
                        printf("Erro: sem memoria.\n");
                }
                break;

            case 3:
                if (!lerInteiro("Valor a remover: ", &valor)) {
                    printf("Entrada invalida!\n");
                    break;
                }
                if (remover(valor))
                    printf("Valor %d removido.\n", valor);
                else
                    printf("Valor %d nao encontrado.\n", valor);
                break;

            case 4:
                if (!lerInteiro("Valor a buscar: ", &valor)) {
                    printf("Entrada invalida!\n");
                    break;
                }
                if (buscar(valor, &indice, &posicao))
                    printf("Valor %d encontrado no indice %d (posicao %d da lista).\n",
                           valor, indice, posicao);
                else
                    printf("Valor %d nao encontrado.\n", valor);
                break;

            case 5:
                exibirTabela();
                break;

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    liberarTabela();
    return 0;
}
