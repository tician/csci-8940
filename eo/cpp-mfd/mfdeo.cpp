#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include "opencv2/core/core.hpp"


#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30
#define NUMBER_INDIVIDUALS			200
#define NUMBER_TRACKING				3
#define NUMBER_GENES				25

#define MUTATION_RATE				0.003
#define CROSSOVER_RATE				0.01
#define CROSSOVER_POINTS			1

//#define FITNESS_TYPE				__float128
#define FITNESS_TYPE				double
#define GENO_TYPE					bitset<NUMBER_GENES>

#define ZERO_FITNESS_LIMIT			1e-6

using namespace std;
using namespace cv;

#define NUMBER_SYMPTOMS				10
bitset<NUMBER_SYMPTOMS> SymptomSet;

class specimen
{
private:
	FITNESS_TYPE fit_;
	GENO_TYPE gen_;

	void calcFitness(void);

public:
	void generate(RNG);
	void mutate(RNG);
	FITNESS_TYPE fitness(void) {return fit_;}
	GENO_TYPE individual(void) {return gen_;}
};

void specimen::generate(RND rudi)
{
	gen_ = rudi.uniform(0, 2^(NUMBER_DIMENSIONS));
	calcFitness();
}

void specimen::mutate(RNG rudi)
{
	uint32_t iter;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (rudi.uniform( 0, (int)(1/MUTATION_RATE) ) < 1)
		{
			gen_.flip(iter);
		}
	}
	calcFitness();
}

void specimen::calcFitness(void)
{
	FITNESS_TYPE temp = 1.0;
	FITNESS_TYPE L1 = 1.0;
	FITNESS_TYPE L2 = 1.0;
	FITNESS_TYPE L3 = 1.0;
	uint32_t iter = 0; jter = 0;

	// Positive Likelihood
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		if (SymptomSet[iter])
		{
			temp = 1.0;
			for (jter=0; jter<NUMBER_GENES; jter++)
			{
				if (gen_[jter])
				{
					temp *= (1.0-qManifestationInDisease[iter][jter]);
				}
			}
			L1 *= (1.0-temp);
			if ((1.0-temp)>ZERO_FITNESS_LIMIT)
			{
				L1 *= (1.0-temp);
			}
			else
			{
				L1 *= ZERO_FITNESS_LIMIT;
			}
		}
	}

	// Negative Likelihood
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (gen_[iter])
		{
			temp = 1.0;
			for (jter=0; jter<NUMBER_SYMPTOMS; jter++)
			{
				if (SymptomSet[jter] == 0)
				{
					temp *= (1.0-qManifestationInDisease[jter][iter]);
				}
			}
			L2 *= (temp);
		}
	}

	// Prior Likelihood
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (gen_[iter])
		{
			L3 *= qPriorLikelihood[iter];
		}
	}

	fit_ = (L1 * L2 * L3);
}


class population
{
private:
	specimen pop_[NUMBER_INDIVIDUALS];
	specimen best_[NUMBER_TRACKING];

	void splicer(RNG);
	void mutator(RNG);

	uint32_t generation_;

public:
	void populator(RNG);
	void breeder(RNG);

	specimen First (void) {return best_[0];}
	specimen Second(void) {return best_[1];}
	specimen Third (void) {return best_[2];}

	uint32_t Age(void) {return generation_;}
};

void population::populator(RNG rudi)
{
	generation_ = 0;

	uint32_t iter;

	for (iter=0; iter<NUMBER_INDIVIDUALS; iter++)
	{
		pop_[iter].generate(rudi);
	}

}

void population::breeder(RNG rudi)
{


}

void population::splicer(RNG rudi)
{
	// Crossover loci (0 ~ 124)

	uint32_t iter, jter;
	uint32_t locus[CROSSOVER_POINTS];

	for (iter=0; iter<CROSSOVER_POINTS; iter++)
	{
		if (rudi.uniform( 0, (int)(1/CROSSOVER_RATE)) < 1)
		{
			locus[iter] = rudi.uniform(0, NUMBER_GENES);
		}
	}

	for (iter=0; iter<CROSSOVER_POINTS; iter++)
	{
		if (locus[iter] > 0)
		{
			for (jter=locus[iter]; jter<NUMBER_GENES; jter++)
			{
				temp.gene[jter] = ba.gene[jter];
				ba.gene[jter] = by.gene[jter];
				by.gene[jter] = temp.gene[jter];
			}
		}
	}
}

void population::mutator(RNG rudi)
{

}


};

int main(void)
{
	RNG randi (0xF0F0F0F0F0F0F0F0);

	uint32_t iter;
	for (iter=1; iter<(2^(NUMBER_SYMPTOMS)); iter++)
	{
		SymptomSet = iter;

		population hoponpop;

		hoponpop.populator(randi);



	// Print data to file

		stringstream strstr (stringstream::in | stringstream::out);
		strstr.clear();	strstr.str("");
		strstr << "./mfd"
			<< "_" << POPULATION_LIMIT
			<< "_" << GENERATION_LIMIT
			<< "_" << CROSSOVER_POINTS
			<< "_" << CROSSOVER_RATE
			<< "_" << MUTATION_RATE
			<< "_" << SymptomSet;
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
			outfile << "Trial: " << iter+1;
			for (jter=0; jter<NUMBER_GENERATIONS; jter++)
			{
				outfile << "\nGen: " << jter;
				outfile << "\nFit1," << First[jter].sin_fit;
				outfile << ",Pop1,";
				for (kter=0; kter<NUMBER_GENES-1; kter++)
				{
					outfile << First[jter].gene[kter] << ",";
				}
				outfile << First[jter].gene[kter];

				outfile << "\nFit2," << Second[jter].sin_fit;
				outfile << ",Pop2,";
				for (kter=0; kter<NUMBER_GENES-1; kter++)
				{
					outfile << Second[jter].gene[kter] << ",";
				}
				outfile << Second[jter].gene[kter];
				outfile << "\nFit3," << Third[jter].sin_fit;
				outfile << ",Pop3,";
				for (kter=0; kter<NUMBER_GENES-1; kter++)
				{
					outfile << Third[jter].gene[kter] << ",";
				}
				outfile << Third[jter].gene[kter] << "\n";
			}
		}
		outfile.close();

	}
	return 0;
}

