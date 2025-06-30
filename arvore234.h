#ifndef ARVORE234_H
#define ARVORE234_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// --- Constantes para a Árvore 2-3-4 (B-Tree de Ordem 4) ---
#define CHAVES_MAXIMAS       3
#define FILHOS_MAXIMOS       (CHAVES_MAXIMAS + 1)
#define CHAVES_MINIMAS       (CHAVES_MAXIMAS / 2)

// --- Estruturas de Dados ---

// Estrutura de nó da árvore 2-3-4
typedef struct No234 {
    int numChaves;
    int chaves[CHAVES_MAXIMAS];
    struct No234 *filhos[FILHOS_MAXIMOS];
    bool folha;
} No234;

// Estrutura de nó da árvore rubro-negra
typedef struct NO {
    int chave;
    struct NO *fesq;
    struct NO *fdir;
    struct NO *pai;
    char cor;      // 'P' = preto, 'V' = vermelho
} noRB;

// Estrutura da árvore RB com sentinelas
typedef struct RB {
    noRB *sentinelaRaiz;
    noRB *sentinelaFolha;
} rb;

// -------------------- ÁRVORE 2-3-4 ---------------------------------------

// Aloca e inicializa um nó 2-3-4
No234* criarNo(bool folha);

// Inserção em árvore 2-3-4
void inserir234(No234 **raizRef, int valor);

// Remoção em árvore 2-3-4
int remover234(No234 **raizRef, int valor);

// Impressões
void imprimirPorNivel(No234 *no, int nivel);
void imprimirVisual(No234 *no, int indent);

// ----------------- CONVERSÃO 2-3-4 → RUBRO-NEGRA -------------------------

// Converte 2-3-4 para RB
rb* converterParaRN(No234 *raiz234);

// CORREÇÃO: Protótipo ajustado para 3 argumentos.
void atualiza_Sentinela_Folha(rb *T, noRB *aux, noRB* sentinela);

// -------------------- ÁRVORE RUBRO-NEGRA ---------------------------------

// Cria uma RB vazia com sentinelas
rb* alocaArvore();

// Aloca um nó RB
noRB* alocaNo(rb *arv,int chave);

// Rotação à esquerda / direita
void rotacaoEsquerda(rb *arv,noRB *x);
void rotacaoDireita(rb *arv,noRB *y);

// Insere e balanceia
void insereNo(rb *arv,noRB *z);
void balanceamentoInsercao(rb *arv,noRB *z);

// Remove e balanceia
int removeNo(rb *arv, int valor);
// CORREÇÃO: Protótipo ajustado para 2 argumentos.
void balanceamentoRemocao(rb *arv, noRB *x);

// Acessores e impressão
noRB* retornaRaiz(rb *arv);
void imprimirPorNivelRN(noRB *no, int nivel);
void imprimirVisualRN(noRB *no, int indent);


#endif // ARVORE234_H