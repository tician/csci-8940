#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include "opencv2/core/core.hpp"

#include <algorithm>
#include <vector>
#include <iterator>

#include <boost/program_options.hpp>
#include "TendencyMatrix10x25.h"


#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30
//#define NUMBER_INDIVIDUALS			80
//#define NUMBER_INDIVIDUALS			120
//#define NUMBER_INDIVIDUALS			160

#define NUMBER_TRACKING				3
#define NUMBER_GENES				25
#define NUMBER_DIMENSIONS			10

//#define MUTATION_RATE				0.001
//#define MUTATION_RATE				0.006
//#define MUTATION_RATE				0.011
//#define CROSSOVER_RATE				0.4
//#define CROSSOVER_RATE				0.6
//#define CROSSOVER_RATE				0.8
//#define CROSSOVER_POINTS			1

//#define ELISTISM					false

//#define FITNESS_TYPE				__float128
//#define FITNESS_TYPE				double
#define GENO_TYPE					bitset<NUMBER_GENES>

#define ZERO_FITNESS_LIMIT			1e-6

using namespace cv;
using namespace boost;
namespace po = boost::program_options;
using namespace std;

#define NUMBER_SYMPTOMS				10
bitset<NUMBER_SYMPTOMS> SymptomSet;

typedef struct specimen_t
{
	specimen_t(void) : fit(0.0), gen(0), calced(0) {};

	FITNESS_TYPE	fit;
	GENO_TYPE		gen;
	uint64_t		calced;
} specimen_t;


bool loci_comp (uint64_t i,uint64_t j) { return (i<=j); }
bool spec_comp (specimen_t i, specimen_t j) { return (i.fit>j.fit); } //{return ( (i.fit-j.fit)>0.0 );}
bool spec_uniq (specimen_t i, specimen_t j) { return (i.gen==j.gen); }


class population
{
private:
	specimen_t *pop_;//[NUMBER_INDIVIDUALS];
//	specimen_t **best_;//[NUMBER_TRACKING][NUMBER_GENERATIONS];
	specimen_t *best_;
	specimen_t best_ever_;

	FITNESS_TYPE *sig_fit_;//[NUMBER_INDIVIDUALS];

	uint64_t fitness_calculation_counter_;

	RNG rudi_;
	uint64_t pop_size_;
	double mu_r_;
	double xo_r_;
	uint64_t xo_p_;
	bool elitism_;

	void roulette(void);
	void selector(GENO_TYPE&);
	void splicer(GENO_TYPE&, GENO_TYPE&);
	void mutator(void);

	uint64_t generation_;
	void bestest(void);


	FITNESS_TYPE calcFitness(GENO_TYPE);
	specimen_t populate(void);
	specimen_t mutate(specimen_t);

public:
	population(RNG& rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, bool elitism);
	~population(void);
	void populator(void);
	void breeder(void);
/*
	specimen_t First (uint64_t);
	specimen_t Second(uint64_t);
	specimen_t Third (uint64_t);
*/
	specimen_t First (void){return best_[0];}
	specimen_t Second(void){return best_[1];}
	specimen_t Third (void){return best_[2];}

	specimen_t BestEver(void){return best_ever_;}

	uint64_t Age(void) {return generation_;}
};

population::population(RNG& rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, bool elitism)
{
	rudi_ = rudi;
	pop_size_ = pop_size;
	fitness_calculation_counter_ = 0;
	mu_r_ = mu_r;
	xo_r_ = xo_r;
	xo_p_ = xo_p;
	elitism_ = elitism;

	pop_ = new specimen_t[pop_size_];
	sig_fit_ = new FITNESS_TYPE[pop_size_];
/*
	best_ = new specimen_t*[NUMBER_TRACKING];
	best_[0] = new specimen_t[NUMBER_GENERATIONS];
	best_[1] = new specimen_t[NUMBER_GENERATIONS];
	best_[2] = new specimen_t[NUMBER_GENERATIONS];
*/
	best_ = new specimen_t[NUMBER_TRACKING];
}

