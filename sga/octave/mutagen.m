function mutate = mutagen(rate)

% Mutate when mutagen()==0
% boost::uniform_int<int> mut_dist(0,(int)(1/MUTATION_RATE));

if ( ( rand()*(1/rate) ) < 1 )
	mutate = 1;
else
	mutate = 0;
end

