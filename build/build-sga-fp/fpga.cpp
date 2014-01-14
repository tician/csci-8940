
#include "fpga.hpp"
/*
using namespace cv;
using namespace boost;
namespace po = boost::program_options;
using namespace std;
*/

typedef struct genotype_t
{
	genotype_t(void) : one(0), two(0), thr(0) {};

	GENO_TYPE		one;
	GENO_TYPE		two;
	GENO_TYPE		thr;
} genotype_t;

typedef struct specimen_t
{
	specimen_t(void) : fit(0.0), calced(0) {};

	FITNESS_TYPE	fit;
	genotype_t		gen;
	uint64_t		calced;
} specimen_t;


bool loci_comp (uint64_t i,uint64_t j)
{
	return (i<=j);
}
bool spec_comp (specimen_t i, specimen_t j)
{
	return (i.fit>j.fit);
}
bool spec_uniq (specimen_t i, specimen_t j)
{
	if (i.gen.one!=j.gen.one)
		return false;
	if (i.gen.two!=j.gen.two)
		return false;
	if (i.gen.thr!=j.gen.thr)
		return false;
	return true;
}


class population
{
private:
	specimen_t *pop_;
	specimen_t *best_;
	specimen_t best_ever_;

	FITNESS_TYPE *sig_fit_;

	uint64_t fitness_calculation_counter_;

	RNG rudi_;
	uint64_t pop_size_;
	double mu_r_;
	double xo_r_;
	uint64_t xo_p_;
	bool elitism_;

	void roulette(void);
	void selector(genotype_t&);
	void splicer(genotype_t&, genotype_t&);
	void mutator(void);

	uint64_t generation_;
	void bestest(void);


	FITNESS_TYPE calcFitness(genotype_t);
	specimen_t populate(void);
	specimen_t mutate(specimen_t);
	void fixer(specimen_t&);

public:
	population(RNG& rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, bool elitism);
	~population(void);
	void populator(void);
	void breeder(void);

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

	best_ = new specimen_t[NUMBER_TRACKING];
}

population::~population(void)
{
	delete[] pop_;
	delete[] sig_fit_;
	delete[] best_;
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
	while( (viter<punk.end()) && (jter<NUMBER_TRACKING) )
	{
		best_[jter] = (*viter);
		jter++; viter++;
	}
	while (jter<NUMBER_TRACKING)
	{
		best_[jter].gen.one = 0;
		best_[jter].gen.two = 0;
		best_[jter].gen.thr = 0;
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
	// Check for lack of improvement in best individual (over 25 generations without improvement, then stop processing)
	if (fitness_calculation_counter_ > (best_ever_.calced + (pop_size_ * 25)) )
		return;

	genotype_t kiddies[pop_size_];
	roulette();

	uint64_t iter;

	if (elitism_)// && (generation_>0))
	{
		for (iter=0; iter<(pop_size_/2)-2; iter++)
		{
			genotype_t mama, papa;

			selector(mama);
			selector(papa);
			splicer(mama, papa);
			kiddies[(2*iter)+0] = mama;
			kiddies[(2*iter)+1] = papa;
		}
		kiddies[(2*iter)+0] = best_[0].gen;
		kiddies[(2*iter)+1] = best_[0].gen;
	}
	else
	{
		for (iter=0; iter<(pop_size_/2); iter++)
		{
			genotype_t mama, papa;

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

void population::selector(genotype_t& nana)
{
	uint64_t iter;
	// Roulette Wheel Selection
	FITNESS_TYPE temp = rudi_.uniform(0, sig_fit_[pop_size_-1]);

	for (iter=0; iter<pop_size_; iter++)
	{
		if (temp <= sig_fit_[iter])
		{
			nana = pop_[iter].gen;
			break;
		}
	}
}

void population::splicer(genotype_t& mama, genotype_t& papa)
{
	// Crossover loci (0 ~ 72)

	uint64_t iter;
	uint64_t *locus;
	locus = new uint64_t[xo_p_];

	GENO_TYPE mask;
	genotype_t ba, by;

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

	it = loci.begin();

	bool up=true;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if ( (iter >= *it) && (it < loci.end()) )
		{
			it++;
			up = ~up;
		}
		mask[iter] = up;
	}

	ba.one = ( mask & mama.one) | (~mask & papa.one) ;
	by.one = (~mask & mama.one) | ( mask & papa.one) ;

	ba.two = ( mask & mama.two) | (~mask & papa.two) ;
	by.two = (~mask & mama.two) | ( mask & papa.two) ;

	ba.thr = ( mask & mama.two) | (~mask & papa.two) ;
	by.thr = (~mask & mama.two) | ( mask & papa.two) ;

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

FITNESS_TYPE population::calcFitness(genotype_t genie)
{
	FITNESS_TYPE temp = 0;
	uint64_t iter;

	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (genie.one[iter]==1)
			temp += West73_Yields[iter].y1;
		if (genie.two[iter]==1)
			temp += West73_Yields[iter].y2;
		if (genie.thr[iter]==1)
			temp += West73_Yields[iter].y3;
	}

	fitness_calculation_counter_++;

	return temp;
}

specimen_t population::populate(void)
{
	specimen_t indi;
//	indi.gen.one = rudi_.uniform(0, 1<<(NUMBER_GENES));
//	indi.gen.two = rudi_.uniform(0, 1<<(NUMBER_GENES));
//	indi.gen.thr = rudi_.uniform(0, 1<<(NUMBER_GENES));
	indi.gen.one = rudi_.uniform(0, 1<<32) | (rudi_.uniform(0, 1<<32)<<32) | (rudi_.uniform(0, 1<<9)<<64);
	indi.gen.two = rudi_.uniform(0, 1<<32) | (rudi_.uniform(0, 1<<32)<<32) | (rudi_.uniform(0, 1<<9)<<64);
	indi.gen.thr = rudi_.uniform(0, 1<<32) | (rudi_.uniform(0, 1<<32)<<32) | (rudi_.uniform(0, 1<<9)<<64);
	fixer(indi);
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
			indi.gen.one.flip(iter);
		}
		if (rudi_.uniform( 0, (int)(1/mu_r_) ) < 1)
		{
			indi.gen.two.flip(iter);
		}
		if (rudi_.uniform( 0, (int)(1/mu_r_) ) < 1)
		{
			indi.gen.thr.flip(iter);
		}
	}
	fixer(indi);
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	return indi;
}

void population::fixer(specimen_t& indi)
{
	// Fix for repeated harvesting
	indi.gen.two &= ~indi.gen.one;
	indi.gen.thr &= ~indi.gen.one;
	indi.gen.thr &= ~indi.gen.two;


	// Fix for adjacency
	uint64_t iter;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		if (indi.gen.one[iter]==1)
		{
			indi.gen.one &= West73_Adjacency[iter];
		}
		if (indi.gen.two[iter]==1)
		{
			indi.gen.two &= West73_Adjacency[iter];
		}
		if (indi.gen.thr[iter]==1)
		{
			indi.gen.thr &= West73_Adjacency[iter];
		}
	}
}