population::~population(void)
{
	delete[] pop_;
	delete[] sig_fit_;
//	delete[] best_[2];
//	delete[] best_[1];
//	delete[] best_[0];
	delete[] best_;
}
/*
specimen_t population::First (uint64_t gen_index)
{
	assert(gen_index<NUMBER_GENERATIONS);
	return best_[0][gen_index];
}
specimen_t population::Second(uint64_t gen_index)
{
	assert(gen_index<NUMBER_GENERATIONS);
	return best_[1][gen_index];
}
specimen_t population::Third (uint64_t gen_index)
{
	assert(gen_index<NUMBER_GENERATIONS);
	return best_[2][gen_index];
}
*/

void population::bestest(void)
{
	std::vector<specimen_t> punk (pop_, pop_+pop_size_);
	std::sort (punk.begin(), punk.end(), spec_comp);


	std::vector<specimen_t>::iterator viter;
	viter = std::unique(punk.begin(), punk.end(), spec_uniq);
	punk.resize( std::distance(punk.begin(),viter) );

	viter = punk.begin();

/*
	uint64_t jter=0;
//	std::vector<specimen_t>::iterator viter = punk.begin();
	best_[jter++][generation_] = (*viter);

	while( (viter<punk.end()) && (jter<NUMBER_TRACKING) )
	{
		if ((*viter).gen != best_[jter][generation_].gen)
		{
			best_[jter++][generation_] = (*viter);
		}
		if (jter>NUMBER_TRACKING)
			break;
		viter++;
	}
	while (jter<NUMBER_TRACKING)
	{
		best_[jter][generation_].gen = 0;
		best_[jter++][generation_].fit = 0;
	}
*/
/*
	uint64_t jter=0;
	best_[jter] = (*viter);
	jter++; viter++;

	while( (viter<punk.end()) && (jter<NUMBER_TRACKING) )
	{
		if ((*viter).gen != best_[jter].gen)
		{
			best_[jter++] = (*viter);
		}
		if (jter>NUMBER_TRACKING)
			break;
		viter++;
	}
	while (jter<NUMBER_TRACKING)
	{
		best_[jter].gen = 0;
		best_[jter++].fit = 0;
	}
*/
	uint64_t jter=0;
	while( (viter<punk.end()) && (jter<NUMBER_TRACKING) )
	{
		best_[jter] = (*viter);
		jter++; viter++;
	}
	while (jter<NUMBER_TRACKING)
	{
		best_[jter].gen = 0;
		best_[jter++].fit = 0;
	}


	if (best_[0].fit > best_ever_.fit)
	{
		best_ever_ = best_[0];
	}
}


void population::populator(void)
{
	generation_ = 0;

	uint64_t iter;

	for (iter=0; iter<pop_size_; iter++)
	{
		pop_[iter] = populate();
	}
	bestest();
}

void population::breeder(void)
{
	GENO_TYPE kiddies[pop_size_];
	roulette();

	uint64_t iter;

	if (elitism_)// && (generation_>0))
	{
		for (iter=0; iter<(pop_size_/2)-2; iter++)
		{
			GENO_TYPE mama, papa;

			selector(mama);
			selector(papa);
			splicer(mama, papa);
			kiddies[(2*iter)+0] = mama;
			kiddies[(2*iter)+1] = papa;
		}
//		kiddies[(2*iter)+0] = best_[0][generation_].gen;
//		kiddies[(2*iter)+1] = best_[0][generation_].gen;
		kiddies[(2*iter)+0] = best_[0].gen;
		kiddies[(2*iter)+1] = best_[0].gen;
	}
	else
	{
		for (iter=0; iter<(pop_size_/2); iter++)
		{
			GENO_TYPE mama, papa;

			selector(mama);
			selector(papa);
			splicer(mama, papa);
			kiddies[(2*iter)+0] = mama;
			kiddies[(2*iter)+1] = papa;
		}
	}
	for (iter=0; iter<pop_size_; iter++)
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
	uint64_t iter;

	sig_fit_[0] = pop_[0].fit;
	for (iter=1; iter<pop_size_; iter++)
	{
		sig_fit_[iter] = sig_fit_[iter-1] + pop_[iter].fit;
	}
}

