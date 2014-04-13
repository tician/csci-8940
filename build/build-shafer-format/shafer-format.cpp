
#include "shafer-format.hpp"

int main(int argc, char* argv[])
{
	string infileName = "./shafer_best.csv";
	string outfileName = "./shafer_best_expanded.csv";
	bool fixingPSO = false;

    try
    {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help",								"Produce help message")
			("i",		po::value<string>(),		"Set Input File")
			("o",		po::value<string>(),		"Set Output File")
			("pso",		po::value<bool>(),			"Fixing PSO")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			cout << desc << "\n";
			return 1;
		}

		if (vm.count("i"))
		{
			infileName= vm["i"].as<string>();
			cout << "Input File name was set to " << infileName << ".\n";
		}
		else
		{
			cout << "Input File name was set to default of " << infileName << ".\n";
		}

		if (vm.count("o"))
		{
			outfileName = vm["o"].as<string>();
			cout << "Output File name was set to " << outfileName << ".\n";
		}
		else
		{
			cout << "Output File name was set to default of " << outfileName << ".\n";
		}

		if (vm.count("pso"))
		{
			fixingPSO = vm["pso"].as<bool>();
			cout << "Fixing PSO: " << fixingPSO << ".\n";
		}
		else
		{
			cout << "Fixing PSO: " << fixingPSO << ".\n";
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


//	stringstream strstr (stringstream::in | stringstream::out);
//	strstr.clear();	strstr.str("");
//	strstr.precision(35);

	ifstream outfileTheBest;
	ofstream outfileTheComprehensive;


	outfileTheBest.open( infileName.c_str(), std::ofstream::in );
	if ( !outfileTheBest.is_open() )
	{
		cerr << "Unable to open file: " << infileName << "\n";
		return 1;
	}

	outfileTheComprehensive.open( outfileName.c_str(), std::ofstream::out | std::ofstream::app );
	if ( !outfileTheComprehensive.is_open() )
	{
		cerr << "Unable to open file: " << outfileName << "\n";
		return 1;
	}


	uint64_t best_default_precision = outfileTheComprehensive.precision();

//	string nextLine;
//	getline(outfileTheBest, nextLine);
//	strstr.clear();	strstr.str(nextLine);

	char nextLine[256];
	outfileTheBest.getline(nextLine, 256);

	while (!outfileTheBest.eof())
	{

/// Parse old file
		if (fixingPSO)
		{
		//PSO
			//NumGens,PopSize,RNG_Seed,Trial,FitEvals,EndGen,Genotype,Fitness
			uint64_t num_gens=0, pop_size=0, rng_seed=0, trial=0, fit_evals=0, end_gen=0;//, geno=0;
			char geno[64];
			FITNESS_TYPE fit_orig=0.0, fit_new=0.0;

//			sscanf(nextLine, "%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lf", &num_gens, &pop_size, &rng_seed, &trial, &fit_evals, &end_gen, &geno, &fit_orig);
			sscanf(nextLine, "%lu,%lu,%lu,%lu,%lu,%lu,%44s,%lf", &num_gens, &pop_size, &rng_seed, &trial, &fit_evals, &end_gen, geno, &fit_orig);

/*
			strstr >> num_gens;
			strstr >> pop_size;
			strstr >> rng_seed;
			strstr >> trial;
			strstr >> fit_evals;
			strstr >> end_gen;
			strstr >> geno;
			strstr >> fit_orig;
*/

/// Calculate x, y, and new fitness
			FITNESS_TYPE x=0, y=0;
//			GENO_TYPE genie (geno);
			string geno_str (geno);
			assert (geno_str.length() == 44);
			GENO_TYPE genie ( geno_str );
			GENO_TYPE tfit ((1<<22)-1);

			tfit = tfit&genie;
			x = (tfit.to_ulong() * 0.00004768372718899898) - 100.0;
			tfit = genie>>22;
			y = (tfit.to_ulong() * 0.00004768372718899898) - 100.0;

			fit_new = 0.5 + (  ( sin(sqrt(x*x+y*y))*sin(sqrt(x*x+y*y)) - 0.5 ) / ( (1.0 + 0.001*(x*x + y*y)) * (1.0 + 0.001*(x*x + y*y)) )  );


/// Output to new file
			outfileTheComprehensive.precision(best_default_precision);
			outfileTheComprehensive << num_gens << "," << pop_size << "," << rng_seed << "," << trial << "," << fit_evals << "," << end_gen << ",";
			outfileTheComprehensive.precision(35);
			outfileTheComprehensive << genie << "," << fit_orig << "," << x << "," << y << "," << fit_new << "\n";
		}
		else
		{
		//GA
			//NumGens,PopSize,Tree_S,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,Geno,Fitness
			uint64_t num_gens=0, pop_size=0, tree_s=0, xo_p=0, elitism=0, rng_seed=0, trial=0, fit_evals=0, end_gen=0;//, geno=0;
			char geno[64];
			double xo_r=0.0, mu_r=0.0;
			FITNESS_TYPE fit_orig=0.0, fit_new=0.0;

//			sscanf(nextLine, "%lu,%lu,%lu,%lu,%lf,%lf,%lu,%lu,%lu,%lu,%lu,%lu,%lf", &num_gens, &pop_size, &tree_s, &xo_p, &xo_r, &mu_r, &elitism, &rng_seed, &trial, &fit_evals, &end_gen, &geno, &fit_orig);
			sscanf(nextLine, "%lu,%lu,%lu,%lu,%lf,%lf,%lu,%lu,%lu,%lu,%lu,%44s,%lf", &num_gens, &pop_size, &tree_s, &xo_p, &xo_r, &mu_r, &elitism, &rng_seed, &trial, &fit_evals, &end_gen, geno, &fit_orig);


/*
			strstr >> num_gens;
			strstr >> pop_size;
			strstr >> tree_s;
			strstr >> xo_p;
			strstr >> xo_r;
			strstr >> mu_r;
			strstr >> elitism;
			strstr >> rng_seed;
			strstr >> trial;
			strstr >> fit_evals;
			strstr >> end_gen;
			strstr >> geno;
			strstr >> fit_orig;
*/
/// Calculate x, y, and new fitness
			FITNESS_TYPE x=0, y=0;
//			GENO_TYPE genie (geno);
			string geno_str (geno);
			assert (geno_str.length() == 44);
			GENO_TYPE genie ( geno_str );
			GENO_TYPE tfit ((1<<22)-1);

			tfit = tfit&genie;
			x = (tfit.to_ulong() * 0.00004768372718899898) - 100.0;
			tfit = genie>>22;
			y = (tfit.to_ulong() * 0.00004768372718899898) - 100.0;

			fit_new = 0.5 + (  ( sin(sqrt(x*x+y*y))*sin(sqrt(x*x+y*y)) - 0.5 ) / ( (1.0 + 0.001*(x*x + y*y)) * (1.0 + 0.001*(x*x + y*y)) )  );


/// Output to new file
			outfileTheComprehensive.precision(best_default_precision);
			outfileTheComprehensive << num_gens << "," << pop_size << "," << tree_s << "," << xo_p << "," << xo_r << "," << mu_r << "," << elitism << ",";
			outfileTheComprehensive << rng_seed << "," << trial << "," << fit_evals << "," << end_gen << ",";
			outfileTheComprehensive.precision(35);
			outfileTheComprehensive << genie << "," << fit_orig << "," << x << "," << y << "," << fit_new << "\n";

		}
//		getline(outfileTheBest, nextLine);
//		strstr.clear();	strstr.str(nextLine);

		outfileTheBest.getline(nextLine, 256);

	}

	outfileTheComprehensive << "\n";
	outfileTheComprehensive.flush();

	outfileTheBest.close();
	outfileTheComprehensive.close();

	return 0;
}

