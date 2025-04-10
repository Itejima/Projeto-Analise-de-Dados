#ifndef ANALISE_H
#define ANALISE_H

#include <stdbool.h>

#define MAX_LINHA 512
#define MAX_NUMERO 32
#define MAX_DATA 11

typedef struct {
    int id;
    char numero[MAX_NUMERO];
    char data_ajuizamento[MAX_DATA]; // yyyy-mm-dd
    int id_classe;
    int id_assunto;
    int ano_eleicao;
} Processo;

typedef struct {
    Processo* processos;
    int quantidade;
    int capacidade;
} ListaProcessos;

ListaProcessos* criar_lista_processos();
void destruir_lista_processos(ListaProcessos* lista);

bool carregar_processos_csv(ListaProcessos* lista, const char* nome_arquivo);
bool salvar_para_csv(const ListaProcessos* lista, const char* nome_arquivo);

void ordenar_por_id(ListaProcessos* lista);
void ordenar_por_data(ListaProcessos* lista);

int contar_por_classe(const ListaProcessos* lista, int id_classe);
int contar_assuntos_diferentes(const ListaProcessos* lista);
void listar_multiplos_assuntos(const char* nome_arquivo);
double calcular_dias_tramitacao(const char* data_ajuizamento);

#endif // ANALISE_H