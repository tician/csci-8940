#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include "opencv2/core/core.hpp"
#include <string>

#include <algorithm>
#include <vector>
#include <iterator>

#include <boost/program_options.hpp>


using namespace cv;
using namespace boost;
namespace po = boost::program_options;
using namespace std;


#define NUMBER_GENES				44

#define FITNESS_TYPE				double
#define GENO_TYPE					bitset<NUMBER_GENES>