void population::selector(GENO_TYPE& nana)
{
	uint64_t iter;
	// Roulette Wheel Selection
	FITNESS_TYPE temp = rudi_.uniform((FITNESS_TYPE)0.0, (FITNESS_TYPE)sig_fit_[pop_size_-1]);

	for (iter=0; iter<pop_size_; iter++)
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
	// Crossover loci (0 ~ 24)

	uint64_t iter;
	uint64_t *locus;//[xo_p_] = {0};
	locus = new uint64_t[xo_p_];

	GENO_TYPE mask;
	GENO_TYPE ba, by;

	for (iter=0; iter<xo_p_; iter++)
	{
		if (rudi_.uniform( 0, (int)(1/xo_r_)) < 1)
		{
			locus[iter] = rudi_.uniform(0, NUMBER_GENES);
		}
	}

	std::vector<uint64_t> loci (locus, locus+xo_p_);
	std::sort (loci.begin(), loci.end(), loci_comp);

	std::vector<uint64_t>::iterator it;
	it = std::unique(loci.begin(), loci.end());
	loci.resize( std::distance(loci.begin(),it) );

//	for (it=loci.begin(); it!=loci.end(); it++)//++it)
//		std::cout << ' ' << *it;

	it = loci.begin();

	bool up=true;
	for (iter=0; iter<xo_p_; iter++)
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
	uint64_t iter;
	for (iter=0; iter<pop_size_; iter++)
	{
		pop_[iter] = mutate(pop_[iter]);
	}
}

FITNESS_TYPE population::calcFitness(GENO_TYPE genie)
{
	FITNESS_TYPE temp = 1.0;
	FITNESS_TYPE L1 = 1.0;
	FITNESS_TYPE L2 = 1.0;
	FITNESS_TYPE L3 = 1.0;
	uint64_t iter = 0, jter = 0;

	// Positive Likelihood
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
//		if (SymptomSet[iter] == 1)
		if (SymptomSet[NUMBER_SYMPTOMS-(iter+1)] == 1)
		{
			temp = 1.0;
			for (jter=0; jter<NUMBER_GENES; jter++)
			{
//				if (genie[NUMBER_GENES-(jter+1)])
				if (genie[jter])
				{
					temp *= (1.0-qManifestationInDisease[iter][jter]);
				}
			}
			L1 *= (1.0-temp);
		}
	}

	// Negative Likelihood
	for (jter=0; jter<NUMBER_GENES; jter++)
	{
//		if (genie[NUMBER_GENES-(jter+1)])
		if (genie[jter])
		{
			temp = 1.0;
			for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
			{
//				if (SymptomSet[iter] == 0)
				if (SymptomSet[NUMBER_SYMPTOMS-(iter+1)] == 0)
				{
					temp *= (1.0-qManifestationInDisease[iter][jter]);
				}
			}
			L2 *= (temp);
		}
	}

	// Prior Likelihood
	for (jter=0; jter<NUMBER_GENES; jter++)
	{
//		if (genie[NUMBER_GENES-(jter+1)])
		if (genie[jter])
		{
			L3 *= qPriorLikelihood[jter];
		}
	}

	fitness_calculation_counter_++;

//	indi.gen = genie;
//	indi.fit = (L1 * L2 * L3);
//	cout << "Geno:" << indi.gen << "\tFit:" << indi.fit << endl;
//	cout << "Geno: " << genie << "\tFit: " << (L1*L2*L3) << endl;
	return ((L1*L2*L3));
}

