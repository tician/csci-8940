#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include "opencv2/core/core.hpp"

#include <algorithm>
#include <vector>
#include <iterator>

#include <boost/program_options.hpp>


using namespace cv;
using namespace boost;
namespace po = boost::program_options;
using namespace std;


#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30000

#define NUMBER_TRACKING				3
#define NUMBER_ATTRIBUTES			44

#define MAXIMIZING_FITNESS_VALUE

#ifdef MAXIMIZING_FITNESS_VALUE
#define HORRIFIC_FITNESS_VALUE		0.0
#else
#define HORRIFIC_FITNESS_VALUE		1.0e12
#endif

#define FITNESS_TYPE				double
#define GENO_TYPE					bitset<NUMBER_ATTRIBUTES>
