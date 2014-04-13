
#include "fpcompare.hpp"

int main(int argc, char* argv[])
{
	string infileName = "./fp_best.csv";
	string outfileName = "./fp_best_plus_yield.csv";
	bool infileYield = false;

    try
    {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help",								"Produce help message")
			("i",		po::value<string>(),		"Set Input File")
			("o",		po::value<string>(),		"Set Output File")
			("y",		po::value<bool>(),			"Infile was for yield")
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

		if (vm.count("y"))
		{
			infileYield = vm["y"].as<bool>();
			cout << "Input File sought best yield: " << infileYield << ".\n";
		}
		else
		{
			cout << "Input File sought best yield: " << infileYield << ".\n";
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


	// Set up the bitset array that you could not initialize.
	uint64_t iter;
	for (iter=0; iter<NUMBER_GENES; iter++)
	{
		West73_Yields[iter].Y1 = West73_Yields[iter].y1 * West73_Yields[iter].area;
		West73_Yields[iter].Y2 = West73_Yields[iter].y2 * West73_Yields[iter].area;
		West73_Yields[iter].Y3 = West73_Yields[iter].y3 * West73_Yields[iter].area;
	}


	stringstream strstr (stringstream::in | stringstream::out);
	strstr.clear();	strstr.str("");

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



	while (!outfileTheBest.eof())
	{
		uint64_t iter;
		strstr.clear();	strstr.str("");
		string nextLine = outfileTheBest.getline();
/// Parse old file
		if (infileYield)
		{
//"NumGens,PopSize,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,
// Geno1,Geno2,Geno3,Yield";
			if (nextLine[0] == 'N')
			{
				outfileTheComprehensive <<	"NumGens,PopSize,XO_P,XO_R,MU_R,Elitism,"
										<<	"RNG_Seed,Trial,FitEvals,EndGen,"
										<<	"Geno1,Geno2,Geno3,Yield,Fitness\n";
			}
			else
			{

				for (iter=0; iter<10; iter++)
				{
					
				}
			}
		}
		else
		{
//"NumGens,PopSize,Tree_S,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,
// Geno1,Geno2,Geno3,Fitness";
			if (nextLine[0] == 'N')
			{
				outfileTheComprehensive <<	"NumGens,PopSize,Tree_S,"
										<<	"XO_P,XO_R,MU_R,Elitism,"
										<<	"RNG_Seed,Trial,FitEvals,EndGen,"
										<<	"Geno1,Geno2,Geno3,Fitness,Yield\n";
			}
			else
			{

				for (iter=0; iter<10; iter++)
				{
					
				}
			}
		}














/// Calculate fitness
		FITNESS_TYPE y1 =0, y2 =0, y3 = 0, fittie = 0;

		for (iter=0; iter<NUMBER_GENES; iter++)
		{
			if (genie.one[iter]==1)
				y1 += West73_Yields[iter].Y1;
			if (genie.two[iter]==1)
				y2 += West73_Yields[iter].Y2;
			if (genie.thr[iter]==1)
				y3 += West73_Yields[iter].Y3;
		}
		fittie = (34467.0-y1)*(34467.0-y1) + (34467.0-y2)*(34467.0-y2) + (34467.0-y3)*(34467.0-y3);


/// Output to new file
//"NumGens,PopSize,Tree_S,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,
// Geno1,Geno2,Geno3,Fitness,Yield";


	uint64_t best_default_precision = outfileTheComprehensive.precision();

	outfileTheComprehensive << "NumGens,PopSize,Tree_S,XO_P,XO_R,MU_R,Elitism,RNG_Seed,Trial,FitEvals,EndGen,Geno1,Geno2,Geno3,Fitness";

		outfileTheComprehensive.precision(best_default_precision);
		outfileTheComprehensive << "\n" << NUMBER_GENERATIONS << "," << pop_size << "," << tree_s << "," << xo_p << "," << xo_r << "," << mu_r << "," << elitism << ",";
		outfileTheComprehensive << rng_seed << "," << trailer_trash << "," << bestever.calced << "," << aged << ",";
		outfileTheComprehensive.precision(35);
		outfileTheComprehensive << bestever.gen.one << "," << bestever.gen.two << "," << bestever.gen.thr << "," << bestever.fit << ;


	}

	outfileTheComprehensive << "\n";
	outfileTheComprehensive.flush();

	outfileTheComprehensive.close();

	return 0;
}

