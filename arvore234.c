#include "arvore234.h"

// --- Contadores Globais para Análise ---
static unsigned long long int contador_split = 0;
static unsigned long long int contador_merge = 0;
static unsigned long long int contador_rotacao = 0;
static unsigned long long int contador_nos = 0;


// Estrutura auxiliar para promoção
typedef struct {
    bool houvePromocao;
    int chavePromovida;
    No234 *direitoNovo;
} Promocao;

// Protótipos internos
static Promocao inserirRec(No234 *noAtual, int valor);
No234* criarNo(bool folha);
static int removerRec(No234 *noAtual, int valor);
void destruir_arvore_rec(No234* no);

// --- Implementação das Funções de Teste ---

void zerar_contadores() {
    contador_split = 0;
    contador_merge = 0;
    contador_rotacao = 0;
    contador_nos = 0;
}

void zerar_contadores_operacao() {
    contador_split = 0;
    contador_merge = 0;
    contador_rotacao = 0;
}

unsigned long long int obter_contador_split() { return contador_split; }
unsigned long long int obter_contador_merge() { return contador_merge; }
unsigned long long int obter_contador_rotacao() { return contador_rotacao; }
unsigned long long int obter_contador_nos() { return contador_nos; }

int calcular_altura(No234* no) {
    if (no == NULL) {
        return -1; // Árvore vazia
    }
    int altura = 0;
    No234* temp = no;
    while (!temp->folha) {
        temp = temp->filhos[0];
        altura++;
    }
    return altura;
}

void destruir_arvore_rec(No234* no) {
    if (no == NULL) return;
    if (!no->folha) {
        for (int i = 0; i <= no->numChaves; i++) {
            destruir_arvore_rec(no->filhos[i]);
        }
    }
    free(no);
}

void destruir_arvore(No234** raizRef) {
    if (raizRef && *raizRef) {
        destruir_arvore_rec(*raizRef);
        *raizRef = NULL;
    }
}

// --- Funções da Árvore (Instrumentadas) ---