specimen_t population::populate(void)
{
	specimen_t indi;
	indi.gen = rudi_.uniform(0, 1<<(NUMBER_GENES));
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	return indi;
}

specimen_t population::mutate(specimen_t indi)
{
	uint64_t iter;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (rudi_.uniform( 0, (int)(1/mu_r_) ) < 1)
		{
			indi.gen.flip(iter);
		}
	}
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	return indi;
}



int main(int argc, char* argv[])
{
	uint64_t pop_size = 80;
	double mu_r = 0.001;
	double xo_r = 0.4;
	uint64_t xo_p = 1;
	bool elitism = false;
	uint64_t num_trials = 10;
//	uint64_t rng_seed = 0xF0F0F0F0;


	double last_tick_count = 0.0;

    try
    {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help",								"Produce help message")
			("ps",		po::value<uint64_t>(),		"Set Population Size")
			("mr",		po::value<double>(),		"Set Mutation Rate")
			("xr",		po::value<double>(),		"Set Crossover Rate")
			("xp",		po::value<uint64_t>(),		"Set Maximum Number of Crossover Points")
			("el",		po::value<bool>(),			"Enable Elitism")
			("trials",	po::value<uint64_t>(),		"Set Number of Trials")
//			("rng",		po::value<uint64_t>(),		"Set RNG seed")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			cout << desc << "\n";
			return 1;
		}

		if (vm.count("ps"))
		{
			pop_size = vm["ps"].as<uint64_t>();
			cout << "Population size was set to " << pop_size << ".\n";
		}
		else
		{
			cout << "Population size was set to default of " << pop_size << ".\n";
		}

		if (vm.count("mr"))
		{
			mu_r = vm["mr"].as<double>();
			cout << "Mutation rate was set to " << mu_r << ".\n";
		}
		else
		{
			cout << "Mutation rate was set to default of " << mu_r << ".\n";
		}

		if (vm.count("xr"))
		{
			xo_r = vm["xr"].as<double>();
			cout << "Crossover rate was set to " << xo_r << ".\n";
		}
		else
		{
			cout << "Crossover rate was set to default of " << xo_r << ".\n";
		}


		if (vm.count("xp"))
		{
			xo_p = vm["xp"].as<uint64_t>();
			cout << "Maximum number of crossover points was set to " << xo_p << ".\n";
		}
		else
		{
			cout << "Maximum number of crossover points was set to default of " << xo_p << ".\n";
		}

		if (vm.count("trials"))
		{
			num_trials = vm["trials"].as<uint64_t>();
			cout << "Population size was set to " << num_trials << ".\n";
		}
		else
		{
			cout << "Population size was set to default of " << num_trials << ".\n";
		}

		if (vm.count("el"))
		{
			elitism = vm["el"].as<bool>();
			cout << "Elitism was set to " << elitism << ".\n";
		}
		else
		{
			cout << "Elitism was set to default of " << elitism << ".\n";
		}

/*
		if (vm.count("rng"))
		{
			rng_seed = vm["rng"].as<uint64_t>();
			cout << "OpenCV RNG was seeded with " << rng_seed << ".\n";
		}
		else
		{
			cout << "OpenCV RNG was seeded with default of " << rng_seed << ".\n";
		}
*/
	}
	catch(std::exception& e)
	{
		cout << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...)
	{
		cout << "You threw an exception, idiot.\n";
		return 1;
	}


