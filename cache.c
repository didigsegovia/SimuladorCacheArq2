//-----------------------------------------------------------------------------
// Declaração de tipos e funções para implementação de cache
//-----------------------------------------------------------------------------
// Bibliotecas

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//-----------------------------------------------------------------------------
// Constantes

#define N_BYTES_PALAVRA		4	// Numero de bytes da palavra

//-----------------------------------------------------------------------------
// Tipos

typedef struct // Posicao da cache
{
	int bit_validade,
		 tag,
		 ordem_acesso ; // Para LRU
} t_posicao ;

typedef struct // Conjunto da cache
{
	t_posicao *via ;
} t_conjunto ;

typedef struct // Cache
{
	t_conjunto *conjunto ;
	int n_blocos,         // Numero total de blocos da cache
		 associatividade,  // Numero de blocos por conjunto
		 n_palavras_bloco, // Numero de palavras do bloco
		 n_conjuntos ;     // Numero de conjuntos da cache
} t_cache ;

//-----------------------------------------------------------------------------
// Prototipos de funcoes

void aloca_cache(t_cache *cache, int n_blocos, int associatividade, int n_palavras_bloco) ;

int busca_e_insere_na_cache(t_cache *cache, int endereco) ;

void libera_cache(t_cache *cache) ;

//-----------------------------------------------------------------------------
// Aloca e inicializa cache

void aloca_cache(t_cache *cache, int n_blocos, int associatividade, int n_palavras_bloco)
{
	int i,
		 j ;

	// Seta parametros da configuracao
	(*cache).n_blocos         = n_blocos ;
	(*cache).associatividade  = associatividade ;
	(*cache).n_palavras_bloco = n_palavras_bloco ;
	(*cache).n_conjuntos      = n_blocos / associatividade ;

	(*cache).conjunto = calloc((*cache).n_conjuntos, sizeof(t_conjunto)) ;

	if ((*cache).conjunto == NULL)
	{
		printf("Erro na alocacao de estrutura de dados\n") ;
		exit(0) ;
	}

	for (i = 0 ; i < (*cache).n_conjuntos ; i ++)
	{
		(*cache).conjunto[i].via = calloc(associatividade, sizeof(t_posicao)) ;

		if ((*cache).conjunto[i].via == NULL)
		{
			printf("Erro na alocacao de estrututra de dados\n") ;
			exit(0) ;
		}

		for (j = 0 ; j < associatividade ; j ++)
		{
			(*cache).conjunto[i].via[j].bit_validade = 0 ;
			(*cache).conjunto[i].via[j].tag          = 0 ;
			(*cache).conjunto[i].via[j].ordem_acesso = 0 ;
		}
	}
}

//-----------------------------------------------------------------------------
// Busca dado na cache e, se não encontra, insere-o
// Valor de retorno: 0 (acerto) ou 1 (falha)

int busca_e_insere_na_cache(t_cache *cache, int endereco)
{
	int endereco_palavra,
		 endereco_bloco,
		 indice,
		 tag,
		 i,
		 i_acerto,	// Posição que causou acerto
		 i_livre,	// Primeira posição livre encontrada no conjunto
		 i_lru,		// Posição com menor valor de ordem_acesso no conjunto (bloco LRU)
		 ordem_min,	// Dentre as posições ocupadas no conjunto, menor valor de ordem_acesso
		 ordem_max,	// Dentre as posições ocupadas no conjunto, maior valor de ordem_acesso
		 result ;

	endereco_palavra = endereco / N_BYTES_PALAVRA ;
	endereco_bloco   = endereco_palavra / (*cache).n_palavras_bloco ;
	indice           = endereco_bloco   % (*cache).n_conjuntos ;
	tag              = endereco_bloco   / (*cache).n_conjuntos ;

	// Procura dado na cache

	i_acerto = -1 ;
	i_livre  = -1 ;
	i_lru    = -1 ;
	ordem_min  = INT_MAX ;
	ordem_max  = -1 ;

	for (i = 0 ; i < (*cache).associatividade ; i ++)
	{
		if ((*cache).conjunto[indice].via[i].bit_validade == 1) // Posicao possui dado válido
		{
			if ((*cache).conjunto[indice].via[i].tag == tag) // Acerto
			{
				i_acerto = i ;
			}

			if (ordem_max < (*cache).conjunto[indice].via[i].ordem_acesso)
			{
				ordem_max = (*cache).conjunto[indice].via[i].ordem_acesso ;
			}
	
			if (ordem_min > (*cache).conjunto[indice].via[i].ordem_acesso)
			{
				i_lru     = i ;
				ordem_min = (*cache).conjunto[indice].via[i].ordem_acesso ;
			}
		}
		else // Posicao não possui dado válido
		{
			if (i_livre == -1)
			{
				i_livre = i ;
			}
		}
	}

	if (i_acerto != -1) // Acerto
	{
		(*cache).conjunto[indice].via[i_acerto].ordem_acesso = ordem_max + 1 ;
		result = 0 ;
	}
	else if (i_livre != -1) // Falha SEM substitucao
	{
		(*cache).conjunto[indice].via[i_livre].bit_validade = 1 ;
		(*cache).conjunto[indice].via[i_livre].tag          = tag ;
		(*cache).conjunto[indice].via[i_livre].ordem_acesso = ordem_max + 1 ;
		result = 1 ;
	}
	else // Falha COM substitucao
	{
		(*cache).conjunto[indice].via[i_lru].tag          = tag ;
		(*cache).conjunto[indice].via[i_lru].ordem_acesso = ordem_max + 1 ;
		result = 1 ;
	}

	return result ;
}

//-----------------------------------------------------------------------------
void libera_cache(t_cache *cache)
{
	free((*cache).conjunto) ;
}
//-----------------------------------------------------------------------------
