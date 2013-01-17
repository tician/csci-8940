
#include "fitness.h"
#include "ExhaustiveResults10x25.h"

#define TRIAL_LIMIT			10
#define GENERATION_LIMIT	50

struct best_t
{
	__float128 fitness[TRIAL_LIMIT] = {};
	uint32_t generation[TRIAL_LIMIT] = {};
}

int main(void)
{
	// Fix tendency values that are less than lower limit
	tendencyFix();

	// Seed random number generator
	
	
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
				uint32_t first_g=0; second_g=0, third_g=0;

				// Cycle through the entire population
				uint32_t individual;
				for (individual=0; individual<POPULATION_LIMIT; individual++)
				{
					if (!FoundOptimum)
					{
						EvaluationsToOptimum[symptom_set-1][trial]++;
					}

					__float128 temp = fitness(diagnosis[individual],symptom_set);

					if ( abs(temp-qOptimumDiagnoses[symptom_set-1][0]) < 0.00001)
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
}

