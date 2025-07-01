#include "arvore234.h"

/* =========================================================================
                  FUNÇÕES REFERENTES À ÁRVORE 2-3-4 (B-TREE ORDEM 4) ========================================================================= */

typedef struct {
    bool houvePromocao;
    int chavePromovida;
    No234 *direitoNovo;
} Promocao;

static Promocao inserirRec(No234 *noAtual, int valor);

No234* criarNo(bool folha) {
    No234 *no = malloc(sizeof(No234));
    no->numChaves = 0;
    no->folha = folha;
    for (int i = 0; i < FILHOS_MAXIMOS; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

static Promocao dividirNo(No234 *noCheio) {
    int meio = CHAVES_MAXIMAS / 2;
    Promocao promo;
    promo.houvePromocao = true;
    promo.chavePromovida = noCheio->chaves[meio];
    promo.direitoNovo = criarNo(noCheio->folha);
    noCheio->chaves[meio] = 0;

    for (int i = meio + 1, j = 0; i <= CHAVES_MAXIMAS; i++, j++) {
        promo.direitoNovo->chaves[j] = noCheio->chaves[i];
        noCheio->chaves[i] = 0;
        promo.direitoNovo->numChaves++;
    }

    if (!noCheio->folha) {
        for (int i = meio + 1, j = 0; i <= FILHOS_MAXIMOS - 1; i++, j++) {
            promo.direitoNovo->filhos[j] = noCheio->filhos[i];
            noCheio->filhos[i] = NULL;
        }
        
    }
    noCheio->numChaves = meio;
    return promo;
}






//MATHEUS FEZ
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




//--------------------------------------
// Funções auxiliares para remoção 
// (mesclarFilhos, emprestarDaEsquerda, emprestarDaDireita)

static void mesclarFilhos(No234 *noPai, int indice) {
    No234 *esq = noPai->filhos[indice];
    No234 *dir = noPai->filhos[indice + 1];
    esq->chaves[esq->numChaves++] = noPai->chaves[indice];
    for (int i = 0; i < dir->numChaves; i++) {
        esq->chaves[esq->numChaves++] = dir->chaves[i];
    }
    if (!esq->folha) {
        // calcula onde começa a inserção dos filhos de dir
        int start = esq->numChaves - dir->numChaves;
        for (int i = 0; i <= dir->numChaves; i++) {
            esq->filhos[esq->numChaves + i - 1] = dir->filhos[i];
        }
    }
    for (int i = indice; i < noPai->numChaves - 1; i++) {
        noPai->chaves[i]     = noPai->chaves[i + 1];
        noPai->filhos[i + 1] = noPai->filhos[i + 2];
    }
    noPai->numChaves--;
    noPai->filhos[noPai->numChaves + 1] = NULL;
    free(dir);
}


static bool emprestarDaEsquerda(No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmaoEsq = noPai->filhos[indice - 1];
    if (irmaoEsq->numChaves > CHAVES_MINIMAS) {
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

static bool emprestarDaDireita(No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmaoDir = noPai->filhos[indice + 1];
    if (irmaoDir->numChaves > CHAVES_MINIMAS) {
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
static int remover234(No234 **raizRef, int valor) {
    if (*raizRef == NULL) {
        return 0;
    }

<<<<<<< Updated upstream
    // Se a chave está em um nó interno, substitua pelo predecessor e remova o predecessor
    if (encontrado) {
        No234 *predecessorNode = noAtual->filhos[pos];
        while (!predecessorNode->folha) {
            predecessorNode = predecessorNode->filhos[predecessorNode->numChaves];
        }
        int predecessor = predecessorNode->chaves[predecessorNode->numChaves - 1];
        noAtual->chaves[pos] = predecessor;
        removerRec(noAtual->filhos[pos], predecessor);
    } else {
        removerRec(noAtual->filhos[pos], valor);
    }
    
    // CORREÇÃO: As duas linhas abaixo usavam 'noPai' incorretamente.
    // A variável correta é 'noAtual', pois ele é o pai do nó que pode ter sofrido underflow.
    if (noAtual->filhos[pos] && noAtual->filhos[pos]->numChaves < CHAVES_MINIMAS) {
        if (pos > 0 && noAtual->filhos[pos - 1]->numChaves > CHAVES_MINIMAS) {
            emprestarDaEsquerda(noAtual, pos);
        } else if (pos < noAtual->numChaves && noAtual->filhos[pos + 1]->numChaves > CHAVES_MINIMAS) {
            emprestarDaDireita(noAtual, pos);
        } else {
            if (pos > 0) {
                mesclarFilhos(noAtual, pos - 1);
=======
    int ok = removerRec(*raizRef, valor);

    // Se raiz ficou vazia e não é folha, descer uma altura
    if (ok && (*raizRef)->numChaves == 0 && !(*raizRef)->folha) {
        No234 *antigaRaiz = *raizRef;
        No234 *novaRaiz   = antigaRaiz->filhos[0];
        // --- ajustes cruciais ---
        novaRaiz->pai    = NULL;
        // ------------------------
        *raizRef = novaRaiz;
        free(antigaRaiz);
    }

    return ok;
}


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
static noRB* converterNo234(No234 *n234, rb *arvRN) {
    if (!n234) return arvRN->sentinelaFolha;

    noRB *sub[5];
    for (int i = 0; i <= n234->numChaves; i++) {
        sub[i] = converterNo234(n234->filhos[i], arvRN);
    }

    noRB *raizLocal;
    noRB *vermelhoEsq = NULL;
    noRB *vermelhoDir = NULL;

    if (n234->numChaves == 1) {
        raizLocal = alocaNo(arvRN, n234->chaves[0]); raizLocal->cor = 'P';
    } else if (n234->numChaves == 2) {
        raizLocal = alocaNo(arvRN, n234->chaves[1]); raizLocal->cor = 'P';
        vermelhoEsq = alocaNo(arvRN, n234->chaves[0]); vermelhoEsq->cor = 'V';
    } else {
        raizLocal = alocaNo(arvRN, n234->chaves[1]); raizLocal->cor = 'P';
        vermelhoEsq = alocaNo(arvRN, n234->chaves[0]); vermelhoEsq->cor = 'V';
        vermelhoDir = alocaNo(arvRN, n234->chaves[2]); vermelhoDir->cor = 'V';
    }

    if (vermelhoEsq) {
        vermelhoEsq->fesq = sub[0]; vermelhoEsq->fdir = sub[1];
        sub[0]->pai = vermelhoEsq; sub[1]->pai = vermelhoEsq;
        vermelhoEsq->pai = raizLocal;
    }
    if (vermelhoDir) {
        vermelhoDir->fesq = sub[2]; vermelhoDir->fdir = sub[3];
        sub[2]->pai = vermelhoDir; sub[3]->pai = vermelhoDir;
        vermelhoDir->pai = raizLocal;
    }

    if (vermelhoEsq && vermelhoDir) {
        raizLocal->fesq = vermelhoEsq; raizLocal->fdir = vermelhoDir;
    } else if (vermelhoEsq) {
        raizLocal->fesq = vermelhoEsq; raizLocal->fdir = sub[2]; sub[2]->pai = raizLocal;
    } else {
        raizLocal->fesq = sub[0]; raizLocal->fdir = sub[1];
        sub[0]->pai = raizLocal; sub[1]->pai = raizLocal;
    }

    return raizLocal;
}

// Interface pública para converter 2-3-4 em RB
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
>>>>>>> Stashed changes
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
        // caso com dois filhos: usar predecessor ao invés de sucessor
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

<<<<<<< Updated upstream
int remover234(No234 **raizRef, int valor) {
    if (*raizRef == NULL) {
        return 0;
    }
    removerRec(*raizRef, valor);
    if ((*raizRef)->numChaves == 0 && !(*raizRef)->folha) {
        No234 *antigaRaiz = *raizRef;
        *raizRef = (*raizRef)->filhos[0];
        free(antigaRaiz);
    }
    return 1;
}

void imprimirPorNivel(No234 *no, int nivel) {
    if (!no) return;
    printf("Nivel %d: ", nivel);
    for (int i = 0; i < no->numChaves; i++) {
        printf("[%d] ", no->chaves[i]);
    }
    printf("\n");
    for (int i = 0; i <= no->numChaves; i++) {
        if (no->filhos[i]) {
            imprimirPorNivel(no->filhos[i], nivel + 1);
        }
    }
}

void imprimirVisual(No234 *no, int indent) {
    if (!no) return;
    for (int i = no->numChaves; i >= 0; i--) {
        if (no->filhos[i]) {
            imprimirVisual(no->filhos[i], indent + 6);
        }
        if (i > 0) {
            for (int k = 0; k < indent; k++) {
                putchar(' ');
            }
            printf("%d\n", no->chaves[i - 1]);
        }
    }
}

/* =========================================================================
         CONVERSÃO DIRETA ÁRVORE 2-3-4 → ÁRVORE RUBRO-NEGRA (RB)
=========================================================================*/

rb* alocaArvore() {
    rb *nova = (rb*)malloc(sizeof(rb));
    nova->sentinelaFolha = (noRB*)malloc(sizeof(noRB));
    nova->sentinelaFolha->cor = 'P';
    nova->sentinelaFolha->fesq = nova->sentinelaFolha->fdir = nova->sentinelaFolha->pai = NULL;
    nova->sentinelaRaiz = (noRB*)malloc(sizeof(noRB));
    nova->sentinelaRaiz->cor = 'P';
    nova->sentinelaRaiz->pai = NULL;
    nova->sentinelaRaiz->fesq = NULL;
    nova->sentinelaRaiz->fdir = nova->sentinelaFolha;
    return nova;
}

noRB* alocaNo(rb *arv, int chave) {
    noRB *novoNo = (noRB*)malloc(sizeof(noRB));
    if (!novoNo) return NULL;
    novoNo->chave = chave;
    novoNo->fesq = novoNo->fdir = arv->sentinelaFolha;
    novoNo->pai = NULL;
    novoNo->cor = 'V';
    return novoNo;
}

void atualiza_Sentinela_Folha(rb *T, noRB *aux, noRB* sentinela) {
    if (aux == NULL || aux == sentinela) return;
    if (aux->fesq == NULL) aux->fesq = sentinela;
    if (aux->fdir == NULL) aux->fdir = sentinela;
    atualiza_Sentinela_Folha(T, aux->fesq, sentinela);
    atualiza_Sentinela_Folha(T, aux->fdir, sentinela);
}

static noRB* converterNo234(No234 *n234, rb *arvRN) {
    if (!n234) return arvRN->sentinelaFolha;

    noRB *filhosConvertidos[FILHOS_MAXIMOS];
    for (int i = 0; i <= n234->numChaves; i++) {
        filhosConvertidos[i] = converterNo234(n234->filhos[i], arvRN);
    }

    noRB *raizLocal = NULL, *filhoVermelhoEsq = NULL, *filhoVermelhoDir = NULL;

    if (n234->numChaves == 1) {
        raizLocal = alocaNo(arvRN, n234->chaves[0]);
        raizLocal->cor = 'P';
        raizLocal->fesq = filhosConvertidos[0];
        raizLocal->fdir = filhosConvertidos[1];
    } else if (n234->numChaves == 2) {
        raizLocal = alocaNo(arvRN, n234->chaves[1]);
        raizLocal->cor = 'P';
        filhoVermelhoEsq = alocaNo(arvRN, n234->chaves[0]);
        filhoVermelhoEsq->cor = 'V';
        raizLocal->fesq = filhoVermelhoEsq;
        raizLocal->fdir = filhosConvertidos[2];
        filhoVermelhoEsq->fesq = filhosConvertidos[0];
        filhoVermelhoEsq->fdir = filhosConvertidos[1];
    } else {
        raizLocal = alocaNo(arvRN, n234->chaves[1]);
        raizLocal->cor = 'P';
        filhoVermelhoEsq = alocaNo(arvRN, n234->chaves[0]);
        filhoVermelhoEsq->cor = 'V';
        filhoVermelhoDir = alocaNo(arvRN, n234->chaves[2]);
        filhoVermelhoDir->cor = 'V';
        raizLocal->fesq = filhoVermelhoEsq;
        raizLocal->fdir = filhoVermelhoDir;
        filhoVermelhoEsq->fesq = filhosConvertidos[0];
        filhoVermelhoEsq->fdir = filhosConvertidos[1];
        filhoVermelhoDir->fesq = filhosConvertidos[2];
        filhoVermelhoDir->fdir = filhosConvertidos[3];
    }

    if (raizLocal->fesq) raizLocal->fesq->pai = raizLocal;
    if (raizLocal->fdir) raizLocal->fdir->pai = raizLocal;
    if (filhoVermelhoEsq) {
        if (filhoVermelhoEsq->fesq) filhoVermelhoEsq->fesq->pai = filhoVermelhoEsq;
        if (filhoVermelhoEsq->fdir) filhoVermelhoEsq->fdir->pai = filhoVermelhoEsq;
    }
    if (filhoVermelhoDir) {
        if (filhoVermelhoDir->fesq) filhoVermelhoDir->fesq->pai = filhoVermelhoDir;
        if (filhoVermelhoDir->fdir) filhoVermelhoDir->fdir->pai = filhoVermelhoDir;
    }
    return raizLocal;
}

rb* converterParaRN(No234 *raiz234) {
    if (!raiz234) return NULL;
    rb *novaRN = alocaArvore();
    noRB *raizRB = converterNo234(raiz234, novaRN);
    novaRN->sentinelaRaiz->fdir = raizRB;
    if (raizRB != novaRN->sentinelaFolha) raizRB->pai = NULL;
    atualiza_Sentinela_Folha(novaRN, raizRB, novaRN->sentinelaFolha);
    return novaRN;
}

/* =========================================================================
                  FUNÇÕES REFERENTES À ÁRVORE RUBRO-NEGRA =========================================================================*/

void rotacaoEsquerda(rb *arv, noRB *x) {
    noRB *y = x->fdir;
    x->fdir = y->fesq;
    if (y->fesq != arv->sentinelaFolha) y->fesq->pai = x;
    y->pai = x->pai;
    if (x->pai == NULL) arv->sentinelaRaiz->fdir = y;
    else if (x == x->pai->fesq) x->pai->fesq = y;
    else x->pai->fdir = y;
    y->fesq = x;
    x->pai = y;
}

void rotacaoDireita(rb *arv, noRB *y) {
    noRB *x = y->fesq;
    y->fesq = x->fdir;
    if (x->fdir != arv->sentinelaFolha) x->fdir->pai = y;
    x->pai = y->pai;
    if (y->pai == NULL) arv->sentinelaRaiz->fdir = x;
    else if (y == y->pai->fdir) y->pai->fdir = x;
    else y->pai->fesq = x;
    x->fdir = y;
    y->pai = x;
}

void balanceamentoInsercao(rb *arv, noRB *z) {
    noRB *y;
    while (z->pai && z->pai->cor == 'V') {
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
                    rotacaoEsquerda(arv, z);
                }
                z->pai->cor = 'P';
                if (z->pai->pai) {
                    z->pai->pai->cor = 'V';
                    rotacaoDireita(arv, z->pai->pai);
                }
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
                    rotacaoDireita(arv, z);
                }
                z->pai->cor = 'P';
                if (z->pai->pai) {
                    z->pai->pai->cor = 'V';
                    rotacaoEsquerda(arv, z->pai->pai);
                }
            }
        }
    }
    arv->sentinelaRaiz->fdir->cor = 'P';
}

void insereNo(rb *arv, noRB *z) {
    noRB *y = NULL;
    noRB *x = arv->sentinelaRaiz->fdir;
    while (x != arv->sentinelaFolha) {
        y = x;
        x = (z->chave < x->chave ? x->fesq : x->fdir);
    }
    z->pai = y;
    if (!y) arv->sentinelaRaiz->fdir = z;
    else if (z->chave < y->chave) y->fesq = z;
    else y->fdir = z;
    z->fesq = z->fdir = arv->sentinelaFolha;
    z->cor = 'V';
    balanceamentoInsercao(arv, z);
}

void balanceamentoRemocao(rb *arv, noRB *x) {
    noRB *w;
    while (x != arv->sentinelaRaiz->fdir && x->cor == 'P') {
        if (x == x->pai->fesq) {
            w = x->pai->fdir;
            if (w->cor == 'V') {
                w->cor = 'P';
                x->pai->cor = 'V';
                rotacaoEsquerda(arv, x->pai);
                w = x->pai->fdir;
            }
            if (w->fesq->cor == 'P' && w->fdir->cor == 'P') {
                w->cor = 'V';
                x = x->pai;
            } else {
                if (w->fdir->cor == 'P') {
                    w->fesq->cor = 'P';
                    w->cor = 'V';
                    rotacaoDireita(arv, w);
                    w = x->pai->fdir;
                }
                w->cor = x->pai->cor;
                x->pai->cor = 'P';
                w->fdir->cor = 'P';
                rotacaoEsquerda(arv, x->pai);
                x = arv->sentinelaRaiz->fdir;
            }
        } else {
            w = x->pai->fesq;
            if (w->cor == 'V') {
                w->cor = 'P';
                x->pai->cor = 'V';
                rotacaoDireita(arv, x->pai);
                w = x->pai->fesq;
            }
            if (w->fdir->cor == 'P' && w->fesq->cor == 'P') {
                w->cor = 'V';
                x = x->pai;
            } else {
                if (w->fesq->cor == 'P') {
                    w->fdir->cor = 'P';
                    w->cor = 'V';
                    rotacaoEsquerda(arv, w);
                    w = x->pai->fesq;
                }
                w->cor = x->pai->cor;
                x->pai->cor = 'P';
                w->fesq->cor = 'P';
                rotacaoDireita(arv, x->pai);
                x = arv->sentinelaRaiz->fdir;
            }
        }
    }
    x->cor = 'P';
}

void transplante(rb* arv, noRB* u, noRB* v) {
    if (u->pai == NULL) {
        arv->sentinelaRaiz->fdir = v;
    } else if (u == u->pai->fesq) {
        u->pai->fesq = v;
    } else {
        u->pai->fdir = v;
    }
    v->pai = u->pai;
}

noRB* minimo(rb* arv, noRB* x) {
    while (x->fesq != arv->sentinelaFolha) {
        x = x->fesq;
    }
    return x;
}

int removeNo(rb *arv, int valor) {
    noRB *z = arv->sentinelaRaiz->fdir;
    while (z != arv->sentinelaFolha && z->chave != valor) {
        z = (valor < z->chave) ? z->fesq : z->fdir;
    }
    if (z == arv->sentinelaFolha) return 0;

    noRB *y = z;
    noRB *x;
    char y_cor_original = y->cor;

    if (z->fesq == arv->sentinelaFolha) {
        x = z->fdir;
        transplante(arv, z, z->fdir);
    } else if (z->fdir == arv->sentinelaFolha) {
        x = z->fesq;
        transplante(arv, z, z->fesq);
    } else {
        y = minimo(arv, z->fdir);
        y_cor_original = y->cor;
        x = y->fdir;
        if (y->pai == z) {
            x->pai = y;
        } else {
            transplante(arv, y, y->fdir);
            y->fdir = z->fdir;
            y->fdir->pai = y;
        }
        transplante(arv, z, y);
        y->fesq = z->fesq;
        y->fesq->pai = y;
        y->cor = z->cor;
    }
    free(z);
    if (y_cor_original == 'P') {
        balanceamentoRemocao(arv, x);
    }
    return 1;
}

noRB* retornaRaiz(rb *arv) {
    if (!arv) return NULL;
    return arv->sentinelaRaiz->fdir;
}

void imprimirPorNivelRN(noRB *no, int nivel) {
    if (no == NULL || no == no->fesq) return;
    printf("Nivel %d: ", nivel);
    printf("%d(%c)\n", no->chave, no->cor);

=======
// Retorna raiz da árvore rubro-negra
noRB* retornaRaiz(rb *arv){
    return arv->sentinelaRaiz->fdir;
}

// Impressão em níveis de árvore rubro-negra
void imprimirPorNivelRN(noRB *no, int nivel) {
    if (!no || no->fesq == NULL) return;

    printf("Nivel %d: %d(%c)\n", nivel, no->chave, no->cor);
>>>>>>> Stashed changes
    imprimirPorNivelRN(no->fesq, nivel + 1);
    imprimirPorNivelRN(no->fdir, nivel + 1);
}

<<<<<<< Updated upstream
void imprimirVisualRN(noRB *no, int indent) {
    if (no == NULL || no == no->fesq) return;
=======
// Impressão visual de árvore rubro-negra
void imprimirVisualRN(noRB *no, int indent) {
    if (!no || no->fesq == NULL) return;
>>>>>>> Stashed changes
    imprimirVisualRN(no->fdir, indent + 4);
    for (int k = 0; k < indent; k++) putchar(' ');
    printf("%d(%c)\n", no->chave, no->cor);
    imprimirVisualRN(no->fesq, indent + 4);
<<<<<<< Updated upstream
}
=======
}

void freeRBNodes(rb *tree, noRB *node) {
    if (!node || node == tree->sentinelaFolha) return;
    freeRBNodes(tree, node->fesq);
    freeRBNodes(tree, node->fdir);
    free(node);
}

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





>>>>>>> Stashed changes
