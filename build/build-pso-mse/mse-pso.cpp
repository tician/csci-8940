#include "mse-pso.hpp"

typedef boost::mt19937 RNG_t;
RNG_t rudi_;

boost::uniform_real<> dist_nc(1, 42);		// must have at least 1
boost::uniform_real<> dist_len(1, 9);
boost::uniform_real<> dist_sen1(1, 168);
boost::uniform_real<> dist_sen2(1, 56);
boost::uniform_real<> dist_scc(1, 7);		// must have at least 1
boost::uniform_real<> dist_rau(1, 92);
boost::uniform_real<> dist_nai(1, 4);

boost::uniform_real<> dist_velo(MIN_VELO, MAX_VELO);


typedef struct genotype_t
{
	genotype_t(void) : nc(0), len(0), sen1(0), sen2(0), scc(0), rau(0), nai(0) {};
//	genotype_t(RNG_t* rudi) :	nc(dist_nc(*rudi)), len(dist_len(*rudi)),
//								sen1(dist_sen1(*rudi)), sen2(dist_sen2(*rudi)),
//								scc(dist_scc(*rudi)), rau(dist_rau(*rudi)),
//								nai(dist_nai(*rudi)) {};
	GENO_TYPE	nc, len, sen1, sen2, scc, rau, nai;
} genotype_t;

typedef struct particle_t
{
	particle_t(void)
	{
		pnc = 0.0; plen = 0.0; psen1 = 0.0; sen2 =  0.0;
		pscc = 0.0; rau = 0.0; pnai = 0.0;

		vnc = 0.0; vlen = 0.0; vsen1 = 0.0; vsen2 = 0.0;
		vscc = 0.0; vrau = 0.0; vnai = 0.0;
	};
	particle_t(bool randomized)
	{
		if (randomized)
		{
			pnc = dist_nc(rudi_);		plen = dist_len(rudi_);
			psen1 = dist_sen1(rudi_);	sen2 = dist_sen2(rudi_);
			pscc = dist_scc(rudi_);		rau = dist_rau(rudi_);
			pnai = dist_nai(rudi_);

			vnc = dist_velo(rudi_);		vlen = dist_velo(rudi_);
			vsen1 = dist_velo(rudi_);	vsen2 = dist_velo(rudi_);
			vscc = dist_velo(rudi_);	vrau = dist_velo(rudi_);
			vnai = dist_velo(rudi_);
		}
		else
		{
			pnc = 0.0; plen = 0.0; psen1 = 0.0; sen2 =  0.0;
			pscc = 0.0; rau = 0.0; pnai = 0.0;

			vnc = 0.0; vlen = 0.0; vsen1 = 0.0; vsen2 = 0.0;
			vscc = 0.0; vrau = 0.0; vnai = 0.0;
		}
	};
	double		pnc, plen, psen1, psen2, pscc, prau, pnai;
	double		vnc, vlen, vsen1, vsen2, vscc, vrau, vnai;
} particle_t;

typedef struct specimen_t
{
	specimen_t(void) : fit(HORRIFIC_FITNESS_VALUE), gen(), calced(0), cc(true), bf(HORRIFIC_FITNESS_VALUE), bc(false) {};

	FITNESS_TYPE	fit;
	genotype_t		gen;
	uint64_t		calced;
	particle_t		cc;

	FITNESS_TYPE	bf;
	particle_t		bc;
} specimen_t;

bool spec_comp (specimen_t, specimen_t);

bool spec_comp (specimen_t i, specimen_t j)
{
#ifdef MAXIMIZING_FITNESS_VALUE
	return (i.fit>j.fit);	// maximize fitness value
#else
	return (i.fit<j.fit);	// minimize fitness value
#endif
}


class population
{
private:
	specimen_t *pop_;
	specimen_t best_in_swarm_;

//	RNG_t *rudi_;
	uint64_t pop_size_;
	uint64_t fitness_calculation_counter_;
	uint64_t gen_limit_;

	uint64_t msrt_;
	uint64_t dnvt_;

	double cog_, soc_, inertia_;

	uint64_t generation_;
	void bestest(void);

//	GENO_TYPE discretize(particle_t);
	void discretize(specimen_t&);
	FITNESS_TYPE calcFitness(genotype_t);
	specimen_t populate(void);

public:
//	population(RNG *rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia, uint64_t gen_limit, uint64_t msrt, uint64_t dnvt);
	population(uint64_t pop_size, double inertia, double cog, double soc, uint64_t gen_limit, uint64_t msrt, uint64_t dnvt);
	~population(void);
	void populator(void);
	void iterate(void);

	specimen_t BestInSwarm(void){return best_in_swarm_;}

	uint64_t Age(void) {return generation_;}
};

