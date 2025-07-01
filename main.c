#include "arvore234.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char nomeArquivo[256];

    // Solicita ao usuário o nome do arquivo e lê via scanf
    printf("Digite o nome do arquivo de entrada: ");
    if (scanf("%255s", nomeArquivo) != 1) {
        fprintf(stderr, "Erro ao ler nome do arquivo.\n");
        return 1;
    }

    // Tenta abrir o arquivo para leitura 
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Não foi possível abrir o arquivo");
        return 1;
    }

    // Cria árvore 2-3-4 (inicialmente vazia)
    arvore234* arv = (arvore234*)malloc(sizeof(arvore234));
    if (!arv) { perror("malloc"); exit(1); }
    arv->raiz = NULL;
    arv->qtdSplit = 0;
    arv->altura = 0;

    // Cria árvore rubro-negra nula
    rb *arvRN = NULL;

    // Leitura de todos os inteiros do arquivo, inserindo na B-Tree
    int valor;
    while (fscanf(arquivo, "%d", &valor) == 1) {
        insereChaveArvore(arv, valor);
    }
    fclose(arquivo);

    // Exibe a árvore 2-3-4 após a leitura inicial 
    printf("\nÁrvore 2-3-4 gerada! \n");
    printf("\n-- Árvore 2-3-4 (por nível) --\n\n");
    imprimirPorNivel(arv->raiz, 0);
    printf("\n-- Árvore 2-3-4 (visual) --\n");
    imprimirVisual(arv->raiz, 0);

    int opcao;
    // Loop principal do menu de operações 
    do {
        printf("\nMenu:\n");
        printf("1. Inserir novo elemento na árvore 2-3-4\n");
        printf("2. Remover elemento da árvore 2-3-4\n");
        printf("3. Imprimir árvore 2-3-4\n");
        printf("4. Converter em uma árvore rubro-negra\n");
        printf("5. Sair\n");
        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao) != 1) {
            fprintf(stderr, "Entrada inválida. Encerrando.\n");
            return 1;
        }

        switch (opcao) {
            case 1:
                // Inserção de novo elemento na árvore 2-3-4
                printf("Digite o valor a inserir: ");
                if (scanf("%d", &valor) == 1) {
                    insereChaveArvore(arv, valor);
                    printf("Valor %d inserido com sucesso.\n", valor);
                }
                break;

            case 2:
                // Remoção de elemento na árvore 2-3-4
                printf("Digite o valor a remover: ");
                if (scanf("%d", &valor) == 1) {
                    int ok = removerChave(arv, valor);
                    if (ok)
                        printf("Valor %d removido da árvore 2-3-4.\n", valor);
                    else
                        printf("Valor %d não encontrado na árvore 2-3-4.\n", valor);
                }
                break;

            case 3:
                // Impressão da árvore 2-3-4
                printf("\n-- Árvore 2-3-4 (por nível) --\n\n");
                imprimirPorNivel(arv->raiz, 0);
                printf("\n-- Árvore 2-3-4 (visual) --\n");
                imprimirVisual(arv->raiz, 0);
                break;

            case 4: {
                // Converter para Rubro-Negra a partir da 2-3-4
                arvRN = converterParaRN(arv->raiz);
            
                // Exibe a RB convertida 
                printf("\nÁrvore Rubro-Negra gerada! \n");
                printf("\n-- Árvore Rubro-Negra (por nível) --\n");
                imprimirPorNivelRN(retornaRaiz(arvRN), 0);
                printf("\n-- Árvore Rubro-Negra (visual) --\n");
                imprimirVisualRN(retornaRaiz(arvRN), 0);
                
                int opcRN;
                // Menu interno para a RB 
                do {
                    printf("\nMenu:\n");
                    printf("1. Inserir novo elemento na árvore rubro-negra\n");
                    printf("2. Remover elemento da árvore rubro-negra\n");
                    printf("3. Imprimir árvore rubro-negra\n");
                    printf("4. Sair (voltar ao menu principal)\n");
                    printf("Escolha uma opção: ");
                    if (scanf("%d", &opcRN) != 1) break;

                    switch (opcRN) {
                        case 1: {
                            // Inserção de novo elemento na RB
                            printf("Digite o valor a inserir na RN: ");
                            if (scanf("%d", &valor) == 1) {
                                noRB *novo = alocaNo(arvRN, valor);
                                insereNo(arvRN, novo);
                                printf("Valor %d inserido em Rubro-Negra.\n", valor);
                            }
                            break;
                        }
                        case 2: {
                            // Remoção de elemento na RB
                            printf("Digite o valor a remover da RN: ");
                            if (scanf("%d", &valor) == 1) {
                                int ok = removeNo(arvRN, valor);
                                if (ok)
                                    printf("Valor %d removido da Rubro-Negra.\n", valor);
                                else
                                    printf("Valor %d não encontrado na Rubro-Negra.\n", valor);
                            }
                            break;
                        }
                        case 3:
                            // Impressão da RB
                            printf("\n-- Árvore Rubro-Negra (por nível) --\n");
                            imprimirPorNivelRN(retornaRaiz(arvRN), 0);
                            printf("\n-- Árvore Rubro-Negra (visual) --\n");
                            imprimirVisualRN(retornaRaiz(arvRN), 0);
                            break;
                        case 4:
                            // Sair do menu RB e retornar ao menu principal
                            printf("Retornando ao menu principal.\n");
                            break;
                        default:
                            printf("Opção inválida. Tente novamente.\n");
                            break;
                    }
                } while (opcRN != 4);
                break;
            }

            case 5:
                // Sair do programa
                printf("Programa encerrado.\n");
                break;

            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    } while (opcao != 5);
    // Libera toda a 2-3-4
    free234(arv->raiz);
    free(arv);

    if (arvRN) {
        freeRB(arvRN);
    }

    return 0;
}

