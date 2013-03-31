
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif



#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30
#define NUMBER_INDIVIDUALS			200

#define NUMBER_GENES				25
#define NUMBER_SYMPTOMS				10

#define MUTATION_RATE				0.003
#define CROSSOVER_RATE				0.1
#define CROSSOVER_POINTS			1

//#define FITNESS_TYPE				__float128
#define FITNESS_TYPE				double


#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

int main(void)
{
	RNG 








// Print data to file

	stringstream strstr (stringstream::in | stringstream::out);
	strstr.clear();	strstr.str("");
	strstr << "./bitmask_73s"
		<< "_" << POPULATION_LIMIT
		<< "_" << GENERATION_LIMIT
		<< "_" << CROSSOVER_POINTS
		<< "_" << CROSSOVER_RATE
		<< "_" << MUTATION_RATE
	string outname;
	outname = strstr.str();
	ofstream outfile;
	outfile.open( outname );

	if ( !outfile.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}
	for (iter=0; iter<NUMBER_TRIALS; iter++)
	{
		outfile << "Trial: " << iter+1;
		for (jter=0; jter<NUMBER_GENERATIONS; jter++)
		{
			outfile << "\nGen: " << jter;
			outfile << "\nFit1," << First[jter].sin_fit;
			outfile << ",Pop1,";
			for (kter=0; kter<NUMBER_GENES-1; kter++)
			{
				outfile << First[jter].gene[kter] << ",";
			}
			outfile << First[jter].gene[kter];
			
			outfile << "\nFit2," << Second[jter].sin_fit;
			outfile << ",Pop2,";
			for (kter=0; kter<NUMBER_GENES-1; kter++)
			{
				outfile << Second[jter].gene[kter] << ",";
			}
			outfile << Second[jter].gene[kter];
			outfile << "\nFit3," << Third[jter].sin_fit;
			outfile << ",Pop3,";
			for (kter=0; kter<NUMBER_GENES-1; kter++)
			{
				outfile << Third[jter].gene[kter] << ",";
			}
			outfile << Third[jter].gene[kter] << "\n";
		}
	}
	outfile.close();

	return 0;
}

