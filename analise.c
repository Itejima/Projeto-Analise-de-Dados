#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "analise.h"

#define CAPACIDADE_INICIAL 100
#define FATOR_CRESCIMENTO 2

ListaProcessos* criar_lista_processos() {
    ListaProcessos* lista = malloc(sizeof(ListaProcessos));
    if (!lista) return NULL;

    lista->processos = malloc(CAPACIDADE_INICIAL * sizeof(Processo));
    if (!lista->processos) {
        free(lista);
        return NULL;
    }

    lista->quantidade = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
    return lista;
}

void destruir_lista_processos(ListaProcessos* lista) {
    if (lista) {
        free(lista->processos);
        free(lista);
    }
}

bool redimensionar_lista(ListaProcessos* lista, int nova_capacidade) {
    Processo* temp = realloc(lista->processos, nova_capacidade * sizeof(Processo));
    if (!temp) return false;

    lista->processos = temp;
    lista->capacidade = nova_capacidade;
    return true;
}

bool validar_data(const char* data) {
    if (strlen(data) != 10) return false;
    if (data[4] != '-' || data[7] != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(data[i])) return false;
    }
    return true;
}

void extrair_data_formatada(const char* data_hora, char* data_saida) {
    strncpy(data_saida, data_hora, 10);
    data_saida[10] = '\0';
}

int extrair_primeiro_numero(const char* texto) {
    while (*texto && !isdigit(*texto)) texto++;
    return atoi(texto);
}

bool carregar_processos_csv(ListaProcessos* lista, const char* nome_arquivo) {
    FILE* fp = fopen(nome_arquivo, "r");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        return false;
    }

    char linha[MAX_LINHA];
    fgets(linha, sizeof(linha), fp); // Pular cabeçalho

    while (fgets(linha, sizeof(linha), fp)) {
        if (lista->quantidade >= lista->capacidade) {
            if (!redimensionar_lista(lista, lista->capacidade * FATOR_CRESCIMENTO)) {
                fclose(fp);
                return false;
            }
        }

        Processo* p = &lista->processos[lista->quantidade];
        char* token = strtok(linha, ",");
        if (!token) continue;
        p->id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(p->numero, token, sizeof(p->numero));

        token = strtok(NULL, ",");
        if (!token) continue;
        extrair_data_formatada(token, p->data_ajuizamento);

        token = strtok(NULL, ",");
        if (!token) continue;
        p->id_classe = extrair_primeiro_numero(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        p->id_assunto = extrair_primeiro_numero(token);

        token = strtok(NULL, ",\n");
        if (!token) continue;
        p->ano_eleicao = atoi(token);

        if (!validar_data(p->data_ajuizamento)) {
            fprintf(stderr, "Data invalida: %s\n", p->data_ajuizamento);
            continue;
        }

        lista->quantidade++;
    }

    fclose(fp);
    printf("Carregados %d processos\n", lista->quantidade);
    return true;
}

int comparar_id(const void* a, const void* b) {
    return ((Processo*)a)->id - ((Processo*)b)->id;
}

void ordenar_por_id(ListaProcessos* lista) {
    qsort(lista->processos, lista->quantidade, sizeof(Processo), comparar_id);
}

int comparar_data(const void* a, const void* b) {
    return strcmp(((Processo*)a)->data_ajuizamento, ((Processo*)b)->data_ajuizamento);
}

void ordenar_por_data(ListaProcessos* lista) {
    qsort(lista->processos, lista->quantidade, sizeof(Processo), comparar_data);
}

bool salvar_para_csv(const ListaProcessos* lista, const char* nome_arquivo) {
    if (lista->quantidade == 0) {
        printf("Nenhum dado para salvar!\n");
        return false;
    }

    FILE* fp = fopen(nome_arquivo, "w");
    if (!fp) {
        perror("Erro ao criar arquivo");
        return false;
    }

    fprintf(fp, "id,numero,data_ajuizamento,id_classe,id_assunto,ano_eleicao\n");

    for (int i = 0; i < lista->quantidade; i++) {
        const Processo* p = &lista->processos[i];
        fprintf(fp, "%d,\"%s\",%s,%d,%d,%d\n",
                p->id, p->numero, p->data_ajuizamento,
                p->id_classe, p->id_assunto, p->ano_eleicao);
    }

    fclose(fp);
    printf("Arquivo %s salvo com sucesso (%d registros)\n", nome_arquivo, lista->quantidade);
    return true;
}

int contar_por_classe(const ListaProcessos* lista, int id_classe) {
    int contagem = 0;
    for (int i = 0; i < lista->quantidade; i++) {
        if (lista->processos[i].id_classe == id_classe) contagem++;
    }
    return contagem;
}

int contar_assuntos_diferentes(const ListaProcessos* lista) {
    int* assuntos = malloc(lista->quantidade * sizeof(int));
    int total = 0;

    for (int i = 0; i < lista->quantidade; i++) {
        int assunto_atual = lista->processos[i].id_assunto;
        bool unico = true;

        for (int j = 0; j < total; j++) {
            if (assuntos[j] == assunto_atual) {
                unico = false;
                break;
            }
        }

        if (unico) assuntos[total++] = assunto_atual;
    }

    free(assuntos);
    return total;
}

void listar_multiplos_assuntos(const char* nome_arquivo) {
    FILE* fp = fopen(nome_arquivo, "r");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        return;
    }

    char linha[MAX_LINHA];
    fgets(linha, sizeof(linha), fp); // Pular cabeçalho

    printf("Processos com multiplos assuntos:\n");
    while (fgets(linha, sizeof(linha), fp)) {
        if (strstr(linha, ",{")) {
            printf("%s", linha);
        }
    }

    fclose(fp);
}

double calcular_dias_tramitacao(const char* data_ajuizamento) {
    struct tm tm = {0};
    int ano, mes, dia;

    if (sscanf(data_ajuizamento, "%d-%d-%d", &ano, &mes, &dia) != 3) {
        return -1;
    }

    tm.tm_year = ano - 1900;
    tm.tm_mon = mes - 1;
    tm.tm_mday = dia;
    tm.tm_isdst = -1;

    time_t inicio = mktime(&tm);
    if (inicio == -1) return -1;

    return difftime(time(NULL), inicio) / (60 * 60 * 24);
}