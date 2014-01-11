#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include "opencv2/core/core.hpp"
#include <cmath>

#include <algorithm>
#include <vector>
#include <iterator>

#include <boost/program_options.hpp>
#include "TendencyMatrix10x25.h"


#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			300
//#define NUMBER_INDIVIDUALS			80
//#define NUMBER_INDIVIDUALS			120
//#define NUMBER_INDIVIDUALS			160

#define NUMBER_TRACKING				3
#define NUMBER_ATTRIBUTES			25
#define NUMBER_DIMENSIONS			10

//#define FITNESS_TYPE				__float128
//#define FITNESS_TYPE				double
#define GENO_TYPE					bitset<NUMBER_ATTRIBUTES>

#define ZERO_FITNESS_LIMIT			1e-6

using namespace cv;
using namespace boost;
namespace po = boost::program_options;
using namespace std;

#define NUMBER_SYMPTOMS				10
bitset<NUMBER_SYMPTOMS> SymptomSet;

typedef struct particle_t
{
	particle_t(void) {
		uint64_t iter;
		for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
		{
			pos[iter] = 0.0;
			vel[iter] = 0.0;
		}
	};

	double pos[NUMBER_ATTRIBUTES];
	double vel[NUMBER_ATTRIBUTES];
} particle_t;

typedef struct specimen_t
{
	specimen_t(void) : fit(0.0), gen(0), calced(0), bf(0.0) {};

	FITNESS_TYPE	fit;
	GENO_TYPE		gen;
	uint64_t		calced;
	particle_t		cc;

	FITNESS_TYPE	bf;
	particle_t		bc;
} specimen_t;

bool loci_comp (uint64_t i,uint64_t j) { return (i<=j); }
bool spec_comp (specimen_t i, specimen_t j) { return (i.fit>j.fit); }
bool spec_uniq (specimen_t i, specimen_t j) { return (i.gen==j.gen); }


class population
{
private:
	specimen_t *pop_;
//	specimen_t **best_;
	specimen_t *best_;

	RNG rudi_;
	uint64_t pop_size_;
	specimen_t best_in_swarm_;
	uint64_t fitness_calculation_counter_;

	particle_t min_, max_;
	double cog_, soc_, inertia_;

	uint64_t generation_;
	void bestest(void);

	GENO_TYPE discretize(particle_t);
	FITNESS_TYPE calcFitness(GENO_TYPE);
	specimen_t populate(void);

public:
	population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia);
	~population(void);
	void populator(void);
	void iterate(void);

//	specimen_t First (uint64_t);
//	specimen_t Second(uint64_t);
//	specimen_t Third (uint64_t);

	specimen_t First (void){return best_[0];}
	specimen_t Second(void){return best_[1];}
	specimen_t Third (void){return best_[2];}

	specimen_t BestInSwarm(void){return best_in_swarm_;}

	uint64_t Age(void) {return generation_;}
};

population::population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia)
{
	rudi_ = rudi;
	pop_size_ = pop_size;
	fitness_calculation_counter_ = 0;

	min_ = min_lim;
	max_ = max_lim;
	cog_ = cog;
	soc_ = soc;
	inertia_ = inertia;

	pop_ = new specimen_t[pop_size_];
//	best_ = new specimen_t*[NUMBER_TRACKING];
//	best_[0] = new specimen_t[NUMBER_GENERATIONS];
//	best_[1] = new specimen_t[NUMBER_GENERATIONS];
//	best_[2] = new specimen_t[NUMBER_GENERATIONS];
	best_ = new specimen_t[NUMBER_TRACKING];
}

