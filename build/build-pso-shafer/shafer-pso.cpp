#include "shafer-pso.hpp"

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
	specimen_t *best_;

	RNG rudi_;
	uint64_t pop_size_;
	specimen_t best_in_swarm_;
	uint64_t fitness_calculation_counter_;
	uint64_t gen_limit_;

	particle_t min_, max_;
	double cog_, soc_, inertia_;

	uint64_t generation_;
	void bestest(void);

//	GENO_TYPE discretize(particle_t);
	void discretize(specimen_t&);
	FITNESS_TYPE calcFitness(GENO_TYPE);
	specimen_t populate(void);

public:
	population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia, uint64_t gen_limit);
	~population(void);
	void populator(void);
	void iterate(void);

	specimen_t First (void){return best_[0];}
	specimen_t Second(void){return best_[1];}
	specimen_t Third (void){return best_[2];}

	specimen_t BestInSwarm(void){return best_in_swarm_;}

	uint64_t Age(void) {return generation_;}
};

population::population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia, uint64_t gen_limit)
{
	rudi_ = rudi;
	pop_size_ = pop_size;
	fitness_calculation_counter_ = 0;
	gen_limit_ = gen_limit;

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
/*
	std::vector<specimen_t> punk (pop_, pop_+pop_size_);
	std::sort (punk.begin(), punk.end(), spec_comp);


	std::vector<specimen_t>::iterator viter;
	viter = std::unique(punk.begin(), punk.end(), spec_uniq);
	punk.resize( std::distance(punk.begin(),viter) );

	viter = punk.begin();
*/

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
/*
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
*/

	uint64_t iter=0;
	best_[0].fit = HORRIFIC_FITNESS_VALUE;

	for (iter=0; iter<pop_size_; iter++)
	{
		if (pop_[iter].fit > best_[0].fit)
		{
			best_[0] = pop_[iter];
		}
		if (pop_[iter].fit > best_in_swarm_.fit)
		{
			best_in_swarm_ = pop_[iter];
		}
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

	// Check for lack of improvement in best particle (over 25 iterations, then stop processing)
	if (fitness_calculation_counter_ > (best_in_swarm_.calced + (pop_size_ * gen_limit_)) )
		return;

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

/*
			// discretization
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
	FITNESS_TYPE temp = 0;

	FITNESS_TYPE x=0, y=0;
	GENO_TYPE tfit ((1<<22)-1);

	tfit = tfit&genie;
	x = (tfit.to_ulong() * 0.00004768372718899898) - 100.0;
	tfit = genie>>22;
	y = (tfit.to_ulong() * 0.00004768372718899898) - 100.0;


#ifdef MAXIMIZING_FITNESS_VALUE
	temp = 0.5 + (  ( sin(sqrt(x*x+y*y))*sin(sqrt(x*x+y*y)) - 0.5 ) / ( (1.0 + 0.001*(x*x + y*y)) * (1.0 + 0.001*(x*x + y*y)) )  );
#else
	temp = HORRIFIC_FITNESS_VALUE;
#endif

	fitness_calculation_counter_++;

	return temp;
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

//	indi.gen = discretize(indi.cc);
	discretize(indi);
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
	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
//		if (par.pos[iter] > 0.5)
//		if ( rudi_.uniform((double)0.0,(double)1.0) < (double)( 1.0/ (1.0+exp(par.cc.vel[jter] * -1.0)) ) )
			genie[iter] = 1;
		else
			genie[iter] = 0;
	}
	return genie;
}
*/
void population::discretize(specimen_t& par)
{
	GENO_TYPE genie;
	uint64_t iter;
	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
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
	}
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
	uint64_t num_gens = NUMBER_GENERATIONS;
	uint64_t gen_limit = NUMBER_GENERATIONS/10;

	double last_tick_count = 0.0;

	string outname = "./outfile_shafer-pso_best.csv";

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
			("gens",	po::value<uint64_t>(),		"Set Number of Generations")
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

		if (vm.count("gens"))
		{
			num_gens = vm["gens"].as<uint64_t>();
			gen_limit = num_gens/10;
			if (gen_limit < 50)
				gen_limit = 50;
			cout << "Number of Generations was set to " << num_gens << ".\n";
		}
		else
		{
			cout << "Number of Trials was set to default of " << num_gens << ".\n";
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


//	RNG randi (rng_seed);

	uint64_t iter;//, jter;
	particle_t mini, maxi;

	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
	{
		mini.pos[iter] = 0.0;
		maxi.pos[iter] = 1.0;

		mini.vel[iter] = -4.0;
		maxi.vel[iter] = 4.0;
	}


	// Print data to file
//	stringstream strstr (stringstream::in | stringstream::out);

	ofstream outfileTheBest;
	outfileTheBest.open( outname.c_str(), std::ofstream::out | std::ofstream::app );

	if ( !outfileTheBest.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}
/*
	ofstream outfileBestGenLog;
	outfileBestGenLog.open( "./outfile_shafer-pso_gen_log.csv", std::ofstream::out | std::ofstream::app );

	if ( !outfileBestGenLog.is_open() )
	{
		cerr << "Unable to open file: " << "./outfile_shafer-pso_gen_log.csv" << "\n";
		return 1;
	}

	ofstream outfileBestFitLog;
	outfileBestFitLog.open( "./outfile_shafer-pso_fit_log.csv", std::ofstream::out | std::ofstream::app );

	if ( !outfileBestFitLog.is_open() )
	{
		cerr << "Unable to open file: " << "./outfile_shafer-pso_fit_log.csv" << "\n";
		return 1;
	}
*/

	uint64_t best_default_precision = outfileTheBest.precision();
//	outfileTheBest.precision(35);

	outfileTheBest << "NumGens,PopSize,RNG_Seed,Trial,FitEvals,EndGen,Genotype,Fitness";

	population *hoponpop;

	last_tick_count = (double) getTickCount();

	uint64_t trailer_trash;
	for (trailer_trash=0; trailer_trash<num_trials; trailer_trash++)
	{
		uint64_t rng_seed = getTickCount();
		RNG randi (rng_seed);

	// population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia, uint64_t gen_limit);
		hoponpop = new population(randi, pop_size, mini, maxi, cog_r, soc_r, inertia_r, gen_limit);

//		cout << "Trial: " << trailer_trash << endl;
		if (trailer_trash==0)
			cout << "Trial: ";
		cout << trailer_trash << " ";
		cout.flush();

		hoponpop->populator();
//		outfileBestFitLog << "\n" << rng_seed;
//		outfileBestGenLog << "\n" << rng_seed;

		uint64_t generational_recursion;
		for (generational_recursion=0; generational_recursion<num_gens; generational_recursion++)
		{
			hoponpop->iterate();

//			specimen_t toad = hoponpop->First();
//			outfileBestGenLog << "," << toad.gen;
//			outfileBestFitLog << "," << toad.fit;
		}

		specimen_t bestinswarm = hoponpop->BestInSwarm();
		uint64_t aged = hoponpop->Age();
//	outfileTheBest << "NumGens,PopSize,RNG_Seed,Trial,FitEvals,EndGen,Genotype,Fitness";
		outfileTheBest.precision(best_default_precision);
		outfileTheBest << "\n" << num_gens << "," << pop_size << "," << rng_seed << "," << trailer_trash << "," << bestinswarm.calced << "," << aged << ",";
		outfileTheBest.precision(35);
		outfileTheBest << bestinswarm.gen << "," << bestinswarm.fit;


//		outfileBestGenLog << "\n";
//		outfileBestGenLog.flush();
//		outfileBestFitLog << "\n";
//		outfileBestFitLog.flush();
		outfileTheBest.flush();

		delete hoponpop;
	}//End of Trial
	cout << endl;

//	outfileBestGenLog << "\n";
//	outfileBestGenLog.flush();
//	outfileBestFitLog << "\n";
//	outfileBestFitLog.flush();

	outfileTheBest << "\n";
	outfileTheBest.flush();

	cout << "\tProcessing time: " << ((double) getTickCount() - last_tick_count)/getTickFrequency() << "[s]\n";

//	outfileBestGenLog.close();
//	outfileBestFitLog.close();
	outfileTheBest.close();

	return 0;
}