//	RNG randi (rng_seed);

	uint64_t iter, jter;

	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		qPriorLikelihood[iter] = ((qPriorProbability[iter])/(1.0-qPriorProbability[iter]));
		if (qPriorLikelihood[iter] > (1.0-ZERO_FITNESS_LIMIT))
			qPriorLikelihood[iter] = (1.0-ZERO_FITNESS_LIMIT);
		if (qPriorLikelihood[iter] < ZERO_FITNESS_LIMIT)
			qPriorLikelihood[iter] = ZERO_FITNESS_LIMIT;
	}

	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		for (jter=0; jter<NUMBER_GENES; jter++)
		{
			if (qManifestationInDisease[iter][jter] < ZERO_FITNESS_LIMIT)
				qManifestationInDisease[iter][jter] = ZERO_FITNESS_LIMIT;
		}
	}



	// Print data to file
	stringstream strstr (stringstream::in | stringstream::out);
	strstr.clear();	strstr.str("");
	strstr << "./mfd"
		<< "_" << pop_size
		<< "_" << NUMBER_GENERATIONS
		<< "_" << xo_p
		<< "_" << xo_r
		<< "_" << mu_r
		<< "_" << elitism
//			<< "_" << trailer_trash			// Current Trial
//		<< "_" << SymptomSet
		<< "_" << "first"
		<< ".csv";
	string outname;
	outname = strstr.str();
	ofstream outfileTheFirst;
	outfileTheFirst.open( outname.c_str() );

	if ( !outfileTheFirst.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}

	strstr.clear();	strstr.str("");
	strstr << "./mfd"
		<< "_" << pop_size
		<< "_" << NUMBER_GENERATIONS
		<< "_" << xo_p
		<< "_" << xo_r
		<< "_" << mu_r
		<< "_" << elitism
//			<< "_" << trailer_trash			// Current Trial
//		<< "_" << SymptomSet
		<< "_" << "second"
		<< ".csv";
	outname = strstr.str();
	ofstream outfileTheSecond;
	outfileTheSecond.open( outname.c_str() );

	if ( !outfileTheSecond.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}

	strstr.clear();	strstr.str("");
	strstr << "./mfd"
		<< "_" << pop_size
		<< "_" << NUMBER_GENERATIONS
		<< "_" << xo_p
		<< "_" << xo_r
		<< "_" << mu_r
		<< "_" << elitism
//			<< "_" << trailer_trash			// Current Trial
//		<< "_" << SymptomSet
		<< "_" << "third"
		<< ".csv";
	outname = strstr.str();
	ofstream outfileTheThird;
	outfileTheThird.open( outname.c_str() );

	if ( !outfileTheThird.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}


	strstr.clear();	strstr.str("");
	strstr << "./mfd_best"
		<< "_" << pop_size
		<< "_" << NUMBER_GENERATIONS
		<< "_" << xo_p
		<< "_" << xo_r
		<< "_" << mu_r
		<< "_" << elitism
//			<< "_" << trailer_trash			// Current Trial
//		<< "_" << SymptomSet
		<< ".csv";
	outname = strstr.str();
	ofstream outfileTheBest;
	outfileTheBest.open( outname.c_str() );

	if ( !outfileTheBest.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}

	outfileTheFirst.precision(35);
	outfileTheSecond.precision(35);
	outfileTheThird.precision(35);
	outfileTheBest.precision(35);

