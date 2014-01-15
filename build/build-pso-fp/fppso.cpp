
#include "fppso.hpp"

typedef struct genotype_t
{
	genotype_t(void) : one(0), two(0), thr(0) {};

	GENO_TYPE one, two, thr;

	void clear(void) { one = 0; two = 0; thr = 0; };
};

typedef struct particle_t
{
	particle_t(void) {
		uint64_t iter;
		for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
		{
			pos[iter] = 0.0;
			vel[iter] = 0.0;
		}
	};

	double pos[NUMBER_DIMENSIONS];
	double vel[NUMBER_DIMENSIONS];
};

typedef struct specimen_t
{
	specimen_t(void) : fit(0.0), calced(0), bf(0.0) {};

	FITNESS_TYPE	fit;		// Current Fitness
	genotype_t		gen;		// Genotype
	uint64_t		calced;		// Fitness Evaluation Number
	particle_t		cc;			// Current C?

	FITNESS_TYPE	bf;			// Best Fitness
	particle_t		bc;			// Best C?
};

bool spec_comp (specimen_t i, specimen_t j) { return (i.fit>j.fit); }
bool spec_uniq (specimen_t i, specimen_t j)
{
	if (i.gen.one != j.gen.one)
		return false;
	if (i.gen.two != j.gen.two)
		return false;
	if (i.gen.thr != j.gen.thr)
		return false;

	return true;
}


class population
{
private:
	specimen_t *pop_;
	specimen_t *best_;

	RNG rudi_;
	uint64_t pop_size_;
	specimen_t best_in_swarm_;
	uint64_t fitness_calculation_counter_;

	particle_t min_, max_;
	double cog_, soc_, inertia_;

	uint64_t generation_;
	void bestest(void);

//	GENO_TYPE discretize(particle_t);
	void discretize(specimen_t&);
	FITNESS_TYPE calcFitness(genotype_t);
	specimen_t populate(void);

public:
	population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia);
	~population(void);
	void populator(void);
	void iterate(void);
	void fixer(specimen_t&);

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
	best_ = new specimen_t[NUMBER_TRACKING];
}

population::~population(void)
{
	delete[] pop_;
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
		best_[jter].gen.clear();
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
	// Check for lack of improvement in best particle (over 25 iterations, then stop processing)
	if (fitness_calculation_counter_ > (best_in_swarm_.calced + (pop_size_ * 25)) )
		return;

	uint64_t iter, jter;

	for (iter=0; iter<pop_size_; iter++)
	{
		for (jter=0; jter<NUMBER_DIMENSIONS; jter++)
		{
			// Update velocity
			pop_[iter].cc.vel[jter] =	(pop_[iter].cc.vel[jter] * inertia_)
				+ ( (rudi_.uniform((double)0.0,(double)1.0) * cog_) * (pop_[iter].bc.pos[jter]     - pop_[iter].cc.pos[jter]) )
				+ ( (rudi_.uniform((double)0.0,(double)1.0) * soc_) * (best_in_swarm_.cc.pos[jter] - pop_[iter].cc.pos[jter]) );

			if (pop_[iter].cc.vel[jter] > max_.vel[jter])
				pop_[iter].cc.vel[jter] = max_.vel[jter];
			else if (pop_[iter].cc.vel[jter] < min_.vel[jter])
				pop_[iter].cc.vel[jter] = min_.vel[jter];

			// Update position
			pop_[iter].cc.pos[jter] += pop_[iter].cc.vel[jter];

			if (pop_[iter].cc.pos[jter] > max_.pos[jter])
				pop_[iter].cc.pos[jter] = max_.pos[jter];
			else if (pop_[iter].cc.pos[jter] < min_.pos[jter])
				pop_[iter].cc.pos[jter] = min_.pos[jter];

/*
			// discretize
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
*/
		}
		discretize(pop_[iter]);
		fixer(pop_[iter]);
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

FITNESS_TYPE population::calcFitness(genotype_t genie)
{
	FITNESS_TYPE temp = 0;
	uint64_t iter;

	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
		if (genie.one[iter]==1)
			temp += West73_Yields[iter].Y1;
		else if (genie.two[iter]==1)
			temp += West73_Yields[iter].Y2;
		else if (genie.thr[iter]==1)
			temp += West73_Yields[iter].Y3;
	}

	fitness_calculation_counter_++;

	return temp;
}

specimen_t population::populate(void)
{
	specimen_t indi;
	uint64_t iter;

	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
		indi.cc.vel[iter] = rudi_.uniform((double)min_.vel[iter],(double)max_.vel[iter]);
		indi.cc.pos[iter] = rudi_.uniform((double)min_.pos[iter],(double)max_.pos[iter]);
	}

//	indi.gen = discretize(indi.cc);
	discretize(indi);
	fixer(indi);
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	indi.bc = indi.cc;
	indi.bf = indi.fit;
	return indi;
}
/*
GENO_TYPE population::discretize(particle_t par)
{
	GENO_TYPE genie;
	uint64_t iter;
	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
		if (par.pos[iter] > 0.5)
			genie[iter] = 1;
		else
			genie[iter] = 0;
	}
	return genie;
}
*/
void population::discretize(specimen_t& par)
{
	uint64_t iter;
	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
/*
		if ( rudi_.uniform((double)0.0,(double)1.0) < (double)( 1.0/ (1.0+exp(par.cc.vel[iter] * -1.0)) ) )
		{
			par.cc.pos[iter] = 1.0;
			par.gen[iter] = 1;
		}
		else
		{
			par.cc.pos[iter] = 0.0;
			par.gen[iter] = 0;
		}
*/
		if ( floor(par.cc.pos[iter]) == 0 )
		{
			par.gen.one[iter] = 0;
			par.gen.two[iter] = 0;
			par.gen.thr[iter] = 0;
		}
		else if ( floor(par.cc.pos[iter]) == 1 )
		{
			par.gen.one[iter] = 1;
			par.gen.two[iter] = 0;
			par.gen.thr[iter] = 0;
		}
		else if ( floor(par.cc.pos[iter]) == 2 )
		{
			par.gen.one[iter] = 0;
			par.gen.two[iter] = 1;
			par.gen.thr[iter] = 0;
		}
		else if ( floor(par.cc.pos[iter]) == 3 )
		{
			par.gen.one[iter] = 0;
			par.gen.two[iter] = 0;
			par.gen.thr[iter] = 1;
		}
	}
}