//population::population(RNG *rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia, uint64_t gen_limit, uint64_t msrt, uint64_t dnvt)
population::population(uint64_t pop_size, double inertia, double cog, double soc, uint64_t gen_limit, uint64_t msrt, uint64_t dnvt)
{
//	rudi_ = rudi;
	pop_size_ = pop_size;
	fitness_calculation_counter_ = 0;
	gen_limit_ = gen_limit;

	best_in_swarm_.fit = HORRIFIC_FITNESS_VALUE;

	msrt_ = msrt;
	dnvt_ = dnvt;

	inertia_ = inertia;
	cog_ = cog;
	soc_ = soc;

	pop_ = new specimen_t[pop_size_];
}

population::~population(void)
{
	delete[] pop_;
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

	for (iter=0; iter<pop_size_; iter++)
	{
#ifdef MAXIMIZING_FITNESS_VALUE
		if (pop_[iter].fit > best_in_swarm_.fit)	// maximize fitness value
#else
		if (pop_[iter].fit < best_in_swarm_.fit)	// minimize fitness value
#endif
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

specimen_t population::populate(void)
{
	specimen_t indi;

//	uint64_t iter;

//	for (iter=0; iter<NUMBER_ATTRIBUTES; iter++)
//	{
//		indi.cc.vel[iter] = rudi_.uniform((double)min_.vel[iter],(double)max_.vel[iter]);
//		indi.cc.pos[iter] = rudi_.uniform((double)min_.pos[iter],(double)max_.pos[iter]);
//	}

//	indi.gen = discretize(indi.cc);
	discretize(indi);
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	indi.bc = indi.cc;
	indi.bf = indi.fit;
	return indi;
}

void population::iterate(void)
{
	// Check for lack of improvement in best particle
	if (fitness_calculation_counter_ > (best_in_swarm_.calced + (pop_size_ * gen_limit_)) )
		return;

	uint64_t iter, jter;

	for (iter=0; iter<pop_size_; iter++)
	{
//		pop_[iter].cc.vel[jter] =	(pop_[iter].cc.vel[jter] * inertia_)
//			+ ( (rudi_.uniform((double)0.0,(double)1.0) * cog_) * (pop_[iter].bc.pos[jter]     - pop_[iter].cc.pos[jter]) )
//			+ ( (rudi_.uniform((double)0.0,(double)1.0) * soc_) * (best_in_swarm_.cc.pos[jter] - pop_[iter].cc.pos[jter]) );

		pop_[iter].cc.vnc =
			(pop_[iter].cc.vnc * inertia_)
			+ ( (rudi_.uniform((double)-1.0,(double)1.0) * cog_) * (pop_[iter].bc.pnc     - pop_[iter].cc.pnc) )
			+ ( (rudi_.uniform((double)-1.0,(double)1.0) * soc_) * (best_in_swarm_.cc.pnc - pop_[iter].cc.pnc) );



//		pop_[iter].cc[jter].vel = (pop_[iter].cc[jter].vel * inertia_) + (pop_[iter].cc[jter].vel * cog_) + (best_in_swarm_[jter].vel * soc_);
		if (pop_[iter].cc.v > MAX_VELO)
			pop_[iter].cc.v = MAX_VELO;
		else if (pop_[iter].cc.v < MIN_VELO)
			pop_[iter].cc.v = MIN_VELO;

//		pop_[iter].cc.pos[jter] = pop_[iter].cc.pos[jter] + (pop_[iter].cc.vel[jter] * 1.0);//time__);
//		if (pop_[iter].cc.pos[jter] > max_.pos[jter])
//			pop_[iter].cc.pos[jter] = max_.pos[jter];
//		else if (pop_[iter].cc.pos[jter] < min_.pos[jter])
//			pop_[iter].cc.pos[jter] = min_.pos[jter];

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

FITNESS_TYPE population::calcFitness(genotype_t genie)
{
	FITNESS_TYPE temp = 0.0;

/// Cardinality Term (minimize number of components in shopping list)
	FITNESS_TYPE cardinality_term = 0.0;
	cardinality_term = (FITNESS_TYPE) 50.0 / (
		(
			(genie.nc/42.0) +
			(genie.len/9.0) +
			(genie.sen1/168.0) +
			(genie.sen2/56.0) +
			(genie.scc/7.0) +
			(genie.rau/92.0) +
			(genie.nai/4.0)
		) * 0.142857 );

/// Ratio of SEN1 to SEN2 (SEN2 are more expensive and less common)
	FITNESS_TYPE sen_ratio = 0.0;
	if (genie.sen1 > (3*genie.sen2))
		sen_ratio = (FITNESS_TYPE) (3.0*genie.sen2)/genie.sen1;
	else
		sen_ratio = (FITNESS_TYPE) genie.sen1/(3.0*genie.sen2);

/// Antenna Constraint Term
	FITNESS_TYPE uhf_connectivity = 1.0;

	uint64_t ant_other = (genie.len*2) + (genie.sen1*1) + (genie.sen2*1) + (genie.rau*1) + (genie.nai*1);
	uint64_t ant_total = genie.nc * 12;
	uint64_t x = ( (genie.nc-1) % 4 ) + 1;
	uint64_t y = (genie.nc-1) / 4;
	uint64_t z = y + 1;
	uint64_t ant_backbone = ( 32 * y ) - ( x * x ) + ( 13 * x );

	uint64_t ant_needed = ant_total - ant_backbone + ant_other;

	if ( (ant_total - ant_needed) > 12)
		uhf_connectivity = (FITNESS_TYPE) ant_needed / (FITNESS_TYPE) ant_total;

/// Mobile Subscriber Radiotelephone Terminal (supported by Radio via RAU)
	uint64_t msrt_supported = (genie.rau*25);
	FITNESS_TYPE msrt_ratio;
	if (msrt_supported < msrt_)
		msrt_ratio = 0.0;
	else
		msrt_ratio = (FITNESS_TYPE) msrt_ / (FITNESS_TYPE) msrt_supported;

/// Digital Nonsecure Voice Terminal (supported by Wire via NC, LEN, SEN1, SEN2)
	uint64_t dnvt_supported = (genie.nc*24) + (genie.len*176) + (genie.sen1*26) + (genie.sen2*41);
	FITNESS_TYPE dnvt_ratio;
	if (dnvt_supported < dnvt_)
		dnvt_ratio = 0.0;
	else
		dnvt_ratio = (FITNESS_TYPE) dnvt_ / (FITNESS_TYPE) dnvt_supported;

/// Minimum Constraints Term
	uint64_t min_term = 1.0;
	if (genie.nc == 0)
		min_term = 0.0;
	if (genie.scc == 0)
		min_term = 0.0;

/// Maximum Constraints Term
	uint64_t max_term = 1.0;
	if (genie.nc > (z*4))
		max_term = 0.0;
	if (genie.len > (z*1))
		max_term = 0.0;
	if (genie.sen1 > (z*12))
		max_term = 0.0;
	if (genie.sen2 > (z*4))
		max_term = 0.0;
	if (genie.scc > (z*1))
		max_term = 0.0;
	if (genie.rau > (z*9))
		max_term = 0.0;
	if (genie.nai > (z*1))
		max_term = 0.0;

/// Fitness
	temp = cardinality_term * sen_ratio * uhf_connectivity * msrt_ratio * dnvt_ratio * min_term * max_term;

	fitness_calculation_counter_++;

	return temp;
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
	par.cc.pnc =
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
	uint64_t msrt = 100;
	uint64_t dnvt = 100;

	double last_tick_count = 0.0;

	string out_filename = "";

	stringstream strstr (stringstream::in | stringstream::out);

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
			("msrt",	po::value<uint64_t>(),		"Set Number of MSRT")
			("dnvt",	po::value<uint64_t>(),		"Set Number of DNVT")
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

		if (vm.count("msrt"))
		{
			msrt = vm["msrt"].as<uint64_t>();
			cout << "Number of MSRT was set to " << msrt << ".\n";
		}
		else
		{
			cout << "Number of MSRT was set to default of " << msrt << ".\n";
		}

		if (vm.count("dnvt"))
		{
			dnvt = vm["dnvt"].as<uint64_t>();
			cout << "Number of DNVT was set to " << dnvt << ".\n";
		}
		else
		{
			cout << "Number of DNVT was set to default of " << dnvt << ".\n";
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



	if (out_filename.length() < 4)
	{
		strstr.clear();	strstr.str("");
		strstr << "./pso_mse_" << msrt << "_" << dnvt << ".csv";
		out_filename = strstr.str();
	}


	// Print data to file
	ofstream outfileTheBest;
	outfileTheBest.open( out_filename.c_str(), std::ofstream::out | std::ofstream::app );

	if ( !outfileTheBest.is_open() )
	{
		cerr << "Unable to open file: " << out_filename << "\n";
		return 1;
	}

	outfileTheBest << "NumGens,PopSize,Inertia,SOC,COG,RNG_Seed,Trial,FitEvals,EndGen,NC,LEN,SEN1,SEN2,SCC,RAU,NAI,Fitness";

	population *hoponpop;

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	last_tick_count = (double) (ts.tv_sec) + (double)(ts.tv_nsec/1000000000.0);

	uint64_t trailer_trash;
	for (trailer_trash=0; trailer_trash<num_trials; trailer_trash++)
	{
		clock_gettime(CLOCK_REALTIME, &ts);
		uint64_t rng_seed = (ts.tv_sec*1000000000) + ts.tv_nsec;

		rudi_.seed(rng_seed);

	// population(RNG& rudi, uint64_t pop_size, particle_t min_lim, particle_t max_lim, double cog, double soc, double inertia, uint64_t gen_limit);
//		hoponpop = new population(randi, pop_size, mini, maxi, cog_r, soc_r, inertia_r, gen_limit);
		hoponpop = new population(pop_size, inertia_r, cog_r, soc_r, gen_limit, msrt, dnvt);

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

