#include "conversao.h"

/* =========================================================================
                  FUNÇÕES REFERENTES À ÁRVORE 2-3-4
========================================================================= */

// --- FUNÇÕES ADICIONAIS PARA TESTE ---

arvore234 *criarArvore234() {
    arvore234 *arv = (arvore234 *)malloc(sizeof(arvore234));
    if (!arv) {
        perror("malloc para arvore234 falhou");
        exit(1);
    }
    arv->raiz = NULL;
    arv->qtdSplit = 0;
    arv->altura = 0;
    arv->qtdMerge = 0;
    arv->qtdRotacao = 0;
    return arv;
}

int contarNos(No234 *no) {
    if (no == NULL) {
        return 0;
    }
    int count = 1;
    for (int i = 0; i <= no->numChaves; i++) {
        count += contarNos(no->filhos[i]);
    }
    return count;
}

// --- FUNÇÕES ORIGINAIS DA ÁRVORE (COM MODIFICAÇÕES) ---

No234 *criarNo(bool folha) {
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

void insereChaveNoFolha(No234 *noAlvo, int chave) {
    int i = noAlvo->numChaves - 1;
    for (; i >= 0; i--) {
        if (noAlvo->chaves[i] > chave)
            noAlvo->chaves[i + 1] = noAlvo->chaves[i];
        else if (noAlvo->chaves[i] < chave)
            break;
    }
    noAlvo->chaves[i + 1] = chave;
    noAlvo->numChaves++;
}

void insereChaveNoInterno(No234 *noAlvo, int chave, No234 *filhoEsquerdo, No234 *filhoDireito) {
    int i = noAlvo->numChaves - 1;
    for (; i >= 0; i--) {
        if (noAlvo->chaves[i] > chave) {
            noAlvo->chaves[i + 1] = noAlvo->chaves[i];
            noAlvo->filhos[i + 2] = noAlvo->filhos[i + 1];
        } else if (noAlvo->chaves[i] < chave)
            break;
    }
    noAlvo->chaves[i + 1] = chave;
    noAlvo->filhos[i + 1] = filhoEsquerdo;
    noAlvo->filhos[i + 2] = filhoDireito;
    filhoEsquerdo->pai = noAlvo;
    filhoDireito->pai = noAlvo;
    noAlvo->numChaves++;
}

No234 *divideNo(arvore234 *arv, No234 *pai, No234 *noCheio, int chave) {
    arv->qtdSplit++;
    int oldNumChaves = noCheio->numChaves;
    if (!pai) {
        pai = criarNo(0);
        arv->raiz = pai;
        for (int i = 0; i < FILHOS_MAXIMOS; i++)
            pai->filhos[i] = NULL;
        noCheio->pai = pai;
        arv->altura++;
    }
    int meio = CHAVES_MAXIMAS / 2;
    int intermediario = noCheio->chaves[meio];
    No234 *noMaior = criarNo(noCheio->folha);
    noMaior->pai = pai;
    int j = 0;
    for (int i = meio + 1; i < CHAVES_MAXIMAS; i++, j++) {
        noMaior->chaves[j] = noCheio->chaves[i];
        noMaior->numChaves++;
        noCheio->chaves[i] = -1;
        noCheio->numChaves--;
    }
    if (noCheio->folha == 0) {
        int totalFilhos = oldNumChaves + 1;
        for (int i = meio + 1, k = 0; i < totalFilhos; i++, k++) {
            noMaior->filhos[k] = noCheio->filhos[i];
            if (noMaior->filhos[k])
                noMaior->filhos[k]->pai = noMaior;
            noCheio->filhos[i] = NULL;
        }
    }
    if (pai->numChaves == CHAVES_MAXIMAS) {
        pai = divideNo(arv, pai->pai, pai, chave);
        noCheio->pai = pai;
        noMaior->pai = pai;
    }
    insereChaveNoInterno(pai, intermediario, noCheio, noMaior);
    noCheio->chaves[meio] = -1;
    noCheio->numChaves--;
    if (chave < intermediario)
        return noCheio;
    else
        return noMaior;
}

void insereChaveArvore(arvore234 *arv, int chave) {
    if (arv->raiz == NULL) {
        arv->raiz = criarNo(true);
        arv->altura = 1;
    }
    No234 *aux = arv->raiz;
    int index;
    while (aux->folha != 1) {
        index = FILHOS_MAXIMOS - 1;
        for (int i = 0; i < CHAVES_MAXIMAS; i++) {
            if (aux->chaves[i] == -1 || chave < aux->chaves[i]) {
                index = i;
                break;
            }
        }
        aux = aux->filhos[index];
    }
    if (aux->numChaves == CHAVES_MAXIMAS) {
        aux = divideNo(arv, aux->pai, aux, chave);
        insereChaveNoFolha(aux, chave);
    } else {
        insereChaveNoFolha(aux, chave);
    }
}

// --- Funções de Remoção (MODIFICADAS PARA INSTRUMENTAÇÃO) ---
static int removerRec(arvore234 *arv, No234 *noAtual, int valor);
static void mesclarFilhos(arvore234 *arv, No234 *noPai, int indice);
static bool emprestarDaEsquerda(arvore234 *arv, No234 *noPai, int indice);
static bool emprestarDaDireita(arvore234 *arv, No234 *noPai, int indice);
static int remover234(arvore234 *arv, No234 **raizRef, int valor);

static void mesclarFilhos(arvore234 *arv, No234 *noPai, int indice) {
    arv->qtdMerge++;  // INSTRUMENTAÇÃO
    No234 *esq = noPai->filhos[indice];
    No234 *dir = noPai->filhos[indice + 1];
    esq->chaves[esq->numChaves++] = noPai->chaves[indice];
    for (int i = 0; i < dir->numChaves; i++) {
        esq->chaves[esq->numChaves++] = dir->chaves[i];
    }
    if (!esq->folha) {
        int start = esq->numChaves - dir->numChaves;
        for (int i = 0; i <= dir->numChaves; i++) {
            esq->filhos[start + i] = dir->filhos[i];
            if (esq->filhos[start + i])
                esq->filhos[start + i]->pai = esq;
        }
    }
    for (int i = indice; i < noPai->numChaves - 1; i++) {
        noPai->chaves[i] = noPai->chaves[i + 1];
        noPai->filhos[i + 1] = noPai->filhos[i + 2];
    }
    noPai->numChaves--;
    noPai->filhos[noPai->numChaves + 1] = NULL;
    free(dir);
}

static bool emprestarDaEsquerda(arvore234 *arv, No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmao = noPai->filhos[indice - 1];
    if (irmao->numChaves > CHAVES_MINIMAS) {
        arv->qtdRotacao++;  // INSTRUMENTAÇÃO
        for (int i = noAtual->numChaves - 1; i >= 0; i--)
            noAtual->chaves[i + 1] = noAtual->chaves[i];
        if (!noAtual->folha) {
            for (int i = noAtual->numChaves; i >= 0; i--)
                noAtual->filhos[i + 1] = noAtual->filhos[i];
        }
        noAtual->chaves[0] = noPai->chaves[indice - 1];
        noAtual->numChaves++;
        noPai->chaves[indice - 1] = irmao->chaves[irmao->numChaves - 1];
        if (!irmao->folha) {
            noAtual->filhos[0] = irmao->filhos[irmao->numChaves];
        }
        irmao->numChaves--;
        return true;
    }
    return false;
}

static bool emprestarDaDireita(arvore234 *arv, No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmao = noPai->filhos[indice + 1];
    if (irmao->numChaves > CHAVES_MINIMAS) {
        arv->qtdRotacao++;  // INSTRUMENTAÇÃO
        noAtual->chaves[noAtual->numChaves++] = noPai->chaves[indice];
        if (!noAtual->folha) {
            noAtual->filhos[noAtual->numChaves] = irmao->filhos[0];
        }
        noPai->chaves[indice] = irmao->chaves[0];
        for (int i = 0; i < irmao->numChaves - 1; i++)
            irmao->chaves[i] = irmao->chaves[i + 1];
        if (!irmao->folha) {
            for (int i = 0; i < irmao->numChaves; i++)
                irmao->filhos[i] = irmao->filhos[i + 1];
        }
        irmao->numChaves--;
        return true;
    }
    return false;
}

static int removerRec(arvore234 *arv, No234 *noAtual, int valor) {
    int pos = 0;
    while (pos < noAtual->numChaves && valor > noAtual->chaves[pos])
        pos++;
    if (noAtual->folha) {
        if (pos < noAtual->numChaves && noAtual->chaves[pos] == valor) {
            for (int j = pos; j < noAtual->numChaves - 1; j++)
                noAtual->chaves[j] = noAtual->chaves[j + 1];
            noAtual->numChaves--;
            return 1;
        }
        return 0;
    }
    int removido = 0;
    if (pos < noAtual->numChaves && noAtual->chaves[pos] == valor) {
        No234 *pre = noAtual->filhos[pos];
        while (!pre->folha)
            pre = pre->filhos[pre->numChaves];
        int pred = pre->chaves[pre->numChaves - 1];
        noAtual->chaves[pos] = pred;
        removido = removerRec(arv, noAtual->filhos[pos], pred);
    } else {
        removido = removerRec(arv, noAtual->filhos[pos], valor);
    }
    No234 *filho = noAtual->filhos[pos];
    if (filho && filho->numChaves < CHAVES_MINIMAS) {
        if (pos > 0 && emprestarDaEsquerda(arv, noAtual, pos)) {
        } else if (pos < noAtual->numChaves && emprestarDaDireita(arv, noAtual, pos)) {
        } else if (pos > 0)
            mesclarFilhos(arv, noAtual, pos - 1);
        else
            mesclarFilhos(arv, noAtual, pos);
    }
    return removido;
}

static int calculaAlturaNo(No234 *no) {
    if (!no) return 0;
    int h = 0;
    while (no && !no->folha) {
        no = no->filhos[0];
        h++;
    }
    return h;
}

static int remover234(arvore234 *arv, No234 **raizRef, int valor) {
    if (*raizRef == NULL) return 0;
    int ok = removerRec(arv, *raizRef, valor);
    if (ok && (*raizRef)->numChaves == 0 && !(*raizRef)->folha) {
        No234 *antigaRaiz = *raizRef;
        No234 *novaRaiz = antigaRaiz->filhos[0];
        if (novaRaiz) novaRaiz->pai = NULL;
        *raizRef = novaRaiz;
        free(antigaRaiz);
    }
    return ok;
}

int removerChave(arvore234 *arv, int valor) {
    if (!arv || !arv->raiz) return 0;
    int ok = remover234(arv, &arv->raiz, valor);
    if (ok) {
        arv->altura = calculaAlturaNo(arv->raiz);
    }
    return ok;
}

// ... (O restante do arquivo, com as funções de impressão, liberação e da Rubro-Negra, permanece igual) ...
void imprimirPorNivel(No234 *no, int nivel) {
    if (!no) return;
    printf("Nivel %d: ", nivel);
    for (int i = 0; i < no->numChaves; i++) printf("%d ", no->chaves[i]);
    printf("\n");
    for (int i = 0; i <= no->numChaves; i++) imprimirPorNivel(no->filhos[i], nivel + 1);
}
void imprimirVisual(No234 *no, int indent) {
    if (!no) return;
    for (int i = no->numChaves; i >= 0; i--) {
        if (no->filhos[i]) imprimirVisual(no->filhos[i], indent + 4);
        if (i > 0) {
            for (int k = 0; k < indent; k++) putchar(' ');
            printf("%d\n", no->chaves[i - 1]);
        }
    }
}
void free234(No234 *no) {
    if (!no) return;
    for (int i = 0; i <= no->numChaves; i++) free234(no->filhos[i]);
    free(no);
}
rb *alocaArvore() {
    rb *n = (rb *)malloc(sizeof(rb));
    n->sentinelaFolha = (noRB *)malloc(sizeof(noRB));
    n->sentinelaFolha->cor = 'P';
    n->sentinelaFolha->fesq = n->sentinelaFolha->fdir = n->sentinelaFolha->pai = NULL;
    n->sentinelaRaiz = (noRB *)malloc(sizeof(noRB));
    n->sentinelaRaiz->cor = 'P';
    n->sentinelaRaiz->pai = NULL;
    n->sentinelaRaiz->fesq = NULL;
    n->sentinelaRaiz->fdir = n->sentinelaFolha;
    return n;
}
noRB *alocaNo(rb *arv, int c) {
    noRB *nn = (noRB *)malloc(sizeof(noRB));
    if (!nn) return NULL;
    nn->chave = c;
    nn->fesq = nn->fdir = arv->sentinelaFolha;
    nn->pai = NULL;
    nn->cor = 'V';
    return nn;
}
void atualiza_Sentinela_Folha(rb *t1, rb *t2, rb *nRB, noRB *a) {
    if (a == t1->sentinelaFolha || a == t2->sentinelaFolha) return;
    if (a->fesq == t1->sentinelaFolha || a->fesq == t2->sentinelaFolha)
        a->fesq = nRB->sentinelaFolha;
    else
        atualiza_Sentinela_Folha(t1, t2, nRB, a->fesq);
    if (a->fdir == t1->sentinelaFolha || a->fdir == t2->sentinelaFolha)
        a->fdir = nRB->sentinelaFolha;
    else
        atualiza_Sentinela_Folha(t1, t2, nRB, a->fdir);
}
static noRB *converterNo234(No234 *n, rb *a) {
    if (!n) return a->sentinelaFolha;
    noRB *s[5];
    for (int i = 0; i <= n->numChaves; i++) s[i] = converterNo234(n->filhos[i], a);
    noRB *rL, *vE = NULL, *vD = NULL;
    if (n->numChaves == 1) {
        rL = alocaNo(a, n->chaves[0]);
        rL->cor = 'P';
    } else if (n->numChaves == 2) {
        rL = alocaNo(a, n->chaves[1]);
        rL->cor = 'P';
        vE = alocaNo(a, n->chaves[0]);
        vE->cor = 'V';
    } else {
        rL = alocaNo(a, n->chaves[1]);
        rL->cor = 'P';
        vE = alocaNo(a, n->chaves[0]);
        vE->cor = 'V';
        vD = alocaNo(a, n->chaves[2]);
        vD->cor = 'V';
    }
    if (vE) {
        vE->fesq = s[0];
        vE->fdir = s[1];
        s[0]->pai = vE;
        s[1]->pai = vE;
        vE->pai = rL;
    }
    if (vD) {
        vD->fesq = s[2];
        vD->fdir = s[3];
        s[2]->pai = vD;
        s[3]->pai = vD;
        vD->pai = rL;
    }
    if (vE && vD) {
        rL->fesq = vE;
        rL->fdir = vD;
    } else if (vE) {
        rL->fesq = vE;
        rL->fdir = s[2];
        s[2]->pai = rL;
    } else {
        rL->fesq = s[0];
        rL->fdir = s[1];
        s[0]->pai = rL;
        s[1]->pai = rL;
    }
    return rL;
}
rb *converterParaRN(No234 *r) {
    if (!r) return NULL;
    rb *n = alocaArvore();
    noRB *rB = converterNo234(r, n);
    n->sentinelaRaiz->fdir = rB;
    rB->pai = NULL;
    atualiza_Sentinela_Folha(n, n, n, rB);
    n->sentinelaRaiz->fesq = n->sentinelaFolha;
    return n;
}
void rotacaoEsquerda(rb *a, noRB *x) {
    noRB *y = x->fdir;
    x->fdir = y->fesq;
    if (y->fesq != a->sentinelaFolha) y->fesq->pai = x;
    y->pai = x->pai;
    if (!x->pai)
        a->sentinelaRaiz->fdir = y;
    else if (x == x->pai->fesq)
        x->pai->fesq = y;
    else
        x->pai->fdir = y;
    y->fesq = x;
    x->pai = y;
}
void rotacaoDireita(rb *a, noRB *y) {
    noRB *x = y->fesq;
    y->fesq = x->fdir;
    if (x->fdir != a->sentinelaFolha) x->fdir->pai = y;
    x->pai = y->pai;
    if (!y->pai)
        a->sentinelaRaiz->fdir = x;
    else if (y == y->pai->fdir)
        y->pai->fdir = x;
    else
        y->pai->fesq = x;
    x->fdir = y;
    y->pai = x;
}
void balanceamentoInsercao(rb *a, noRB *z) {
    noRB *y;
    while (z != a->sentinelaRaiz->fdir && z->pai->cor == 'V') {
        if (z->pai == z->pai->pai->fesq) {
            y = z->pai->pai->fdir;
            if (y->cor == 'V') {
                z->pai->cor = 'P';
                y->cor = 'P';
                z->pai->pai->cor = 'V';
                z = z->pai->pai;
            } else {
                if (z == z->pai->fdir) {
                    z = z->pai;
                    rotacaoEsquerda(a, z);
                }
                z->pai->cor = 'P';
                z->pai->pai->cor = 'V';
                rotacaoDireita(a, z->pai->pai);
            }
        } else {
            y = z->pai->pai->fesq;
            if (y->cor == 'V') {
                z->pai->cor = 'P';
                y->cor = 'P';
                z->pai->pai->cor = 'V';
                z = z->pai->pai;
            } else {
                if (z == z->pai->fesq) {
                    z = z->pai;
                    rotacaoDireita(a, z);
                }
                z->pai->cor = 'P';
                z->pai->pai->cor = 'V';
                rotacaoEsquerda(a, z->pai->pai);
            }
        }
    }
    a->sentinelaRaiz->fdir->cor = 'P';
}
void insereNo(rb *a, noRB *z) {
    noRB *y = NULL;
    noRB *x = a->sentinelaRaiz->fdir;
    while (x != a->sentinelaFolha) {
        y = x;
        x = (z->chave < x->chave ? x->fesq : x->fdir);
    }
    z->pai = y;
    if (!y)
        a->sentinelaRaiz->fdir = z;
    else if (z->chave < y->chave)
        y->fesq = z;
    else
        y->fdir = z;
    z->fesq = z->fdir = a->sentinelaFolha;
    z->cor = 'V';
    balanceamentoInsercao(a, z);
}
void balanceamentoRemocao(rb *a, noRB *x, noRB *p) {
    noRB *w;
    while (x != a->sentinelaRaiz->fdir && x->cor == 'P') {
        if (x == p->fesq) {
            w = p->fdir;
            if (w->cor == 'V') {
                w->cor = 'P';
                p->cor = 'V';
                rotacaoEsquerda(a, p);
                w = p->fdir;
            }
            if (w->fesq->cor == 'P' && w->fdir->cor == 'P') {
                w->cor = 'V';
                x = p;
                p = x->pai;
            } else {
                if (w->fdir->cor == 'P') {
                    w->fesq->cor = 'P';
                    w->cor = 'V';
                    rotacaoDireita(a, w);
                    w = p->fdir;
                }
                w->cor = p->cor;
                p->cor = 'P';
                w->fdir->cor = 'P';
                rotacaoEsquerda(a, p);
                x = a->sentinelaRaiz->fdir;
                break;
            }
        } else {
            w = p->fesq;
            if (w->cor == 'V') {
                w->cor = 'P';
                p->cor = 'V';
                rotacaoDireita(a, p);
                w = p->fesq;
            }
            if (w->fdir->cor == 'P' && w->fesq->cor == 'P') {
                w->cor = 'V';
                x = p;
                p = x->pai;
            } else {
                if (w->fesq->cor == 'P') {
                    w->fdir->cor = 'P';
                    w->cor = 'V';
                    rotacaoEsquerda(a, w);
                    w = p->fesq;
                }
                w->cor = p->cor;
                p->cor = 'P';
                w->fesq->cor = 'P';
                rotacaoDireita(a, p);
                x = a->sentinelaRaiz->fdir;
                break;
            }
        }
    }
    x->cor = 'P';
}
int removeNo(rb *a, int v) {
    noRB *z = a->sentinelaRaiz->fdir, *x, *y;
    char cO;
    while (z != a->sentinelaFolha && z->chave != v) z = (v < z->chave ? z->fesq : z->fdir);
    if (z == a->sentinelaFolha) return 0;
    y = z;
    cO = y->cor;
    if (z->fesq == a->sentinelaFolha) {
        x = z->fdir;
        if (!z->pai)
            a->sentinelaRaiz->fdir = x;
        else if (z == z->pai->fesq)
            z->pai->fesq = x;
        else
            z->pai->fdir = x;
        x->pai = z->pai;
    } else if (z->fdir == a->sentinelaFolha) {
        x = z->fesq;
        if (!z->pai)
            a->sentinelaRaiz->fdir = x;
        else if (z == z->pai->fesq)
            z->pai->fesq = x;
        else
            z->pai->fdir = x;
        x->pai = z->pai;
    } else {
        y = z->fesq;
        while (y->fdir != a->sentinelaFolha) y = y->fdir;
        cO = y->cor;
        x = y->fesq;
        if (y->pai == z)
            x->pai = y;
        else {
            y->pai->fdir = x;
            x->pai = y->pai;
            y->fesq = z->fesq;
            y->fesq->pai = y;
        }
        if (!z->pai)
            a->sentinelaRaiz->fdir = y;
        else if (z == z->pai->fesq)
            z->pai->fesq = y;
        else
            z->pai->fdir = y;
        y->pai = z->pai;
        y->fdir = z->fdir;
        y->fdir->pai = y;
        y->cor = z->cor;
    }
    free(z);
    if (cO == 'P') balanceamentoRemocao(a, x, x->pai);
    return 1;
}
noRB *retornaRaiz(rb *a) { return a->sentinelaRaiz->fdir; }
void imprimirPorNivelRN(noRB *n, int l) {
    if (!n || n->fesq == NULL) return;
    printf("Nivel %d: %d(%c)\n", l, n->chave, n->cor);
    imprimirPorNivelRN(n->fesq, l + 1);
    imprimirPorNivelRN(n->fdir, l + 1);
}
void imprimirVisualRN(noRB *n, int i) {
    if (!n || n->fesq == NULL) return;
    imprimirVisualRN(n->fdir, i + 4);
    for (int k = 0; k < i; k++) putchar(' ');
    printf("%d(%c)\n", n->chave, n->cor);
    imprimirVisualRN(n->fesq, i + 4);
}
void freeRBNodes(rb *t, noRB *n) {
    if (!n || n == t->sentinelaFolha) return;
    freeRBNodes(t, n->fesq);
    freeRBNodes(t, n->fdir);
    free(n);
}
void freeRB(rb *t) {
    if (!t) return;
    noRB *r = t->sentinelaRaiz->fdir;
    freeRBNodes(t, r);
    free(t->sentinelaFolha);
    free(t->sentinelaRaiz);
    free(t);
}