void population::fixer(specimen_t& indi)
{
	uint64_t iter, jter;

	// Randomize list of indices to decrease bias
	Mat lister(1, NUMBER_DIMENSIONS, CV_8UC1);
	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
		lister.at<uint8_t>(iter) = iter;
	}
	randShuffle(lister, 1, &rudi_);

	GENO_TYPE checker;
	// Fix for adjacency
	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
		if (indi.gen.one[lister.at<uint8_t>(iter)]==1)
		{
			// Fix particle
			checker = West73_Adjacency[lister.at<uint8_t>(iter)] & indi.gen.one;
			if (checker.any())
			{
				for (jter=0; jter<NUMBER_DIMENSIONS; jter++)
				{
					if (checker[iter]==1)
						indi.cc.pos[iter] = rudi_.uniform(0.0,1.0);
				}
			}

			// Fix genotype
			indi.gen.one &= West73_NotAdjacency[lister.at<uint8_t>(iter)];
		}
		else if (indi.gen.two[lister.at<uint8_t>(iter)]==1)
		{
			// Fix particle
			checker = West73_Adjacency[lister.at<uint8_t>(iter)] & indi.gen.two;
			if (checker.any())
			{
				for (jter=0; jter<NUMBER_DIMENSIONS; jter++)
				{
					if (checker[iter]==1)
						indi.cc.pos[iter] = rudi_.uniform(0.0,1.0);
				}
			}

			// Fix genotype
			indi.gen.two &= West73_NotAdjacency[lister.at<uint8_t>(iter)];
		}
		else if (indi.gen.thr[lister.at<uint8_t>(iter)]==1)
		{
			// Fix particle
			checker = West73_Adjacency[lister.at<uint8_t>(iter)] & indi.gen.thr;
			if (checker.any())
			{
				for (jter=0; jter<NUMBER_DIMENSIONS; jter++)
				{
					if (checker[iter]==1)
						indi.cc.pos[iter] = rudi_.uniform(0.0,1.0);
				}
			}

			// Fix genotype
			indi.gen.thr &= West73_NotAdjacency[lister.at<uint8_t>(iter)];
		}
	}

/*
	// Fix for repeated harvesting
	uint64_t temp = rudi_.uniform(1,4);
	if (temp==1)
	{
		indi.gen.two &= ~indi.gen.one;
		indi.gen.thr &= ~indi.gen.one;

		temp = rudi_.uniform(0,2);
		if (temp==0)
			indi.gen.thr &= ~indi.gen.two;
		else
			indi.gen.two &= ~indi.gen.thr;
	}
	else if (temp==2)
	{
		indi.gen.one &= ~indi.gen.two;
		indi.gen.thr &= ~indi.gen.two;

		temp = rudi_.uniform(0,2);
		if (temp==0)
			indi.gen.thr &= ~indi.gen.one;
		else
			indi.gen.one &= ~indi.gen.thr;
	}
	else if (temp==3)
	{
		indi.gen.one &= ~indi.gen.thr;
		indi.gen.two &= ~indi.gen.thr;

		temp = rudi_.uniform(0,2);
		if (temp==0)
			indi.gen.two &= ~indi.gen.one;
		else
			indi.gen.one &= ~indi.gen.two;
	}
*/
}




