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
        perror("Nao foi possivel abrir o arquivo");
        return 1;
    }

    // Cria e inicializa a raiz da árvore 2-3-4
    No234 *raiz = NULL;
    int valor;

    // Leitura de todos os inteiros do arquivo, inserindo na B-Tree
    while (fscanf(arquivo, "%d", &valor) == 1) {
        inserir234(&raiz, valor);
    }
    fclose(arquivo);

    // Exibe a árvore 2-3-4 após a leitura inicial 
    printf("\nArvore 2-3-4 gerada! \n");
    printf("\n-- Arvore 2-3-4 (por nivel) --\n");
    imprimirPorNivel(raiz, 0);
    printf("\n-- Arvore 2-3-4 (visual) --\n");
    imprimirVisual(raiz, 0);

    int opcao;
    // Loop principal do menu de operações 
    do {
        printf("\nMenu:\n");
        printf("1. Inserir novo elemento na arvore 2-3-4\n");
        printf("2. Remover elemento da arvore 2-3-4\n");
        printf("3. Imprimir arvore 2-3-4\n");
        printf("4. Converter em uma arvore rubro-negra\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            // Limpa o buffer de entrada em caso de erro
            while(getchar() != '\n');
            opcao = 0; // Reseta para opção inválida
        }

        switch (opcao) {
            case 1:
                printf("Digite o valor a inserir: ");
                if (scanf("%d", &valor) == 1) {
                    inserir234(&raiz, valor);
                    printf("Valor %d inserido com sucesso.\n", valor);
                } else {
                     while(getchar() != '\n');
                     printf("Entrada invalida.\n");
                }
                break;

            case 2:
                printf("Digite o valor a remover: ");
                if (scanf("%d", &valor) == 1) {
                    if (remover234(&raiz, valor))
                        printf("Valor %d removido (ou processo de remocao concluido).\n", valor);
                    else
                        printf("Valor %d nao encontrado na arvore 2-3-4.\n", valor);
                } else {
                     while(getchar() != '\n');
                     printf("Entrada invalida.\n");
                }
                break;

            case 3:
                printf("\n-- Arvore 2-3-4 (por nivel) --\n");
                imprimirPorNivel(raiz, 0);
                printf("\n-- Arvore 2-3-4 (visual) --\n");
                imprimirVisual(raiz, 0);
                break;

            case 4: {
                rb *arvRN = converterParaRN(raiz);
            
                printf("\nArvore Rubro-Negra gerada! \n");
                printf("\n-- Arvore Rubro-Negra (por nivel) --\n");
                imprimirPorNivelRN(retornaRaiz(arvRN), 0);
                printf("\n-- Arvore Rubro-Negra (visual) --\n");
                imprimirVisualRN(retornaRaiz(arvRN), 0);
                
                int opcRN;
                do {
                    printf("\nMenu Rubro-Negra:\n");
                    printf("1. Inserir novo elemento\n");
                    printf("2. Remover elemento\n");
                    printf("3. Imprimir arvore\n");
                    printf("4. Voltar ao menu principal\n");
                    printf("Escolha uma opcao: ");
                    if (scanf("%d", &opcRN) != 1) {
                         while(getchar() != '\n');
                         opcRN = 0;
                    }

                    switch (opcRN) {
                        case 1:
                            printf("Digite o valor a inserir na RN: ");
                            if (scanf("%d", &valor) == 1) {
                                noRB *novo = alocaNo(arvRN, valor);
                                insereNo(arvRN, novo);
                                printf("Valor %d inserido na Rubro-Negra.\n", valor);
                            }
                            break;
                        case 2:
                            printf("Digite o valor a remover da RN: ");
                            if (scanf("%d", &valor) == 1) {
                                if (removeNo(arvRN, valor))
                                    printf("Valor %d removido da Rubro-Negra.\n", valor);
                                else
                                    printf("Valor %d nao encontrado na Rubro-Negra.\n", valor);
                            }
                            break;
                        case 3:
                            printf("\n-- Arvore Rubro-Negra (por nivel) --\n");
                            imprimirPorNivelRN(retornaRaiz(arvRN), 0);
                            printf("\n-- Arvore Rubro-Negra (visual) --\n");
                            imprimirVisualRN(retornaRaiz(arvRN), 0);
                            break;
                        case 4:
                            printf("Retornando ao menu principal.\n");
                            break;
                        default:
                            printf("Opcao invalida. Tente novamente.\n");
                            break;
                    }
                } while (opcRN != 4);
                break;
            }

            case 5:
                printf("Programa encerrado.\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 5);

    return 0;
}