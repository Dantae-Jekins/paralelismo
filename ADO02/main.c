#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

// Algoritmo em paralelo para determinar a soma de prefixos de um vetor

// CONVENÇÕES:
// size = 
// tamanho do vetor, deve ser potência de 2, ou ajeitado para ser

// n =
// número de tarefas, usado para definir quais tarefas para qual thread

// n/ num_p  =  
// núm de tarefas sobre núm de threads (tarefa por thread)

// under_limit, upper_limit = 
// de qual tarefa pra qual tarefa (intervalo [under, upper]) essa thread trata


#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

int main() {

	int *vec1;
	int size = 4;
	vec1 = malloc(sizeof(int) * size);
	
	
	// Preenche aleatoriamente e printa o vetor
	srand(time(NULL));
	printf("\n vec1: ");
	for (int i = 0; i < size; i++ ) {
		vec1[i] = rand()%50;
		printf("%d, ", vec1[i]);
	}


	// Tarefa1 - Cópia em paralelo para vetor auxiliar vec2
	int *vec2 = malloc(sizeof(int) * (size*2));

	int n = size;
	#pragma omp parallel shared(vec1, vec2, n) 
	{
		// Teorema de brent
		int num_threads = omp_get_num_threads();
		int tid = omp_get_thread_num();
		double gap = max(1, (n/num_threads)); // min 1 tarefa por thread
		int lower_limit = tid * gap;
		int upper_limit = (tid+1) * gap;
		// fim teorema de brent

        // vê se a thread é obsoleta (lower limit acima do n de tarefas)
		if (upper_limit <= n) {
			for( int i = lower_limit; i < upper_limit; i++) {
				vec2[size+i] = vec1[i];
			}
		}
	}


	// Tarefa2 - loop sequencial soma as folhas em esquema de árvore binária
	printf("\n\n");
	for (int j = log2(size) - 1; j >= 0; j--) {

		int n = (int) pow(2, j);
		#pragma omp parallel shared(vec1, vec2, n, j) 
		{
			// Teorema de brent
			int num_threads = omp_get_num_threads();
			int tid = omp_get_thread_num();
			double gap = max(1, (n/num_threads)); // min 1 tarefa por thread
			int lower_limit = tid * gap;
			int upper_limit = (tid+1) * gap;
			// fim teorema de brent

            // vê se a thread é obsoleta (lower limit acima do n de tarefas)
			if (lower_limit < n) {
                // AQUI VAI O CÓDIGO EM PARALELO PARA CADA PROCESSADOR

				int addr = n; // começo
				for(int i = addr+lower_limit; i<addr+upper_limit; i++) {
					vec2[i] = vec2[i*2] + vec2[i*2+1];
				}
			}	
		}

		// Printa vec2 (estrutura auxiliar)
		printf("\n vec2 n=%d: ", n);
		for (int i = 0; i < size*2; i++) {
			printf("%d, ", vec2[i]);
		}
	};


    printf("\n");
    // Tarefa3 - descida na árvore para ajeitar a soma de prefixos
    int *vec3 = malloc(sizeof(int) * (size*2));
    vec3[1] = vec2[1];
	for (int j = 1; j <= log2(size); j++) {
		
        int n = (int) pow(2, j);
		#pragma omp parallel shared(vec1, vec2, n, j) 
		{
			// Teorema de brent
			int num_threads = omp_get_num_threads();
			int tid = omp_get_thread_num();
			double gap = max(1, (n/num_threads));  // min 1 tarefa por thread
			int lower_limit = tid * gap;
			int upper_limit = (tid+1) * gap;
			// fim teorema de brent

            // vê se a thread é obsoleta (lower limit acima do n de tarefas)
			if (lower_limit < n) {
                // AQUI VAI O CÓDIGO EM PARALELO PARA CADA PROCESSADOR

				int addr = n; // começo
				for(int i = addr+lower_limit; i<addr+upper_limit; i++) {
                    if ( i%2 == 0) {
                        vec3[i] = vec3[i/2] - vec2[i+1];
                    }

                    else {
                        vec3[i] = vec3[(i-1)/2];
                    }
				}
			}	
		}

		// Printa vec3 (estrutura final, soma de prefixos)
		printf("\n vec3 n=%d: ", n);
		for (int i = 0; i < size*2; i++) {
			printf("%d, ", vec3[i]);
		}
	};


	free(vec2);
    free(vec3);
}
