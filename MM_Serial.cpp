#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Tamanho da matriz A */
#define numLinhas_A 1000   // Número de linhas da matriz A
#define numColunas_A 1000  // Número de colunas da matriz A
/* Tamanho da matriz B */
#define numLinhas_B 1000   // Número de linhas da matriz B
#define numColunas_B 1000  // Número de colunas da matriz B

void iniciar_aleatorio(void);
float aleatorio(int, int);
float** alocarMatriz(int linhas, int colunas);
void liberaMatriz(float** m, int colunas);

int main(int argc, char* argv[])
{
	/* Variáveis auxiliares */
	int i, j, k = 0;

	/* Marcaçõees de tempo da multiplicação de matrizes */
	double tempo = 0.0;
	clock_t t_inicio, t_final;

	float A[numLinhas_A][numColunas_A],
		B[numLinhas_B][numColunas_B],
		C[numLinhas_A][numColunas_B];

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

	// Preenchimento das matrizes A e B
	printf("Preenchendo as matrizes A e B...\n");
	for (i = 0; i < numLinhas_A; i++)
		for (j = 0; j < numColunas_A; j++)
			A[i][j] = aleatorio(0, 9);
	for (i = 0; i < numColunas_B; i++)
		for (j = 0; j < numColunas_B; j++)
			B[i][j] = aleatorio(0, 9);
	printf("\nPronto.\n");
	
	// Matriz A
	printf("\n******************************************************\n");
	printf("Matriz A:\n");
	for (i = 0; i < numLinhas_A; i++)
	{
		printf("\n");
		for (j = 0; j < numColunas_A; j++)
			printf("%6.2f   ", A[i][j]);
	}
	printf("\n******************************************************\n");

	// Matriz B
	printf("\n******************************************************\n");
	printf("Matriz B:\n");
	for (i = 0; i < numLinhas_B; i++)
	{
		printf("\n");
		for (j = 0; j < numColunas_B; j++)
			printf("%6.2f   ", B[i][j]);
	}
	printf("\n******************************************************\n");
	
	// Marca o tempo antes da multiplicação
	t_inicio = clock();

	// Multiplicação das matrizes -> C = A * B
	for (k = 0; k < numColunas_B; k++)
		for (i = 0; i < numLinhas_A; i++)
		{
			C[i][k] = 0.0;
			for (j = 0; j < numColunas_A; j++)
				C[i][k] = C[i][k] + A[i][j] * B[j][k];
		}

	// Marca o tempo depois da multiplicação
	t_final = clock();

	// Calcula o tempo de execução da multiplicação
	tempo = ((t_final - t_inicio) * 1000 / CLOCKS_PER_SEC);
	
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
	
	printf("Tempo: %f ms\n", tempo);

	//liberaMatriz(A, numColunas_A);
	//liberaMatriz(B, numColunas_B);
	//liberaMatriz(C, numColunas_B);
}

/* Funções auxiliares para gerar números pseudo-aleatórios */
void iniciar_aleatorio(void)
{
	srand((unsigned int)time(NULL));
	rand();
	rand();
	rand();
}

float aleatorio(int a, int b)
{
	float r;
	r = (float)rand() / RAND_MAX;
	return (float)(a + r * (b - a));
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

void liberaMatriz(float** m, int colunas)
{
	int i;

	for (i = 0; i < colunas; i++)
	{
		free(m[i]);
	}
	free(m);
}
