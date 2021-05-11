//-----------------------------------------------------------------------------
// Simulador de cache L1: um único nível de cache
//
// Para compilar: gcc simcacheL1.c -o simcacheL1 -Wall
// Para executar: simcacheL1 arquivo_configuracao arquivo_acessos
//-----------------------------------------------------------------------------
// Bibliotecas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.c"

//-----------------------------------------------------------------------------
// Programa principal

int main (int argc, char **argv)
{
	char nome_arq_config[100],
		  nome_arq_acessos[100] ;

	FILE *arq_config,		// Arquivo com configuração da cache
		  *arq_acessos ;	// Arquivo de trace com acessos a memória

	t_cache cacheL1 ; // Cache L1
	t_cache cacheL2;

	int n_blocosL1,			// Número total de blocos da cache L1
		 associatividadeL1,	// Número de blocos por conjunto da cache L1
		 n_palavras_blocoL1,	// Número de palavras do bloco da cache L1
		 n_acessos_cacheL1,	// Número total de acessos à cache L1
		 n_falhas_cacheL1,	// Número de falhas na cache L1
		 result,
		 result_acesso,
		 endereco ;				// Endereço de memória acessado

	int n_blocosL2,			// Variaveis de cache l2
		 associatividadeL2,	
	 	 n_palavras_blocoL2,
	 	 n_acessos_cacheL2,	
	 	 n_falhas_cacheL2,
	 	 result2,
		 result_acesso2,
		 endereco2;				// En

	// Inicializacao
	if (argc != 3)
	{
		printf("\nUso: simcacheL1 arquivo_configuracao arquivo_acessos\n") ;
		exit(0) ;
	}

	// Abre arquivo de configuração e lê n_blocos, associatividade e n_palavras_bloco da cache L1
	strcpy(nome_arq_config, argv[1]) ;
	arq_config = fopen(nome_arq_config, "rt") ;
	if (arq_config == NULL)
	{
		printf("\nArquivo de configuração não encontrado\n") ;
		exit(0) ;
	}
	fscanf(arq_config, "%d %d %d", &n_blocosL1, &associatividadeL1, &n_palavras_blocoL1) ;
	fclose(arq_config) ;

	// Abre arquivo de acessos
	strcpy(nome_arq_acessos, argv[2]) ;
   arq_acessos = fopen(nome_arq_acessos, "rt") ;
	if (arq_acessos == NULL)
	{
		printf("\nArquivo de acessos não encontrado\n") ;
		exit(0) ;
	}

	// Inicializa medidas de desempenho
	n_acessos_cacheL1 = 0 ;
	n_falhas_cacheL1  = 0 ;

	// Aloca e inicializa estrutura de dados da cache L1
	aloca_cache(&cacheL1, n_blocosL1, associatividadeL1, n_palavras_blocoL1) ;

	// Enquanto não termina arquivo de acessos
	while (! feof(arq_acessos))
	{
		// Lê um endereço do arquivo de acessos
		result = fscanf(arq_acessos, "%d", &endereco) ;

		if ((result != 0) && (result != EOF))
		{
			// Procura bloco na cache e, se não encontra, insere bloco na cache
			// Valor de retorno = 0 (acerto) ou 1 (falha)
			result_acesso = busca_e_insere_na_cache(&cacheL1, endereco) ;

			n_acessos_cacheL1 ++ ;

			if (result_acesso == 1) // Se houve falha na cache
			{
				n_falhas_cacheL1 ++ ;
			}
		}
	}

	// Finalizacao

	// Fecha arquivo de acessos
	fclose(arq_acessos) ;

	// Imprime medidas de desempenho
	printf("%10d %10d\n", n_acessos_cacheL1, n_falhas_cacheL1) ;

	// Libera estrutura de dados da cache
	libera_cache(&cacheL1) ;

	return 0 ;
}
//-----------------------------------------------------------------------------
