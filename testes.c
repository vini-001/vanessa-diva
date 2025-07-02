#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conversao.h" // Depende da versão modificada deste arquivo

// Função auxiliar para embaralhar um array (algoritmo de Fisher-Yates)
// Usado para garantir que a remoção dos elementos seja em ordem aleatória.
void embaralhar(int *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

/**
 *  Executa o plano de testes para a operação de inserção.
 * * Cria árvores com 100, 1.000, 10.000 e 100.000 elementos aleatórios,
 * coletando e imprimindo as métricas de quantidade de splits, altura final
 * e total de nós ocupados para cada tamanho de amostra.
 */
void teste_insercao() {
    printf("--- Testes de Insercao ---\n");
    printf("| %-20s | %-20s | %-15s | %-20s |\n", "Tamanho da amostra", "Quantidade de split", "Altura da arvore", "Total de blocos");
    printf("|----------------------|----------------------|-----------------|----------------------|\n");

    int tamanhos[] = {100, 1000, 10000, 100000};
    int num_tamanhos = sizeof(tamanhos) / sizeof(int);

    for (int i = 0; i < num_tamanhos; i++) {
        int tamanho = tamanhos[i];
        arvore234* arv = criarArvore234(); // Cria uma árvore nova e limpa

        // Insere 'tamanho' elementos com valores aleatórios
        for (int j = 0; j < tamanho; j++) {
            insereChaveArvore(arv, rand());
        }

        // Coleta e imprime as métricas
        printf("| %-20d | %-20d | %-15d | %-20d |\n",
               tamanho,
               arv->qtdSplit,
               arv->altura,
               contarNos(arv->raiz));

        // Libera a memória da árvore usada neste teste
        free234(arv->raiz);
        free(arv);
    }
    printf("\n");
}

/**
 *  Executa o plano de testes para a operação de remoção.
 * * Para cada percentual (10%, 20%, 35%, 50%), uma árvore base com 10.000
 * elementos é criada. Em seguida, o percentual correspondente de elementos
 * é removido em ordem aleatória. As métricas de rotações, merges, altura final
 * e blocos restantes são coletadas e impressas.
 */
void teste_remocao() {
    printf("--- Testes de Remocao (a partir de arvore com 10.000 elementos) ---\n");
    printf("| %-20s | %-20s | %-20s | %-15s | %-20s |\n", "% Remocao", "Qtde de rotacoes", "Qtde de merges", "Altura final", "Blocos finais");
    printf("|----------------------|----------------------|----------------------|-----------------|----------------------|\n");

    const int tamanho_base = 10000;
    float percentuais[] = {0.10, 0.20, 0.35, 0.50};
    int num_percentuais = sizeof(percentuais) / sizeof(float);

    // Cria um conjunto de dados fixo para garantir que a árvore base seja sempre a mesma
    int* numeros_base = malloc(tamanho_base * sizeof(int));
    for(int i = 0; i < tamanho_base; i++) {
        numeros_base[i] = i;
    }

    for (int i = 0; i < num_percentuais; i++) {
        float p = percentuais[i];
        arvore234* arv = criarArvore234();

        // 1. Monta a árvore base com 10.000 nós.
        for (int j = 0; j < tamanho_base; j++) {
            insereChaveArvore(arv, numeros_base[j]);
        }
        
        // Zera contadores de operação para medir apenas a remoção.
        arv->qtdMerge = 0;
        arv->qtdRotacao = 0;
        
        // Cria uma cópia dos números e a embaralha para remover aleatoriamente.
        int a_remover = (int)(tamanho_base * p);
        int* numeros_para_remover = malloc(tamanho_base * sizeof(int));
        for(int j=0; j<tamanho_base; j++) numeros_para_remover[j] = numeros_base[j];
        embaralhar(numeros_para_remover, tamanho_base);

        // 2. Executa a remoção.
        for (int j = 0; j < a_remover; j++) {
            removerChave(arv, numeros_para_remover[j]);
        }

        // Coleta e imprime as métricas
        printf("| %-20.0f%% | %-20d | %-20d | %-15d | %-20d |\n",
               p * 100,
               arv->qtdRotacao,
               arv->qtdMerge,
               arv->altura,
               contarNos(arv->raiz));

        // Libera a memória usada neste teste
        free234(arv->raiz);
        free(arv);
        free(numeros_para_remover);
    }
    free(numeros_base);
    printf("\n");
}

int main() {
    // Usa uma semente fixa para garantir que os resultados sejam os mesmos a cada execução.
    // Para resultados diferentes, pode-se usar srand(time(NULL));
    srand(time(NULL));

    printf("Executando plano de testes para a Arvore 2-3-4...\n\n");

    teste_insercao();
    teste_remocao();

    return 0;
}