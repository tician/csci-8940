

#include "genie.h"
#include "ExhaustiveResults10x25.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main(void)
{
	randomizers genie;
	uint32_t iter, jter;

	// Fix tendency values that are less than lower limit
	tendencyFix();

	// Get PriorLikelihoods
	priorLikelihoodSetup();

	// Population
	population_t populi;

	// Seed random number generator
//	srand(time(NULL));

//	std::default_random_engine generator;

	// Possible Diagnoses: 0 ~ 33 554 431 (2^(25)-1) (None to All)
//	std::uniform_int_distribution<uint32_t> specimen_generator(0,(1<<(NUMBER_GENES))-1);
//	uint32_t pop_creator = std::bind( specimen_generator, generator );

	// Mutate when mutagen()==0
//	std::uniform_int_distribution<int> mut_dist(0,(uint32_t)(1/MUTATION_RATE));
//	uint32_t mutagen = std::bind( mut_dist, generator );

	// Crossover loci (0 ~ 24)
//	std::uniform_int_distribution<int> crossover_point_generator(0,NUMBER_GENES-1);
//	uint32_t splicer = std::bind( crossover_point_generator, generator );

	// Generate first generation
	for (iter=0; iter<NUMBER_INDIVIDUALS; iter++)
	{
//		population[iter] = rand() % (1<<NUMBER_GENES);
		populi.pop[iter] = pop_creator();
	}

	// Store some statistics
	population_statistics_t First[NUMBER_SYMPTOMS];
	population_statistics_t Second[NUMBER_SYMPTOMS];
	population_statistics_t Third[NUMBER_SYMPTOMS];
	uint32_t EvaluationsToOptimum[(1<<NUMBER_SYMPTOMS)-1][NUMBER_TRIALS];

	// Cycle through all possible symptom sets except healthy
	uint32_t symptom_set;
	for (symptom_set=1; symptom_set<(1<<NUMBER_SYMPTOMS); symptom_set++)
	{
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nSymptom Set: " << symptom_set
			<< "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

		// Repeat for some number of trials
		uint32_t trial;
		for (trial=0; trial<NUMBER_TRIALS; trial++)
		{
			cout << "~~~~~~~~~~~~~~~~~~~~\nTrial: " << trial << "\n~~~~~~~~~~~~~~~~~~~~\n";
			uint8_t FoundOptimum = 0;
			EvaluationsToOptimum[symptom_set-1][trial] = 0;

			// Repeat for some number of generations
			uint32_t generation;
			for (generation=0; generation<NUMBER_GENERATIONS; generation++)
			{
				__float128 first_f=0.0, second_f=0.0, third_f=0.0;
				uint32_t  first_g=0,   second_g=0,   third_g=0;

				// Cycle through the entire population
				uint32_t individual;
				__float128 sin_fit[NUMBER_INDIVIDUALS];
				__float128 sig_fit[NUMBER_INDIVIDUALS];
				for (individual=0; individual<NUMBER_INDIVIDUALS; individual++)
				{
					sin_fit = fitness(population[individual],symptom_set);


					// Update Sigma Fitness array for roulette wheel selection
					if (individual > 1)
					{
						sig_fit(individual) = sig_fit(individual-1) + sin_fit(individual);
					}
					else
					{
						sig_fit(individual) = sin_fit(individual);
					}


					if (!FoundOptimum)
					{
						EvaluationsToOptimum[symptom_set-1][trial]++;
						if ( abs(temp-qOptimumDiagnoses[symptom_set-1][0]) < 0.00001 ) )
						{
							FoundOptimum=1;
						}
					}



					if (temp>third_f)
					{
						if (temp>second_f)
						{
							if (temp>first_f)
							{
								third_f = second_f;
								second_f = first_f;
								first_f = temp;

								third_g = second_g;
								second_g = first_g;
								first_g = generation;
							}
							else
							{
								third_f = second_f;
								second_f = temp;

								third_g = second_g;
								second_g = generation;
							}
						}
						else
						{
							third_f = temp;

							third_g = generation;
						}
					}
				}
				// End of fitness evaluations
				cout << "Generation: " << generation << "\tFirst: " << first_f << "\t Second: " << second_f << "\t Third: " << third_f << endl;

				// Update First, Second, and Third best fitness values
				if (first_f > Third[symptom_set-1].fitness)
				{
					if (first_f > Second[symptom_set-1].fitness)
					{
						if (first_f > First[symptom_set-1].fitness)
						{
							First[symptom_set-1].fitness = first_f;
							First[symptom_set-1].generation = first_g;

							if (second_f > Second[symptom_set-1].fitness)
							{
								Second[symptom_set-1].fitness = second_f;
								Second[symptom_set-1].generation = second_g;

								if (third_f > Third[symptom_set-1].fitness)
								{
									Third[symptom_set-1].fitness = third_f;
									Third[symptom_set-1].generation = third_g;
								}
							}
							else if (second_f > Third[symptom_set-1].fitness)
							{
								Third[symptom_set-1].fitness = second_f;
								Third[symptom_set-1].generation = second_g;
							}
						}
						else
						{
							Second[symptom_set-1].fitness = first_t
							Second[symptom_set-1].generation = first_g;
						}
					}
					else
					{
						Third[symptom_set-1].fitness = first_f;
						Third[symptom_set-1].generation = first_g;
					}
				}



			}
			// End of generation
		}
		// End of Trial
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

