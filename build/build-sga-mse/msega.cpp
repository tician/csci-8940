
#include "msega.hpp"

typedef boost::mt19937 RNG_t;

typedef struct genotype_t
{
	genotype_t(void) : nc(0), len(0), sen1(0), sen2(0), scc(0) , rau(0), nai(0) {};

	GENO_TYPE		nc, len, sen1, sen2, scc, rau, nai;
} genotype_t;

typedef struct specimen_t
{
	specimen_t(void) : fit(HORRIFIC_FITNESS_VALUE), gen(), calced(0) {};

	FITNESS_TYPE	fit;
	genotype_t		gen;
	uint64_t		calced;
} specimen_t;


bool spec_comp (specimen_t, specimen_t);
bool spec_uniq (specimen_t, specimen_t);

bool spec_comp (specimen_t i, specimen_t j)
{
#ifdef MAXIMIZING_FITNESS_VALUE
	return (i.fit>j.fit);	// maximize fitness value
#else
	return (i.fit<j.fit);	// minimize fitness value
#endif
}
bool spec_uniq (specimen_t i, specimen_t j)
{
	if (i.gen.nc!=j.gen.nc)
		return false;
	if (i.gen.len!=j.gen.len)
		return false;
	if (i.gen.sen1!=j.gen.sen1)
		return false;
	if (i.gen.sen2!=j.gen.sen2)
		return false;
	if (i.gen.scc!=j.gen.scc)
		return false;
	if (i.gen.rau!=j.gen.rau)
		return false;
	if (i.gen.nai!=j.gen.nai)
		return false;
	return true;
}


class population
{
private:
	specimen_t *pop_;
	genotype_t *kiddies_;
	specimen_t best_ever_;

	FITNESS_TYPE *sig_fit_;

	uint64_t fitness_calculation_counter_;

	RNG_t *rudi_;
	boost::uniform_int<> dist_nc;//(1, 42);		// must have at least 1
	boost::uniform_int<> dist_len;//(1, 9);
	boost::uniform_int<> dist_sen1;//(1, 168);
	boost::uniform_int<> dist_sen2;//(1, 56);
	boost::uniform_int<> dist_scc;//(1, 7);		// must have at least 1
	boost::uniform_int<> dist_rau;//(1, 92);
	boost::uniform_int<> dist_nai;//(1, 4);

	uint64_t pop_size_;
	double mu_r_;
	double xo_r_;
	uint64_t xo_p_;
	uint64_t tree_s_;
	bool elitism_;
	uint64_t gen_limit_;

	uint64_t msrt_;
	uint64_t dnvt_;

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
	population(RNG_t *rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, uint64_t tree_s, bool elitism, uint64_t gen_limit, uint64_t msrt, uint64_t dnvt);
	~population(void);
	void populator(void);
	void breeder(void);

	specimen_t BestEver(void){return best_ever_;}

	uint64_t Age(void) {return generation_;}
};

population::population(RNG_t *rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, uint64_t tree_s, bool elitism, uint64_t gen_limit, uint64_t msrt, uint64_t dnvt)
: dist_nc(1, 42)
, dist_len(1, 9)
, dist_sen1(1, 168)
, dist_sen2(1, 56)
, dist_scc(1, 7)
, dist_rau(1, 92)
, dist_nai(1, 4)
{

	rudi_ = rudi;
	pop_size_ = pop_size;
	fitness_calculation_counter_ = 0;
	mu_r_ = mu_r;
	xo_r_ = xo_r;
	xo_p_ = xo_p;
	tree_s_ = tree_s;
	elitism_ = elitism;
	gen_limit_ = gen_limit;

	msrt_ = msrt;
	dnvt_ = dnvt;

	pop_ = new specimen_t[pop_size_];
	kiddies_ = new genotype_t[pop_size_];
	if (tree_s_==0)
		sig_fit_ = new FITNESS_TYPE[pop_size_];

}

population::~population(void)
{
	delete[] pop_;
	delete[] kiddies_;
	if (tree_s_==0)
		delete[] sig_fit_;
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
		best_[jter++].fit = HORRIFIC_FITNESS_VALUE;
	}


//	if (best_[0].fit > best_ever_.fit)	// maximize fitness value
	if (best_[0].fit < best_ever_.fit)	// minimize fitness value
	{
		best_ever_ = best_[0];
	}
