#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

// Definição da estrutura Sala (Nó da árvore binária de cômodos)
typedef struct Sala {
    char nome[50];
    char pista[100];  // Pista associada à sala (pode ser uma string vazia)
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Definição da estrutura para a árvore de pistas (BST)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

// Definição da tabela hash (Associa pistas a suspeitos)
typedef struct HashTable {
    char* pista;
    char* suspeito;
    struct HashTable* proximo;
} HashTable;

// Função para criar uma nova sala com nome e pista
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// Função para criar um nó de pista na árvore de pistas (BST)
PistaNode* criarPistaNode(const char* pista) {
    PistaNode* novoNode = (PistaNode*)malloc(sizeof(PistaNode));
    strcpy(novoNode->pista, pista);
    novoNode->esquerda = NULL;
    novoNode->direita = NULL;
    return novoNode;
}

// Função para inserir uma pista na árvore de pistas (BST)
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) {
        return criarPistaNode(pista);
    }

    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else {
        raiz->direita = inserirPista(raiz->direita, pista);
    }

    return raiz;
}

// Função para criar uma tabela hash
unsigned int hash(const char* pista) {
    unsigned int hashValue = 0;
    for (int i = 0; pista[i] != '\0'; i++) {
        hashValue = (hashValue << 5) + pista[i];
    }
    return hashValue % TABLE_SIZE;
}

// Função para inserir uma pista e seu suspeito na tabela hash
void inserirNaHash(HashTable* tabela[], const char* pista, const char* suspeito) {
    unsigned int index = hash(pista);
    HashTable* novoNode = (HashTable*)malloc(sizeof(HashTable));
    novoNode->pista = strdup(pista);
    novoNode->suspeito = strdup(suspeito);
    novoNode->proximo = tabela[index];
    tabela[index] = novoNode;
}

// Função para encontrar o suspeito associado a uma pista
char* encontrarSuspeito(HashTable* tabela[], const char* pista) {
    unsigned int index = hash(pista);
    HashTable* node = tabela[index];
    while (node != NULL) {
        if (strcmp(node->pista, pista) == 0) {
            return node->suspeito;
        }
        node = node->proximo;
    }
    return NULL;
}

// Função para explorar as salas e coletar pistas
void explorarSalasComPistas(Sala* salaAtual, PistaNode** arvorePistas, HashTable* tabela[]) {
    char opcao;
    Sala* salaVisitada = salaAtual;

    // Enquanto não chegamos a um nó folha
    while (salaVisitada != NULL) {
        printf("\nVocê está na sala: %s\n", salaVisitada->nome);
        if (salaVisitada->pista[0] != '\0') {
            printf("Você encontrou uma pista: %s\n", salaVisitada->pista);
            *arvorePistas = inserirPista(*arvorePistas, salaVisitada->pista);  // Adiciona a pista na árvore BST
            // Associa a pista ao suspeito na tabela hash
            if (strcmp(salaVisitada->nome, "Cozinha") == 0) {
                inserirNaHash(tabela, salaVisitada->pista, "Suspeito A");
            } else if (strcmp(salaVisitada->nome, "Biblioteca") == 0) {
                inserirNaHash(tabela, salaVisitada->pista, "Suspeito B");
            } else {
                inserirNaHash(tabela, salaVisitada->pista, "Suspeito C");
            }
        }

        if (salaVisitada->esquerda == NULL && salaVisitada->direita == NULL) {
            printf("Você chegou a um cômodo sem mais caminhos. Fim da exploração!\n");
            break;
        }

        printf("Escolha um caminho:\n");
        printf("e: Ir para a esquerda\n");
        printf("d: Ir para a direita\n");
        printf("s: Sair\n");
        printf("Escolha: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' || opcao == 'E') {
            if (salaVisitada->esquerda != NULL) {
                salaVisitada = salaVisitada->esquerda;
            } else {
                printf("Não há caminho à esquerda!\n");
            }
        } else if (opcao == 'd' || opcao == 'D') {
            if (salaVisitada->direita != NULL) {
                salaVisitada = salaVisitada->direita;
            } else {
                printf("Não há caminho à direita!\n");
            }
        } else if (opcao == 's' || opcao == 'S') {
            printf("Saindo da exploração...\n");
            break;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// Função para exibir as pistas coletadas em ordem alfabética
void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) {
        return;
    }

    // Exibir as pistas da subárvore esquerda
    exibirPistas(raiz->esquerda);

    // Exibir a pista do nó atual
    printf("%s\n", raiz->pista);

    // Exibir as pistas da subárvore direita
    exibirPistas(raiz->direita);
}

// Função para verificar se o suspeito tem pistas suficientes
int verificarSuspeitoFinal(HashTable* tabela[], PistaNode* arvorePistas, const char* suspeito) {
    PistaNode* node = arvorePistas;
    int pistasEncontradas = 0;

    while (node != NULL) {
        char* suspeitoAssociado = encontrarSuspeito(tabela, node->pista);
        if (suspeitoAssociado != NULL && strcmp(suspeitoAssociado, suspeito) == 0) {
            pistasEncontradas++;
        }
        node = node->direita;
    }

    return pistasEncontradas >= 2;  // Verifica se há pelo menos 2 pistas apontando para o suspeito
}

int main() {
    // Criando as salas (nós) da mansão com pistas associadas
    Sala* hall = criarSala("Hall de entrada", "Pista 1: A chave está no jardim.");
    Sala* salaEsquerda = criarSala("Sala de estar", "Pista 2: O culpado está mais perto do que você imagina.");
    Sala* salaDireita = criarSala("Cozinha", "Pista 3: A faca está na mesa.");
    Sala* salaEsquerda2 = criarSala("Jardim", "Pista 4: Verifique o banco da praça.");
    Sala* salaDireita2 = criarSala("Biblioteca", "Pista 5: O livro da prateleira esconde um segredo.");

    // Definindo as conexões entre as salas
    hall->esquerda = salaEsquerda;
    hall->direita = salaDireita;
    salaEsquerda->esquerda = salaEsquerda2;
    salaDireita->direita = salaDireita2;

    // Árvore para armazenar as pistas coletadas
    PistaNode* arvorePistas = NULL;

    // Tabela hash para associar pistas aos suspeitos
    HashTable* tabela[TABLE_SIZE] = { NULL };

    // Começando a exploração a partir do Hall de entrada
    printf("Bem-vindo ao Detective Quest - Fase Mestre!\n");
    explorarSalasComPistas(hall, &arvorePistas, tabela);

    // Exibindo todas as pistas coletadas em ordem alfabética
    printf("\nPistas coletadas durante a exploração:\n");
    exibirPistas(arvorePistas);

    // Solicitar ao jogador a acusação de um suspeito
    char suspeito[50];
    printf("\nQuem você acha que é o culpado? ");
    scanf(" %49[^\n]", suspeito);

    // Verificar se há pistas suficientes para sustentar a acusação
    if (verificarSuspeitoFinal(tabela, arvorePistas, suspeito)) {
        printf("A acusação de %s está correta! As pistas são consistentes.\n", suspeito);
    } else {
        printf("A acusação de %s não é suportada pelas pistas. Continue investigando!\n", suspeito);
    }

    // Liberar a memória alocada
    free(hall);
    free(salaEsquerda);
    free(salaDireita);
    free(salaEsquerda2);
    free(salaDireita2);

    return 0;
}