population::~population(void)
{
	delete[] pop_;
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

	if (best_[0][generation_].fit > best_in_swarm_.fit)
		best_in_swarm_ = best_[0][generation_];
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


	if (best_[0].fit > best_in_swarm_.fit)
	{
		best_in_swarm_ = best_[0];
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

void population::iterate(void)
{
//	GENO_TYPE kiddies[pop_size_];

	uint64_t iter, jter;

	for (iter=0; iter<pop_size_; iter++)
	{
		for (jter=0; jter<NUMBER_ATTRIBUTES; jter++)
		{
			pop_[iter].cc.vel[jter] =	(pop_[iter].cc.vel[jter] * inertia_)
				+ ( (rudi_.uniform((double)0.0,(double)1.0) * cog_) * (pop_[iter].bc.pos[jter]     - pop_[iter].cc.pos[jter]) )
				+ ( (rudi_.uniform((double)0.0,(double)1.0) * soc_) * (best_in_swarm_.cc.pos[jter] - pop_[iter].cc.pos[jter]) );

//			pop_[iter].cc[jter].vel = (pop_[iter].cc[jter].vel * inertia_) + (pop_[iter].cc[jter].vel * cog_) + (best_in_swarm_[jter].vel * soc_);
			if (pop_[iter].cc.vel[jter] > max_.vel[jter])
				pop_[iter].cc.vel[jter] = max_.vel[jter];
			else if (pop_[iter].cc.vel[jter] < min_.vel[jter])
				pop_[iter].cc.vel[jter] = min_.vel[jter];

//			pop_[iter].cc.pos[jter] = pop_[iter].cc.pos[jter] + (pop_[iter].cc.vel[jter] * 1.0);//time__);
//			if (pop_[iter].cc.pos[jter] > max_.pos[jter])
//				pop_[iter].cc.pos[jter] = max_.pos[jter];
//			else if (pop_[iter].cc.pos[jter] < min_.pos[jter])
//				pop_[iter].cc.pos[jter] = min_.pos[jter];

			if ( rudi_.uniform((double)0.0,(double)1.0) < (double)( 1.0/ (1.0+exp(pop_[iter].cc.vel[jter] * -1.0)) ) )
			{
				pop_[iter].cc.pos[jter] = 1.0;
				pop_[iter].gen[jter] = 1;
			}
			else
			{
				pop_[iter].cc.pos[jter] = 0.0;
				pop_[iter].gen[jter] = 0;
			}
		}
//		pop_[iter].gen = discretize(pop_[iter].cc);
		pop_[iter].fit = calcFitness(pop_[iter].gen);
		pop_[iter].calced = fitness_calculation_counter_;

		if (pop_[iter].fit > pop_[iter].bf)
		{
			pop_[iter].bc = pop_[iter].cc;
			pop_[iter].bf = pop_[iter].fit;
		}
//		cout << "ID: " << iter << "\tGEN: " << pop_[iter].gen << endl;
	}

	generation_++;
	bestest();
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
		if (SymptomSet[NUMBER_SYMPTOMS-(iter+1)] == 1)
		{
			temp = 1.0;
			for (jter=0; jter<NUMBER_ATTRIBUTES; jter++)
			{
				if (genie[jter])//indi.gen[jter])
				{
					temp *= (1.0-qManifestationInDisease[iter][jter]);
				}
			}
			L1 *= (1.0-temp);
		}
	}

	// Negative Likelihood
	for (jter=0; jter<NUMBER_ATTRIBUTES; jter++)
	{
		if (genie[jter])//indi.gen[iter])
		{
			temp = 1.0;
			for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
			{
				if (SymptomSet[NUMBER_SYMPTOMS-(iter+1)] == 0)
				{
					temp *= (1.0-qManifestationInDisease[iter][jter]);
				}
			}
			L2 *= (temp);
		}
	}

	// Prior Likelihood
	for (jter=0; jter<NUMBER_ATTRIBUTES; jter++)
	{
		if (genie[jter])//indi.gen[iter])
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
	//indi.gen = rudi_.uniform(0, 1<<(NUMBER_ATTRIBUTES));
	uint64_t iter;

	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		indi.cc.vel[iter] = rudi_.uniform((double)min_.vel[iter],(double)max_.vel[iter]);
		indi.cc.pos[iter] = rudi_.uniform((double)min_.pos[iter],(double)max_.pos[iter]);
	}

	indi.gen = discretize(indi.cc);
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	indi.bc = indi.cc;
	indi.bf = indi.fit;
	return indi;
}

GENO_TYPE population::discretize(particle_t par)
{
	GENO_TYPE genie;
	uint64_t iter;
	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		if (par.pos[iter] > 0.5)
			genie[iter] = 1;
		else
			genie[iter] = 0;
	}
	return genie;
}