/*
	outfileTheFirst << "SymptomSet,Trial";
	outfileTheSecond << "SymptomSet,Trial";
	outfileTheThird << "SymptomSet,Trial";
*/
	outfileTheFirst << "RNG_Seed,SymptomSet,Trial";
	outfileTheSecond << "RNG_Seed,SymptomSet,Trial";
	outfileTheThird << "RNG_Seed,SymptomSet,Trial";
	outfileTheBest << "RNG_Seed,SymptomSet,Trial,FitCalc,Genotype,Fitness";

	for (iter=0; iter<NUMBER_GENERATIONS; iter++)
	{
		outfileTheFirst << ",G" << iter;
		outfileTheSecond << ",G" << iter;
		outfileTheThird << ",G" << iter;
	}

	outfileTheFirst << "\n";
	outfileTheSecond << "\n";
	outfileTheThird << "\n";

	population *hoponpop;

	uint64_t symptoms;
	for (symptoms=1; symptoms<(1<<NUMBER_SYMPTOMS); symptoms++)
	{
		SymptomSet = symptoms;

		last_tick_count = (double) getTickCount();
		cout << "Symptom set: " << SymptomSet << endl;

		uint64_t trailer_trash;
		for (trailer_trash=0; trailer_trash<num_trials; trailer_trash++)
		{
			uint64_t rng_seed = getTickCount();
			RNG randi (rng_seed);

		// population(RNG& rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, bool elitism);
			hoponpop = new population(randi, pop_size, mu_r, xo_r, xo_p, elitism);

//			cout << "Trial: " << trailer_trash << endl;
			if (trailer_trash==0)
				cout << "Trial: ";
			cout << trailer_trash << " ";

//			hoponpop.populator();
			hoponpop->populator();

			specimen_t indiFirst [NUMBER_GENERATIONS];
			specimen_t indiSecond[NUMBER_GENERATIONS];
			specimen_t indiThird [NUMBER_GENERATIONS];

			uint64_t generational_recursion;
			for (generational_recursion=0; generational_recursion<NUMBER_GENERATIONS; generational_recursion++)
			{
//				cout << "Generation: " << generational_recursion << endl;
//				hoponpop.breeder();
				hoponpop->breeder();
//				cout << "Generation over\n";

				indiFirst[generational_recursion] = hoponpop->First ();
				indiSecond[generational_recursion] = hoponpop->Second();
				indiThird[generational_recursion] = hoponpop->Third ();
			}

/*
			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
//				indiFirst[iter] = hoponpop.First (iter);
//				indiSecond[iter] = hoponpop.Second(iter);
//				indiThird[iter] = hoponpop.Third (iter);
				indiFirst[iter] = hoponpop->First (iter);
				indiSecond[iter] = hoponpop->Second(iter);
				indiThird[iter] = hoponpop->Third (iter);
			}
*/
			specimen_t bestever = hoponpop->BestEver();
			outfileTheBest << "\n" << rng_seed << "," << SymptomSet << "," << trailer_trash << "," << bestever.calced << "," << bestever.gen << "," << bestever.fit;

			outfileTheFirst << rng_seed << "," << SymptomSet << "," << trailer_trash;
			outfileTheSecond << rng_seed << "," << SymptomSet << "," << trailer_trash;
			outfileTheThird << rng_seed << "," << SymptomSet << "," << trailer_trash;

			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
				outfileTheFirst << "," << indiFirst[iter].gen;
				outfileTheSecond << "," << indiSecond[iter].gen;
				outfileTheThird << "," << indiThird[iter].gen;
			}
			outfileTheFirst << "\n" << rng_seed << "," << SymptomSet << "," << trailer_trash;
			outfileTheSecond << "\n" << rng_seed << "," << SymptomSet << "," << trailer_trash;
			outfileTheThird << "\n" << rng_seed << "," << SymptomSet << "," << trailer_trash;


			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
				outfileTheFirst << "," << indiFirst[iter].fit;
				outfileTheSecond << "," << indiSecond[iter].fit;
				outfileTheThird << "," << indiThird[iter].fit;
			}
			outfileTheFirst << "\n";
			outfileTheSecond << "\n";
			outfileTheThird << "\n";

			delete hoponpop;
		}//End of Trial
		cout << endl;

		outfileTheFirst << "\n";
		outfileTheSecond << "\n";
		outfileTheThird << "\n";
		outfileTheBest << "\n";

		outfileTheFirst.flush();
		outfileTheSecond.flush();
		outfileTheThird.flush();
		outfileTheBest.flush();

		cout << "\tProcessing time: " << ((double) getTickCount() - last_tick_count)/getTickFrequency() << "[s]\n";

	}//End of Symptom Set
	outfileTheFirst.close();
	outfileTheSecond.close();
	outfileTheThird.close();
	outfileTheBest.close();

	return 0;
}