*/
	uint64_t iter;
	for (iter=0; iter<pop_size_; iter++)
	{

#ifdef MAXIMIZING_FITNESS_VALUE
		if (pop_[iter].fit > best_ever_.fit)	// maximize fitness value
#else
		if (pop_[iter].fit < best_ever_.fit)	// minimize fitness value
#endif
		{
			best_ever_ = pop_[iter];
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

	indi.gen.nc = dist_nc(*rudi_);
	indi.gen.len = dist_len(*rudi_);
	indi.gen.sen1 = dist_sen1(*rudi_);
	indi.gen.sen2 = dist_sen2(*rudi_);
	indi.gen.scc = dist_scc(*rudi_);
	indi.gen.rau = dist_rau(*rudi_);
	indi.gen.nai = dist_nai(*rudi_);

//	fixer(indi);
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	return indi;
}

void population::breeder(void)
{
	// Check for lack of improvement in best individual (over 25 generations without improvement, then stop processing)
	if (fitness_calculation_counter_ > (best_ever_.calced + (pop_size_ * gen_limit_)) )
		return;

	if (tree_s_==0)
		roulette();

	uint64_t iter;

	if (elitism_)
	{
		for (iter=0; iter<(pop_size_/2)-2; iter++)
		{
			genotype_t mama, papa;

			selector(mama);
			selector(papa);
			splicer(mama, papa);
			kiddies_[(2*iter)+0] = mama;
			kiddies_[(2*iter)+1] = papa;
		}
		kiddies_[(2*iter)+0] = best_ever_.gen;
		kiddies_[(2*iter)+1] = best_ever_.gen;
	}
	else
	{
		for (iter=0; iter<(pop_size_/2); iter++)
		{
			genotype_t mama, papa;

			selector(mama);
			selector(papa);
			splicer(mama, papa);
			kiddies_[(2*iter)+0] = mama;
			kiddies_[(2*iter)+1] = papa;
		}
	}
	for (iter=0; iter<pop_size_; iter++)
	{
		pop_[iter].gen = kiddies_[iter];
	}
	mutator();
	generation_++;
	bestest();
}


void population::roulette(void)
{
	// Roulette Wheel Selection
	uint64_t iter;

#ifdef MAXIMIZING_FITNESS_VALUE
	sig_fit_[0] = pop_[0].fit;
	for (iter=1; iter<pop_size_; iter++)
	{
		sig_fit_[iter] = sig_fit_[iter-1] + pop_[iter].fit;
	}
#else
	sig_fit_[0] = (HORRIFIC_FITNESS_VALUE - pop_[0].fit);
	for (iter=1; iter<pop_size_; iter++)
	{
		sig_fit_[iter] = sig_fit_[iter-1] + (HORRIFIC_FITNESS_VALUE - pop_[iter].fit);
	}
#endif
}


void population::selector(genotype_t& nana)
{
	if (tree_s_ == 0)
	{
		/// Roulette Wheel Selection
		uint64_t iter;
//		FITNESS_TYPE temp = rudi_.uniform((FITNESS_TYPE) 0.0, (FITNESS_TYPE)sig_fit_[pop_size_-1]);
		boost::uniform_real<> dist_roulette((FITNESS_TYPE) 0.0, (FITNESS_TYPE)sig_fit_[pop_size_-1]);
		FITNESS_TYPE temp = dist_roulette(*rudi_);

		for (iter=0; iter<pop_size_; iter++)
		{
			if (temp <= sig_fit_[iter])
			{
				nana = pop_[iter].gen;
				break;
			}
		}
	}
	else
	{
		/// Tournament Selection
		uint64_t iter, indi;
		FITNESS_TYPE temp = HORRIFIC_FITNESS_VALUE;
		boost::uniform_int<> dist_tournament(0, pop_size_-1);
		for (iter=0; iter<tree_s_; iter++)
		{
//			indi = rudi_.uniform( 0, pop_size_ );
			indi = dist_tournament(*rudi_);
#ifdef MAXIMIZING_FITNESS_VALUE
			if (pop_[indi].fit > temp)
#else
			if (pop_[indi].fit < temp)
#endif
				nana = pop_[indi].gen;
		}
	}
}

void population::splicer(genotype_t& mama, genotype_t& papa)
{
	// Crossover loci (0 ~ 6)

	genotype_t ba = mama, by = papa;

	boost::uniform_real<> dist_xovr(0,(1/xo_r_));
	boost::uniform_int<> dist_loci(0,NUMBER_GENES-1);

	uint64_t iter;
	for (iter=0; iter<xo_p_; iter++)
	{
//		if (rudi_.uniform( 0, (int)(1/xo_r_)) < 1)
		if (dist_xovr(*rudi_) < 1.0)
		{
//			locus[iter] = rudi_.uniform(0, NUMBER_GENES);
			uint64_t locus = dist_loci(*rudi_);
			if (locus==0)
			{
				ba.nc = papa.nc;
				by.nc = mama.nc;
			}
			else if (locus==1)
			{
				ba.len = papa.len;
				by.len = mama.len;
			}
			else if (locus==2)
			{
				ba.sen1 = papa.sen1;
				by.sen1 = mama.sen1;
			}
			else if (locus==3)
			{
				ba.sen2 = papa.sen2;
				by.sen2 = mama.sen2;
			}
			else if (locus==4)
			{
				ba.scc = papa.scc;
				by.scc = mama.scc;
			}
			else if (locus==5)
			{
				ba.rau = papa.rau;
				by.rau = mama.rau;
			}
			else if (locus==6)
			{
				ba.nai = papa.nai;
				by.nai = mama.nai;
			}
		}
	}

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

specimen_t population::mutate(specimen_t indi)
{
	boost::uniform_real<> dist_mutate(0, (1/mu_r_));
	if ( dist_mutate(*rudi_) < 1)
	{
		indi.gen.nc = dist_nc(*rudi_);
	}
	if ( dist_mutate(*rudi_) < 1)
	{
		indi.gen.len = dist_len(*rudi_);
	}
	if ( dist_mutate(*rudi_) < 1)
	{
		indi.gen.sen1 = dist_sen1(*rudi_);
	}
	if ( dist_mutate(*rudi_) < 1)
	{
		indi.gen.sen2 = dist_sen2(*rudi_);
	}
	if ( dist_mutate(*rudi_) < 1)
	{
		indi.gen.scc = dist_scc(*rudi_);
	}
	if ( dist_mutate(*rudi_) < 1)
	{
		indi.gen.rau = dist_rau(*rudi_);
	}
	if ( dist_mutate(*rudi_) < 1)
	{
		indi.gen.nai = dist_nai(*rudi_);
	}


//	fixer(indi);
	indi.fit = calcFitness(indi.gen);
	indi.calced = fitness_calculation_counter_;
	return indi;
}

void population::fixer(specimen_t& indi)
{

}

FITNESS_TYPE population::calcFitness(genotype_t genie)
{
	FITNESS_TYPE temp = 0.0;

/// Cardinality Term (minimize number of components in shopping list)
	FITNESS_TYPE cardinality_term = 0.0;
	cardinality_term = 50 / (
		(
			(genie.nc/42) +
			(genie.len/9) +
			(genie.sen1/168) +
			(genie.sen2/56) +
			(genie.scc/7) +
			(genie.rau/92) +
			(genie.nai/4)
		) * 0.142857 );

/// Ratio of SEN1 to SEN2 (SEN2 are more expensive and less common)
	FITNESS_TYPE sen_ratio = 0.0;
	if (genie.sen1 > (3*genie.sen2))
		sen_ratio = (3*genie.sen2)/genie.sen1;
	else
		sen_ratio = genie.sen1/(3*genie.sen2);

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
		uhf_connectivity = (ant_needed/ant_total);

/// Mobile Subscriber Radiotelephone Terminal (supported by Radio via RAU)
	uint64_t msrt_supported = (genie.rau*25);
	FITNESS_TYPE msrt_ratio;
	if (msrt_supported < msrt_)
		msrt_ratio = 0.0;
	else
		msrt_ratio = msrt_ / msrt_supported;

/// Digital Nonsecure Voice Terminal (supported by Wire via NC, LEN, SEN1, SEN2)
	uint64_t dnvt_supported = (genie.nc*24) + (genie.len*176) + (genie.sen1*26) + (genie.sen2*41);
	FITNESS_TYPE dnvt_ratio;
	if (dnvt_supported < dnvt_)
		dnvt_ratio = 0.0;
	else
		dnvt_ratio = dnvt_ / dnvt_supported;

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
	temp = cardinality_term * sen_ratio * uhf_connectivity * msrt_ratio * dnvt_ratio * max_term;

	fitness_calculation_counter_++;

	return temp;
}


int main(int argc, char* argv[])
{
	uint64_t pop_size = 80;
	double mu_r = 0.001;
	double xo_r = 0.4;
	uint64_t xo_p = 1;
	uint64_t tree_s = 0;
	bool elitism = false;
	uint64_t num_trials = NUMBER_TRIALS;
	uint64_t num_gens = NUMBER_GENERATIONS;
	uint64_t gen_limit = NUMBER_GENERATIONS/10;
	uint64_t msrt = 100;
	uint64_t dnvt = 100;

	string	out_filename = "";

	double last_tick_count = 0.0;

	stringstream strstr (stringstream::in | stringstream::out);

    try
    {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help",								"Produce help message")
			("ps",		po::value<uint64_t>(),		"Set Population Size")
			("mr",		po::value<double>(),		"Set Mutation Rate")
			("xr",		po::value<double>(),		"Set Crossover Rate")
			("xp",		po::value<uint64_t>(),		"Set Maximum Number of Crossover Points")
			("tree",	po::value<uint64_t>(),		"Set Size of Tournament Tree")
			("el",		po::value<bool>(),			"Enable Elitism")
			("trials",	po::value<uint64_t>(),		"Set Number of Trials")
			("gens",	po::value<uint64_t>(),		"Set Number of Generations")
			("msrt",	po::value<uint64_t>(),		"Set Number of MSRT")
			("dnvt",	po::value<uint64_t>(),		"Set Number of DNVT")
			("out",		po::value<string>(),		"Set output filename")
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

		if (vm.count("tree"))
		{
			tree_s = vm["tree"].as<uint64_t>();
			cout << "Tournament tree size was set to " << tree_s << ".\n";
		}
		else
		{
			cout << "Tournament tree size was set to default of " << tree_s << ".\n";
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
			cout << "Number of Generations was set to default of " << num_gens << ".\n";
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

		if (vm.count("el"))
		{
			elitism = vm["el"].as<bool>();
			cout << "Elitism was set to " << elitism << ".\n";
		}
		else
		{
			cout << "Elitism was set to default of " << elitism << ".\n";
		}

		if (vm.count("out"))
		{
			out_filename = vm["out"].as<string>();
			cout << "Output filename was set to: " << out_filename << ".\n";
		}
		else
		{
			cout << "Output filename was set to default of: " << out_filename << ".\n";
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
		strstr << "./sga_mse_" << msrt << "_" << dnvt << ".csv";
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

//	outfileTheBest.precision(35);
//	uint64_t best_default_precision = outfileTheBest.precision();

	outfileTheBest << "NumGens,PopSize,Tree_S,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,NC,LEN,SEN1,SEN2,SCC,RAU,NAI,Fitness";

	population *hoponpop;


//	last_tick_count = (double) getTickCount();
	last_tick_count = (double) std::time(0);

	uint64_t trailer_trash;
	for (trailer_trash=0; trailer_trash<num_trials; trailer_trash++)
	{
//		uint64_t rng_seed = getTickCount();
		uint64_t rng_seed = std::time(0);

		RNG_t randi (rng_seed);

	// population(RNG_t& rudi, uint64_t pop_size, double mu_r, double xo_r, uint64_t xo_p, uint64_t tree_s, bool elitism, uint64_t gen_limit, uint64_t msrt, uint64_t dnvt);
		hoponpop = new population(&randi, pop_size, mu_r, xo_r, xo_p, tree_s, elitism, gen_limit, msrt, dnvt);

//		cout << "Trial: " << trailer_trash << endl;
		if (trailer_trash==0)
			cout << "Trial: ";
		cout << trailer_trash << " ";
		cout.flush();

		hoponpop->populator();

		uint64_t generational_recursion;
		for (generational_recursion=0; generational_recursion<num_gens; generational_recursion++)
		{
//			cout << "Generation: " << generational_recursion << endl;
//			hoponpop.breeder();
			hoponpop->breeder();
//			cout << "Generation over\n";
		}

		specimen_t bestever = hoponpop->BestEver();
		uint64_t aged = hoponpop->Age();
//		outfileTheBest << "NumGens,PopSize,Tree_S,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,NC,LEN,SEN1,SEN2,SCC,RAU,NAI,Fitness";
//		outfileTheBest.precision(best_default_precision);
		outfileTheBest << "\n" << num_gens << "," << pop_size << "," << tree_s << "," << xo_p << "," << xo_r << "," << mu_r << "," << elitism << ",";
		outfileTheBest << rng_seed << "," << trailer_trash << "," << bestever.calced << "," << aged << ",";
		outfileTheBest << bestever.gen.nc << "," << bestever.gen.len << "," << bestever.gen.sen1 << "," << bestever.gen.sen2 << ",";
		outfileTheBest << bestever.gen.scc << "," << bestever.gen.rau << "," << bestever.gen.nai << ",";
//		outfileTheBest.precision(16);
		outfileTheBest << bestever.fit;

		outfileTheBest.flush();

		delete hoponpop;
	}//End of Trial
	cout << endl;

	outfileTheBest << "\n";
	outfileTheBest.flush();

//	cout << "\tProcessing time: " << ((double) getTickCount() - last_tick_count)/getTickFrequency() << "[s]\n";
	cout << "\tProcessing time: " << ((double) std::time(0) - last_tick_count) << "[s]\n";

	outfileTheBest.close();

	return 0;
}

