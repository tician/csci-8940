#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include "opencv2/core/core.hpp"

#include <algorithm>
#include <vector>

#include "TendencyMatrix10x25.h"


#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30
#define NUMBER_INDIVIDUALS			200
#define NUMBER_TRACKING				3
#define NUMBER_GENES				25
#define NUMBER_DIMENSIONS			10

#define MUTATION_RATE				0.003
#define CROSSOVER_RATE				0.01
#define CROSSOVER_POINTS			1

//#define FITNESS_TYPE				__float128
//#define FITNESS_TYPE				double
#define GENO_TYPE					bitset<NUMBER_GENES>

#define ZERO_FITNESS_LIMIT			1e-6

using namespace std;
using namespace cv;

#define NUMBER_SYMPTOMS				10
bitset<NUMBER_SYMPTOMS> SymptomSet;

typedef struct
{
	FITNESS_TYPE	fit;
	GENO_TYPE		gen;
} specimen_t;


bool loci_comp (uint32_t i,uint32_t j) { return (i<=j); }
bool spec_comp (specimen_t i, specimen_t j) { return ((FITNESS_TYPE)i.fit>=(FITNESS_TYPE)j.fit); }


class population
{
private:
	specimen_t pop_[NUMBER_INDIVIDUALS];
	specimen_t best_[NUMBER_TRACKING][NUMBER_GENERATIONS];

	FITNESS_TYPE sig_fit_[NUMBER_INDIVIDUALS];

	RNG rudi_;

	void roulette(void);
	void selector(GENO_TYPE&);
	void splicer(GENO_TYPE&, GENO_TYPE&);
	void mutator(void);

	uint32_t generation_;
	void bestest(void);


	void calcFitness(specimen_t&);
	specimen_t populate(void);
	specimen_t mutate(specimen_t);

public:
	population(RNG& rudi) {rudi_ = rudi;}
	void populator(void);
	void breeder(void);

	specimen_t First (uint32_t);
	specimen_t Second(uint32_t);
	specimen_t Third (uint32_t);

	uint32_t Age(void) {return generation_;}
};

specimen_t population::First (uint32_t gen_index)
{
	assert(gen_index<NUMBER_GENERATIONS);
	return best_[0][gen_index];
//	if (gen_index<NUMBER_GENERATIONS)
//		return best_[0][gen_index];
//	else
//		return NULL;
}
specimen_t population::Second(uint32_t gen_index)
{
	assert(gen_index<NUMBER_GENERATIONS);
	return best_[1][gen_index];
//	if (gen_index<NUMBER_GENERATIONS)
//		return best_[1][gen_index];
//	else
//		return NULL;
}
specimen_t population::Third (uint32_t gen_index)
{
	assert(gen_index<NUMBER_GENERATIONS);
	return best_[2][gen_index];
//	if (gen_index<NUMBER_GENERATIONS)
//		return best_[2][gen_index];
//	else
//		return NULL;
}

void population::bestest(void)
{
	std::vector<specimen_t> punk (pop_, pop_+NUMBER_INDIVIDUALS);
	std::sort (punk.begin(), punk.end(), spec_comp);


	uint32_t iter=0, jter=0;;
	best_[jter++][generation_] = punk[iter++];

	while( (iter<punk.size()) && (jter<NUMBER_TRACKING) )
	{
		if (punk[iter].gen != best_[jter][generation_].gen)
		{
			best_[jter++][generation_] = punk[iter];
		}
		iter++;
	}
	while (jter<NUMBER_TRACKING)
	{
		best_[jter][generation_].gen = 0;
		best_[jter++][generation_].fit = 0;
	}
}


void population::populator(void)
{
	generation_ = 0;

	uint32_t iter;

	for (iter=0; iter<NUMBER_INDIVIDUALS; iter++)
	{
		pop_[iter] = populate();
	}
	bestest();
}

void population::breeder(void)
{
	GENO_TYPE kiddies[NUMBER_INDIVIDUALS];
	roulette();

	uint32_t iter;
	for (iter=0; iter<(NUMBER_INDIVIDUALS/2); iter++)
	{
		GENO_TYPE mama, papa;

		selector(mama);
		selector(papa);
		splicer(mama, papa);
		kiddies[(2*iter)+0] = mama;
		kiddies[(2*iter)+1] = papa;
	}

	for (iter=0; iter<NUMBER_INDIVIDUALS; iter++)
	{
		pop_[iter].gen = kiddies[iter];
	}
	mutator();
	generation_++;
	bestest();
}

void population::roulette(void)
{
	// Roulette Wheel Selection
	uint32_t iter;

	sig_fit_[0] = pop_[0].fit;
	for (iter=1; iter<NUMBER_INDIVIDUALS; iter++)
	{
		sig_fit_[iter] = sig_fit_[iter-1] + pop_[iter].fit;
	}
}

void population::selector(GENO_TYPE& nana)
{
	uint32_t iter;
	// Roulette Wheel Selection
	FITNESS_TYPE temp = rudi_.uniform(0.0, sig_fit_[NUMBER_INDIVIDUALS-1]);

	for (iter=0; iter<NUMBER_INDIVIDUALS; iter++)
	{
		if (temp <= sig_fit_[iter])
		{
			nana = pop_[iter].gen;
			break;
		}
	}
}

