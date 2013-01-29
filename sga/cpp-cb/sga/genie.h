/*
 * I was really fucking bored and depressed...
 */


#include <stdint.h>
#include <float.h>
#include "TendencyMatrix10x25.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			50
#define NUMBER_INDIVIDUALS			200

#define MUTATION_RATE				0.003
#define CROSSOVER_POINTS			1

#define ZERO_FITNESS_LIMIT			1.0e-5


__float128 qPriorLikelihood[NUMBER_GENES];

struct population_statistics_t
{
	__float128 fitness[NUMBER_TRIALS];
	uint32_t generation[NUMBER_TRIALS];
};

struct specimen_t
{
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
	boost::uniform_int<int> pop_dist(0,(1<<(NUMBER_GENES))-1);

	// Mutate when mutagen()==0
	boost::uniform_int<int> mut_dist(0,(int)(1/MUTATION_RATE));

	// Crossover loci (0 ~ 24)
	boost::uniform_int<int> xvr_dist(0,NUMBER_GENES-1);
public:
	specimen_t populator(void)
	{
		specimen_t bread;

		uint32_t iter;
		uint32_t indi = pop_dist();

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
		if (mut_dist(gen)<1)
			return true;
		else
			return false;
	}
	uint32_t splicer (void)
	{
		return (uint32_t) xvr_dist(gen);
	}
};


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

__float128 fitness(specimen_t diagnosis,  symptoms)
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


