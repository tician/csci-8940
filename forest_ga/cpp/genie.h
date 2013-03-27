
#include <stdint.h>
#include <float.h>

#include <boost/random.hpp>

#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30
#define NUMBER_INDIVIDUALS			200

#define NUMBER_GENES				25
#define NUMBER_SYMPTOMS				10

#define MUTATION_RATE				0.003
#define CROSSOVER_RATE				0.1
#define CROSSOVER_POINTS			1

//#define FITNESS_TYPE				__float128
#define FITNESS_TYPE				double

FITNESS_TYPE qPriorLikelihood[NUMBER_GENES];

struct specimen_t
{
	FITNESS_TYPE sin_fit;
	bool gene[NUMBER_GENES];
};

struct population_t
{
	specimen_t pop[NUMBER_INDIVIDUALS];
};

class randomizers
{
private:
	boost::mt19937 rng;

	// Possible Diagnoses: 0 ~ 33 554 431 (2^(25)-1) (None to All)
	boost::uniform_int<> search_space(0,(1<<(NUMBER_GENES))-1);
	boost::variate_generator< boost::mt19937&, boost::uniform_int<> >
			specimen_generator(rng, search_space);

	// Mutate when mutagen()==0
	boost::uniform_int<> mutation_space(0,(int)(1/MUTATION_RATE));
	boost::variate_generator< boost::mt19937&, boost::uniform_int<> >
			mutation_generator(rng, mutation_space);

	// Crossover when cpg()==0
	boost::uniform_int<> crossover_points(0,(int)(1/CROSSOVER_RATE));
	boost::variate_generator< boost::mt19937&, boost::uniform_int<> >
			crossover_point_generator(rng, crossover_points);
	// Crossover loci (0 ~ 24)
	boost::uniform_int<> crossover_locations(0,NUMBER_GENES-1);
	boost::variate_generator< boost::mt19937&, boost::uniform_int<> >
			crossover_location_generator(rng, crossover_locations);
public:
	specimen_t populator(void)
	{
		specimen_t bread;
		bread.sin_fit = -100.0;

		uint32_t iter;
		uint32_t indi = specimen_generator();

		for (iter=0; iter<NUMBER_GENES; iter++)
		{
			if ((indi&(1<<iter))>0)
				bread.gene[iter]=true;
			else
				bread.gene[iter]=false;
		}

		return bread;
	}
	bool mutagen(void)
	{
		if (mutation_generator()<1)
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
			if (crossover_point_generator() < 1)
			{
				locus[iter] = crossover_location_generator();
			}
		}
		ba = ma.clone();
		by = pa.clone();

		specimen_t temp;

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
};

bool fitness_sorter(specimen_t i, specimen_t j)
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

FITNESS_TYPE fitness_function(specimen_t diagnosis, uint32_t symptoms)
{
	FITNESS_TYPE L1 = 1.0;
	FITNESS_TYPE L2 = 1.0;
	FITNESS_TYPE L3 = 1.0;
	uint32_t iter, jter;

// Positive Likelihood
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		if (symptoms&(1<<iter))
		{
			FITNESS_TYPE temp = 1.0;
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
			FITNESS_TYPE temp = 1.0;
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


