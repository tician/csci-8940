
#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>

using namespace std;

int main(void)
{
	randomizers genie;
	uint32_t iter, jter, kter;

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
		populi.pop[iter] = genie.populator();
	}

	// Store some statistics
	specimen_t First[NUMBER_TRIALS][NUMBER_GENERATIONS];
	specimen_t Second[NUMBER_TRIALS][NUMBER_GENERATIONS];
	specimen_t Third[NUMBER_TRIALS][NUMBER_GENERATIONS];

//	uint32_t EvaluationsToOptimum[(1<<NUMBER_SYMPTOMS)-1][NUMBER_TRIALS];

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

			// Repeat for some number of generations
			uint32_t generation;
			for (generation=0; generation<NUMBER_GENERATIONS; generation++)
			{
				// Cycle through the entire population
				uint32_t individual;
//				FITNESS_TYPE sin_fit[NUMBER_INDIVIDUALS];
				FITNESS_TYPE sig_fit[NUMBER_INDIVIDUALS];
				for (individual=0; individual<NUMBER_INDIVIDUALS; individual++)
				{

					if ( (individual < 1) | (generation < 1) )
					{
						populi.pop[individual].sin_fit = fitness_function(populi.pop[individual],symptom_set);
					}
					else
					{
						if (cloned(populi[individual],populi[individual-1]))
						{
							populi.pop[individual].sin_fit = populi.pop[individual-1].sin_fit;
						}
						else
						{
							for (iter=0; iter<individual; iter++)
							{
								if (cloned(populi.pop[individual],populi.pop[iter]))
								{
									populi.pop[individual].sin_fit = populi.pop[iter].sin_fit;
									break;
								}
							}
							if (populi.pop[individual].sin_fit < -10)
							{
								populi.pop[individual].sin_fit = fitness_function(populi.pop[individual],symptom_set);
							}
						}
					}



					// Update Sigma Fitness array for roulette wheel selection
					if (individual > 1)
					{
						sig_fit[individual] = sig_fit[individual-1] + populi.pop[individual].sin_fit;
					}
					else
					{
						sig_fit[individual] = populi.pop[individual].sin_fit;
					}
				}
//				std::vector<speciment_t> virus (populi.pop, (populi.pop+sizeof(population_t)))
				std::sort (&populi.pop[0], &populi.pop[NUMBER_INDIVIDUALS], fitness_sorter);
			}
			// End of Trial
		}
		// End of Species

		stringstream strstr (stringstream::in | stringstream::out);
		strstr.clear();	strstr.str("");
		strstr << "./output"
			<< "_" << ZERO_FITNESS_LIMIT
			<< "_" << CROSSOVER_RATE
			<< "_" << MUTATION_RATE
			<< "_" << POPULATION_LIMIT
			<< "_" << GENERATION_LIMIT
			<< "_" << CROSSOVER_POINTS
			<< "_" << symptom_set;
		string outname;
		outname = strstr.str();
		ofstream outfile;
		outfile.open( outname );

		if ( !outfile.is_open() )
		{
			cerr << "Unable to open file: " << outname << "\n";
			return 1;
		}
		for (iter=0; iter<NUMBER_TRIALS; iter++)
		{
			outfile << "\n\nSymptom_Set: " << symptom_set;
			outfile << "\nTrial: " << iter+1;
			for (jter=0; jter<NUMBER_GENERATIONS; jter++)
			{
				outfile << "\n  Generation: " << jter+1;
				outfile << "\n    First Fitness:  " << First[jter].sin_fit;
				outfile << "\n     First Pop: ";
				for (kter=0; kter<NUMBER_GENES; kter++)
				{
					outfile << First[jter].gene[kter] << ",";
				}

				outfile << ";\n    Second Fitness:  " << Second[jter].sin_fit;
				outfile << "\n     Second Pop: ";
				for (kter=0; kter<NUMBER_GENES; kter++)
				{
					outfile << Second[jter].gene[kter] << ",";
				}

				outfile << ";\n    Third Fitness:  " << Third[jter].sin_fit;
				outfile << "\n     Third Pop: ";
				for (kter=0; kter<NUMBER_GENES; kter++)
				{
					outfile << First[jter].gene[kter] << ",";
				}
				outfile << ";\n";
			}
		}
		outfile.close();

	}


	return 0;
}

