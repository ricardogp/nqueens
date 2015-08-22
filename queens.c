/*
 * Solve the N-queens problem using genetic algorithm
 * */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define WORST_FITNESS 56

int nQueens;
int verboseMode = 0; 

typedef struct
{
	int *solution; /* Each element represents an colunm and it stores the line position.*/
	int fitness;
	
} Individual;


/* Dump entire individual data. */
void printIndividual(Individual* ind)
{
	int i;	
	
	printf("[%d", ind->solution[0]);	
	
	for (i = 1; i < nQueens; ++i)
		printf(",%d", ind->solution[i]);		
	
	printf("] fitness = %d\n", ind->fitness);	
}

/* Compare two individuals using the fitness information.*/
int compareIndividuals(const void* p1, const void* p2)
{
	Individual** indA = (Individual**) p1;
	Individual** indB = (Individual**) p2;
	
	if (!*indA || !*indB)
		return 0;
		
	return (-(*indA)->fitness) - (-(*indB)->fitness);
}

/* Calculate and set the fitness. */
void getFitness(Individual* ind)
{
	int i, j, k;
		
	for (i = 0; i < nQueens; ++i)
	{		
		/* Checking if there are others queens in the same line. */
		for (j = 0; j < nQueens ; ++j)
			if (ind->solution[j] == ind->solution[i] && i != j)
				--ind->fitness;
		
		/* Checking if there are queens in the same diagonal in a column before. */
		j = i - 1;
		k = 1;		
		while (j >= 0)
		{	
			if (ind->solution[j] == ind->solution[i] - k)
				--ind->fitness;				
			if (ind->solution[j] == ind->solution[i] + k)
				--ind->fitness;
			--j;
			++k;
		}
		
		/* Checking if there are queens in the same diagonal in a column after. */
		j = i + 1;
		k = 1;		
		while (j < nQueens)
		{
			if (ind->solution[j] == ind->solution[i] - k)
				--ind->fitness;
			if (ind->solution[j] == ind->solution[i] + k)
				--ind->fitness;
			++j;
			++k;
		}
	}
}

Individual* createIndividual()
{
	int i;
	Individual* ind = (Individual*) malloc(sizeof(Individual));
	
	if (!ind)
	{
		printf("Memory allocation failed.\n");
		return NULL;
	}
		
	ind->solution = (int *) malloc(sizeof(int) * nQueens);
	
	if (!ind->solution)
	{
		printf("Memory allocation failed.\n");
		free(ind);
		return NULL;
	}
		
	for (i = 0; i < nQueens; ++i)
		ind->solution[i] = rand() % nQueens; 
	
	getFitness(ind);	
	return ind;
}

Individual *crossover(Individual *ind1, Individual *ind2)
{
	int i;
	const int crossPoint = 1 + rand() % (nQueens - 2);
	Individual* indTemp;
	
	indTemp = (Individual*) malloc(sizeof(Individual*));	
	
	if (!indTemp)
	{
		printf("Memory allocation failed.\n");
		return NULL;
	}
	
	indTemp->solution = (int *) malloc(sizeof(int) * nQueens);
	
	if (!indTemp->solution)
	{
		printf("Memory allocation failed.\n");
		free(indTemp);
		return NULL;
	}
	
	indTemp->fitness = 0;
	
	for (i = 0; i < nQueens; ++i)
		indTemp->solution[i] = ind1->solution[i];
	
	for (i = crossPoint; i < nQueens; ++i)
		indTemp->solution[i] = ind2->solution[i];
	
	getFitness(indTemp);	
	return indTemp;
}

void mutation(Individual *ind)
{
	const int max = nQueens - 1;
	ind->solution[rand() % max] = rand() % max;	
	getFitness(ind);
}

void nextPopulation(Individual **population, int populationSize)
{
	int i;	
	int nullCount = 0;
	int indA, indB;
	int mutationCount = 0;
	int* nullIndexes = (int*) malloc(sizeof(int) * populationSize);
	
	if (!nullIndexes)
	{
		printf("Memory allocation failed.\n");
		return;
	}

	// Selecting the individuals that will be moved to the next population.
	// This will be done based only in probability.
	for (i = 0; i < populationSize; ++i)
	{		
		// Individuals with good fitness have more probability to be selected.
		if (rand() % WORST_FITNESS < -(population[i]->fitness))
		{
			free(population[i]->solution);
			free(population[i]);
			population[i] = NULL;
			nullIndexes[nullCount++] = i;
		}
	}
	
	for (i = 0; i < nullCount; ++i)
	{
		do
		{
			indA = rand() % populationSize;			
		} while(population[indA] == NULL);
		
		do
		{
			indB = rand() % populationSize;			
		} while(population[indB] == NULL || indA == indB);
		
		population[nullIndexes[i]] = crossover(population[indA], population[indB]);
	}
	
	for (i = 0; i < populationSize; ++i)
	{
		if (rand() % 1000 < 5)
		{
			mutation(population[i]);
			++mutationCount;
		}
	}
	
	if (verboseMode)
	{
		printf("copied: %d born: %d mutation: %.2f%% best: ", populationSize - nullCount, nullCount, (double) mutationCount / populationSize * 100);
		printIndividual(population[0]);
	}

	free(nullIndexes);
}

int main(int argc, char** argv)
{
	int i, j;
	Individual **population;
	int individualCount;
	int numSteps;	
	int maxSolutions = 1;
	
	srand(time(NULL));
	
	if (argc < 4)
	{
		printf("\n\nInvalid parameters. Use: %s N_QUEENS POPULATION_SIZE STEPS MAX_SOLUTIONS\n\n", argv[0]);
		return 0;
	}
		
	nQueens			= atoi(argv[1]);
	individualCount = atoi(argv[2]);
	numSteps		= atoi(argv[3]);
	
	if (argc > 4)
	{
		maxSolutions = atoi(argv[4]);
	}

	if (argc > 5)
	{
		if (!strcmp("-v", argv[5]))
			verboseMode = 1;
	}
	
	if (nQueens % 2)
	{
		printf("\nNUM_QUEENS must be an even number.\n");
		return 0;
	}
	
	printf("-----------------------------------------------\n");
	printf("Solving %d queens problem.\n", nQueens);
	printf("Population size: %d\n", individualCount);
	printf("Max number of generation(s): %d\n", numSteps);
	printf("Will stop after finding %d solution(s)\n", maxSolutions);
	printf("-----------------------------------------------\n");
		
	population = (Individual**) malloc (sizeof(Individual*) * individualCount);
	
	for (i = 0; i < individualCount; ++i)
	{
		population[i] = createIndividual();		
	}
	
	for (i = 0; i < numSteps; ++i)
	{		
		qsort(population, individualCount, sizeof(Individual*), compareIndividuals);						
		
		j = 0;
		while (population[j]->fitness == 0)
			++j;
		
		if (j >= maxSolutions)
		{		
			printf("Population #%d.\n", i + 1);
			j = 0;
			while (population[j]->fitness == 0 && j < maxSolutions)
			{
				printf("Solution %d: ", j + 1);
				printIndividual(population[j++]);
			}				
			break;
		}
		if (verboseMode)
		{
			printf("Building population %d: ", i);
		}
		nextPopulation(population, individualCount);		
	}
	
	printf("\n");
	
	for (i = 0; i < individualCount; ++i)
	{
		free(population[i]->solution);
		free(population[i]);
	}
	
	free(population);
	return 0;
}
