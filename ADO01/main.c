#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

// Algoritmo em paralelo para determinar o elemento de valor máximo de um vetor
// O elemento máximo vai se encontrar em vec2[1]

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

int main() {

	int *vec1;
	int size = 8;
	vec1 = malloc(sizeof(int) * size);
	
	
	// Preenche aleatoriamente e printa o vetor
	srand(time(NULL));
	printf("\n vec1: ");
	for (int i = 0; i < size; i++ ) {
		vec1[i] = rand()%50;
		printf("%d, ", vec1[i]);
	}


	// Tarefa1 - Cópia em paralelo
	int *vec2 = malloc(sizeof(int) * (size*2));
	int n = size;
	#pragma omp parallel shared(vec1, vec2, n) 
	{
		// Teorema de brent
		int num_threads = omp_get_num_threads();
		int tid = omp_get_thread_num();
		double gap = max(1, (n/num_threads));
		int lower_limit = tid * gap;
		int upper_limit = (tid+1) * gap;
		// fim teorema de brent

		if (upper_limit <= n) {
			for( int i = lower_limit; i < upper_limit; i++) {
				vec2[size+i] = vec1[i];
			}
		}
	}


	// Tarefa2 - loop sequencial
	printf("\n\n");
	for (int j = log2(size) - 1; j >= 0; j--) {
		int n = (int) pow(2, j);
		
		#pragma omp parallel shared(vec1, vec2, n, j) 
		{
			// Teorema de brent
			int num_threads = omp_get_num_threads();
			int tid = omp_get_thread_num();
			double gap = max(1, (n/num_threads)); 
			int lower_limit = tid * gap;
			int upper_limit = (tid+1) * gap;
			// fim teorema de brent

			if (lower_limit < n) {
				int addr = n; // começo
				for(int i = addr+lower_limit; i<addr+upper_limit; i++) {
					vec2[i] = max(vec2[i*2], vec2[i*2+1]);
				}
			}	
		}

		// Printa vec2
		printf("\n vec2 n=%d: ", n);
		for (int i = 0; i < size*2; i++) {
			printf("%d, ", vec2[i]);
		}
	};
	
	free(vec2);
}