int main(int argc, char* argv[])
{
	uint64_t pop_size = 80;
	double inertia_r = 0.1;
	double cog_r = 0.3;
	double soc_r  = 0.3;
	uint64_t num_trials = NUMBER_TRIALS;
	bool enable_history = false;

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
			("history",	po::value<bool>(),			"Enable full history")
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

		if (vm.count("history"))
		{
			enable_history = vm["history"].as<bool>();
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
	particle_t mini, maxi;

	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
		West73_Adjacency[iter].reset();
		West73_NotAdjacency[iter].set();
		West73_Yields[iter].Y1 = West73_Yields[iter].y1 * West73_Yields[iter].area;
		West73_Yields[iter].Y2 = West73_Yields[iter].y2 * West73_Yields[iter].area;
		West73_Yields[iter].Y3 = West73_Yields[iter].y3 * West73_Yields[iter].area;
	}
	for (iter=0; iter<196; iter++)
	{
		West73_Adjacency[West73_Adjacency_builder[iter].id-1].set(West73_Adjacency_builder[iter].ad-1);
		West73_NotAdjacency[West73_Adjacency_builder[iter].id-1].reset(West73_Adjacency_builder[iter].ad-1);
	}

	for (iter=0; iter<NUMBER_DIMENSIONS; iter++)
	{
		mini.pos[iter] = 0.0;
		maxi.pos[iter] = 4.0;

		mini.vel[iter] = -4.0;
		maxi.vel[iter] = 4.0;
	}


	// Print data to file
	stringstream strstr (stringstream::in | stringstream::out);
	ofstream outfileTheFirst;
	ofstream outfileTheSecond;
	ofstream outfileTheThird;
	string outname;

	if (enable_history)
	{
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
			<< "_" << inertia_r
			<< "_" << cog_r
			<< "_" << soc_r
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
			<< "_" << inertia_r
			<< "_" << cog_r
			<< "_" << soc_r
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

//	outfileTheBest.precision(35);
	uint64_t best_default_precision = outfileTheBest.precision();

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

	outfileTheBest << "NumGens,PopSize,Inertia,SOC,COG,RNG_Seed,Trial,FitEvals,EndGen,Geno1,Geno2,Geno3,Fitness";

	population *hoponpop;

	last_tick_count = (double) getTickCount();

	uint64_t trailer_trash;
	for (trailer_trash=0; trailer_trash<num_trials; trailer_trash++)
	{
		uint64_t rng_seed = getTickCount();
		RNG randi (rng_seed);

	// population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia);
		hoponpop = new population(randi, pop_size, mini, maxi, cog_r, soc_r, inertia_r);

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
//				cout << "Generation: " << generational_recursion << endl;
//				hoponpop.iterate();
			hoponpop->iterate();
//				cout << "Generation over\n";

			if (enable_history)
			{
				indiFirst[generational_recursion] = hoponpop->First ();
				indiSecond[generational_recursion] = hoponpop->Second();
				indiThird[generational_recursion] = hoponpop->Third ();
			}
		}

		specimen_t bestinswarm = hoponpop->BestInSwarm();
		uint64_t aged = hoponpop->Age();
//		outfileTheBest << "NumGens,PopSize,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,Geno1,Geno2,Geno3,Fitness";
		outfileTheBest.precision(best_default_precision);
		outfileTheBest << "\n" << NUMBER_GENERATIONS << "," << pop_size << "," << inertia_r << "," << soc_r << "," << cog_r << ",";
		outfileTheBest << rng_seed << "," << trailer_trash << "," << bestinswarm.calced << "," << aged << ",";
		outfileTheBest.precision(35);
		outfileTheBest << bestinswarm.gen.one << "," << bestinswarm.gen.two << "," << bestinswarm.gen.thr << "," << bestinswarm.fit;

		if (enable_history)
		{
			outfileTheFirst << rng_seed << "," << trailer_trash;
			outfileTheSecond << rng_seed << ","<< trailer_trash;
			outfileTheThird << rng_seed << "," << trailer_trash;

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

