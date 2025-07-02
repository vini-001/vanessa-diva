// adaptado pra testes  
#ifndef CONVERSAO_H
#define CONVERSAO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CHAVES_MAXIMAS       3
#define FILHOS_MAXIMOS       (CHAVES_MAXIMAS + 1)
#define CHAVES_MINIMAS       (CHAVES_MAXIMAS / 2)

// Estrutura de nó da árvore 2-3-4
typedef struct No234 {
    int numChaves;
    int chaves[CHAVES_MAXIMAS];
    struct No234 *filhos[FILHOS_MAXIMOS];
    bool folha;
    struct No234 *pai;
} No234;

// Estrutura de árvore 2-3-4
typedef struct {
    No234 *raiz;
    int qtdSplit;
    int altura;
    // ADIÇÕES PARA TESTES
    int qtdMerge;
    int qtdRotacao;
} arvore234;

// Estrutura de nó da árvore rubro-negra
typedef struct NO {
    int chave;
    struct NO *fesq;
    struct NO *fdir;
    struct NO *pai;
    char cor;
} noRB;

// Estrutura da árvore RB com sentinelas
struct RB {
    noRB *sentinelaRaiz;
    noRB *sentinelaFolha;
};
typedef struct RB rb;

// -------------------- ÁRVORE 2-3-4 ---------------------------------------

void insereChaveArvore(arvore234* arv, int chave);
int removerChave(arvore234 *arv, int valor);
void imprimirPorNivel(No234 *no, int nivel);
void imprimirVisual(No234 *no, int indent);
void free234(No234 *no);

// ----------------- FUNÇÕES ADICIONAIS PARA TESTE -------------------------
arvore234* criarArvore234();
int contarNos(No234 *no);

// ----------------- CONVERSÃO 2-3-4 → RUBRO-NEGRA -------------------------
rb* converterParaRN(No234 *raiz234);
void atualiza_Sentinela_Folha(rb *T1,rb *T2,rb *novaRB,noRB *aux);
rb* alocaArvore();
noRB* alocaNo(rb *arv,int chave);
void rotacaoEsquerda(rb *arv,noRB *x);
void rotacaoDireita(rb *arv,noRB *y);
void insereNo(rb *arv,noRB *z);
void balanceamentoInsercao(rb *arv,noRB *z);
int removeNo(rb *arv, int valor);
void balanceamentoRemocao(rb *arv, noRB *x, noRB *pai);
noRB* retornaRaiz(rb *arv);
void imprimirPorNivelRN(noRB *no, int nivel);
void imprimirVisualRN(noRB *no, int indent);
void freeRBNodes(rb *tree, noRB *node);
void freeRB(rb *tree);

#endif // CONVERSAO_H