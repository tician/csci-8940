#include <fstream>
#include <iostream>
#include <sstream>
//#include <bitset>
#include <sys/time.h>

#include <algorithm>
#include <vector>
#include <iterator>

#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>


using namespace boost;
namespace po = boost::program_options;
using namespace std;


#define NUMBER_TRIALS				10
#define NUMBER_GENERATIONS			30000

#define NUMBER_ATTRIBUTES			7

#define MAXIMIZING_FITNESS_VALUE

#ifdef MAXIMIZING_FITNESS_VALUE
#define HORRIFIC_FITNESS_VALUE		-1.0
#else
#define HORRIFIC_FITNESS_VALUE		1.0e12
#endif

#define FITNESS_TYPE				double
#define GENO_TYPE					uint64_t
//#define GENO_TYPE					bitset<NUMBER_ATTRIBUTES>

#define MIN_VELO					-4.0
#define MAX_VELO					4.0
