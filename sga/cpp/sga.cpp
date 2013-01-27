
#include "fitness.h"
#include "ExhaustiveResults10x25.h"

//#include <stdlib.h>
//#include <time.h>

//#include <random>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

#include <fstream>
#include <iostream>
#include <sstream>


#define TRIAL_LIMIT			10
#define GENERATION_LIMIT	50
#define POPULATION_LIMIT	200

#define MUTATION_RATE		0.003
#define CROSSOVER_POINTS	1

using namespace std;

class randomizers
{
private:
	boost::random::mersenne_twister::mt19937 gen;

	// Possible Diagnoses: 0 ~ 33 554 431 (2^(25)-1) (None to All)
	boost::uniform_int<int> pop_dist(0,(1<<(NUMBER_DISEASES))-1);

	// Mutate when mutagen()==0
	boost::uniform_int<int> mut_dist(0,(int)(1/MUTATION_RATE));

	// Crossover loci (0 ~ 24)
	boost::uniform_int<int> xvr_dist(0,NUMBER_DISEASES-1);
public:
	uint32_t populator(void)
	{
		return (uint32_t) pop_dist(gen);
	}
	uint32_t mutagen(void)
	{
		return (uint32_t) mut_dist(gen);
	}
	uint32_t splicer (void)
	{
		return (uint32_t) xvr_dist(gen);
	}
};

struct best_t
{
	__float128 fitness[TRIAL_LIMIT];
	uint32_t generation[TRIAL_LIMIT];
};

int main(void)
{
	uint32_t iter, jter;

	// Fix tendency values that are less than lower limit
	tendencyFix();

	// Population
	__float128 population[POPULATION_LIMIT];

	// Seed random number generator
//	srand(time(NULL));

//	std::default_random_engine generator;

	// Possible Diagnoses: 0 ~ 33 554 431 (2^(25)-1) (None to All)
//	std::uniform_int_distribution<uint32_t> pop_dist(0,(1<<(NUMBER_DISEASES))-1);
//	uint32_t pop_creator = std::bind( pop_dist, generator );

	// Mutate when mutagen()==0
//	std::uniform_int_distribution<int> mut_dist(0,(uint32_t)(1/MUTATION_RATE));
//	uint32_t mutagen = std::bind( mut_dist, generator );

	// Crossover loci (0 ~ 24)
//	std::uniform_int_distribution<int> xvr_dist(0,NUMBER_DISEASES-1);
//	uint32_t splicer = std::bind( xvr_dist, generator );

	// Generate first generation
	for (iter=0; iter<POPULATION_LIMIT; iter++)
	{
//		population[iter] = rand() % (1<<NUMBER_DISEASES);
		population[iter] = pop_creator();
	}
	
	// 
	// Store some statistics
	best_t First[NUMBER_SYMPTOMS];
	best_t Second[NUMBER_SYMPTOMS];
	best_t Third[NUMBER_SYMPTOMS];
	uint32_t EvaluationsToOptimum[(1<<NUMBER_SYMPTOMS)-1][TRIAL_LIMIT];

	// Cycle through all possible symptom sets except healthy
	uint32_t symptom_set;
	for (symptom_set=1; symptom_set<(1<<NUMBER_SYMPTOMS); symptom_set++)
	{
		// Repeat for some number of trials
		uint32_t trial;
		for (trial=0; trial<TRIAL_LIMIT; trial++)
		{
			uint8_t FoundOptimum = 0;
			EvaluationsToOptimum[symptom_set-1][trial] = 0;

			// Repeat for some number of generations
			uint32_t generation;
			for (generation=0; generation<GENERATION_LIMIT; generation++)
			{
				__float128 first_f=0.0, second=0.0, third=0.0;
				uint32_t first_g=0, second_g=0, third_g=0;

				// Cycle through the entire population
				uint32_t individual;
				for (individual=0; individual<POPULATION_LIMIT; individual++)
				{
					if (!FoundOptimum)
					{
						EvaluationsToOptimum[symptom_set-1][trial]++;
					}

					__float128 temp = fitness(population[individual],symptom_set);

					if ( abs(temp-qOptimumDiagnoses[symptom_set-1][0]) < 0.00001 )
					{
						FoundOptimum=1;
					}

					if (temp>first_f)
					{
						third_f = second_f;
						second_f = first_f;
						first_f = temp;
						
						third_g = second_g;
						second_g = first_g;
						first_g = generation;
					}
				}
				// End of fitness evaluations
				
				// Update First, Second, and Third best fitness values
				if (first_f > First[symptom_set-1].fitness)
				{
					if (second_f > Second[symptom_set-1].fitness)
					{
						if (third_f > Third[symptom_set-1].fitness)
						{
							Third[symptom_set-1].fitness = third_f;
							Third[symptom_set-1].generation = third_g;
						}
						else
						{
							Third[symptom_set-1].fitness = Second[symptom_set-1].fitness;
							Third[symptom_set-1].generation = Second[symptom_set-1].generation;
						}
						Second[symptom_set-1].fitness = second_f;
						Second[symptom_set-1].generation = second_g;
					}
					else
					{
						Second[symptom_set-1].fitness = First[symptom_set-1].fitness;
						Second[symptom_set-1].generation = First[symptom_set-1].generation;
					}
					First[symptom_set-1].fitness = temp;
					First[symptom_set-1].generation = generation;
				}
				// End of generation
			}
			// End of Trial
		}
	}

	stringstream strstr (stringstream::in | stringstream::out);
	strstr.clear();	strstr.str("");
	strstr << "./output_";
	for (iter=0; iter<8; iter++)
	{
		strstr << splicer;
	}
	strstr << ".txt";
	string outname;
	outname = strstr.str();
	ofstream outfile;
	outfile.open( outname );

	if ( !outfile.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}
	for (iter=1; iter<(1<<NUMBER_SYMPTOMS); iter++)
	{
		outfile << "symptom_set:\t" << iter << "\t" << First[iter-1] << "\t" << Second[iter-1] << "\t" << Third[iter-1] << "\n";
	}
	outfile.close();

	return 0;
}