void population::splicer(GENO_TYPE& mama, GENO_TYPE& papa)
{
	// Crossover loci (0 ~ 124)

	uint32_t iter;
	uint32_t locus[CROSSOVER_POINTS] = {0};

	GENO_TYPE mask;
	GENO_TYPE ba, by;

	for (iter=0; iter<CROSSOVER_POINTS; iter++)
	{
		if (rudi_.uniform( 0, (int)(1/CROSSOVER_RATE)) < 1)
		{
			locus[iter] = rudi_.uniform(0, NUMBER_GENES);
		}
	}

	std::vector<uint32_t> loci (locus, locus+CROSSOVER_POINTS);
	std::sort (loci.begin(), loci.end(), loci_comp);

	std::vector<uint32_t>::iterator it;
	it = std::unique(loci.begin(), loci.end());
	loci.resize( std::distance(loci.begin(),it) );

//	for (it=loci.begin(); it!=loci.end(); it++)//++it)
//		std::cout << ' ' << *it;

	it = loci.begin();

	bool up=true;
	for (iter=0; iter<CROSSOVER_POINTS; iter++)
	{
		if ( (iter >= *it) && (it < loci.end()) )
		{
			it++;
			up = ~up;
		}
		mask[iter] = up;
	}

	ba = ( mask & mama) | (~mask & papa) ;
	by = (~mask & mama) | ( mask & papa) ;

	mama = ba;
	papa = by;
}

void population::mutator(void)
{
	uint32_t iter;
	for (iter=0; iter<NUMBER_INDIVIDUALS; iter++)
	{
		pop_[iter] = mutate(pop_[iter]);
	}
}

void population::calcFitness(specimen_t& indi)
{
	FITNESS_TYPE temp = 1.0;
	FITNESS_TYPE L1 = 1.0;
	FITNESS_TYPE L2 = 1.0;
	FITNESS_TYPE L3 = 1.0;
	uint32_t iter = 0, jter = 0;

	// Positive Likelihood
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		if (SymptomSet[iter])
		{
			temp = 1.0;
			for (jter=0; jter<NUMBER_GENES; jter++)
			{
				if (indi.gen[jter])
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
		if (indi.gen[iter])
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
		if (indi.gen[iter])
		{
			L3 *= qPriorLikelihood[iter];
		}
	}

	indi.fit = (L1 * L2 * L3);
}

specimen_t population::populate(void)
{
	specimen_t indi;
	indi.gen = rudi_.uniform(0, 2^(NUMBER_DIMENSIONS));
	calcFitness(indi);
	return indi;
}

specimen_t population::mutate(specimen_t indi)
{
	uint32_t iter;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (rudi_.uniform( 0, (int)(1/MUTATION_RATE) ) < 1)
		{
			indi.gen.flip(iter);
		}
	}
	calcFitness(indi);
	return indi;
}









int main(void)
{
	RNG randi (0xF0F0F0F0F0F0F0F0);

	uint32_t iter;

	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		qPriorLikelihood[iter] = ((qPriorProbability[iter])/(1.0-qPriorProbability[iter]));
	}

	uint32_t symptoms;
	for (symptoms=1; symptoms<(2<<NUMBER_SYMPTOMS); symptoms++)
	{
		SymptomSet = symptoms;

		uint32_t trailer_trash;
		for (trailer_trash=0; trailer_trash<NUMBER_TRIALS; trailer_trash++)
		{
			population hoponpop(randi);

			hoponpop.populator();


			uint32_t generational_recursion;
			for (generational_recursion=0; generational_recursion<NUMBER_GENERATIONS; generational_recursion++)
			{
				hoponpop.breeder();
			}

			// Print data to file
			stringstream strstr (stringstream::in | stringstream::out);
			strstr.clear();	strstr.str("");
			strstr << "./mfd"
				<< "_" << NUMBER_INDIVIDUALS
				<< "_" << NUMBER_GENERATIONS
				<< "_" << CROSSOVER_POINTS
				<< "_" << CROSSOVER_RATE
				<< "_" << MUTATION_RATE
				<< "_" << trailer_trash			// Current Trial
				<< "_" << SymptomSet;
			string outname;
			outname = strstr.str();
			ofstream outfile;
			outfile.open( outname.c_str() );




			//	hoponpop.First (generational_recursion);
			//	hoponpop.Second(generational_recursion);
			//	hoponpop.Third (generational_recursion);

			if ( !outfile.is_open() )
			{
				cerr << "Unable to open file: " << outname << "\n";
				return 1;
			}
			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
				specimen_t indi;
				indi = hoponpop.First (iter);
				outfile << indi.fit << "," << indi.gen << ",";
				indi = hoponpop.Second(iter);
				outfile << indi.fit << "," << indi.gen << ",";
				indi = hoponpop.Third (iter);
				outfile << indi.fit << "," << indi.gen << "\n";
			}
			outfile.close();

		}//End of Trial
	}//End of Symptom Set
	return 0;
}

