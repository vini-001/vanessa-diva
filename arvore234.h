#ifndef ARVORE234_H
#define ARVORE234_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CHAVES_MAXIMAS       3
#define FILHOS_MAXIMOS       (CHAVES_MAXIMAS + 1)
#define CHAVES_MINIMAS       (CHAVES_MAXIMAS / 2)

typedef struct No234 {
    int numChaves;
    int chaves[CHAVES_MAXIMAS + 1];
    struct No234 *filhos[FILHOS_MAXIMOS + 1];
    bool folha;
} No234;

// -------------------- ÁRVORE 2-3-4 ---------------------------------------

void inserir234(No234 **raizRef, int valor);
int remover234(No234 **raizRef, int valor);

// -------------------- FUNÇÕES DE TESTE E ANÁLISE -------------------------

// Zera todos os contadores (operações e nós).
void zerar_contadores();

// Zera apenas os contadores de operação (split, merge, rotação).
void zerar_contadores_operacao();

// Retorna os valores dos contadores.
unsigned long long int obter_contador_split();
unsigned long long int obter_contador_merge();
unsigned long long int obter_contador_rotacao();
unsigned long long int obter_contador_nos();

// Calcula a altura da árvore (raiz no nível 0).
int calcular_altura(No234* no);

// Libera toda a memória usada pela árvore.
void destruir_arvore(No234** raizRef);

#endif // ARVORE234_H