No234* criarNo(bool folha) {
    contador_nos++;
    No234 *no = malloc(sizeof(No234));
    no->numChaves = 0;
    no->folha = folha;
    for (int i = 0; i < FILHOS_MAXIMOS + 1; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

static Promocao dividirNo(No234 *noCheio) {
    contador_split++;
    int meio = CHAVES_MAXIMAS / 2;
    Promocao promo;
    promo.houvePromocao = true;
    promo.chavePromovida = noCheio->chaves[meio];
    promo.direitoNovo = criarNo(noCheio->folha);

    for (int i = meio + 1, j = 0; i < noCheio->numChaves; i++, j++) {
        promo.direitoNovo->chaves[j] = noCheio->chaves[i];
        promo.direitoNovo->numChaves++;
    }

    if (!noCheio->folha) {
        for (int i = meio + 1, j = 0; i <= noCheio->numChaves; i++, j++) {
            promo.direitoNovo->filhos[j] = noCheio->filhos[i];
            noCheio->filhos[i] = NULL;
        }
    }

    noCheio->numChaves = meio;
    return promo;
}

static Promocao inserirRec(No234 *noAtual, int valor) {
    Promocao promo;
    promo.houvePromocao = false;

    if (noAtual->folha) {
        int i = noAtual->numChaves - 1;
        while (i >= 0 && valor < noAtual->chaves[i]) {
            noAtual->chaves[i + 1] = noAtual->chaves[i];
            i--;
        }
        noAtual->chaves[i + 1] = valor;
        noAtual->numChaves++;
        if (noAtual->numChaves > CHAVES_MAXIMAS) {
            return dividirNo(noAtual);
        }
    } else {
        int i = 0;
        while (i < noAtual->numChaves && valor > noAtual->chaves[i]) {
            i++;
        }
        Promocao promoFilho = inserirRec(noAtual->filhos[i], valor);
        if (promoFilho.houvePromocao) {
            int posInsercao = noAtual->numChaves - 1;
            while (posInsercao >= 0 && promoFilho.chavePromovida < noAtual->chaves[posInsercao]) {
                noAtual->chaves[posInsercao + 1] = noAtual->chaves[posInsercao];
                noAtual->filhos[posInsercao + 2] = noAtual->filhos[posInsercao + 1];
                posInsercao--;
            }
            noAtual->chaves[posInsercao + 1] = promoFilho.chavePromovida;
            noAtual->filhos[posInsercao + 2] = promoFilho.direitoNovo;
            noAtual->numChaves++;
            if (noAtual->numChaves > CHAVES_MAXIMAS) {
                return dividirNo(noAtual);
            }
        }
    }
    return promo;
}

void inserir234(No234 **raizRef, int valor) {
    if (*raizRef == NULL) {
        *raizRef = criarNo(true);
        (*raizRef)->chaves[0] = valor;
        (*raizRef)->numChaves = 1;
        return;
    }
    Promocao promo = inserirRec(*raizRef, valor);
    if (promo.houvePromocao) {
        No234 *novaRaiz = criarNo(false);
        novaRaiz->chaves[0] = promo.chavePromovida;
        novaRaiz->filhos[0] = *raizRef;
        novaRaiz->filhos[1] = promo.direitoNovo;
        novaRaiz->numChaves = 1;
        *raizRef = novaRaiz;
    }
}

static void mesclarFilhos(No234 *noPai, int indice) {
    contador_merge++;
    No234 *esq = noPai->filhos[indice];
    No234 *dir = noPai->filhos[indice + 1];

    int num_filhos_esq_original = esq->numChaves + 1;

    esq->chaves[esq->numChaves++] = noPai->chaves[indice];

    for (int i = 0; i < dir->numChaves; i++) {
        esq->chaves[esq->numChaves++] = dir->chaves[i];
    }

    if (!esq->folha) {
        for (int i = 0; i <= dir->numChaves; i++) {
            esq->filhos[num_filhos_esq_original + i] = dir->filhos[i];
        }
    }

    for (int i = indice; i < noPai->numChaves - 1; i++) {
        noPai->chaves[i] = noPai->chaves[i + 1];
        noPai->filhos[i + 1] = noPai->filhos[i + 2];
    }
    noPai->numChaves--;
    noPai->filhos[noPai->numChaves + 1] = NULL;
    
    free(dir);
    contador_nos--;
}

static bool emprestarDaEsquerda(No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmaoEsq = noPai->filhos[indice - 1];
    if (irmaoEsq->numChaves > CHAVES_MINIMAS) {
        contador_rotacao++;
        for (int i = noAtual->numChaves - 1; i >= 0; i--) {
            noAtual->chaves[i + 1] = noAtual->chaves[i];
        }
        if (!noAtual->folha) {
            for (int i = noAtual->numChaves; i >= 0; i--) {
                noAtual->filhos[i + 1] = noAtual->filhos[i];
            }
        }
        noAtual->chaves[0] = noPai->chaves[indice - 1];
        noAtual->numChaves++;
        noPai->chaves[indice - 1] = irmaoEsq->chaves[irmaoEsq->numChaves - 1];
        if (!irmaoEsq->folha) {
            noAtual->filhos[0] = irmaoEsq->filhos[irmaoEsq->numChaves];
        }
        irmaoEsq->numChaves--;
        return true;
    }
    return false;
}

static bool emprestarDaDireita(No234 *noPai, int indice) {
    No234 *noAtual = noPai->filhos[indice];
    No234 *irmaoDir = noPai->filhos[indice + 1];
    if (irmaoDir->numChaves > CHAVES_MINIMAS) {
        contador_rotacao++;
        noAtual->chaves[noAtual->numChaves++] = noPai->chaves[indice];
        noPai->chaves[indice] = irmaoDir->chaves[0];
        if (!irmaoDir->folha) {
            noAtual->filhos[noAtual->numChaves] = irmaoDir->filhos[0];
        }
        for (int i = 0; i < irmaoDir->numChaves - 1; i++) {
            irmaoDir->chaves[i] = irmaoDir->chaves[i + 1];
        }
        if (!irmaoDir->folha) {
            for (int i = 0; i < irmaoDir->numChaves; i++) {
                irmaoDir->filhos[i] = irmaoDir->filhos[i + 1];
            }
        }
        irmaoDir->numChaves--;
        return true;
    }
    return false;
}

static int removerRec(No234 *noAtual, int valor) {
    int pos = 0;
    while (pos < noAtual->numChaves && valor > noAtual->chaves[pos]) {
        pos++;
    }
    bool encontrado = (pos < noAtual->numChaves && noAtual->chaves[pos] == valor);

    if (noAtual->folha) {
        if (encontrado) {
            for (int j = pos; j < noAtual->numChaves - 1; j++) {
                noAtual->chaves[j] = noAtual->chaves[j + 1];
            }
            noAtual->numChaves--;
            return 1;
        }
        return 0;
    }

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
    
    if (noAtual->filhos[pos] && noAtual->filhos[pos]->numChaves < CHAVES_MINIMAS) {
        if (pos > 0 && noAtual->filhos[pos - 1]->numChaves > CHAVES_MINIMAS) {
            emprestarDaEsquerda(noAtual, pos);
        } else if (pos < noAtual->numChaves && noAtual->filhos[pos + 1]->numChaves > CHAVES_MINIMAS) {
            emprestarDaDireita(noAtual, pos);
        } else {
            if (pos > 0) {
                mesclarFilhos(noAtual, pos - 1);
            } else {
                mesclarFilhos(noAtual, pos);
            }
        }
    }
    return 1;
}

int remover234(No234 **raizRef, int valor) {
    if (raizRef == NULL || *raizRef == NULL) {
        return 0;
    }
    removerRec(*raizRef, valor);
    if ((*raizRef)->numChaves == 0 && !(*raizRef)->folha) {
        No234 *antigaRaiz = *raizRef;
        *raizRef = (*raizRef)->filhos[0];
        free(antigaRaiz);
        contador_nos--;
    }
    return 1;
}