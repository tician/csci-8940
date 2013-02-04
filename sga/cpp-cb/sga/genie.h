/*
 * I was really fucking bored and depressed...
 */


#include <stdint.h>
#include <float.h>
#include "TendencyMatrix10x25.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30
#define NUMBER_INDIVIDUALS			200

#define MUTATION_RATE				0.003
#define CROSSOVER_RATE				0.1
#define CROSSOVER_POINTS			1

#define ZERO_FITNESS_LIMIT			1.0e-5


__float128 qPriorLikelihood[NUMBER_GENES];

struct specimen_t
{
	__float128 sin_fit;
	bool gene[NUMBER_GENES];
};

struct population_t
{
	specimen_t pop[NUMBER_INDIVIDUALS];
};

class randomizers
{
private:
	boost::random::mersenne_twister::mt19937 gen = boost::random::mersenne_twister::mt19937();

	// Possible Diagnoses: 0 ~ 33 554 431 (2^(25)-1) (None to All)
	boost::uniform_int<int> specimen_generator(0,(1<<(NUMBER_GENES))-1);

	// Mutate when mutagen()==0
	boost::uniform_int<int> mutation_generator(0,(int)(1/MUTATION_RATE));

	// Crossover when cpg()==0
	boost::uniform_int<int> crossover_point_generator(0,(int)(1/CROSSOVER_RATE));
	// Crossover loci (0 ~ 24)
	boost::uniform_int<int> crossover_point_generator(0,NUMBER_GENES-1);
public:
	specimen_t populator(void)
	{
		specimen_t bread;
		bread.sin_fit = -100.0;

		uint32_t iter;
		uint32_t indi = specimen_generator();

		for (iter=0; iter<NUMBER_GENES; iter++)
		{
			if (indi&(1<<iter)>0)
				bread.gene[iter]=true;
			else
				bread.gene[iter]=false;
		}

		return bread;
	}
	bool mutagen(void)
	{
		if (mutation_generator(gen)<1)
			return true;
		else
			return false;
	}
	void splicer (specimen_t ma, specimen_t pa, specimen_t* ba, specimen_t* by)
	{
		// Crossover loci (0 ~ 24)
		// boost::uniform_int<int> crossover_point_generator(0,NUMBER_DISEASES-1);

		uint32_t iter, jter;
		uint32_t locus[CROSSOVER_POINTS];

		for (iter=0; iter<CROSSOVER_POINTS; iter++)
		{
			if (crossover_generator(gen) < 1)
			{
				locus[iter] = crossover_point_generator(gen);
			}
		}

		specimen_t temp;

		for (iter=0; iter<CROSSOVER_POINTS; iter++)
		{
			if (locus[iter] > 0)
			{
				for (jter=locus[iter]; jter<NUMBER_GENES; jter++)
				{
					temp[jter] = ma[jter];
					ma[jter] = pa[jter];
					pa[jter] = temp[jter];
				}
			}
		}
	}
};

bool fitness_sorter(specimen_t i, speciment_t j)
{
	return (i.fitness<j.fitness);
}

bool cloned(specimen_t i, specimen_t j)
{
	uint32_t iter, match_count=0;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (i.gene[iter] == j.gene[iter])
		{
			match_count++;
		}
		else
		{
			break;
		}
	}

	// If you have 5 matches and I take one away, how many do you have left to light a fire then die in it?
	//  -- breakerslion (#1221108 http://fstdt.com/QuoteComment.aspx?QID=77098&Page=2)
	if ( match_count == (NUMBER_GENES-1) )
		return true;

	return false;
}

void priorLikelihoodSetup(void)
{
	uint32_t iter;
	for (iter=0; iter<(NUMBER_GENES); iter++)
	{
		qPriorLikelihood[iter] = (qPriorProbability[iter])/(1.0-qPriorProbability[iter]);
	}
}

void tendencyFix(void)
{
	uint32_t iter, jter;
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		for (jter=0; jter<NUMBER_GENES; jter++)
		{
			if (qManifestationInDisease[iter][jter] < ZERO_FITNESS_LIMIT)
			{
				qManifestationInDisease[iter][jter] = ZERO_FITNESS_LIMIT;
			}
		}
	}
}

__float128 fitness_function(specimen_t diagnosis,  symptoms)
{
	__float128 L1 = 1.0;
	__float128 L2 = 1.0;
	__float128 L3 = 1.0;
	uint32_t iter, jter;

// Positive Likelihood
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		if (symptoms&(1<<iter))
		{
			__float128 temp = 1.0;
			for (jter=0; jter<NUMBER_GENES; jter++)
			{
				if (diagnosis[jter])
				{
					temp *= (1.0-qManifestationInDisease[iter][jter]);
				}
			}
			L1 *= (1.0-temp);
		}
	}

// Negative Likelihood
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (diagnosis[iter])
		{
			__float128 temp = 1.0;
			for (jter=0; jter<NUMBER_SYMPTOMS; jter++)
			{
				if (!(symptoms&(1<<jter)))
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
		if (diagnosis[iter])
		{
			L3 *= qPriorLikelihood[iter];
		}
	}

	return (L1 * L2 * L3);
}


