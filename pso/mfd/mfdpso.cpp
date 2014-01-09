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

typedef struct
{
	double pos[NUMBER_ATTRIBUTES];
	double vel[NUMBER_ATTRIBUTES];
} particle_t;

typedef struct
{
	FITNESS_TYPE	fit;
	GENO_TYPE		gen;
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
	specimen_t **best_;

	RNG rudi_;
	uint64_t pop_size_;
	specimen_t best_in_swarm_;

	particle_t min_, max_;
	double cog_, soc_, inertia_;

	uint64_t generation_;
	void bestest(void);

	GENO_TYPE population::discretize(particle_t);
	FITNESS_TYPE calcFitness(GENO_TYPE);
	specimen_t populate(void);

public:
	population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia);
	void populator(void);
	void iterate(void);

	specimen_t First (uint64_t);
	specimen_t Second(uint64_t);
	specimen_t Third (uint64_t);

	uint64_t Age(void) {return generation_;}
};

population::population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia)
{
	rudi_ = rudi;
	pop_size_ = pop_size;

	min_ = min_lim;
	max_ = max_lim;
	cog_ = cog;
	soc_ = soc;
	inertia_ = inertia;

	pop_ = new specimen_t[pop_size_];
	best_ = new specimen_t*[NUMBER_TRACKING];
	best_[0] = new specimen_t[NUMBER_GENERATIONS];
	best_[1] = new specimen_t[NUMBER_GENERATIONS];
	best_[2] = new specimen_t[NUMBER_GENERATIONS];
}

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

void population::bestest(void)
{
	std::vector<specimen_t> punk (pop_, pop_+pop_size_);
	std::sort (punk.begin(), punk.end(), spec_comp);


	std::vector<specimen_t>::iterator viter;
	viter = std::unique(punk.begin(), punk.end(), spec_uniq);
	punk.resize( std::distance(punk.begin(),viter) );

	viter = punk.begin();

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
	GENO_TYPE kiddies[pop_size_];

	uint64_t iter, jter;

	for (iter=0; iter<pop_size_; iter++)
	{
		for (jter=0; jter<NUMBER_ATTRIBUTES; jter)
		{
//			pop_[iter].cc[jter].vel = (pop_[iter].cc[jter].vel * inertia_) + (pop_[iter].cc[jter].vel * cog_) + (best_in_swarm_[jter].vel * soc_);
			pop_[iter].cc[jter].vel = (pop_[iter].cc[jter].vel * inertia_) + (pop_[iter].bc[jter].vel * cog_) + (best_in_swarm_[jter].vel * soc_);
			pop_[iter].cc[jter].pos = pop_[iter].cc[jter].pos + (pop_[iter].cc[jter].vel * time__)
		}
		pop_[iter].gen = discretize(pop_[iter].cc);
		pop_[iter].fit = calcFitness(pop_[iter].gen);
		if (pop_[iter].fit > pop_[iter].bf)
		{
			pop_[iter].bc = pop_[iter].cc;
			pop_[iter].bf = pop_[iter].fit;
		}
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
		if (SymptomSet[iter])
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
	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		if (genie[iter])//indi.gen[iter])
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
	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		if (genie[iter])//indi.gen[iter])
		{
			L3 *= qPriorLikelihood[iter];
		}
	}

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

	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		indi.cc.vel[iter] = rudi_.uniform(min_.vel[iter],max_.vel[iter]);
		indi.cc.pos[iter] = rudi_.uniform(min_.pos[iter],max_.pos[iter]);
	}

	indi.gen = discretize(indi.cc);
	indi.fit = calcFitness(indi.gen);
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
	uint64_t rng_seed = 0xF0F0F0F0;

	double last_tick_count = 0.0;

    try
    {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help",								"Produce help message")
			("popsize",	po::value<uint64_t>(),		"Set Population Size")
			("interia",	po::value<double>(),		"Set Particle Interia")
			("cog",		po::value<double>(),		"Set pull of best position of particle")
			("soc",		po::value<double>(),		"Set pull of best position of swarm")
			("rng",		po::value<uint64_t>(),		"Set RNG seed")
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


		if (vm.count("rng"))
		{
			rng_seed = vm["rng"].as<uint64_t>();
			cout << "OpenCV RNG was seeded with " << rng_seed << ".\n";
		}
		else
		{
			cout << "OpenCV RNG was seeded with default of " << rng_seed << ".\n";
		}
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


	RNG randi (rng_seed);

	uint64_t iter;
	particle_t mini, maxi;

	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		qPriorLikelihood[iter] = ((qPriorProbability[iter])/(1.0-qPriorProbability[iter]));

		mini[iter].pos = 0.0;
		mini[iter].vel = 0.0;

		maxi[iter].pos = 1.0;
		maxi[iter].vel = 1.0;
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

	outfileTheFirst << "SymptomSet,Trial";
	outfileTheSecond << "SymptomSet,Trial";
	outfileTheThird << "SymptomSet,Trial";

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
		for (trailer_trash=0; trailer_trash<NUMBER_TRIALS; trailer_trash++)
		{
		// population(RNG& rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, bool elitism);
//			population hoponpop(randi, pop_size, mu_r, xo_r, xo_p, elitism);
//	population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia);
			hoponpop = new population(randi, pop_size, mini, maxi, cog_r, soc_r, inertia_r);

//			cout << "Trial: " << trailer_trash << endl;

//			hoponpop.populator();
			hoponpop->populator();


			uint64_t generational_recursion;
			for (generational_recursion=0; generational_recursion<NUMBER_GENERATIONS; generational_recursion++)
			{
//				cout << "Generation: " << generational_recursion << endl;
//				hoponpop.iterate();
				hoponpop->iterate();
//				cout << "Generation over\n";
			}

			specimen_t indiFirst [NUMBER_GENERATIONS];
			specimen_t indiSecond[NUMBER_GENERATIONS];
			specimen_t indiThird [NUMBER_GENERATIONS];
			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
//				indiFirst[iter] = hoponpop.First (iter);
//				indiSecond[iter] = hoponpop.Second(iter);
//				indiThird[iter] = hoponpop.Third (iter);
				indiFirst[iter] = hoponpop->First (iter);
				indiSecond[iter] = hoponpop->Second(iter);
				indiThird[iter] = hoponpop->Third (iter);
			}

			outfileTheFirst << SymptomSet << "," << trailer_trash;
			outfileTheSecond << SymptomSet << "," << trailer_trash;
			outfileTheThird << SymptomSet << "," << trailer_trash;

			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
				outfileTheFirst << "," << indiFirst[iter].gen;
				outfileTheSecond << "," << indiSecond[iter].gen;
				outfileTheThird << "," << indiThird[iter].gen;
			}
			outfileTheFirst << "\n" << SymptomSet << "," << trailer_trash;
			outfileTheSecond << "\n" << SymptomSet << "," << trailer_trash;
			outfileTheThird << "\n" << SymptomSet << "," << trailer_trash;


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

		outfileTheFirst << "\n";
		outfileTheSecond << "\n";
		outfileTheThird << "\n";

		outfileTheFirst.flush();
		outfileTheSecond.flush();
		outfileTheThird.flush();

		cout << "\tProcessing time: " << ((double) getTickCount() - last_tick_count)/getTickFrequency() << "[s]\n";

	}//End of Symptom Set
	outfileTheFirst.close();
	outfileTheSecond.close();
	outfileTheThird.close();

	return 0;
}

