/***************************************************************************************
Aplicação de Multiplicação de Matrizes:

Usar uma matriz densa com números de ponto flutuante - tipo float - rand() / RAND_MAX
Variar o tamanho da matriz: 1000 x 1000; 5000 x 5000; 10.000 x 10.000
Variar o número de processos: 1, 2, 8, 10, 16, 30, 32, 64, 100, 128 (opcionalmente)
ou : 1, 2, 16, 32, 64, 128 (necessariamente)

No experimento: mostrar os gráficos de tempo de execução e de speedup;
pegar a média de 10 execuções e calcular o desvio padrão.


Nome: Vinícius Barreto de Sousa Neto
***************************************************************************************/

#include "stdio.h"
#include "mpi.h"
#include "time.h"
#include "stdlib.h"

#define MESTRE 0            // Rank do processo mestre 
#define FROM_MESTRE 1       // tipoMensagem (MESTRE -> ESCRAVO) 
#define FROM_ESCRAVO 2      // tipoMensagem (ESCRAVO -> MESTRE) 
#define numLinhas_A 1000	  // Número de linhas da matriz A
#define numColunas_A 1000   // Número de colunas da matriz A
#define numLinhas_B 1000	  // Número de linhas da matriz B
#define numColunas_B 1000	  // Número de colunas da matriz B

MPI_Status status;

void iniciar_aleatorio(void);
float aleatorio(int, int);
float** alocarMatriz(int linhas, int colunas);
void liberaMatriz(float** m, int colunas);