int main(int argc, char* argv[])
{
	uint64_t pop_size = 80;
	double inertia_r = 0.1;
	double cog_r = 0.3;
	double soc_r  = 0.3;
//	uint64_t rng_seed = 0xF0F0F0F0;
//	uint64_t rng_seed = getTickCount();
	uint64_t num_trials = NUMBER_TRIALS;

	double last_tick_count = 0.0;

    try
    {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help",								"Produce help message")
			("popsize",	po::value<uint64_t>(),		"Set Population Size")
			("inertia",	po::value<double>(),		"Set Particle Inertia")
			("cog",		po::value<double>(),		"Set Cognitive effect of particle")
			("soc",		po::value<double>(),		"Set Social effect of swarm")
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

		if (vm.count("popsize"))
		{
			pop_size = vm["popsize"].as<uint64_t>();
			cout << "Population size was set to " << pop_size << ".\n";
		}
		else
		{
			cout << "Population size was set to default of " << pop_size << ".\n";
		}

		if (vm.count("inertia"))
		{
			inertia_r = vm["inertia"].as<double>();
			cout << "Inertia was set to " << inertia_r << ".\n";
		}
		else
		{
			cout << "Inertia was set to default of " << inertia_r << ".\n";
		}

		if (vm.count("cog"))
		{
			cog_r = vm["cog"].as<double>();
			cout << "COG was set to " << cog_r << ".\n";
		}
		else
		{
			cout << "COG was set to default of " << cog_r << ".\n";
		}


		if (vm.count("soc"))
		{
			soc_r = vm["soc"].as<double>();
			cout << "SOC was set to " << soc_r << ".\n";
		}
		else
		{
			cout << "SOC was set to default of " << soc_r << ".\n";
		}

		if (vm.count("trials"))
		{
			num_trials = vm["trials"].as<uint64_t>();
			cout << "Number of Trials was set to " << num_trials << ".\n";
		}
		else
		{
			cout << "Number of Trials was set to default of " << num_trials << ".\n";
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
	particle_t mini, maxi;

	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		qPriorLikelihood[iter] = ((qPriorProbability[iter])/(1.0-qPriorProbability[iter]));
		if (qPriorLikelihood[iter] > (1.0-ZERO_FITNESS_LIMIT))
			qPriorLikelihood[iter] = (1.0-ZERO_FITNESS_LIMIT);
		if (qPriorLikelihood[iter] < ZERO_FITNESS_LIMIT)
			qPriorLikelihood[iter] = ZERO_FITNESS_LIMIT;

		mini.pos[iter] = 0.0;
		maxi.pos[iter] = 1.0;

		mini.vel[iter] = -4.0;
		maxi.vel[iter] = 4.0;
	}

	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		for (jter=0; jter<NUMBER_ATTRIBUTES; jter++)
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
		<< "_" << inertia_r
		<< "_" << cog_r
		<< "_" << soc_r
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
		<< "_" << inertia_r
		<< "_" << cog_r
		<< "_" << soc_r
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
		<< "_" << inertia_r
		<< "_" << cog_r
		<< "_" << soc_r
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
		<< "_" << inertia_r
		<< "_" << cog_r
		<< "_" << soc_r
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
	outfileTheFirst << "RNG_Seed=" << rng_seed << "\n";
	outfileTheSecond << "RNG_Seed=" << rng_seed << "\n";
	outfileTheThird << "RNG_Seed=" << rng_seed << "\n";

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

		// population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia);
			hoponpop = new population(randi, pop_size, mini, maxi, cog_r, soc_r, inertia_r);

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
//				hoponpop.iterate();
				hoponpop->iterate();
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
			specimen_t bestinswarm = hoponpop->BestInSwarm();
			outfileTheBest << "\n" << rng_seed << "," << SymptomSet << "," << trailer_trash << "," << bestinswarm.calced << "," << bestinswarm.gen << "," << bestinswarm.fit;

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

