#include <stdio.h>
#include <stdlib.h>
#include "analise.h"

void exibir_menu() {
    printf("\n=== MENU DE ANALISE DE PROCESSOS ===\n");
    printf("1. Carregar dados do arquivo CSV\n");
    printf("2. Ordenar e salvar por ID\n");
    printf("3. Ordenar e salvar por data\n");
    printf("4. Contar processos por classe\n");
    printf("5. Contar assuntos diferentes\n");
    printf("6. Listar processos com multiplos assuntos\n");
    printf("7. Calcular dias em tramitacao\n");
    printf("8. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    ListaProcessos* lista = criar_lista_processos();
    if (!lista) {
        fprintf(stderr, "Erro ao criar lista de processos\n");
        return 1;
    }

    int opcao;
    char nome_arquivo[] = "processo_043_202409032338.csv";

    do {
        exibir_menu();
        scanf("%d", &opcao);
        getchar(); // consumir newline

        switch (opcao) {
            case 1:
                carregar_processos_csv(lista, nome_arquivo);
                break;
            case 2:
                ordenar_por_id(lista);
                salvar_para_csv(lista, "ordenado_por_id.csv");
                printf("Processos ordenados por ID e salvos em 'ordenado_por_id.csv'.\n");
                break;
            case 3:
                ordenar_por_data(lista);
                salvar_para_csv(lista, "ordenado_por_data.csv");
                printf("Processos ordenados por data e salvos em 'ordenado_por_data.csv'.\n");
                break;
            case 4: {
                int id;
                printf("Informe o ID da classe: ");
                scanf("%d", &id);
                getchar();
                int total = contar_por_classe(lista, id);
                printf("Total de processos com classe %d: %d\n", id, total);
                break;
            }
            case 5: {
                int total = contar_assuntos_diferentes(lista);
                printf("Total de assuntos diferentes: %d\n", total);
                break;
            }
            case 6:
                listar_multiplos_assuntos(nome_arquivo);
                break;
            case 7:
                for (int i = 0; i < lista->quantidade; i++) {
                    double dias = calcular_dias_tramitacao(lista->processos[i].data_ajuizamento);
                    printf("Processo %d: %.0f dias\n", lista->processos[i].id, dias);
                }
                break;
            case 8:
                printf("Encerrando a aplicacao...\n");
                destruir_lista_processos(lista);
                exit(0);
            default:
                printf("Opcao invalida!\n");
        }

    } while (opcao != 8);

    destruir_lista_processos(lista);
    return 0;
}