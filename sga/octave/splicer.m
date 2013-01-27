function locus = splicer(genes)

% Crossover loci (0 ~ 24)
% boost::uniform_int<int> xvr_dist(0,NUMBER_DISEASES-1);

locus = uint32( rand() * genes );

