#include "conversao.h"

/* =========================================================================
                  FUNÇÕES REFERENTES À ÁRVORE 2-3-4 (B-TREE ORDEM 4) 
========================================================================= */

// Estrutura auxiliar para promoção de chaves durante inserção
typedef struct {
    bool houvePromocao;
    int chavePromovida;
    No234 *direitoNovo;
} Promocao;

// Cria um novo nó 2-3-4
// Retorna ponteiro para nó criado
No234* criarNo(bool folha) {
    No234 *no = malloc(sizeof(No234));
    no->numChaves = 0;
    no->folha = folha;
    no->pai = NULL;
    for (int i = 0; i < CHAVES_MAXIMAS; i++)
        no->chaves[i] = -1;
    for (int i = 0; i < FILHOS_MAXIMOS; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

// Insere chave em nó folha
void insereChaveNoFolha(No234* noAlvo, int chave){
    int i = noAlvo->numChaves - 1;

    for(; i >= 0; i--){
        if(noAlvo->chaves[i] > chave)
            noAlvo->chaves[i + 1] = noAlvo->chaves[i];

        else if(noAlvo->chaves[i] < chave)
            break;
    }

    noAlvo->chaves[i + 1] = chave;

    noAlvo->numChaves++;
}

// Insere chave em nó interno
void insereChaveNoInterno(No234* noAlvo, int chave, No234* filhoEsquerdo, No234* filhoDireito){
    int i = noAlvo->numChaves - 1;

    for(; i >= 0; i--){
        if(noAlvo->chaves[i] > chave){
            noAlvo->chaves[i + 1] = noAlvo->chaves[i];
            noAlvo->filhos[i + 2] = noAlvo->filhos[i + 1];
        }

        else if(noAlvo->chaves[i] < chave)
            break;
    }

    noAlvo->chaves[i + 1] = chave;

    noAlvo->filhos[i + 1] = filhoEsquerdo;
    noAlvo->filhos[i + 2] = filhoDireito;
    filhoEsquerdo->pai = noAlvo;
    filhoDireito ->pai = noAlvo;

    noAlvo->numChaves++;
}


// Divide nó cheio em dois nós e sobe a chave intermediária
// Retorna ponteiro para o nó que contém a chave inserida
No234* divideNo(arvore234* arv, No234* pai, No234* noCheio, int chave){
    arv->qtdSplit++;

    int oldNumChaves = noCheio->numChaves;

    if(!pai){
        pai = criarNo(0); 
        arv->raiz = pai;   
        for (int i = 0; i < FILHOS_MAXIMOS; i++)
            pai->filhos[i] = NULL;
        noCheio->pai = pai;
        arv->altura++;
    }

    int meio = CHAVES_MAXIMAS / 2;
    int intermediario = noCheio->chaves[meio];

    No234* noMaior = criarNo(noCheio->folha); 
    noMaior->pai = pai; 

    int j = 0;
    for(int i = meio + 1; i < CHAVES_MAXIMAS; i++, j++){
        noMaior->chaves[j] = noCheio->chaves[i];
        noMaior->numChaves++;

        noCheio->chaves[i] = -1;
        noCheio->numChaves--;
    }

    if(noCheio->folha == 0){
        int totalFilhos = oldNumChaves + 1;
        for (int i = meio + 1, k = 0; i < totalFilhos; i++, k++){
            noMaior->filhos[k] = noCheio->filhos[i];
            if (noMaior->filhos[k])
                noMaior->filhos[k]->pai = noMaior;
            noCheio->filhos[i] = NULL;
        }
        
    }

    if(pai->numChaves == CHAVES_MAXIMAS){
        pai = divideNo(arv, pai->pai, pai, chave);

        noCheio->pai = pai;
        noMaior->pai = pai;
    }

    insereChaveNoInterno(pai, intermediario, noCheio, noMaior);

    noCheio->chaves[meio] = -1;
    noCheio->numChaves--;

    if(chave < intermediario)
        return noCheio;
    else
        return noMaior;
}

// Interface pública de inserção
void insereChaveArvore(arvore234* arv, int chave){
    if (arv->raiz == NULL) {
        arv->raiz   = criarNo(true);
        arv->altura = 1;      
    }
    
    No234* aux = arv->raiz;
    int index;

    while(aux->folha != 1) {
        index = FILHOS_MAXIMOS - 1;

        for(int i = 0; i < CHAVES_MAXIMAS; i++) {
            if(aux->chaves[i] == -1 || chave < aux->chaves[i]) {
                index = i;
                break;
            }
        }

        aux = aux->filhos[index];
    }

    if(aux->numChaves == CHAVES_MAXIMAS){
        aux = divideNo(arv, aux->pai, aux, chave);

        insereChaveNoFolha(aux, chave);
    }

    //Se não está cheio, apenas inserir
    else{
        insereChaveNoFolha(aux, chave);
    }
}

// Funções auxiliares para remoção 
// (mesclarFilhos, emprestarDaEsquerda, emprestarDaDireita)
static void mesclarFilhos(No234 *noPai, int indice) {
    No234 *esq = noPai->filhos[indice];
    No234 *dir = noPai->filhos[indice + 1];

    // Mover chave do pai para o filho esquerdo
    esq->chaves[esq->numChaves++] = noPai->chaves[indice];

    // Copiar chaves do filho direito para o esquerdo
    for (int i = 0; i < dir->numChaves; i++) {
        esq->chaves[esq->numChaves++] = dir->chaves[i];
    }

    // Se não for folha, mover filhos também e atualizar pai
    if (!esq->folha) {
        // calcula onde começa a inserção dos filhos de dir
        int start = esq->numChaves - dir->numChaves;
        for (int i = 0; i <= dir->numChaves; i++) {
            esq->filhos[start + i] = dir->filhos[i];
            if (esq->filhos[start + i])
                esq->filhos[start + i]->pai = esq;
        }
    }

    // Ajustar nó pai: remover chave e ponteiro direito
    for (int i = indice; i < noPai->numChaves - 1; i++) {
        noPai->chaves[i]     = noPai->chaves[i + 1];
        noPai->filhos[i + 1] = noPai->filhos[i + 2];
    }
    noPai->numChaves--;

    // Zera a referência ao nó que foi mesclado e libera memória
    noPai->filhos[indice + 1] = NULL;
    free(dir);
}


// Retorna true se conseguiu emprestar, false caso contrário
static bool emprestarDaEsquerda(No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmao = noPai->filhos[indice - 1];

    if (irmao->numChaves > CHAVES_MINIMAS) {
        // Deslocar tudo em noAtual para a direita
        for (int i = noAtual->numChaves - 1; i >= 0; i--) {
            noAtual->chaves[i + 1] = noAtual->chaves[i];
        }
        if (!noAtual->folha) {
            for (int i = noAtual->numChaves; i >= 0; i--) {
                noAtual->filhos[i + 1] = noAtual->filhos[i];
            }
        }

        // Mover chave do pai para noAtual
        noAtual->chaves[0] = noPai->chaves[indice - 1];
        noAtual->numChaves++;

        // Atualizar chave no pai com última de irmao
        noPai->chaves[indice - 1] = irmao->chaves[irmao->numChaves - 1];
        if (!irmao->folha) {
            noAtual->filhos[0] = irmao->filhos[irmao->numChaves];
        }
        irmao->numChaves--;
        return true;
    }
    return false;
}

// Retorna true se conseguiu emprestar, false caso contrário
static bool emprestarDaDireita(No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmao = noPai->filhos[indice + 1];

    if (irmao->numChaves > CHAVES_MINIMAS) {
        // Mover chave do pai para noAtual
        noAtual->chaves[noAtual->numChaves++] = noPai->chaves[indice];

        // Ajustar filhos
        if (!noAtual->folha) {
            noAtual->filhos[noAtual->numChaves] = irmao->filhos[0];
        }

        // Atualizar chave no pai com primeira de irmao
        noPai->chaves[indice] = irmao->chaves[0];

        // Deslocar tudo em irmao para a esquerda
        for (int i = 0; i < irmao->numChaves - 1; i++) {
            irmao->chaves[i] = irmao->chaves[i + 1];
        }
        if (!irmao->folha) {
            for (int i = 0; i < irmao->numChaves; i++) {
                irmao->filhos[i] = irmao->filhos[i + 1];
            }
        }
        irmao->numChaves--;
        return true;
    }
    return false;
}

// Função recursiva de remoção
// Retorna 1 se removeu, 0 caso contrário
static int removerRec(No234 *noAtual, int valor) {
    int pos = 0;
    // Encontrar posição do valor
    while (pos < noAtual->numChaves && valor > noAtual->chaves[pos]) {
        pos++;
    }

    // Caso folha
    if (noAtual->folha) {
        // Remover valor na folha se existir
        if (pos < noAtual->numChaves && noAtual->chaves[pos] == valor) {
            for (int j = pos; j < noAtual->numChaves - 1; j++) {
                noAtual->chaves[j] = noAtual->chaves[j + 1];
            }
            noAtual->numChaves--;
            return 1; // remoção ocorreu
        }
        return 0; // valor não encontrado
    }

    int removido = 0;
    // Caso nó interno
    if (pos < noAtual->numChaves && noAtual->chaves[pos] == valor) {
        // Substituir pelo predecessor
        No234 *pre = noAtual->filhos[pos];
        while (!pre->folha) {
            pre = pre->filhos[pre->numChaves];
        }
        int pred = pre->chaves[pre->numChaves - 1];
        noAtual->chaves[pos] = pred;
        // Remover predecessor na subárvore esquerda
        removido = removerRec(noAtual->filhos[pos], pred);
    } else {
        // Descer para remover no filho adequado
        removido = removerRec(noAtual->filhos[pos], valor);
    }

    // Após tentativa de remoção, verificar underflow
    No234 *filho = noAtual->filhos[pos];
    if (filho->numChaves < CHAVES_MINIMAS) {
        if (pos > 0 && emprestarDaEsquerda(noAtual, pos)) {
            // empréstimo à esquerda equilibra
        } else if (pos < noAtual->numChaves && emprestarDaDireita(noAtual, pos)) {
            // empréstimo à direita equilibra
        } else if (pos > 0) {
            mesclarFilhos(noAtual, pos - 1);
        } else {
            mesclarFilhos(noAtual, pos);
        }
    }

    return removido;
}

// Interface pública de remoção
// Retorna 1 se removeu, 0 caso contrário
static int remover234(No234 **raizRef, int valor) {
    if (*raizRef == NULL) {
        return 0;
    }

    int ok = removerRec(*raizRef, valor);

    // Se raiz ficou vazia e não é folha, descer uma altura
    if (ok && (*raizRef)->numChaves == 0 && !(*raizRef)->folha) {
        No234 *antigaRaiz = *raizRef;
        No234 *novaRaiz   = antigaRaiz->filhos[0];
        novaRaiz->pai    = NULL;
        *raizRef = novaRaiz;
        free(antigaRaiz);
    }

    return ok;
}

// Calcula altura de subárvore
// Retorna altura da subárvore
static int calculaAlturaNo(No234 *no) {
    if (!no) return 0;
    int maior = 0;
    // para cada filho, mede sua altura
    for (int i = 0; i <= no->numChaves; i++) {
        int h = calculaAlturaNo(no->filhos[i]);
        if (h > maior) maior = h;
    }
    return maior + 1;
}

// Interface pública de remoção
// Retorna 1 se removeu, 0 caso contrário
int removerChave(arvore234 *arv, int valor) {
    int ok = remover234(&arv->raiz, valor);
    if (ok) {
        arv->altura = calculaAlturaNo(arv->raiz);
    }
    return ok;
}

// Impressão em níveis de árvore 2-3-4
void imprimirPorNivel(No234 *no, int nivel) {
    if (!no) return;
    printf("Nivel %d: ", nivel);
    for (int i = 0; i < no->numChaves; i++) {
        printf("%d ", no->chaves[i]);
    }
    printf("\n");
    for (int i = 0; i <= no->numChaves; i++) {
        imprimirPorNivel(no->filhos[i], nivel + 1);
    }
}

// Impressão visual de árvore 2-3-4
void imprimirVisual(No234 *no, int indent) {
    if (!no) return;
    for (int i = no->numChaves; i >= 0; i--) {
        if (no->filhos[i]) {
            imprimirVisual(no->filhos[i], indent + 4);
        }
        if (i > 0) {
            for (int k = 0; k < indent; k++) {
                putchar(' ');
            }
            printf("%d\n", no->chaves[i - 1]);
        }
    }
}

// Liberação de memória de árvore 2-3-4
void free234(No234 *no) {
  if (!no) return;
  for (int i = 0; i <= no->numChaves; i++)
    free234(no->filhos[i]);
  free(no);
}

/* =========================================================================
         CONVERSÃO DIRETA ÁRVORE 2-3-4 → ÁRVORE RUBRO-NEGRA (RB)
=========================================================================*/

// Inicializa árvore RB com sentinelas.
// Retorna ponteiro para árvore criada.
rb* alocaArvore(){
    rb *nova=(rb*)malloc(sizeof(rb));
    nova->sentinelaFolha=(noRB*)malloc(sizeof(noRB));
    nova->sentinelaFolha->cor='P';
    nova->sentinelaFolha->fesq=nova->sentinelaFolha->fdir=nova->sentinelaFolha->pai=NULL;
    nova->sentinelaRaiz=(noRB*)malloc(sizeof(noRB));
    nova->sentinelaRaiz->cor='P';
    nova->sentinelaRaiz->pai=NULL;
    nova->sentinelaRaiz->fesq=NULL;
    nova->sentinelaRaiz->fdir=nova->sentinelaFolha;
    return nova;
}

// Cria nó RB com chave e filhos na sentinela de folha.
// Retorna ponteiro para nó criado.
noRB* alocaNo(rb *arv,int chave){
    noRB *novoNo=(noRB*)malloc(sizeof(noRB));
    if(!novoNo) return NULL;
    novoNo->chave=chave;
    novoNo->fesq=novoNo->fdir=arv->sentinelaFolha;
    novoNo->pai=NULL;
    novoNo->cor='V';
    return novoNo;
}

// Aponta os ponteiros nulos de folhas para a sentinelaFolha 
void atualiza_Sentinela_Folha(rb *T1,rb *T2,rb *novaRB,noRB *aux){
    if(aux==T1->sentinelaFolha||aux==T2->sentinelaFolha) return;
    if(aux->fesq==T1->sentinelaFolha||aux->fesq==T2->sentinelaFolha)
        aux->fesq=novaRB->sentinelaFolha;
    else
        atualiza_Sentinela_Folha(T1,T2,novaRB,aux->fesq);
    if(aux->fdir==T1->sentinelaFolha||aux->fdir==T2->sentinelaFolha)
        aux->fdir=novaRB->sentinelaFolha;
    else
        atualiza_Sentinela_Folha(T1,T2,novaRB,aux->fdir);
}

// Função recursiva que converte nó 2-3-4 em subárvore RB
// Retorna ponteiro para nó RB criado.
static noRB* converterNo234(No234 *n234, rb *arvRN) {
    // Caso base: nó 2-3-4 é nulo, retorna sentinela de folha da RB
    if (!n234) return arvRN->sentinelaFolha;

    // Converter subárvores 2-3-4 em subárvores RB
    // Armazenar em array para facilitar a construção da RB
    noRB *sub[5];
    for (int i = 0; i <= n234->numChaves; i++) {
        sub[i] = converterNo234(n234->filhos[i], arvRN);
    }

    noRB *raizLocal;
    noRB *vermelhoEsq = NULL;
    noRB *vermelhoDir = NULL;

    if (n234->numChaves == 1) {
        // Se só tem uma chave, a raiz local é preta
        raizLocal = alocaNo(arvRN, n234->chaves[0]); raizLocal->cor = 'P';
    } else if (n234->numChaves == 2) {
        // Se 2 chaves → raiz com a chave maior (preta) + um filho vermelho à esquerda
        raizLocal = alocaNo(arvRN, n234->chaves[1]); raizLocal->cor = 'P';
        vermelhoEsq = alocaNo(arvRN, n234->chaves[0]); vermelhoEsq->cor = 'V';
    } else {
        // Se 3 chaves → raiz com chave do meio (preta) + dois filhos vermelhos
        raizLocal = alocaNo(arvRN, n234->chaves[1]); raizLocal->cor = 'P';
        vermelhoEsq = alocaNo(arvRN, n234->chaves[0]); vermelhoEsq->cor = 'V';
        vermelhoDir = alocaNo(arvRN, n234->chaves[2]); vermelhoDir->cor = 'V';
    }

    // Liga os nós vermelhos criados (se existirem) às subárvores apropriadas
    if (vermelhoEsq) {
        // Conecta subárvores 0 e 1 ao nó vermelho esquerdo
        vermelhoEsq->fesq = sub[0]; vermelhoEsq->fdir = sub[1];
        sub[0]->pai = vermelhoEsq; sub[1]->pai = vermelhoEsq;
        vermelhoEsq->pai = raizLocal;
    }
    if (vermelhoDir) {
        // Conecta subárvores 2 e 3 ao nó vermelho direito
        vermelhoDir->fesq = sub[2]; vermelhoDir->fdir = sub[3];
        sub[2]->pai = vermelhoDir; sub[3]->pai = vermelhoDir;
        vermelhoDir->pai = raizLocal;
    }

    //    Conecta raizLocal às suas duas “filhas”:
    //    - Se houver dois nós vermelhos: eles viram filhos direto e esquerdo.
    //    - Se houver só um nó vermelho (caso de 2 chaves): o outro ponteiro
    //      aponta para a subárvore remanescente.
    if (vermelhoEsq && vermelhoDir) {
        raizLocal->fesq = vermelhoEsq; raizLocal->fdir = vermelhoDir;
    } else if (vermelhoEsq) {
        raizLocal->fesq = vermelhoEsq; raizLocal->fdir = sub[2]; sub[2]->pai = raizLocal;
    } else {
        raizLocal->fesq = sub[0]; raizLocal->fdir = sub[1];
        sub[0]->pai = raizLocal; sub[1]->pai = raizLocal;
    }

    // Devolve a raiz da subárvore RB correspondente a n234
    return raizLocal;
}

// Interface pública para converter 2-3-4 em RB
// Retorna ponteiro para árvore RB criada.
rb* converterParaRN(No234 *raiz234) {
    if (!raiz234) return NULL;
    rb *novaRN = alocaArvore();
    noRB *raizRB = converterNo234(raiz234, novaRN);
    novaRN->sentinelaRaiz->fdir = raizRB; 
    raizRB->pai = NULL;
    atualiza_Sentinela_Folha(novaRN, novaRN, novaRN, raizRB);
    novaRN->sentinelaRaiz->fesq = novaRN->sentinelaFolha;
    return novaRN;
}


/* =========================================================================
                  FUNÇÕES REFERENTES À ÁRVORE RUBRO-NEGRA  =========================================================================*/

// Rotação simples a esquerda
void rotacaoEsquerda(rb *arv,noRB *x){
    noRB *y=x->fdir;
    x->fdir=y->fesq;
    if(y->fesq!=arv->sentinelaFolha) y->fesq->pai=x;
    y->pai=x->pai;
    if(!x->pai) arv->sentinelaRaiz->fdir=y;
    else if(x==x->pai->fesq) x->pai->fesq=y;
    else x->pai->fdir=y;
    y->fesq=x;
    x->pai=y;
}

// Rotação simples a direita
void rotacaoDireita(rb *arv,noRB *y){
    noRB *x=y->fesq;
    y->fesq=x->fdir;
    if(x->fdir!=arv->sentinelaFolha) x->fdir->pai=y;
    x->pai=y->pai;
    if(!y->pai) arv->sentinelaRaiz->fdir=x;
    else if(y==y->pai->fdir) y->pai->fdir=x;
    else y->pai->fesq=x;
    x->fdir=y;
    y->pai=x;
}

// Balanceamento após inserção de z
void balanceamentoInsercao(rb *arv,noRB *z){
    noRB *y;
    while(z!=arv->sentinelaRaiz->fdir && z->pai->cor=='V'){
        if(z->pai==z->pai->pai->fesq){
            y=z->pai->pai->fdir;
            if(y->cor=='V'){
                z->pai->cor='P';
                y->cor='P';
                z->pai->pai->cor='V';
                z=z->pai->pai;
            } else {
                if(z==z->pai->fdir){
                    z=z->pai;
                    rotacaoEsquerda(arv,z);
                }
                z->pai->cor='P';
                z->pai->pai->cor='V';
                rotacaoDireita(arv,z->pai->pai);
            }
        } else {
            y=z->pai->pai->fesq;
            if(y->cor=='V'){
                z->pai->cor='P';
                y->cor='P';
                z->pai->pai->cor='V';
                z=z->pai->pai;
            } else {
                if(z==z->pai->fesq){
                    z=z->pai;
                    rotacaoDireita(arv,z);
                }
                z->pai->cor='P';
                z->pai->pai->cor='V';
                rotacaoEsquerda(arv,z->pai->pai);
            }
        }
    }
    arv->sentinelaRaiz->fdir->cor='P';
}

// Inserção de nó na árvore rubro-negra
void insereNo(rb *arv,noRB *z){
    noRB *y=NULL;
    noRB *x=arv->sentinelaRaiz->fdir;
    while(x!=arv->sentinelaFolha){
        y=x;
        x=(z->chave<x->chave?x->fesq:x->fdir);
    }
    z->pai=y;
    if(!y) arv->sentinelaRaiz->fdir=z;
    else if(z->chave<y->chave) y->fesq=z;
    else y->fdir=z;
    z->fesq=z->fdir=arv->sentinelaFolha;
    z->cor='V';
    balanceamentoInsercao(arv,z);
}

// Balanceamento após remoção de nó x
void balanceamentoRemocao(rb *arv,noRB *x,noRB *pai){
    noRB *w;
    while(x!=arv->sentinelaRaiz->fdir && x->cor=='P'){
        if(x==pai->fesq){
            w=pai->fdir;
            if(w->cor=='V'){
                w->cor='P';
                pai->cor='V';
                rotacaoEsquerda(arv,pai);
                w=pai->fdir;
            }
            if(w->fesq->cor=='P' && w->fdir->cor=='P'){
                w->cor='V';
                x=pai;
                pai=x->pai;
            } else {
                if(w->fdir->cor=='P'){
                    w->fesq->cor='P';
                    w->cor='V';
                    rotacaoDireita(arv,w);
                    w=pai->fdir;
                }
                w->cor=pai->cor;
                pai->cor='P';
                w->fdir->cor='P';
                rotacaoEsquerda(arv,pai);
                x=arv->sentinelaRaiz->fdir;
                break;
            }
        } else {
            w=pai->fesq;
            if(w->cor=='V'){
                w->cor='P';
                pai->cor='V';
                rotacaoDireita(arv,pai);
                w=pai->fesq;
            }
            if(w->fdir->cor=='P' && w->fesq->cor=='P'){
                w->cor='V';
                x=pai;
                pai=x->pai;
            } else {
                if(w->fesq->cor=='P'){
                    w->fdir->cor='P';
                    w->cor='V';
                    rotacaoEsquerda(arv,w);
                    w=pai->fesq;
                }
                w->cor=pai->cor;
                pai->cor='P';
                w->fesq->cor='P';
                rotacaoDireita(arv,pai);
                x=arv->sentinelaRaiz->fdir;
                break;
            }
        }
    }
    x->cor='P';
}

// Função para remover um nó da árvore rubro-negra
// Retorna 1 se removeu, 0 caso contrário
int removeNo(rb *arv, int valor) {
    noRB *z = arv->sentinelaRaiz->fdir;
    noRB *x, *y;
    char corOriginal;

    // busca do nó a remover
    while (z != arv->sentinelaFolha && z->chave != valor)
        z = (valor < z->chave ? z->fesq : z->fdir);
    if (z == arv->sentinelaFolha)
        return 0;  // não encontrado

    y = z;
    corOriginal = y->cor;

    // caso com 0 ou 1 filho — mesma lógica de antes
    if (z->fesq == arv->sentinelaFolha) {
        x = z->fdir;
        if (!z->pai)
            arv->sentinelaRaiz->fdir = x;
        else if (z == z->pai->fesq)
            z->pai->fesq = x;
        else
            z->pai->fdir = x;
        x->pai = z->pai;

    } else if (z->fdir == arv->sentinelaFolha) {
        x = z->fesq;
        if (!z->pai)
            arv->sentinelaRaiz->fdir = x;
        else if (z == z->pai->fesq)
            z->pai->fesq = x;
        else
            z->pai->fdir = x;
        x->pai = z->pai;

    } else {
        // caso com dois filhos: usar predecessor 
        y = z->fesq;
        // desce até o mais à direita na subárvore esquerda
        while (y->fdir != arv->sentinelaFolha)
            y = y->fdir;

        corOriginal = y->cor;
        x = y->fesq;

        if (y->pai == z) {
            x->pai = y;
        } else {
            // substitui y pelo seu filho esquerdo
            y->pai->fdir = x;
            x->pai = y->pai;
            // conecta filho esquerdo de z como filho esquerdo de y
            y->fesq = z->fesq;
            y->fesq->pai = y;
        }

        // transplanta y no lugar de z
        if (!z->pai)
            arv->sentinelaRaiz->fdir = y;
        else if (z == z->pai->fesq)
            z->pai->fesq = y;
        else
            z->pai->fdir = y;

        y->pai = z->pai;
        // conecta filho direito de z em y
        y->fdir = z->fdir;
        y->fdir->pai = y;
        // preserva a cor de z
        y->cor = z->cor;
    }

    free(z);

    // realiza balanceamento se necessário
    if (corOriginal == 'P')
        balanceamentoRemocao(arv, x, x->pai);

    return 1;
}

// Retorna raiz da árvore rubro-negra
noRB* retornaRaiz(rb *arv){
    return arv->sentinelaRaiz->fdir;
}

// Impressão em níveis de árvore rubro-negra
void imprimirPorNivelRN(noRB *no, int nivel) {
    if (!no || no->fesq == NULL) return;

    printf("Nivel %d: %d(%c)\n", nivel, no->chave, no->cor);
    imprimirPorNivelRN(no->fesq, nivel + 1);
    imprimirPorNivelRN(no->fdir, nivel + 1);
}

// Impressão visual de árvore rubro-negra
void imprimirVisualRN(noRB *no, int indent) {
    if (!no || no->fesq == NULL) return;
    imprimirVisualRN(no->fdir, indent + 4);
    for (int k = 0; k < indent; k++) putchar(' ');
    printf("%d(%c)\n", no->chave, no->cor);
    imprimirVisualRN(no->fesq, indent + 4);
}

// Liberação de memória de nós de árvore rubro-negra
void freeRBNodes(rb *tree, noRB *node) {
    if (!node || node == tree->sentinelaFolha) return;
    freeRBNodes(tree, node->fesq);
    freeRBNodes(tree, node->fdir);
    free(node);
}

// Liberação de memória de árvore rubro-negra
void freeRB(rb *tree) {
    if (!tree) return;
    // libera todos os nós de verdade
    noRB *root = tree->sentinelaRaiz->fdir;
    freeRBNodes(tree, root);
    // libera as sentinelas
    free(tree->sentinelaFolha);
    free(tree->sentinelaRaiz);
    // libera a struct de controle
    free(tree);
}