int main(int argc, char** argv) {

	int numProcessos,   // Número de processos
		  idProcesso,     // Rank do processo
		  numEscravos,    // Número de processos escravos
		  origem,         // Rank do processo origem (quem envia a mensagem)
      destino,        // Rank do processo destino (quem recebe a mensagem)
		  tipoMsg,        // Tipo da mensagem
		  linhas,         // Linhas da Matriz A a serem distribuídas para os processos escravos
		//numLinhas_A,
		//numColunas_A,
		//numLinhas_B,
		//numColunas_B,

		/* Variáveis auxiliares */
		divisaoLinhas, restoLinhas, offset,
		i, j, k, count;

	/* Marcações de tempo da multiplicação de matrizes */
	double t_inicio, t_final;
	
	float A[numLinhas_A][numColunas_A],
		  B[numLinhas_B][numColunas_B],
		  C[numLinhas_A][numColunas_B];

	/* Inicialização MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &idProcesso);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessos);
	numEscravos = numProcessos - 1;

	
	/* Tamanho das matrizes A e B através dos argumentos de linha de comando (NÃO ESTOU CONSEGUINDO FAZER ASSIM)
	printf("Leitura dos argumentos...\n");
	numLinhas_A = atoi(argv[1]);
	printf("\nargv[1] = %d\n", numLinhas_A);
	numColunas_A = atoi(argv[2]);
	printf("\nargv[2] = %d\n", numColunas_A);
	numLinhas_B = atoi(argv[3]);
	printf("\nargv[3] = %d\n", numLinhas_B);
	numColunas_B = atoi(argv[4]);
	printf("\nargv[4] = %d\n", numColunas_B);
	printf("\nPronto.\n");

	/* Alocação dinâmica das matrizes (NÃO ESTOU CONSEGUINDO FAZER ASSIM)
	float** A, ** B, ** C;

	printf("\nAlocando matrizes...\n");
	A = alocarMatriz(numLinhas_A, numColunas_A);
	B = alocarMatriz(numLinhas_B, numColunas_B);
	C = alocarMatriz(numLinhas_A, numColunas_B);
	printf("\nPronto.");
	*/

	/******* Processo mestre (rank == 0) ***********/
	// Preenchimento das matrizes A e B
	if (idProcesso == MESTRE) {

		printf("MM_Paralelo iniciou com %d processos.\n", numProcessos);

		printf("Inicializando matrizes...\n");
		for (i = 0; i < numLinhas_A; i++)
			for (j = 0; j < numColunas_A; j++)
				//A[i][j] = ((double) i + (double)j);   // Apenas para verificação do resultado
				A[i][j] = aleatorio(0, 9);
		for (i = 0; i < numLinhas_B; i++)
			for (j = 0; j < numColunas_B; j++)
				//B[i][j] = ((double) i * (double) j);  // Apenas para verificação do resultado
				B[i][j] = aleatorio(0, 9);
		printf("\nPronto.\n");

		/* Impressão das matrizes A */
		printf("\n******************************************************\n");
		printf("Matriz A:\n");
		for (i = 0; i < numLinhas_A; i++)
		{
			printf("\n");
			for (j = 0; j < numColunas_A; j++)
				printf("%6.2f   ", A[i][j]);
		}
		printf("\n******************************************************\n");
		/* Impressão das matrizes B */
		printf("\n******************************************************\n");
		printf("Matriz B:\n");
		for (i = 0; i < numLinhas_B; i++)
		{
			printf("\n");
			for (j = 0; j < numColunas_B; j++)
				printf("%6.2f   ", B[i][j]);
		}
		printf("\n******************************************************\n");

		/* Tempo inicial (A partir da divisão de tarefas) */
		t_inicio = MPI_Wtime();

		/* Divisão das tarefas entre os processos escravos */
		printf("\nConfigurando distribuição das tarefas...\n");
		divisaoLinhas = numLinhas_A / numEscravos; // Divisão das linhas da matriz A pelo número de escravos
		restoLinhas = numLinhas_A % numEscravos;   // Resto da divisão anterior
		offset = 0;								   // Offset auxiliar
		tipoMsg = FROM_MESTRE;					   // Tipo da mensagem
		printf("\nPronto.\n");

		for (destino = 1; destino <= numEscravos; destino++) {

			if (destino <= restoLinhas)
			{
				linhas = divisaoLinhas + 1;
			}
			else
			{
				linhas = divisaoLinhas;
			}
			printf("\nEnviando %d linhas da matriz A para o processo %d com offset = %d...\n", linhas, destino, offset);
			MPI_Send(&offset, 1, MPI_INT, destino, tipoMsg, MPI_COMM_WORLD);
			MPI_Send(&linhas, 1, MPI_INT, destino, tipoMsg, MPI_COMM_WORLD);
			count = linhas * numColunas_A;
			MPI_Send(&A[offset][0], count, MPI_DOUBLE, destino, tipoMsg, MPI_COMM_WORLD);
			count = numColunas_A * numColunas_B;
			MPI_Send(&B, count, MPI_DOUBLE, destino, tipoMsg, MPI_COMM_WORLD);
			offset = offset + linhas;
		}
		printf("\nPronto.\n");

		/* Recebe os resultados de todos os processos escravos */
		tipoMsg = FROM_ESCRAVO;
		for (i = 1; i <= numEscravos; i++) {
			printf("\nRecebendo o resultado do processo escravo %d...\n", i);
			origem = i;
			MPI_Recv(&offset, 1, MPI_INT, origem, tipoMsg, MPI_COMM_WORLD, &status);
			MPI_Recv(&linhas, 1, MPI_INT, origem, tipoMsg, MPI_COMM_WORLD, &status);
			count = linhas * numColunas_B;
			MPI_Recv(&C[offset][0], count, MPI_DOUBLE, origem, tipoMsg, MPI_COMM_WORLD, &status);
		}
		//liberaMatriz(A, numColunas_A);
		//liberaMatriz(B, numColunas_B);
		//liberaMatriz(C, numColunas_B);
		printf("\nPronto.\n");
		/* Tempo final (após receber os resultados dos processos escravos) */
		t_final = MPI_Wtime();

		// Matriz C
		printf("\n******************************************************\n");
		printf("Matriz C:\n");
		for (i = 0; i < numLinhas_A; i++)
		{
			printf("\n");
			for (j = 0; j < numColunas_B; j++)
				printf("%6.2f   ", C[i][j]);
		}
		printf("\n******************************************************\n");

		printf("Tempo: %f\n", (t_final - t_inicio));

	}

	/************ Processo escravo (rank > 0) *************/
	if (idProcesso > MESTRE) {

		tipoMsg = FROM_MESTRE;
		origem = MESTRE;
		MPI_Recv(&offset, 1, MPI_INT, origem, tipoMsg, MPI_COMM_WORLD, &status);
		MPI_Recv(&linhas, 1, MPI_INT, origem, tipoMsg, MPI_COMM_WORLD, &status);
		count = linhas * numColunas_A;
		MPI_Recv(&A, count, MPI_DOUBLE, origem, tipoMsg, MPI_COMM_WORLD, &status);
		count = numColunas_A * numColunas_B;
		MPI_Recv(&B, count, MPI_DOUBLE, origem, tipoMsg, MPI_COMM_WORLD, &status);

		for (k = 0; k < numColunas_B; k++)       // Multiplicação da partição local
			for (i = 0; i < linhas; i++) {
				C[i][k] = 0.0;
				for (j = 0; j < numColunas_A; j++)
					C[i][k] = C[i][k] + A[i][j] * B[j][k];
			}

		tipoMsg = FROM_ESCRAVO;
		MPI_Send(&offset, 1, MPI_INT, MESTRE, tipoMsg, MPI_COMM_WORLD);
		MPI_Send(&linhas, 1, MPI_INT, MESTRE, tipoMsg, MPI_COMM_WORLD);
		count = linhas * numColunas_B;
		MPI_Send(&C, count, MPI_DOUBLE, MESTRE, tipoMsg, MPI_COMM_WORLD);
	}

	/* Finalização MPI */
	MPI_Finalize();
}

/* Funções auxiliares para gerar números pseudo-aleatórios */
void iniciar_aleatorio(void)
{
	srand((unsigned int) time(NULL));
	rand();
	rand();
	rand();
}

float aleatorio(int a, int b)
{
	float r;
	r = (float) rand() / RAND_MAX;
	return (float) (a + r * (b - a));
}

float** alocarMatriz(int linhas, int colunas)
{

	int i;

	float** m = (float**) malloc(linhas * sizeof(float*));

	for (i = 0; i < linhas; i++) {
		m[i] = (float*) malloc(colunas * sizeof(float));
	}
	return m;
}

void liberaMatriz(float **m, int colunas)
{
	int i;

	for (i = 0; i < colunas; i++)
	{
		free(m[i]);
	}
	free(m);
}