int main(int argc, char* argv[])
{
	uint64_t pop_size = 80;
	double mu_r = 0.001;
	double xo_r = 0.4;
	uint64_t xo_p = 1;
	bool elitism = false;
	uint64_t num_trials = NUMBER_TRIALS;
//	uint64_t rng_seed = 0xF0F0F0F0;
	bool enable_history = false;

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
			("history",	po::value<bool>(),			"Enable History")
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
			cout << "Number of Trials was set to " << num_trials << ".\n";
		}
		else
		{
			cout << "Number of Trials was set to default of " << num_trials << ".\n";
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

		if (vm.count("enable_history"))
		{
			enable_history = vm["enable_history"].as<bool>();
			cout << "History enabled: " << enable_history << ".\n";
		}
		else
		{
			cout << "History was set to default of " << enable_history << ".\n";
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


	// Set up the bitset array that you could not initialize.
	uint64_t iter;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		West73_Adjacency[iter].set();
	}
	for (iter=0; iter<196; iter++)
	{
		West73_Adjacency[West73_Adjacency_builder[iter].id-1].reset(West73_Adjacency_builder[iter].ad-1);
	}



	// Print data to file
	stringstream strstr (stringstream::in | stringstream::out);
	string outname;
	ofstream outfileTheFirst;
	ofstream outfileTheSecond;
	ofstream outfileTheThird;

	if (enable_history)
	{
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
		outname = strstr.str();
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
		outfileTheThird.open( outname.c_str() );

		if ( !outfileTheThird.is_open() )
		{
			cerr << "Unable to open file: " << outname << "\n";
			return 1;
		}

		outfileTheFirst.precision(35);
		outfileTheSecond.precision(35);
		outfileTheThird.precision(35);
	}

	strstr.clear();	strstr.str("");
	strstr << "./sga_fp_best"
//		<< "_" << pop_size
//		<< "_" << NUMBER_GENERATIONS
//		<< "_" << xo_p
//		<< "_" << xo_r
//		<< "_" << mu_r
//		<< "_" << elitism
//			<< "_" << trailer_trash			// Current Trial
//		<< "_" << SymptomSet
		<< ".csv";
	outname = strstr.str();
	ofstream outfileTheBest;
	outfileTheBest.open( outname.c_str(), std::ofstream::out | std::ofstream::app );

	if ( !outfileTheBest.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}

	outfileTheBest.precision(35);

	if (enable_history)
	{
		outfileTheFirst << "RNG_Seed,Trial";
		outfileTheSecond << "RNG_Seed,Trial";
		outfileTheThird << "RNG_Seed,Trial";

		for (iter=0; iter<NUMBER_GENERATIONS; iter++)
		{
			outfileTheFirst << ",Ga" << iter << ",Gb" << iter << ",Gc" << iter;
			outfileTheSecond << ",Ga" << iter << ",Gb" << iter << ",Gc" << iter;
			outfileTheThird << ",Ga" << iter << ",Gb" << iter << ",Gc" << iter;
		}

		outfileTheFirst << "\n";
		outfileTheSecond << "\n";
		outfileTheThird << "\n";
	}

	outfileTheBest << "NumGens,PopSize,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,Geno1,Geno2,Geno3,Fitness";

	population *hoponpop;


	last_tick_count = (double) getTickCount();

	uint64_t trailer_trash;
	for (trailer_trash=0; trailer_trash<num_trials; trailer_trash++)
	{
		uint64_t rng_seed = getTickCount();
		RNG randi (rng_seed);

	// population(RNG& rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, bool elitism);
		hoponpop = new population(randi, pop_size, mu_r, xo_r, xo_p, elitism);

//		cout << "Trial: " << trailer_trash << endl;
		if (trailer_trash==0)
			cout << "Trial: ";
		cout << trailer_trash << " ";

		hoponpop->populator();

		specimen_t indiFirst [NUMBER_GENERATIONS];
		specimen_t indiSecond[NUMBER_GENERATIONS];
		specimen_t indiThird [NUMBER_GENERATIONS];

		uint64_t generational_recursion;
		for (generational_recursion=0; generational_recursion<NUMBER_GENERATIONS; generational_recursion++)
		{
//			cout << "Generation: " << generational_recursion << endl;
//			hoponpop.breeder();
			hoponpop->breeder();
//			cout << "Generation over\n";

			if (enable_history)
			{
				indiFirst[generational_recursion] = hoponpop->First ();
				indiSecond[generational_recursion] = hoponpop->Second();
				indiThird[generational_recursion] = hoponpop->Third ();
			}
		}

		specimen_t bestever = hoponpop->BestEver();
		uint64_t aged = hoponpop->Age();
//		outfileTheBest << "NumGens,PopSize,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,Geno1,Geno2,Geno3,Fitness";
		outfileTheBest << "\n" << NUMBER_GENERATIONS << "," << pop_size << "," << xo_p << "," << xo_r << "," << mu_r << "," << elitism << ","
		<< rng_seed << "," << trailer_trash << "," << bestever.calced << "," << aged << ","
		<< bestever.gen.one << "," << bestever.gen.two << "," << bestever.gen.thr << "," << bestever.fit;

		if (enable_history)
		{
			outfileTheFirst << rng_seed << ","<< trailer_trash;
			outfileTheSecond << rng_seed << ","<< trailer_trash;
			outfileTheThird << rng_seed << ","<< trailer_trash;

			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
				outfileTheFirst << "," << indiFirst[iter].gen.one << "," << indiFirst[iter].gen.two << "," << indiFirst[iter].gen.thr;
				outfileTheSecond << "," << indiSecond[iter].gen.one << "," << indiFirst[iter].gen.two << "," << indiFirst[iter].gen.thr;
				outfileTheThird << "," << indiThird[iter].gen.one << "," << indiFirst[iter].gen.two << "," << indiFirst[iter].gen.thr;
			}
			outfileTheFirst << "\n" << rng_seed << "," << trailer_trash;
			outfileTheSecond << "\n" << rng_seed << "," << trailer_trash;
			outfileTheThird << "\n" << rng_seed << "," << trailer_trash;

			for (iter=0; iter<NUMBER_GENERATIONS; iter++)
			{
				outfileTheFirst << "," << indiFirst[iter].fit;
				outfileTheSecond << "," << indiSecond[iter].fit;
				outfileTheThird << "," << indiThird[iter].fit;
			}
			outfileTheFirst << "\n";
			outfileTheSecond << "\n";
			outfileTheThird << "\n";
		}

		delete hoponpop;
	}//End of Trial
	cout << endl;

	if (enable_history)
	{
		outfileTheFirst << "\n";
		outfileTheSecond << "\n";
		outfileTheThird << "\n";

		outfileTheFirst.flush();
		outfileTheSecond.flush();
		outfileTheThird.flush();
	}

	outfileTheBest << "\n";
	outfileTheBest.flush();

	cout << "\tProcessing time: " << ((double) getTickCount() - last_tick_count)/getTickFrequency() << "[s]\n";

	if (enable_history)
	{
		outfileTheFirst.close();
		outfileTheSecond.close();
		outfileTheThird.close();
	}
	outfileTheBest.close();

	return 0;
}

